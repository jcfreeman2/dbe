#ifndef DBE_APPS_GRAPHBUILDER_HPP_
#define DBE_APPS_GRAPHBUILDER_HPP_

#include "conffwk/ConfigObject.hpp"

#include "confmodel/Session.hpp"
#include "ers/ers.hpp"

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/labeled_graph.hpp"

#include <string>
#include <vector>

namespace dbe {

  class GraphBuilder {

  public:

    using ConfigObject = dunedaq::conffwk::ConfigObject;

    struct VertexLabel;
    struct EdgeLabel;

    // Switching container type to boost::listS seems to cause
    // compilation problems with the boost::write_graphviz function...
    using Graph_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexLabel, EdgeLabel>;
    using Edge_t = boost::graph_traits<Graph_t>::edge_descriptor;
    using Vertex_t = boost::graph_traits<Graph_t>::vertex_descriptor;

    enum class ObjectKind {
      kSession = 0,
      kSegment,
      kApplication,
      kModule
    };

    struct VertexLabel {

      VertexLabel() = default;
      
      VertexLabel(std::string uid_arg, std::string classname_arg) :
	uid(uid_arg),
	classname(classname_arg),
	label(uid_arg + "@" + classname_arg),
	displaylabel(uid_arg + "\n" + classname_arg)
      {}

      std::string uid;
      std::string classname;
      std::string label;
      std::string displaylabel;
    };

    struct EdgeLabel {
      std::string displaylabel {"undefined"};
    };

    struct EnhancedObject {

      EnhancedObject(const ConfigObject& config_object_arg, ObjectKind kind_arg) :
	config_object {config_object_arg},
	kind {kind_arg}
      {}

      ConfigObject config_object;
      ObjectKind kind;

      Vertex_t vertex_in_graph;

      // What objects does this one own?
      std::vector<std::string> related_object_names;

      // What objects does this one send data to, and what's their connection called?
      struct ReceivingInfo {
	std::string connection_name;
	std::string receiver_label;

	bool operator==(const ReceivingInfo& other) const {
	  return connection_name == other.connection_name && receiver_label == other.receiver_label;
	}
      };

      std::vector<ReceivingInfo> receiving_object_infos; 
    };

    explicit GraphBuilder(const std::string& oksfilename);

    void construct_graph(const ObjectKind level, const std::string& root_obj_uid);
    void write_graph(const std::string& outputfilename) const;
    
    GraphBuilder(const GraphBuilder&) = delete;
    GraphBuilder(GraphBuilder&&) = delete;
    GraphBuilder& operator=(const GraphBuilder&) = delete;
    GraphBuilder& operator=(GraphBuilder&&) = delete;

  private:

    void find_candidate_objects(const ObjectKind level);
    std::vector<dunedaq::conffwk::ConfigObject> find_related_objects(const ConfigObject& starting_obj);
    void calculate_graph(const ObjectKind level, const std::string& root_obj_uid);
    
    void find_objects_and_connections(const ObjectKind level, const ConfigObject& object);
    void calculate_network_connections();

    const std::string m_oksfilename;
    dunedaq::conffwk::Configuration* m_confdb;

    std::map<ObjectKind, std::vector<std::string>> m_included_classes;
    std::vector<std::string> m_ignored_application_uids;
    
    std::vector<ConfigObject> m_all_objects;
    std::vector<ConfigObject> m_candidate_objects;
    std::vector<ConfigObject> m_passed_objects;

    std::unordered_map<std::string, EnhancedObject> m_objects_for_graph;

    std::unordered_map<std::string, std::vector<std::string>> m_incoming_connections;
    std::unordered_map<std::string, std::vector<std::string>> m_outgoing_connections;
    
    Graph_t m_graph;
    
    dunedaq::confmodel::Session* m_session;
    std::string m_session_name;

  };
} // namespace dbe


ERS_DECLARE_ISSUE(dbe,                 
                  GeneralGraphToolError,
                  "A graph tool error occured: " << errmsg,
                  ((std::string)errmsg)
)


#endif // DBE_APPS_GRAPHBUILDER_HPP_
