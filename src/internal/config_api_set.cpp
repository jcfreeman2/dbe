/*
 * config_api_set.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: Leonidas Georgopoulos
 */

#include "config_api_set.h"
#include "config_api_info.h"
#include "config_api.hpp"
#include "config_api_commands.h"
#include "config_object_key.hpp"
#include "Conversion.h"
#include "dbcontroller.h"

#include "config/Schema.hpp"

#include <QString>

#include <stdint.h>
#include <sys/types.h>
#include <algorithm>
#include <iterator>

//------------------------------------------------------------------------------------------
// NAMESPACE DBE::CONFIG::API::SET::NOACTIONS
//------------------------------------------------------------------------------------------
namespace dbe
{
namespace config
{
namespace api
{
namespace set
{
namespace noactions
{

void relation ( tref object, daq::config::relationship_t const & arelation,
                std::vector<dbe::cokey> const & keys )
{
  std::vector<dbe::inner::configobject::tref> trefs;

  for ( dbe::cokey const & key : keys )
  {
    trefs.push_back ( dbe::inner::dbcontroller::get ( key ) );
  }

  dbe::config::api::set::noactions::relation ( object, arelation, trefs );
}

void relation ( tref src, daq::config::relationship_t const & edge,
                std::vector<tref> const & targets )
{
  try
  {
    if ( not targets.empty() )
    {
      if ( info::relation::is_simple ( edge ) )
      {
        src.set_obj ( edge.p_name, targets.back() );
      }
      else
      {
        src.set_objs ( edge.p_name, targets );
      }
    }
    else
    {
     	src.set_obj_null( edge.p_name, info::relation::is_simple(edge) );
    }
  }
  catch ( daq::config::Exception const & e )
  {
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, config::errors::parse ( e ) );
  }
}
//------------------------------------------------------------------------------------------
}
}
}
}
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
// NAMESPACE DBE::CONFIG::API::SET
//------------------------------------------------------------------------------------------
namespace dbe
{
namespace config
{
namespace api
{
namespace set
{

//------------------------------------------------------------------------------------------
void relation ( tref src, daq::config::relationship_t const & edge,
                QStringList const & targets )
{
  std::vector<std::string> targetnames;

  for ( QString const & name : targets )
  {
    targetnames.push_back ( name.toStdString() );
  }

  commands::modobj ( src, edge, targetnames );
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
void attribute ( tref Object, daq::config::attribute_t const & attribute_info,
                 QStringList const & values )
{
  switch ( attribute_info.p_type )
  {
  case daq::config::bool_type:
  {
    if ( attribute_info.p_is_multi_value )
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<std::vector<bool>> ( values, attribute_info.p_int_format ) );
    }
    else
    {
      commands::modobj ( Object, attribute_info,
                         convert::to<bool> ( values, attribute_info.p_int_format ) );
    }

    break;
  }

  case daq::config::enum_type:
  case daq::config::date_type:
  case daq::config::time_type:
  case daq::config::string_type:
  case daq::config::class_type:
  {
    if ( attribute_info.p_is_multi_value )
    {
      commands::modobj ( Object, attribute_info,
                         convert::to<std::vector<std::string>> ( values ) );
    }
    else
    {
      commands::modobj ( Object, attribute_info, convert::to<std::string> ( values ) );
    }

    break;
  }

  case daq::config::float_type:
  {
    if ( attribute_info.p_is_multi_value )
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<std::vector<float>> ( values, attribute_info.p_int_format ) );
    }
    else
    {
      commands::modobj ( Object, attribute_info,
                         convert::to<float> ( values, attribute_info.p_int_format ) );
    }

    break;
  }

  case daq::config::double_type:
  {
    if ( attribute_info.p_is_multi_value )
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<std::vector<double>> ( values, attribute_info.p_int_format ) );
    }
    else
    {
      commands::modobj ( Object, attribute_info,
                         convert::to<double> ( values, attribute_info.p_int_format ) );
    }

    break;
  }

  case daq::config::s8_type:
  {
    if ( attribute_info.p_is_multi_value )
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<std::vector<int8_t>> ( values, attribute_info.p_int_format ) );
    }
    else
    {
      commands::modobj ( Object, attribute_info,
                         convert::to<int8_t> ( values, attribute_info.p_int_format ) );
    }

    break;
  }

  case daq::config::s16_type:
  {
    if ( attribute_info.p_is_multi_value )
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<std::vector<int16_t>> ( values, attribute_info.p_int_format ) );
    }
    else
    {
      commands::modobj ( Object, attribute_info,
                         convert::to<int16_t> ( values, attribute_info.p_int_format ) );
    }

    break;
  }

  case daq::config::s32_type:
  {
    if ( attribute_info.p_is_multi_value )
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<std::vector<int32_t>> ( values, attribute_info.p_int_format ) );
    }
    else
    {
      commands::modobj ( Object, attribute_info,
                         convert::to<int32_t> ( values, attribute_info.p_int_format ) );
    }

    break;
  }

  case daq::config::s64_type:
  {
    if ( attribute_info.p_is_multi_value )
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<std::vector<int64_t>> ( values, attribute_info.p_int_format ) );
    }
    else
    {
      commands::modobj ( Object, attribute_info,
                         convert::to<int64_t> ( values, attribute_info.p_int_format ) );
    }

    break;
  }

  case daq::config::u8_type:
  {
    if ( attribute_info.p_is_multi_value )
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<std::vector<u_int8_t>> ( values, attribute_info.p_int_format ) );
    }
    else
    {
      commands::modobj ( Object, attribute_info,
                         convert::to<u_int8_t> ( values, attribute_info.p_int_format ) );
    }

    break;
  }

  case daq::config::u16_type:
  {
    if ( attribute_info.p_is_multi_value )
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<std::vector<u_int16_t>> ( values, attribute_info.p_int_format ) );
    }
    else
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<u_int16_t> ( values, attribute_info.p_int_format ) );
    }

    break;
  }

  case daq::config::u32_type:
  {
    if ( attribute_info.p_is_multi_value )
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<std::vector<u_int32_t>> ( values, attribute_info.p_int_format ) );
    }
    else
    {
      commands::modobj (
        Object, attribute_info,
        convert::to<u_int32_t> ( values, attribute_info.p_int_format ) );
    }

    break;
  }

  case daq::config::u64_type:
  {
    if ( attribute_info.p_is_multi_value ) commands::modobj (
        Object, attribute_info,
        convert::to<std::vector<u_int64_t>> ( values, attribute_info.p_int_format ) );
    else commands::modobj (
        Object, attribute_info,
        convert::to<u_int64_t> ( values, attribute_info.p_int_format ) );

    break;
  }

  default:
    break;
  }
}
//------------------------------------------------------------------------------------------
}
}
}
}
//------------------------------------------------------------------------------------------

