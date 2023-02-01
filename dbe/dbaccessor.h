/*
 * dbaccessor.h
 *
 *  Created on: Feb 10, 2016
 *      Author: Leonidas Georgopoulos
 */

#ifndef DBE_DBACCESSOR_H_
#define DBE_DBACCESSOR_H_

#include "cptr.hpp"
#include "types.h"

#include <config/Configuration.h>
#include <string>

namespace dbe
{

namespace config
{
namespace api
{
struct rwdacc;

namespace info
{
class onclass;
}

namespace get
{
class file;
}

}
} //namespace config

//------------------------------------------------------------------------------------------

class dbholder
{
  typedef dbe::types::common::type_mutex t_mutex;
  typedef dbe::types::common::type_lock t_lock;

  static Configuration * database;
  static cptr<Configuration> database_concurrent_ptr;
  static dbe::types::common::type_mutex database_lock;

  friend class confaccessor;
  friend class dbaccessor;
};

class dbaccessor
{
  typedef dbholder::t_mutex t_mutex;
  typedef dbholder::t_lock t_lock;

  /**
   * Get a thread safe pointer to the configuration database
   * @return
   */
  static cptr<Configuration> dbptr();

  static bool is_loaded();

  friend class confaccessor;

  friend class dbe::config::api::rwdacc;

  friend class dbe::config::api::get::file;

  friend class dbe::config::api::info::onclass;

};
//------------------------------------------------------------------------------------------
}// end namespace dbe
#endif /* DBE_DBACCESSOR_H_ */
