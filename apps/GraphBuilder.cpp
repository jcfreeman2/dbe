
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

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <regex>
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
    std::vector<ConfigObject> session_objects {};

    m_confdb->get("Session", session_objects);

    if (session_objects.size() == 1) {
      m_session_name = session_objects[0].UID();
    } else {
      std::stringstream errmsg;
      errmsg << "Did not find one and only one Session instance in \"" << m_oksfilename << "\" and its includes";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }

    // The following not-brief section of code is dedicated to
    // determining which applications in the configuration are
    // disabled
    
    // First, we need the session object to check if an application
    // has been disabled

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

    std::vector<ConfigObject> all_class_objects {};

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

  void GraphBuilder::calculate_graph(const ObjectKind level, const std::string& obj_uid) {

    find_candidate_objects(level);
    
    bool found = false;
    for (auto& obj: m_candidate_objects) {
      if (obj.UID() == obj_uid) {
	found = true;
	const int top_level = 1;
	find_objects_and_connections(obj, top_level);   // Put differently, "find the vertices"
	break;
      }
    }

    if (!found) {
      std::stringstream errmsg;
      errmsg << "Unable to find requested object \"" << obj_uid << "\"";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }

    calculate_network_connections();  // Put differently, "find the edges between the vertices"
  }

  void GraphBuilder::calculate_network_connections() {
    
    for (auto& incoming : m_incoming_network_connections) {

      std::regex incoming_pattern(incoming.first);

      for (auto& outgoing : m_outgoing_network_connections) {

	std::regex outgoing_pattern(outgoing.first);
	
	bool match = false;
	
	if (incoming.first == outgoing.first) {
	  match = true;
	} else if (incoming.first.find(".*") != std::string::npos) {
	  if (std::regex_match(outgoing.first, incoming_pattern)) {
	    match = true;
	  }
	} else if (outgoing.first.find(".*") != std::string::npos) {
	  if (std::regex_match(incoming.first, outgoing_pattern)) {
	    match = true;
	  }
	}

	if (match) {
	  for (auto& receiver : incoming.second) {
	    for (auto& sender : outgoing.second) {
	      std::cout << sender << " sends to " << receiver << " via " << incoming.first << "\n";

	      auto sender_iterator = std::ranges::find_if(m_objects_for_graph,
							  [sender](const EnhancedObject& obj) {
							    return obj.config_object.UID() == sender;
							  });

	      auto receiver_iterator = std::ranges::find_if(m_objects_for_graph,
							    [receiver](const EnhancedObject& obj) {
							      return obj.config_object.UID() == receiver;
							    });

	      auto sender_index = std::distance(m_objects_for_graph.begin(), sender_iterator);
	      auto receiver_index = std::distance(m_objects_for_graph.begin(), receiver_iterator);

	      const EnhancedObject::ReceivingInfo receiving_info {incoming.first, receiver_index};

	      auto res = std::ranges::find(m_objects_for_graph[sender_index].receiving_object_infos, receiving_info );

	      if (res == m_objects_for_graph[sender_index].receiving_object_infos.end() ) {
		m_objects_for_graph[sender_index].receiving_object_infos.push_back(receiving_info);
	      }
	    }
	  }
	}
      }
    }
  }
  
  size_t
  GraphBuilder::find_objects_and_connections(const ConfigObject& object, int level) {

    ObjectKind kind = ObjectKind::kSession;
    
    if (object.class_name().find("Segment") != std::string::npos ) {
      kind = ObjectKind::kSegment;
    } else if (object.class_name().find("Application") != std::string::npos ) {
      kind = ObjectKind::kApplication;
    } else if (object.class_name().find("Module") != std::string::npos ) {
      kind = ObjectKind::kModule;
    }
    
    EnhancedObject enhanced_object { object, kind, level };
    
    // If we've got a session or a segment, look at its OKS-relations,
    // and recursively process those relation objects which are on the
    // candidates list and haven't already been processed
    
    if (enhanced_object.kind == ObjectKind::kSession || enhanced_object.kind ==	ObjectKind::kSegment) {

      for (auto& related_object: find_related_objects(enhanced_object.config_object)) {

	if (std::find(m_candidate_objects.begin(), m_candidate_objects.end(), related_object) != m_candidate_objects.end()) {  // If it's in the candidate list
	  if (std::find(m_passed_objects.begin(), m_passed_objects.end(), related_object) == m_passed_objects.end()) { // And it hasn't already been added to the "passed" list
	    std::cout << "CONNECT " << object.UID() << " to " << related_object.UID() << "\n";

	    auto related_object_index = find_objects_and_connections(related_object, level + 1);
	    enhanced_object.related_object_indices.push_back(related_object_index);
	  }
	}
      }
    }
    // If we've got an application object, try to determine what
    // modules are in it and what their connections are. Recursively
    // process the modules, and then add connection info to class-wide
    // member maps to calculate edges corresponding to the connections
    // for the plotted graph later

    else if (enhanced_object.kind == ObjectKind::kApplication) {

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
	  
	  for (auto in : module->get_inputs()) {
	    if (in->config_object().class_name() == "NetworkConnection") {
	      m_incoming_network_connections[in->config_object().UID()].push_back( object.UID() );
	    }
	  }

	  for (auto out : module->get_outputs()) {
	    if (out->config_object().class_name() == "NetworkConnection") {
	      std::cout << out->config_object().UID() << " goes out of " << object.UID() << "\n";

	      m_outgoing_network_connections[out->config_object().UID()].push_back( object.UID() );
	    }
	  }
	}
      }
    }

    size_t enhanced_object_index = std::numeric_limits<size_t>::max();
    
    if (std::find(m_passed_objects.begin(), m_passed_objects.end(), object) == m_passed_objects.end()) {
      m_passed_objects.push_back( object );
      m_objects_for_graph.push_back( enhanced_object );
      enhanced_object_index = m_objects_for_graph.size() - 1;
    } else {
      assert(false);
    }

    return enhanced_object_index;
  }

  void GraphBuilder::construct_graph(const ObjectKind level, const std::string& obj_uid) {
    calculate_graph(level, obj_uid);
    
    for (auto& enhanced_object : m_objects_for_graph) {

      auto& obj = enhanced_object.config_object;

      std::cout << obj.UID() << " owns the following: ";
      for (auto& eoi : enhanced_object.related_object_indices) {
	std::cout << m_objects_for_graph[eoi].config_object.UID() << " ";
      }
      std::cout << "\n";
      
      enhanced_object.vertex_in_graph = boost::add_vertex( VertexLabel(obj.UID(), obj.class_name()), m_graph);
    }

    for (auto i_p = 0; i_p < m_objects_for_graph.size(); ++i_p) {
      for (auto& i_c : m_objects_for_graph[i_p].related_object_indices) {
	boost::add_edge(m_objects_for_graph[i_p].vertex_in_graph,
			m_objects_for_graph[i_c].vertex_in_graph,
			{""}, // No label for an edge which just describes ownership
			m_graph);
      }
    }

    for (auto& possible_sender_object : m_objects_for_graph) {
      for (auto receiver_info : possible_sender_object.receiving_object_infos) {

	auto edge = boost::add_edge(
				    possible_sender_object.vertex_in_graph,
				    m_objects_for_graph[receiver_info.receiver_index].vertex_in_graph,
				    { receiver_info.connection_name },
				    m_graph).first;
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

  void GraphBuilder::write_graph(const std::string& outputfilename) const {

    std::ofstream outputfile;

    outputfile.open( outputfilename );
    
    if ( outputfile.is_open() ) {
      boost::write_graphviz(outputfile,
			    m_graph,
			    boost::make_label_writer(boost::get(&GraphBuilder::VertexLabel::displaylabel, m_graph)),
			    boost::make_label_writer(boost::get(&GraphBuilder::EdgeLabel::displaylabel, m_graph))
			    );
    } else {
      std::stringstream errmsg;
      errmsg << "Unable to open requested file \"" << outputfilename << "\" for writing";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }
  };

  
} // namespace dbe

