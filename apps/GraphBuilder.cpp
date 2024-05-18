
#include "GraphBuilder.hpp"

#include "dbe/config_reference.hpp"
#include "dbe/config_api.hpp"

#include "ers/ers.hpp"

#include "coredal/Session.hpp"
#include "coredal/Connection.hpp"
#include "coredal/DaqModule.hpp"

#include "appdal/DFApplication.hpp"
#include "appdal/DFOApplication.hpp"
#include "appdal/ReadoutApplication.hpp"
#include "appdal/SmartDaqApplication.hpp"
#include "appdal/TriggerApplication.hpp"
#include "appdal/MLTApplication.hpp"
#include "appdal/TPStreamWriterApplication.hpp"

#include "appdal/appdalIssues.hpp"


#include "boost/graph/graphviz.hpp"

#include <QFileInfo>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define GENERAL_DEBUG_LVL 10

namespace dbe {

  GraphBuilder::GraphBuilder(const std::string& oksfilename) :
    m_oksfilename(oksfilename),
    m_included_classes {  // Classes which match the first token in the list are the roots in the graph
      { TopGraphLevel::kSession, {"Session", "Segment", "Application"} },
      { TopGraphLevel::kSegment, {"Segment", "Application"} },
      { TopGraphLevel::kApplication, {"Application"} },
      { TopGraphLevel::kModule, {"Module", ""} }
    },
    m_session(nullptr)
  {

    // Open the database represented by the OKS XML file

    QFileInfo database_file(QString::fromStdString(m_oksfilename));

    if (database_file.exists()) {
      QString path_to_database = QString(database_file.absoluteFilePath());

      confaccessor::setdbinfo( path_to_database, dbe::dbinfo::oks);
	
      if (confaccessor::load()) {
	TLOG () << "Database initialized from \"" << m_oksfilename << "\"";
      } else {
	throw dbe::GeneralGraphToolError(ERS_HERE, "Could not load database. Check environment variable DUNEDAQ_DB_PATH");
      }
    } else {
      std::stringstream errmsg;
      errmsg << "Cannot open database. File error for file \"" << m_oksfilename << "\"";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }

    // Get the session in the database. Currently (May-13-2024) can handle one and only one session

    auto session_objects = config::api::info::onclass::objects<false>("Session", false);
    if (session_objects.size() != 1) {
      std::stringstream errmsg;
      errmsg << "Did not find one and only one Session instance in \"" << m_oksfilename << "\" and its includes";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }

    // We need the session object to check if an application has been disabled

    // Note the "const_cast" is needed since "get_underlying_object"
    // returns a const pointer, but since m_session is a member needed
    // by multiple functions and can't be determined until after we've
    // opened the database and found the session, we need to change
    // its initial value here. Once this is done, it shouldn't be
    // changed again.
    
    m_session = const_cast<dunedaq::coredal::Session*>(config::api::info::onclass::get_underlying_object<dunedaq::coredal::Session>(session_objects[0].UID()));
    
    if (m_session == nullptr) {
      std::stringstream errmsg;
      errmsg << "Unable to get session with UID \"" << session_objects[0].UID() << "\"";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }

    std::vector<std::string> all_classnames =
      { config::api::info::onclass::allnames<std::vector<std::string>>() };

    for (const auto& classname : all_classnames) {

      auto all_class_objects = config::api::info::onclass::objects<false>(classname, false);
      for (const auto& obj : all_class_objects) {
	m_all_objects.emplace_back( obj ); 
      }

      if (classname.find("Application") != std::string::npos ) {

	for (const auto& appobj : all_class_objects) {

	  // May-14-2024: The following code is based on what's found in appdal's generate_modules_test.cxx
	  
	  auto daqapp = config::api::info::onclass::get_underlying_object<dunedaq::appdal::SmartDaqApplication>(appobj.UID());
	  
	  if (daqapp != nullptr) {

	    auto res = daqapp->cast<dunedaq::coredal::ResourceBase>();
	    
	    if (res != nullptr && res->disabled(*m_session)) {
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
    for (const auto& obj: m_candidate_objects) {
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
    for (const auto& obj: m_candidate_objects) {
      if (obj.UID() == obj_uid) {
	found = true;
	m_passed_objects.emplace_back(obj);
	auto vertex = boost::add_vertex( VertexLabel(obj.UID(), obj.class_name()), m_graph);
	add_connected_objects(obj, vertex, false);
      
	if (obj.class_name().find("Application")) {

	  auto daqapp = config::api::info::onclass::get_underlying_configuration()->get<dunedaq::appdal::SmartDaqApplication>(obj.UID());
	  if (daqapp) {
	    auto modules = daqapp->generate_modules(
					       config::api::info::onclass::get_underlying_configuration(), m_oksfilename, m_session);

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
    
  void GraphBuilder::add_connected_objects(const tref& starting_obj, const Vertex_t& starting_vtx, bool add_edges) {

    VertexLabel starting_vtx_label(starting_obj.UID(), starting_obj.class_name());
    
    std::vector<tref> connected_objs = config::api::graph::linked::by::object<tref> ( starting_obj ); 

    for (const auto& connected_obj: connected_objs) {

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

