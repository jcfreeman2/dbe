/*
 * config_api.hpp
 *
 *  Created on: Nov 9, 2015
 *      Author: lgeorgop
 */

#ifndef DBE_CONFIG_API_HPP_
#define DBE_CONFIG_API_HPP_

#include "config_api_set.h"
#include "config_api_get.h"
#include "config_api_info.h"
#include "dbaccessor.h"
#include "confaccessor.h"
#include "change_attribute.hpp"
#include "messenger.h"

#include <algorithm>

namespace dbe
{
namespace config
{
namespace api
{
namespace info
{

template<typename T> inline T onclass::allnames()
{
  typedef ::config::fmap<::config::fset> type_cmap;
  type_cmap const & baseclasses = dbaccessor::dbptr()->superclasses();

  T config_class_list;

  std::transform ( baseclasses.begin(), baseclasses.end(),
                   std::back_inserter ( config_class_list ),
                   [] ( type_cmap::value_type const & element )
  {
    return element.first->c_str();
  } );

  if ( not config_class_list.empty() )
  {
    std::sort ( std::begin ( config_class_list ), std::end ( config_class_list ) );
  }

  return config_class_list;
}
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
namespace get
{
template<typename T> inline std::vector<std::string> attribute::read (
  inner::configobject::tref obj, daq::config::attribute_t const & attr )
{

  if ( attr.p_is_multi_value )
  {
    std::vector<T> values;
    obj.get ( attr.p_name, values );

    std::vector<std::string> result;

    if ( attr.p_int_format == daq::config::int_format_t::na_int_format )
    {
	  result.assign(values.size(), "");
      std::transform ( values.begin(), values.end(), result.begin(),
                       ( std::string ( * ) ( T const & ) ) convert::valtostr<T> );
    }
    else
    {
      for ( T i : values )
      {
        result.push_back ( convert::valtostr ( i, attr.p_int_format ) );
      }
    }

    return result;
  }
  else
  {
    T value;
    obj.get ( attr.p_name, value );

    if ( attr.p_int_format == daq::config::int_format_t::na_int_format )
    {
      return
      { convert::valtostr ( value ) };
    }
    else
    {
      return
      { convert::valtostr ( value, attr.p_int_format ) };
    }
  }
}
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
namespace commands
{
template<class T>
void modobj ( tref Object, daq::config::attribute_t const & AttributeData, T Value )
{
  try
  {
    std::string const description = std::string ( "Attribute " ) + AttributeData.p_name;

    dbe::config_internal_change Change
    {
      dbe::config_internal_change::MODIFIED,
      description,
      Object.UID(),
      Object.class_name(),
      Object.contained_in() };

    confaccessor::get_commands()->push (
      new dbe::actions::ChangeAttribute<T> ( Object, AttributeData, Value ) );
    confaccessor::get_internal_change_stack()->push ( Change );

  }
  catch ( daq::dbe::ObjectChangeWasNotSuccessful const & dbe_err )
  {
    WARN ( "The object attribute could not be changed", dbe::config::errors::parse ( dbe_err ).c_str() );
  }
  catch ( daq::config::Exception const & e )
  {
    WARN ( "Change Attribute: The attribute could not be changed",
           dbe::config::errors::parse ( e ).c_str() );
  }
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
template<>
void modobj<std::vector<std::string>> ( tref Object,
                                        daq::config::attribute_t const & AttributeData,
                                        std::vector<std::string> Value );
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
template<class T>
void set::noactions::attribute ( tref Object,
                                 daq::config::attribute_t const & AttributeData,
                                 T NewValueData,
                                 bool NotEmit )
{
  if ( not confaccessor::enabled() )
  {
    throw daq::dbe::ChangeNotAllowed ( ERS_HERE );
  }

  try
  {
    Object.set_by_ref ( AttributeData.p_name, NewValueData );

    if ( !NotEmit )
    {
      confaccessor::ref().force_emit_object_changed ( "", Object );
    }
  }
  catch ( daq::config::Exception const & error )
  {
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, error );
  }
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
template<typename T>
void set::noactions::anenum ( tref Object, daq::config::attribute_t const & AttributeData,
                              T NewValueData,
                              bool NotEmit )
{
  if ( not confaccessor::enabled() )
  {
    throw daq::dbe::ChangeNotAllowed ( ERS_HERE );
  }

  try
  {
    Object.set_enum ( AttributeData.p_name, NewValueData );

    if ( !NotEmit )
    {
      confaccessor::ref().force_emit_object_changed ( "", Object );
    }
  }
  catch ( daq::config::Exception const & error )
  {
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, error );
  }
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
template<typename T>
void set::noactions::aclass ( tref Object, daq::config::attribute_t const & AttributeData,
                              T NewValueData,
                              bool NotEmit )
{
  if ( not confaccessor::enabled() )
  {
    throw daq::dbe::ChangeNotAllowed ( ERS_HERE );
  }

  try
  {
    Object.set_class ( AttributeData.p_name, NewValueData );

    if ( !NotEmit )
    {
      confaccessor::ref().force_emit_object_changed ( "", Object );
    }
  }
  catch ( daq::config::Exception const & error )
  {
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, error );
  }
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
template<typename T>
void set::noactions::adate ( tref Object, daq::config::attribute_t const & AttributeData,
                             T NewValueData,
                             bool NotEmit )
{
  if ( not confaccessor::enabled() )
  {
    throw daq::dbe::ChangeNotAllowed ( ERS_HERE );
  }

  try
  {
    Object.set_date ( AttributeData.p_name, NewValueData );

    if ( !NotEmit )
    {
      confaccessor::ref().force_emit_object_changed ( "", Object );
    }

  }
  catch ( daq::config::Exception const & error )
  {
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, error );
  }
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
template<typename T>
void set::noactions::atime ( tref Object, daq::config::attribute_t const & AttributeData,
                             T NewValueData,
                             bool NotEmit )
{
  if ( not confaccessor::enabled() )
  {
    throw daq::dbe::ChangeNotAllowed ( ERS_HERE );
  }

  try
  {
    Object.set_time ( AttributeData.p_name, NewValueData );

    if ( !NotEmit )
    {
      confaccessor::ref().force_emit_object_changed ( "", Object );
    }
  }
  catch ( daq::config::Exception const & error )
  {
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, error );
  }
}

}
/* namespace api */
} /* namespace config */
} /* namespace dbe */

#endif /* DBE_CONFIG_API_HPP_ */
