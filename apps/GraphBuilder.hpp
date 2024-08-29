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
    using Graph_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexLabel>;
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


      bool operator>(const VertexLabel& other) const {
        if (uid > other.uid) {
	  return true;
        } else if (uid == other.uid) {
	  return classname > other.classname;
        }
        return false;
      }

      bool operator<(const VertexLabel& other) const {
        return other > *this;
      }

      bool operator==(const VertexLabel& other) const {
	return !(*this < other) && !(other < *this); // !(a < b) && !(b < a) is equivalent to a == b
      }

      std::string uid;
      std::string classname;
      std::string label;
      std::string displaylabel;
    };
    
    struct EnhancedObject {

      EnhancedObject(const ConfigObject& config_object_arg, ObjectKind kind_arg, int level_arg) :
	config_object {config_object_arg},
	kind {kind_arg},
	level {level_arg}
      {}
      
      const ConfigObject config_object;
      const ObjectKind kind;
      const int level; // How deep in the graph should this object be?

      Vertex_t vertex_in_graph;
      
      // What objects does this one own?
      std::vector<size_t> related_object_indices;

      // What objects does this one send data to?
      std::vector<size_t> receiving_object_indices; 

    };

    explicit GraphBuilder(const std::string& oksfilename);

    void calculate_graph(const ObjectKind level, const std::string& root_obj_uid);
    void construct_graph(const ObjectKind level, const std::string& root_obj_uid);
    
    Graph_t get_graph() const { return m_graph; };
    
    GraphBuilder(const GraphBuilder&) = delete;
    GraphBuilder(GraphBuilder&&) = delete;
    GraphBuilder& operator=(const GraphBuilder&) = delete;
    GraphBuilder& operator=(GraphBuilder&&) = delete;

  private:

    void find_candidate_objects(const ObjectKind level);
    std::vector<dunedaq::conffwk::ConfigObject> find_related_objects(const ConfigObject& starting_obj);

    size_t find_objects_and_connections(const ConfigObject& object, int level);

    const std::string m_oksfilename;
    dunedaq::conffwk::Configuration* m_confdb {nullptr};

    std::map<ObjectKind, std::vector<std::string>> m_included_classes;
    std::vector<std::string> m_ignored_application_uids;
    
    std::vector<ConfigObject> m_all_objects;
    std::vector<ConfigObject> m_candidate_objects;
    std::vector<ConfigObject> m_passed_objects;

    std::vector<EnhancedObject> m_objects_for_graph;

    std::unordered_map<std::string, std::vector<std::string>> m_incoming_network_connections;
    std::unordered_map<std::string, std::vector<std::string>> m_outgoing_network_connections;
    
    Graph_t m_graph;
    
    dunedaq::confmodel::Session* m_session {nullptr};
    std::string m_session_name;
  };

  void write_graph(const GraphBuilder::Graph_t& graph, const std::string& outputfilename = "");
  
} // namespace dbe


ERS_DECLARE_ISSUE(dbe,                 
                  GeneralGraphToolError,
                  "A graph tool error occured: " << errmsg,
                  ((std::string)errmsg)
)


#endif // DBE_APPS_GRAPHBUILDER_HPP_
