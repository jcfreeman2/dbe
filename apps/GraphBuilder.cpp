
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
#include <sstream>
#include <string>
#include <vector>

#define GENERAL_DEBUG_LVL 10

namespace dbe {

  GraphBuilder::GraphBuilder(const std::string& oksfilename) :
    m_oksfilename { oksfilename },
    m_confdb { nullptr },
    m_included_classes {
      { ObjectKind::kSession, {"Session", "Segment", "Application"} },
      { ObjectKind::kSegment, {"Segment", "Application"} },
      { ObjectKind::kApplication, {"Application", "Module"} },
      { ObjectKind::kModule, {"Module"} }
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
	find_objects_and_connections(level, obj);   // Put differently, "find the vertices"
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

    for (auto& incoming : m_incoming_connections) {

      std::regex incoming_pattern(incoming.first);

      for (auto& outgoing : m_outgoing_connections) {

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

	      // We just want to plot applications sending to other
	      // applications and queues sending to other
	      // queues. Showing, e.g., a queue directly sending to
	      // some other application via a network connection makes
	      // the plot too busy.

	      if (!m_objects_for_graph.contains(sender) || !m_objects_for_graph.contains(receiver)) {
		continue;
	      } else if (m_objects_for_graph.at(sender).kind != m_objects_for_graph.at(receiver).kind) {
		continue;
	      }

	      const EnhancedObject::ReceivingInfo receiving_info {incoming.first, receiver };

	      auto res = std::ranges::find(m_objects_for_graph.at(sender).receiving_object_infos, receiving_info );

	      if (res == m_objects_for_graph.at(sender).receiving_object_infos.end() ) {
		m_objects_for_graph.at(sender).receiving_object_infos.push_back(receiving_info);
	      }
	    }
	  }
	}
      }
    }
  }
  
  void
  GraphBuilder::find_objects_and_connections(const ObjectKind level, const ConfigObject& object) {

    ObjectKind kind = ObjectKind::kSession;
    
    if (object.class_name().find("Segment") != std::string::npos ) {
      kind = ObjectKind::kSegment;
    } else if (object.class_name().find("Application") != std::string::npos ) {
      kind = ObjectKind::kApplication;
    } else if (object.class_name().find("Module") != std::string::npos ) {
      kind = ObjectKind::kModule;
    }
    
    EnhancedObject enhanced_object { object, kind };
    
    // If we've got a session or a segment, look at its OKS-relations,
    // and recursively process those relation objects which are on the
    // candidates list and haven't already been processed
    
    if (enhanced_object.kind == ObjectKind::kSession || enhanced_object.kind ==	ObjectKind::kSegment) {

      for (auto& related_object: find_related_objects(enhanced_object.config_object)) {

	if (std::find(m_candidate_objects.begin(), m_candidate_objects.end(), related_object) != m_candidate_objects.end()) {  // If it's in the candidate list
	  if (std::find(m_passed_objects.begin(), m_passed_objects.end(), related_object) == m_passed_objects.end()) { // And it hasn't already been added to the "passed" list
	    std::cout << "CONNECT " << object.UID() << " to " << related_object.UID() << "\n";

	    find_objects_and_connections(level, related_object);
	    enhanced_object.related_object_names.push_back(related_object.UID());
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

	std::vector<std::string> allowed_conns {};

	if (level == ObjectKind::kSession || level == ObjectKind::kSegment) {
	  allowed_conns = {"NetworkConnection"};
	} else if (level == ObjectKind::kApplication || level == ObjectKind::kModule){
	  allowed_conns = {"NetworkConnection", "Queue", "QueueWithSourceId"};
	}

	for (const auto& module : modules) {

	  for (auto in : module->get_inputs()) {

	    // Elsewhere in the code it'll be useful to know if the
	    // connection is a network or a queue, so include the
	    // class name in the std::string key

	    const std::string key = in->config_object().UID() + "@" + in->config_object().class_name();

	    if (std::ranges::find(allowed_conns, in->config_object().class_name()) != allowed_conns.end()) {
	      m_incoming_connections[key].push_back( object.UID() );
	      m_incoming_connections[key].push_back( module->UID() );
	    }
	  }

	  for (auto out : module->get_outputs()) {

	    const std::string key = out->config_object().UID() + "@" + out->config_object().class_name();

	    if (std::ranges::find(allowed_conns, out->config_object().class_name()) != allowed_conns.end()) {
	      m_outgoing_connections[key].push_back( object.UID() );
	      m_outgoing_connections[key].push_back( module->UID() );
	    }
	  }

	  if (std::ranges::find(m_included_classes[level], "Module") != m_included_classes[level].end()) {
	    find_objects_and_connections(level, module->config_object());
	    enhanced_object.related_object_names.push_back(module->UID());
	  }
	}
      }
    }

    if (std::find(m_passed_objects.begin(), m_passed_objects.end(), object) == m_passed_objects.end()) {
      m_passed_objects.push_back( object );
      m_objects_for_graph.insert( {object.UID(), enhanced_object}  );
    } else {
      assert(false);
    }
  }

  void GraphBuilder::construct_graph(const ObjectKind level, const std::string& obj_uid) {
    calculate_graph(level, obj_uid);
    
    for (auto& enhanced_object : m_objects_for_graph | std::views::values) {

      auto& obj = enhanced_object.config_object;
      
      enhanced_object.vertex_in_graph = boost::add_vertex( VertexLabel(obj.UID(), obj.class_name()), m_graph);
    }

    for (auto& parent_obj : m_objects_for_graph | std::views::values) {
      for (auto& child_obj_name : parent_obj.related_object_names) {
	boost::add_edge(parent_obj.vertex_in_graph,
			m_objects_for_graph.at(child_obj_name).vertex_in_graph,
			{""}, // No label for an edge which just describes ownership
			m_graph);
      }
    }

    for (auto& possible_sender_object : m_objects_for_graph | std::views::values) {
      for (auto receiver_info : possible_sender_object.receiving_object_infos) {

	auto at_pos = receiver_info.connection_name.find("@");
	auto uid = receiver_info.connection_name.substr(0, at_pos);
	auto classname = receiver_info.connection_name.substr(at_pos + 1);

	std::string connection_label {""};

	//	if (classname == "NetworkConnection") {
	if (true) {
	  connection_label = uid;
	}

	// If we're plotting at the level of a session or segment,
	// show the connections as between applications; if we're
	// doing this for a single application, show them entering and
	// exiting the individual modules

	if (level == ObjectKind::kSession || level == ObjectKind::kSegment) {
	  if (m_objects_for_graph.at(receiver_info.receiver_label).kind == ObjectKind::kModule) {
	    continue;
	  }
	}

	if (level == ObjectKind::kApplication || level == ObjectKind::kModule) {
	  if (m_objects_for_graph.at(receiver_info.receiver_label).kind == ObjectKind::kApplication) {
	    continue;
	  }
	}

	boost::add_edge(
			possible_sender_object.vertex_in_graph,
			m_objects_for_graph.at(receiver_info.receiver_label).vertex_in_graph,
			{ connection_label },
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

    std::stringstream outputstream;

    boost::write_graphviz(outputstream,
			  m_graph,
			  boost::make_label_writer(boost::get(&GraphBuilder::VertexLabel::displaylabel, m_graph)),
			  boost::make_label_writer(boost::get(&GraphBuilder::EdgeLabel::displaylabel, m_graph))
			  );

    // It's arguably hacky to edit the DOT code generated by
    // boost::write_graphviz after the fact to give vertices colors,
    // but the fact is that the color-assigning logic in Boost's graph
    // library is so messy and clumsy that this is a worthwhile
    // tradeoff

    std::string dotfilestring = outputstream.str();

    for (auto& eo : m_objects_for_graph | std::views::values) {
      if (eo.kind == ObjectKind::kApplication) {
	std::stringstream labelstringstr;
	labelstringstr << "label=\"" << eo.config_object.UID() << "\n";

	auto insert_point = dotfilestring.find(labelstringstr.str());
	if (insert_point != std::string::npos) {
	  dotfilestring.insert(insert_point, "color=red, fontcolor=red, ");
	} else {
	  assert(false);
	}
      }
    }

    // Take advantage of the fact that the edges describing ownership
    // rather than data flow (e.g., hsi-segment owning hsi-01, the
    // FakeHSIApplication) have null labels in order to turn them into
    // arrow-free dotted lines
    
    std::string unlabeled_edge = "label=\"\"";
    std::string edge_modifier = ", style=\"dotted\", arrowhead=\"none\"";

    size_t pos = 0;
    while ((pos = dotfilestring.find(unlabeled_edge, pos)) != std::string::npos) {
      dotfilestring.replace(pos, unlabeled_edge.length(), unlabeled_edge + edge_modifier);
      pos += (unlabeled_edge + edge_modifier).length();
    }

    std::ofstream outputfile;
    outputfile.open( outputfilename );
    
    if ( outputfile.is_open() ) {
      outputfile << dotfilestring.c_str();
    } else {
      std::stringstream errmsg;
      errmsg << "Unable to open requested file \"" << outputfilename << "\" for writing";
      throw dbe::GeneralGraphToolError(ERS_HERE, errmsg.str());
    }
  };

  
} // namespace dbe

