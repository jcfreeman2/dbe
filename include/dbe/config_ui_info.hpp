/*
 * configuration.h
 *
 *  Created on: Mar 22, 2016
 *      Author: Leonidas Georgopoulos
 */

#ifndef DBE_CONFIG_UI_INFO_H_
#define DBE_CONFIG_UI_INFO_H_

#include "dbe/datahandler.hpp"
#include "dbe/GraphicalClass.hpp"
#include "oksdbinterfaces/ConfigObject.hpp"
#include "oksdbinterfaces/Configuration.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace dbe
{
namespace ui
{
namespace config
{

/**
 * Class used to read the current partition configuration in order to retrieve
 * parameter for running the application and other necessary parameters
 */
class info
{
public:
  info ( std::vector<std::string> const & file );

  GraphicalClass graphical ( std::string const & ) const;
  ViewConfiguration view ( std::string const & ) const;
  Window window ( std::string const & ) const;

  std::vector<Window> windows() const;
  std::vector<ViewConfiguration> views() const;
  std::vector<GraphicalClass> graphicals() const;

private:
  void parse();
  void parse_graphical ( std::shared_ptr<Configuration> , ConfigObject & );
  void parse_window ( std::shared_ptr<Configuration> , ConfigObject & );

  std::map<std::string, Window> this_windows;
  std::map<std::string, GraphicalClass> this_graphical;
  std::map<std::string, ViewConfiguration> this_views;

  std::vector<std::string> this_full_filenames;
};
} /* namespace config */
} /* namespace ui */
} /* namespace dbe */

#endif /* DBE_CONFIG_UI_INFO_H_ */
