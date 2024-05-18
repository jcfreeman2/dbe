#ifndef DBE_APPS_GRAPHBUILDER_HPP_
#define DBE_APPS_GRAPHBUILDER_HPP_

#include "dbe/config_reference.hpp"
#include "dbe/config_api.hpp"

#include "coredal/Session.hpp"
#include "ers/ers.hpp"

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/labeled_graph.hpp"

#include <string>
#include <vector>

namespace dbe {

  class GraphBuilder {

  public:

    enum class TopGraphLevel {
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
    
    using Graph_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexLabel>;
    using Edge_t = boost::graph_traits<Graph_t>::edge_descriptor;
    using Vertex_t = boost::graph_traits<Graph_t>::vertex_descriptor;
    
    explicit GraphBuilder(const std::string& oksfilename);

    void construct_graph(const TopGraphLevel level);
    void construct_graph(const TopGraphLevel level, const std::string& root_obj_uid);
    
    Graph_t get_graph() const { return m_graph; };
    
    GraphBuilder(const GraphBuilder&) = delete;
    GraphBuilder(GraphBuilder&&) = delete;
    GraphBuilder& operator=(const GraphBuilder&) = delete;
    GraphBuilder& operator=(GraphBuilder&&) = delete;

  private:

    void add_connected_objects(const tref& starting_obj, const Vertex_t& starting_vtx, bool add_edges);
    void find_candidate_objects(const TopGraphLevel level);

    const std::string m_oksfilename;
    std::map<TopGraphLevel, std::vector<std::string>> m_included_classes;

    std::vector<std::string> m_ignored_application_uids;
    std::vector<tref> m_all_objects;
    std::vector<tref> m_candidate_objects;
    std::vector<tref> m_passed_objects;
    
    Graph_t m_graph;

    dunedaq::coredal::Session* m_session {nullptr};
  };

  void write_graph(const GraphBuilder::Graph_t& graph, const std::string& outputfilename = "");
  
} // namespace dbe


ERS_DECLARE_ISSUE(dbe,                 
                  GeneralGraphToolError,
                  "A graph tool error occured: " << errmsg,
                  ((std::string)errmsg)
)


#endif // DBE_APPS_GRAPHBUILDER_HPP_
