
#include "GraphBuilder.hpp"

#include "conffwk/Configuration.hpp"
#include "conffwk/Schema.hpp"

#include "ers/ers.hpp"

#include "confmodel/Session.hpp"
#include "confmodel/Connection.hpp"
#include "confmodel/DaqModule.hpp"

#include "appmodel/DFApplication.hpp"
#include "appmodel/DFOApplication.hpp"
#include "appmodel/ReadoutApplication.hpp"
#include "appmodel/SmartDaqApplication.hpp"
#include "appmodel/TriggerApplication.hpp"
#include "appmodel/MLTApplication.hpp"
#include "appmodel/TPStreamWriterApplication.hpp"

#include "appmodel/appmodelIssues.hpp"


#include "boost/graph/graphviz.hpp"

#include <QFileInfo>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define GENERAL_DEBUG_LVL 10

namespace dbe {

  GraphBuilder::GraphBuilder(const std::string& oksfilename) :
    m_oksfilename { oksfilename },
    m_included_classes {  // Classes which match the first token in the list are the roots in the graph
      { ObjectKind::kSession, {"Session", "Segment", "Application"} },
      { ObjectKind::kSegment, {"Segment", "Application"} },
      { ObjectKind::kApplication, {"Application"} },
      { ObjectKind::kModule, {"Module", ""} }
    },
    m_session { nullptr }
  {

    // Open the database represented by the OKS XML file

    try {
      m_confdb = new dunedaq::conffwk::Configuration("oksconflibs:" + m_oksfilename);
    } catch (dunedaq::conffwk::Generic& exc) {
      TLOG() << "Failed to load OKS database: " << exc << "\n";
      throw exc;
    }

    // Get the session in the database. Currently (May-13-2024) can handle one and only one session
    std::vector<ConfigObject> session_objects;

    m_confdb->get("Session", session_objects);

    if (session_objects.size() == 1) {
      m_session_name = session_objects[0].UID();
    } else {
      std::stringstream errmsg;
      errmsg << "Did not find one and only one Session instance in \"" << m_oksfilename << "\" and its includes";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }

    // We need the session object to check if an application has been disabled

    // Note the "const_cast" is needed since "m_confdb->get"
    // returns a const pointer, but since m_session is a member needed
    // by multiple functions and can't be determined until after we've
    // opened the database and found the session, we need to change
    // its initial value here. Once this is done, it shouldn't be
    // changed again.

    m_session = const_cast<dunedaq::confmodel::Session*>(
							 m_confdb->get<dunedaq::confmodel::Session>(m_session_name));
  
    if (m_session == nullptr) {
      std::stringstream errmsg;
      errmsg << "Unable to get session with UID \"" << m_session_name << "\"";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }

    std::vector<ConfigObject> all_class_objects;

    using classmap = dunedaq::conffwk::fmap<dunedaq::conffwk::fset>;

    classmap classrepresentors = m_confdb->superclasses();
    std::vector<std::string> classnames;

    std::transform(classrepresentors.begin(),
		   classrepresentors.end(),
		   std::back_inserter(classnames),
		   [](const classmap::value_type& classrepresentor) {
		     return *classrepresentor.first;
		   });

    std::sort(classnames.begin(), classnames.end());

    std::vector<ConfigObject> every_object_of_class;

    for (auto& classname : classnames) {

      every_object_of_class.clear();
      all_class_objects.clear();
      TLOG_DEBUG(GENERAL_DEBUG_LVL) << "CLASS NAME IS " << classname << ": ";

      m_confdb->get(classname, every_object_of_class);

      std::copy_if(every_object_of_class.begin(),
		   every_object_of_class.end(),
		   std::back_inserter(all_class_objects),
		   [&classname](const ConfigObject& obj) {
		     if (obj.class_name() == classname)  {
		       TLOG_DEBUG(GENERAL_DEBUG_LVL) << obj.UID() << " ";
		       return true;
		     } else {
		       return false;
		     }
		   });
      TLOG_DEBUG(GENERAL_DEBUG_LVL) << std::endl;

      std::copy(all_class_objects.begin(), all_class_objects.end(), std::back_inserter(m_all_objects));

      if (classname.find("Application") != std::string::npos ) {
	for (const auto& appobj : all_class_objects) {

	  auto daqapp = m_confdb->get<dunedaq::appmodel::SmartDaqApplication>(appobj.UID());

	  if (daqapp != nullptr) {

	    auto res = daqapp->cast<dunedaq::confmodel::ResourceBase>();
	    
	    if (res && res->disabled(*m_session)) {
	      m_ignored_application_uids.push_back( appobj.UID() );
	      std::cout << "Skipping disabled application " << appobj.UID() << "@" << daqapp->class_name() << "\n";
	      continue;
	    }
	  } else {
	    std::cout << "daqapp for " << appobj.UID() << "@" << appobj.class_name() << " came up empty" << "\n";
	    m_ignored_application_uids.push_back( appobj.UID() );
	  }
	}
      }
    }
  }

  void GraphBuilder::find_candidate_objects(const ObjectKind level) {
    m_candidate_objects.clear();

    std::string cname;
    for (const auto& obj: m_all_objects) {
      for (const auto& classname : this->m_included_classes[ level ]) {

	cname = obj.class_name();
	if (cname.find(classname) != std::string::npos && \
	    std::find(m_ignored_application_uids.begin(), m_ignored_application_uids.end(), obj.UID()) == m_ignored_application_uids.end()) {
	  m_candidate_objects.emplace_back(obj);
	}
      }
    }

    for (const auto& obj : m_candidate_objects) {
      std::cout << "Candidate " << obj.UID() << "@" << obj.class_name() << "\n";
    }
  }

  void GraphBuilder::construct_graph(const ObjectKind level, const std::string& obj_uid) {

    find_candidate_objects(level);
    
    bool found = false;
    for (auto& obj: m_candidate_objects) {
      if (obj.UID() == obj_uid) {
	found = true;
	find_objects_and_connections(obj, 1); 
	break;
      }
    }

    if (!found) {
      std::stringstream errmsg;
      errmsg << "Unable to find requested object \"" << obj_uid << "\"";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }

    // std::unordered_map<std::string, std::vector<VertexLabel>> outgoing_connections {};
    // std::unordered_map<std::string, std::vector<VertexLabel>> incoming_connections {};

    // for (auto& obj: m_passed_objects) {

    //   if (obj.class_name().find("Application")) {

    // 	decltype(m_confdb) localdatabase;

    // 	try {
    // 	  localdatabase = new dunedaq::conffwk::Configuration("oksconflibs:" + m_oksfilename);
    // 	} catch (dunedaq::conffwk::Generic& exc) {
    // 	  TLOG() << "Failed to load OKS database: " << exc << "\n";
    // 	  throw exc;
    // 	}
	  
    // 	auto daqapp = localdatabase->get<dunedaq::appmodel::SmartDaqApplication>(obj.UID());
    // 	if (daqapp) {

    // 	  auto local_session = const_cast<dunedaq::confmodel::Session*>(
    // 									localdatabase->get<dunedaq::confmodel::Session>(m_session_name));
    // 	  auto modules = daqapp->generate_modules(localdatabase, m_oksfilename, local_session);

    // 	  // This map will exist until it's determined how to check a Graph_t for an already-existing vertex
    // 	  std::map<VertexLabel, Vertex_t> io_vertices;

    // 	  for (const auto& module : modules) {
	    
    // 	    // auto module_vertex = boost::add_vertex( VertexLabel(module->UID(), module->class_name()), m_graph);

    // 	    // // Easier to look at an Application graph without the lines between it and the (obviously-owned) modules
    // 	    // // boost::add_edge(vertex, module_vertex, m_graph);

    // 	    for (auto connection : module->get_inputs()) {
	      
    // 	      auto connection_vertex_label = VertexLabel(connection->config_object().UID(),
    // 							 connection->config_object().class_name());

    // 	      // if (io_vertices.find(connection_vertex_label) == io_vertices.end()) {
    // 	      // io_vertices[connection_vertex_label] = boost::add_vertex(connection_vertex_label, m_graph);
    // 	      // }

    // 	      // boost::add_edge(io_vertices[connection_vertex_label], module_vertex, m_graph);

    // 	      // Fill a map associating applications with a given incoming network connection
	      
    // 	      if (connection->config_object().class_name() == "NetworkConnection") {
    // 		incoming_connections[connection->config_object().UID()].emplace_back( VertexLabel(obj.UID(), obj.class_name() ));
    // 	      }
	      
    // 	    }

    // 	    for (auto connection : module->get_outputs()) {
    // 	      auto connection_vertex_label = VertexLabel(connection->config_object().UID(),
    // 							 connection->config_object().class_name());

    // 	      // if (io_vertices.find(connection_vertex_label) == io_vertices.end()) {
    // 	      // io_vertices[connection_vertex_label] = boost::add_vertex(connection_vertex_label, m_graph);
    // 	      // }

    // 	      // boost::add_edge(module_vertex, io_vertices[connection_vertex_label], m_graph);

    // 	      // Fill a map associating applications with a given outgoing network connection
	      
    // 	      if (connection->config_object().class_name() == "NetworkConnection") {
    // 		outgoing_connections[connection->config_object().UID()].emplace_back( VertexLabel(obj.UID(), obj.class_name() ));
    // 	      }
    // 	    }
    // 	  }
    // 	}
    //   }
    // }

    // TLOG() << "JCF: HERE WE GO";

    // for (auto& outgoing : outgoing_connections) {
    //   std::cout << "\n" << outgoing.first << "\n";

    //   if (incoming_connections.contains(outgoing.first)) {
	
    // 	for (auto& sending_app : outgoing.second) {
    // 	  for (auto& receiving_app : incoming_connections[outgoing.first]) {
    // 	    std::cout << sending_app.label << " sends data to " << receiving_app.label << "\n";
    // 	  }
    // 	}
    //   } else {
    // 	std::cout << outgoing.first << " sends data into the void" << "\n";
    //   }
    // }

    // std::cout << "ADDRESSES: " << m_map_to_vertices[ "hsi-to-tc-app@HSIEventToTCApplication" ] << " " << m_map_to_vertices[ "hsi-01@FakeHSIApplication" ] << "\n";
    
    // boost::add_edge(*m_map_to_vertices[ "hsi-to-tc-app@HSIEventToTCApplication" ],
    // 		    *m_map_to_vertices[ "hsi-01@FakeHSIApplication" ],
    // 		    m_graph);
    // std::cout << "DONE WITH ADDING EDGE" << "\n";
    // //auto some_vertex = boost::vertex(VertexLabel("hsi-01", "FakeHSIApplication"), m_graph);

  }

  void GraphBuilder::find_objects_and_connections(const ConfigObject& object, int level) {

    ObjectKind kind = ObjectKind::kSession;
    
    if (object.class_name().find("Segment") != std::string::npos ) {
      kind = ObjectKind::kSegment;
    } else if (object.class_name().find("Application") != std::string::npos ) {
      kind = ObjectKind::kApplication;
    } else if (object.class_name().find("Module") != std::string::npos ) {
      kind = ObjectKind::kModule;
    }
    
    EnhancedConfigObject enhanced_object { object, kind, level };

    if (std::find(m_passed_objects.begin(), m_passed_objects.end(), object) == m_passed_objects.end()) {
      m_passed_objects.push_back( object );
      m_objects_for_graph.push_back( enhanced_object );
    }

    if (enhanced_object.kind == ObjectKind::kSession || enhanced_object.kind ==	ObjectKind::kSegment) {

      for (auto& related_object: find_related_objects(enhanced_object.config_object)) {

	if (std::find(m_candidate_objects.begin(), m_candidate_objects.end(), related_object) != m_candidate_objects.end()) {  // If it's in the candidate list
	  if (std::find(m_passed_objects.begin(), m_passed_objects.end(), related_object) == m_passed_objects.end()) { // And it hasn't already been added to the "passed" list
	    std::cout << "CONNECT " << object.UID() << " to " << related_object.UID() << "\n";

	    find_objects_and_connections(related_object, level + 1);
	  }
	}
      }
    } else if (enhanced_object.kind == ObjectKind::kApplication) {

      dunedaq::conffwk::Configuration* local_database;

      try {                                                                                                
	local_database = new dunedaq::conffwk::Configuration("oksconflibs:" + m_oksfilename);               
      } catch (dunedaq::conffwk::Generic& exc) {                                                           
	TLOG() << "Failed to load OKS database: " << exc << "\n";                                          
	throw exc;                                                                                         
      }

      auto daqapp = local_database->get<dunedaq::appmodel::SmartDaqApplication>(object.UID());                 
      if (daqapp) {
	auto local_session = const_cast<dunedaq::confmodel::Session*>(
								      local_database->get<dunedaq::confmodel::Session>(m_session_name));
	auto modules = daqapp->generate_modules(local_database, m_oksfilename, local_session);

	for (const auto& module : modules) {
	  std::cout << "CONNECT " << object.UID() << " to " << module->UID() << "\n";
	  find_objects_and_connections(module->config_object(), level + 1);
	}
      }
    }
  }

  std::vector<dunedaq::conffwk::ConfigObject>
  GraphBuilder::find_related_objects(const ConfigObject& starting_obj) {

    std::vector<ConfigObject> connected_objects;

    dunedaq::conffwk::class_t classdef = m_confdb->get_class_info( starting_obj.class_name(), false );

    for (const dunedaq::conffwk::relationship_t& relationship : classdef.p_relationships) {

      // The ConfigObject::get(...) function doesn't have a
      // const-qualifier on it for no apparent good reason; we need
      // this cast in order to call it
      
      auto starting_obj_casted = const_cast<ConfigObject&>(starting_obj);
      
      if (relationship.p_cardinality == dunedaq::conffwk::only_one ||
	  relationship.p_cardinality == dunedaq::conffwk::zero_or_one) {
	ConfigObject connected_obj;
	starting_obj_casted.get(relationship.p_name, connected_obj);
	connected_objects.push_back(connected_obj);
      } else {
	std::vector<ConfigObject> connected_objects_in_relationship;
	starting_obj_casted.get(relationship.p_name, connected_objects_in_relationship);
	connected_objects.insert(connected_objects.end(), connected_objects_in_relationship.begin(), connected_objects_in_relationship.end());
      }
    }

    return connected_objects;
  }

  void write_graph(const GraphBuilder::Graph_t& graph, const std::string& outputfilename) {

    std::ofstream outputfile;

    if (outputfilename != "") {
      outputfile.open( outputfilename );

      if ( outputfile.is_open() ) {
	boost::write_graphviz(outputfile, graph, boost::make_label_writer(boost::get(&GraphBuilder::VertexLabel::displaylabel, graph)));
      } else {
	std::stringstream errmsg;
	errmsg << "Unable to open requested file \"" << outputfilename << "\" for writing";
	throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
      }
    } else {
        boost::write_graphviz(std::cout, graph, boost::make_label_writer(boost::get(&GraphBuilder::VertexLabel::displaylabel, graph)));
    }
  };

  
} // namespace dbe

