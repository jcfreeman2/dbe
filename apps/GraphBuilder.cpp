
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
      { TopGraphLevel::kSession, {"Session", "Segment", "Application"} },
      { TopGraphLevel::kSegment, {"Segment", "Application"} },
      { TopGraphLevel::kApplication, {"Application"} },
      { TopGraphLevel::kModule, {"Module", ""} }
    },
    m_session { nullptr }
  {

    // Open the database represented by the OKS XML file

    try {
      m_confdb = new dunedaq::conffwk::Configuration("oksconflibs:" + oksfilename);
    } catch (dunedaq::conffwk::Generic& exc) {
      TLOG() << "Failed to load OKS database: " << exc << "\n";
      throw exc;
    }

    // Get the session in the database. Currently (May-13-2024) can handle one and only one session
    std::vector<ConfigObject> session_objects;

    m_confdb->get("Session", session_objects);

    if (session_objects.size() != 1) {
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
							 m_confdb->get<dunedaq::confmodel::Session>(session_objects[0].UID()));
  
    if (m_session == nullptr) {
      std::stringstream errmsg;
      errmsg << "Unable to get session with UID \"" << session_objects[0].UID() << "\"";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }

    std::vector<ConfigObject> every_object;
    std::vector<ConfigObject> all_class_objects;

    using classmap = dunedaq::conffwk::fmap<dunedaq::conffwk::fset>;

    classmap classrepresentors = m_confdb->superclasses();
    std::vector<std::string> classnames;

    std::transform(classrepresentors.begin(),
		   classrepresentors.end(),
		   std::back_inserter(classnames),
		   [](const classmap::value_type& classrepresentor) {
		     //return classrepresentor.first->c_str();
		     return *classrepresentor.first;
		   });

    std::sort(classnames.begin(), classnames.end());

    for (auto& classname : classnames) {

      every_object.clear();
      all_class_objects.clear();
      TLOG_DEBUG(GENERAL_DEBUG_LVL) << "CLASS NAME IS " << classname << ": ";

      m_confdb->get(classname, every_object);

      std::copy_if(every_object.begin(),
		   every_object.end(),
		   std::back_inserter(all_class_objects),
		   [&classname](const ConfigObject& obj) {
		     if (obj.class_name() == classname)  {
		       TLOG_DEBUG(GENERAL_DEBUG_LVL) << obj.UID() << " ";
		       return true;
		     } else {
		       return false;
		     }
		     return obj.class_name() == classname;
		   });
      TLOG_DEBUG(GENERAL_DEBUG_LVL) << std::endl;

      std::copy(all_class_objects.begin(), all_class_objects.end(), std::back_inserter(m_all_objects));

      if (classname.find("Application") != std::string::npos ) {
	for (const auto& appobj : all_class_objects) {

	  auto daqapp = m_confdb->get<dunedaq::appmodel::SmartDaqApplication>(appobj.UID());

	  if (daqapp != nullptr) {
	    //TLOG() << appobj.UID() << " is of class " << daqapp->class_name() << "\n";

	    auto res = daqapp->cast<dunedaq::confmodel::ResourceBase>();
	    
	    if (res && res->disabled(*m_session)) {
	      m_ignored_application_uids.push_back( appobj.UID() );
	      TLOG() << "Skipping disabled application " << appobj.UID() << "@" << daqapp->class_name();
	      continue;
	    }
	  } else {
	    TLOG() << "daqapp for " << appobj.UID() << "@" << appobj.class_name() << " came up empty";
	    m_ignored_application_uids.push_back( appobj.UID() );
	  }
	}
      }
    }
  }


  void GraphBuilder::construct_graph(const TopGraphLevel level) {

    find_candidate_objects(level);

    std::string cname;
    for (auto& obj: m_candidate_objects) {
      cname = obj.class_name();

      // The idea in the if-statement is the first element of the
      // vector for a given level is a token which should be found in
      // the highest-level object class names
      
      if (cname.find(m_included_classes[ level ][0]) != std::string::npos) {

	m_passed_objects.emplace_back(obj);
	auto vertex = boost::add_vertex( VertexLabel(obj.UID(), obj.class_name()), m_graph);

	add_connected_objects(obj, vertex, true);
      }
    }
  }

  void GraphBuilder::construct_graph(const TopGraphLevel level, const std::string& obj_uid) {

    find_candidate_objects(level);
    
    bool found = false;
    for (auto& obj: m_candidate_objects) {
      if (obj.UID() == obj_uid) {
	found = true;
	m_passed_objects.emplace_back(obj);
	auto vertex = boost::add_vertex( VertexLabel(obj.UID(), obj.class_name()), m_graph);
	add_connected_objects(obj, vertex, false);
      
	if (obj.class_name().find("Application")) {

	  auto daqapp = m_confdb->get<dunedaq::appmodel::SmartDaqApplication>(obj.UID());
	  if (daqapp) {
	    auto modules = daqapp->generate_modules(m_confdb, m_oksfilename, m_session);

	    // This map will exist until it's determined how to check a Graph_t for an already-existing vertex
	    std::map<VertexLabel, Vertex_t> io_vertices;

	    for (const auto& module : modules) {

	      auto module_vertex = boost::add_vertex( VertexLabel(module->UID(), module->class_name()), m_graph);

	      // Easier to look at an Application graph without the lines between it and the (obviously-owned) modules
	      // boost::add_edge(vertex, module_vertex, m_graph);

	      for (auto connection : module->get_inputs()) {

		auto connection_vertex_label = VertexLabel(connection->config_object().UID(),
							   connection->config_object().class_name());

		if (io_vertices.find(connection_vertex_label) == io_vertices.end()) {
		  io_vertices[connection_vertex_label] = boost::add_vertex(connection_vertex_label, m_graph);
		}

		boost::add_edge(io_vertices[connection_vertex_label], module_vertex, m_graph);
	      }

	      for (auto connection : module->get_outputs()) {

		auto connection_vertex_label = VertexLabel(connection->config_object().UID(),
							   connection->config_object().class_name());

		if (io_vertices.find(connection_vertex_label) == io_vertices.end()) {
		  io_vertices[connection_vertex_label] = boost::add_vertex(connection_vertex_label, m_graph);
		}

		boost::add_edge(module_vertex, io_vertices[connection_vertex_label], m_graph);
	      }
	    }
	  }
	}
      }
    }

    if (!found) {
      std::stringstream errmsg;
      errmsg << "Unable to find requested object \"" << obj_uid << "\"";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }
  }

  void GraphBuilder::find_candidate_objects(const TopGraphLevel level) {
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
      TLOG() << "Candidate " << obj.UID() << "@" << obj.class_name();
    }
  }

  std::vector<dunedaq::conffwk::ConfigObject>
  GraphBuilder::find_connected_objects(ConfigObject& starting_obj) {

    std::vector<ConfigObject> connected_objects;

    dunedaq::conffwk::class_t classdef = m_confdb->get_class_info( starting_obj.class_name(), false );

    for (const dunedaq::conffwk::relationship_t& relationship : classdef.p_relationships) {
      if (relationship.p_cardinality == dunedaq::conffwk::only_one ||
	  relationship.p_cardinality == dunedaq::conffwk::zero_or_one) {
	ConfigObject connected_obj;
	starting_obj.get(relationship.p_name, connected_obj);
	connected_objects.push_back(connected_obj);
      } else {
	std::vector<ConfigObject> connected_objects_in_relationship;
	starting_obj.get(relationship.p_name, connected_objects_in_relationship);
	connected_objects.insert(connected_objects.end(), connected_objects_in_relationship.begin(), connected_objects_in_relationship.end());
      }
    }

    return connected_objects;
  }

  void GraphBuilder::add_connected_objects(ConfigObject& starting_obj, const Vertex_t& starting_vtx, bool add_edges) {

    VertexLabel starting_vtx_label(starting_obj.UID(), starting_obj.class_name());

    for (auto& connected_obj: find_connected_objects(starting_obj)) {

      if (std::find(m_candidate_objects.begin(), m_candidate_objects.end(), connected_obj) != m_candidate_objects.end()) {  // If it's in the candidate list
	if (std::find(m_passed_objects.begin(), m_passed_objects.end(), connected_obj) == m_passed_objects.end()) { // And it hasn't already been added to the "passed" list

	  TLOG_DEBUG(GENERAL_DEBUG_LVL) << "Found connection between " << starting_obj.UID() << " and " << connected_obj.UID();
	  m_passed_objects.emplace_back(connected_obj);

	  VertexLabel connected_vtx_label(connected_obj.UID(), connected_obj.class_name());
	  const auto& connected_vtx = boost::add_vertex(connected_vtx_label, m_graph);

	  if (add_edges) {
	    boost::add_edge(starting_vtx, connected_vtx, m_graph);
	  }
	  
	  add_connected_objects(connected_obj, connected_vtx, true);
	}
      }
    }
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

