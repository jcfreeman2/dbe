/*
 * config_api_graph.cpp
 *
 *  Created on: Apr 20, 2016
 *      Author: Leonidas Georgopoulos
 */

#include "config_api_graph.h"
#include "config_api_info.h"
#include "confobject_desc.hpp"
#include "dbcontroller.h"
#include "messenger.h"

#include "config/Schema.hpp"

#include <algorithm>

//------------------------------------------------------------------------------------------
//                                      DBE::CONFIG::API::GRAPH NAMESPACE
//------------------------------------------------------------------------------------------
namespace dbe
{
namespace config
{
namespace api
{
namespace graph
{


//------------------------------------------------------------------------------------------
namespace linked
{
namespace through
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*
 * Objects referenced by the given object through a specific attribute
 */
template<>
tref attribute<tref> ( tref object, daq::config::attribute_t const & attr )
{
  return object.get<tref> ( attr.p_name );
}

template<>
std::vector<tref> attribute<std::vector<tref>> (
                                              tref item, daq::config::attribute_t const & attr )
{
  if ( attr.p_type == daq::config::class_type )
  {
    if ( attr.p_is_multi_value )
    {
      std::vector<tref> adj;
      item.get ( attr.p_name, adj );
      return adj;
    }
    else if ( attr.p_is_not_null )
    {
      try
      {
        return
        { attribute<tref> ( item, attr ) };
      }
      catch ( daq::dbe::config_object_retrieval_result_is_null const & e )
      {
        if ( attr.p_is_not_null )
        {
          WARN (
            "Query returned null value for an attr that has p_is_not_null set true",
            "Program parsing data validation failure", item.full_name(),
            " for attribute ", attr.p_name );
        }
        else
        {
          DEBUG ( "Query returned null value", "Program parsing data validation" );
        }
      }
    }
  }

  return
    {};
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
/*
 * Objects referenced by the given object through a specific relation
 */
template<>
tref relation<tref> ( tref object, daq::config::relationship_t const & r )
{
  return object.get<tref> ( r.p_name );
}

template<>
std::vector<tref> relation<std::vector<tref>> (
                                             tref item, daq::config::relationship_t const & r )
{
  if ( dbe::config::api::info::relation::is_simple ( r ) )
  {
    try
    {
      return
      { through::relation<tref> ( item, r ) };
    }
    catch ( daq::dbe::config_object_retrieval_result_is_null const & e )
    {
      return
        {};
    }
  }
  else
  {
    std::vector<tref> adjacent;
    item.get ( r.p_name, adjacent );
    return adjacent;
  }
}

template<>
QStringList relation<QStringList> (
  tref item, daq::config::relationship_t const & r )
{
  QStringList result;
  std::vector<tref> adjacent = relation<std::vector<tref>> ( item, r );

  for ( auto const & x : adjacent )
  {
    result.push_back ( x.UID().c_str() );
  }

  return result;
}

template<>
std::vector<dbe::config_object_description<std::string>> relation <
                                                      std::vector<dbe::config_object_description<std::string> > > (
                                                        tref object, daq::config::relationship_t const & relation )
{
  std::vector<dbe::config_object_description<std::string>> result;

  std::vector<tref> adjacent = linked::through::relation<std::vector<tref>> ( object,
                                                                              relation );

  for ( auto const & x : adjacent )
  {
    result.push_back ( x );
  }

  return result;
}

//------------------------------------------------------------------------------------------
} // end namespace through
} // end namespace link  

//------------------------------------------------------------------------------------------
template<typename T>
inline T direct::linked ( ConfigObject & input,
                          daq::config::relationship_t const & relation )
{
  T value;
  input.get ( relation.p_name, value );
  return value;
}

template
ConfigObject direct::linked<ConfigObject> (
  ConfigObject &, daq::config::relationship_t const & );

template
std::vector<ConfigObject>
direct::linked<std::vector<ConfigObject>> (
  ConfigObject &, daq::config::relationship_t const & );
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
/*
 * Retrieve objects referenced by an object through any of its relations
 */
template<typename T>
std::vector<T> linked::through::relations ( inner::configobject::tref const & o )
{
  daq::config::class_t classdef
  { info::onclass::definition ( o.class_name(), false ) };

  std::vector<T> voisins;

  for ( daq::config::relationship_t const & r : classdef.p_relationships )
  {
    std::vector<T> a = relation<std::vector<T>> ( o, r );
    voisins.insert ( voisins.end(), a.begin(), a.end() );
  }

  voisins.erase ( std::unique ( std::begin ( voisins ), std::end ( voisins ) ),
                  std::end ( voisins ) );
  return voisins;
}
template std::vector<tref> linked::through::relations<tref> ( tref const & );

/*
 * Retrieve objects linked by an object through any of its attributes
 */
template<typename T>
std::vector<T> linked::through::attributes ( inner::configobject::tref const & o )
{
  daq::config::class_t classdef
  { info::onclass::definition ( o.class_name(), true ) };

  std::vector<T> voisins;

  for ( daq::config::attribute_t const & attr : classdef.p_attributes )
  {
    std::vector<tref> a = attribute<std::vector<T>> ( o, attr );
    voisins.insert ( voisins.end(), a.begin(), a.end() );
  }

  voisins.erase ( std::unique ( std::begin ( voisins ), std::end ( voisins ) ),
                  std::end ( voisins ) );
  return voisins;
}
template std::vector<tref> linked::through::attributes<tref> ( tref const & );
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
/*
 * Retrieve links by operating directly on a ConfigObject
 */
template<typename T>
inline T direct::linked ( ConfigObject & input, daq::config::attribute_t const & relation )
{
  T value;
  input.get ( relation.p_name, value );
  return value;
}

template<>
inline QStringList direct::linked<QStringList> (
  ConfigObject & input, daq::config::relationship_t const & relation )
{
  QStringList result;
  std::vector<ConfigObject> adjacent;

  if ( info::relation::is_simple ( relation ) )
  {
    adjacent =
    { direct::linked<ConfigObject> ( input, relation ) };
  }
  else
  {
    adjacent = direct::linked<std::vector<ConfigObject>> ( input, relation );
  }

  for ( auto const & x : adjacent )
  {
    result.push_back ( x.UID().c_str() );
  }

  return result;
}

template
ConfigObject direct::linked<ConfigObject> ( ConfigObject &,
                                            daq::config::attribute_t const & );
template
std::vector<ConfigObject>
direct::linked<std::vector<ConfigObject>> ( ConfigObject &,
                                            daq::config::attribute_t const & );
template
QStringList direct::linked<QStringList> (
  ConfigObject &, daq::config::relationship_t const & );
//------------------------------------------------------------------------------------------


template
tref linked::through::attribute<tref> (
  inner::configobject::tref, daq::config::attribute_t const & );
template
std::vector<tref> linked::through::attribute<std::vector<tref>> (
                                                               inner::configobject::tref, daq::config::attribute_t const & );
template
tref linked::through::relation<tref> ( tref, daq::config::relationship_t const & );
template
std::vector<tref>
linked::through::relation<std::vector<tref>> ( tref, daq::config::relationship_t const & );
template
QStringList linked::through::relation<QStringList> ( tref,
                                                     daq::config::relationship_t const & );
template
std::vector<dbe::config_object_description<std::string>> linked::through::relation <
std::vector<dbe::config_object_description<std::string> > > (
  dbe::inner::configobject::tref, daq::config::relationship_t const & );
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
namespace linked
{
namespace by
{
/*
 * Get objects that are linked by a given object
 */
template<typename T> std::vector<T> object ( inner::configobject::tref const & o )
{
  std::vector<T> links = through::attributes<T> ( o );
  std::vector<T> rlinks = through::relations<T> ( o );

  if ( not rlinks.empty() )
  {
    links.insert ( links.end(), rlinks.begin(), rlinks.end() );
  }

  links.erase ( std::unique ( links.begin(), links.end() ), links.end() );

  return links;
}

} // end namespace by
} // end namespace link
template
std::vector<inner::configobject::tref>
linked::by::object<inner::configobject::tref> ( inner::configobject::tref const & );
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
namespace linked
{
namespace to
{

/*
 * Get objects which link to a given object
 */
template<>
std::vector<inner::configobject::tref> object<inner::configobject::tref> (
  inner::configobject::tref const & o )
{
  // Use with caution ... takes for ever to complete due to the underlying
  // ConfigObject implementation
  return o.referenced_by ( "*", false );
}

} // end namespace to
} // end namespace link
template
std::vector<inner::configobject::tref>
linked::to::object<inner::configobject::tref> ( inner::configobject::tref const & );
//------------------------------------------------------------------------------------------
}
}
}
}
//------------------------------------------------------------------------------------------

