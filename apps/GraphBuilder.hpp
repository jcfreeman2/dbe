#ifndef DBE_APPS_GRAPHBUILDER_HPP_
#define DBE_APPS_GRAPHBUILDER_HPP_

#include "ers/ers.hpp"

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
    
    explicit GraphBuilder(const std::string& oksfilename);

    void construct_graph(const TopGraphLevel level);
    
    GraphBuilder(const GraphBuilder&) = delete;
    GraphBuilder(GraphBuilder&&) = delete;
    GraphBuilder& operator=(const GraphBuilder&) = delete;
    GraphBuilder& operator=(GraphBuilder&&) = delete;

  private:

    const std::string m_oksfilename;
    std::map<TopGraphLevel, std::vector<std::string>> m_included_classes;
    
  };

} // namespace dbe


ERS_DECLARE_ISSUE(dbe,                 
                  GeneralGraphToolError,
                  "A graph tool error occured: " << errmsg,
                  ((std::string)errmsg)
)

#endif // DBE_APPS_GRAPHBUILDER_HPP_
