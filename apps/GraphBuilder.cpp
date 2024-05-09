
#include "GraphBuilder.hpp"

#include "boost/graph/graphviz.hpp"

#include <QFileInfo>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define GENERAL_DEBUG_LVL 10

namespace dbe {

  GraphBuilder::GraphBuilder(const std::string& oksfilename) :
    m_oksfilename(oksfilename),
    m_included_classes {  // Classes which match the first token in the list are the roots in the graph
      { TopGraphLevel::kSession, {"Session", "Segment", "Application"} },
		       { TopGraphLevel::kSegment, {"Segment", "Application"} },
		       { TopGraphLevel::kApplication, {"Application","Module", "Queue"} },
		       { TopGraphLevel::kModule, {"Module", ""} }
    }
  {

    QFileInfo database_file(QString::fromStdString(oksfilename));

    if (database_file.exists()) {
      QString path_to_database = QString(database_file.absoluteFilePath());

      {
	confaccessor::setdbinfo( path_to_database, dbe::dbinfo::oks);
	
	if (confaccessor::load()) {
	  TLOG () << "Database initialized from \"" << oksfilename << "\"";
	} else {
	  throw dbe::GeneralGraphToolError(ERS_HERE, "Could not load database. Check environment variable DUNEDAQ_DB_PATH");
	}
      }
    } else {
      std::stringstream errmsg;
      errmsg << "Cannot open database. File error for file \"" << oksfilename << "\"";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }

    std::vector<std::string> all_classnames =
      { config::api::info::onclass::allnames<std::vector<std::string>>() };

    for (const auto& classname : all_classnames) {
      for (const auto& obj : config::api::info::onclass::objects<false>(classname, false)) {
	m_all_objects.emplace_back( obj );
      }
    }
  }

  void GraphBuilder::construct_graph(const TopGraphLevel level) {
    auto oksclasslist = m_included_classes[ level ];
    
    std::string cname;
    for (const auto& obj: m_all_objects) {
      for (const auto& classname : oksclasslist) {

	cname = obj.class_name();
	if (cname.find(classname) != std::string::npos) {
	  m_candidate_objects.emplace_back(obj);
	}
      }
    }

    for (const auto& obj: m_candidate_objects) {
      cname = obj.class_name();
      if (cname.find(oksclasslist[0]) != std::string::npos) {

	m_passed_objects.emplace_back(obj);
	auto vertex = boost::add_vertex( VertexLabel(obj.UID(), obj.class_name()), m_graph);

	add_connected_objects(obj, vertex);
      }
    }
  }
    
  void GraphBuilder::add_connected_objects(const tref& starting_obj, const Vertex_t& starting_vtx) {
    
    VertexLabel starting_vtx_label(starting_obj.UID(), starting_obj.class_name());
    
    std::vector<tref> connected_objs = config::api::graph::linked::by::object<tref> ( starting_obj ); 

    for (const auto& connected_obj: connected_objs) {

      if (std::find(m_candidate_objects.begin(), m_candidate_objects.end(), connected_obj) != m_candidate_objects.end()) {  // If it's in the candidate list
	if (std::find(m_passed_objects.begin(), m_passed_objects.end(), connected_obj) == m_passed_objects.end()) { // And it hasn't already been added to the "passed" list

	  TLOG_DEBUG(GENERAL_DEBUG_LVL) << "Found connection between " << starting_obj.UID() << " and " << connected_obj.UID();
	  m_passed_objects.emplace_back(connected_obj);

	  VertexLabel connected_vtx_label(connected_obj.UID(), connected_obj.class_name());
	  const auto& connected_vtx = boost::add_vertex(connected_vtx_label, m_graph);
	  boost::add_edge(starting_vtx, connected_vtx, m_graph); 
	  
	  add_connected_objects(connected_obj, connected_vtx);
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

