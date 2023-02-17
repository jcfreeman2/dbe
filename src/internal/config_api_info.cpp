/*
 * config_api_info.cpp
 *
 *  Created on: Apr 20, 2016
 *      Author: Leonidas Georgopoulos
 */

#include "dbe/config_api_info.hpp"
#include "dbe/config_api.hpp"
#include "dbe/treenode.hpp"
#include "dbe/Sorting.hpp"
#include "oksdbinterfaces/Schema.hpp"
#include <QString>
//------------------------------------------------------------------------------------------
//                                    DBE::CONFIG::API::INFO NAMESPACE
//------------------------------------------------------------------------------------------
/*
 * This namespace groups methods for providing information about config related
 * structures and objects
 */
namespace dbe
{
namespace config
{
namespace api
{
namespace info
{
//------------------------------------------------------------------------------------------
template<>
std::vector<dbe::tref> onclass::objects<true> ( std::string const & cname,
                                                bool const keep_inherited )
{
  std::vector<dbe::tref> objectrefs = inner::dbcontroller::gets ( cname );

  if ( not keep_inherited )
  {
    objectrefs.erase (
      std::remove_if ( objectrefs.begin(), objectrefs.end(),
                       [&cname] ( dbe::tref anobject )
    {
      return ( anobject.class_name() != cname );
    } ),
    objectrefs.end() );
  }

  std::sort ( objectrefs.begin(), objectrefs.end(), SortObjects() );

  return objectrefs;
}
template std::vector<dbe::tref> onclass::objects<true> ( std::string const &, bool const );

template<>
std::vector<dbe::tref> onclass::objects<false> ( std::string const & cname,
                                                 bool const keep_inherited )
{
  std::vector<dbe::tref> objectrefs = inner::dbcontroller::gets ( cname );

  if ( not keep_inherited )
  {
    objectrefs.erase (
      std::remove_if ( objectrefs.begin(), objectrefs.end(),
                       [&cname] ( dbe::tref anobject )
    {
      return ( anobject.class_name() != cname );
    } ),
    objectrefs.end() );
  }

  return objectrefs;
}
template std::vector<dbe::tref> onclass::objects<false> ( std::string const &, bool const );
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
bool onclass::derived ( std::string const & fromclass, std::string const & aclass )
{
  dunedaq::oksdbinterfaces::class_t aclassdef
  { dbe::config::api::info::onclass::definition ( aclass, false ) };

  for ( std::string const & x : aclassdef.p_superclasses )
  {
    if ( fromclass == x )
    {
      return true;
    }
  }

  return false;
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
bool has_obj ( std::string const & classname, std::string const & object_uid )
{
  try
  {
    return not inner::dbcontroller::get (
    { object_uid, classname} ).is_null();
  }
  catch ( daq::dbe::config_object_retrieval_result_is_null const & e )
  {
    return false;
  }

  return true;
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
dunedaq::oksdbinterfaces::class_t onclass::definition ( const std::string & cn, bool direct_only )
{
  try
  {
    return dbaccessor::dbptr()->get_class_info ( cn, direct_only );
  }
  catch ( dunedaq::oksdbinterfaces::NotFound const & Ex )
  {
    return dunedaq::oksdbinterfaces::class_t();
  }
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
dunedaq::oksdbinterfaces::attribute_t attributematch ( QString const & AttributeName,
                                          QString const & ClassName )
{
  const dunedaq::oksdbinterfaces::class_t & ClassInfo = dbe::config::api::info::onclass::definition (
                                             ClassName.toStdString(), false );
  const std::vector<dunedaq::oksdbinterfaces::attribute_t> AttributeList = ClassInfo.p_attributes;

  for ( auto & Attribute : AttributeList )
  {
    if ( Attribute.p_name == AttributeName.toStdString() )
    {
      return Attribute;
    }
  }

  return dunedaq::oksdbinterfaces::attribute_t();
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
bool relation::is_simple ( dunedaq::oksdbinterfaces::relationship_t const & relation )
{
  return ( relation.p_cardinality == dunedaq::oksdbinterfaces::only_one )
         or ( relation.p_cardinality == dunedaq::oksdbinterfaces::zero_or_one );
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
template<> dunedaq::oksdbinterfaces::relationship_t info::relation::match<std::string> (
	std::string const & arelation,
	std::string const & aclass )
{
  dunedaq::oksdbinterfaces::class_t const & aninfo_for_class =
  		dbe::config::api::info::onclass::definition ( aclass, false );
  std::vector<dunedaq::oksdbinterfaces::relationship_t> const relations =
  		aninfo_for_class.p_relationships;

  for ( auto & r : relations )
  {
    if ( r.p_name == arelation )
    {
      return r;
    }
  }
  return dunedaq::oksdbinterfaces::relationship_t();
}
template dunedaq::oksdbinterfaces::relationship_t info::relation::match<std::string> (
	std::string const & ,
	std::string const & );
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
template<> dunedaq::oksdbinterfaces::relationship_t info::relation::match<QString>(
	QString const & arelation,
	QString const & aclass )
{
	return match(aclass.toStdString(), arelation.toStdString());
}
template dunedaq::oksdbinterfaces::relationship_t info::relation::match<QString>(
	QString const & ,
	QString const & );
//------------------------------------------------------------------------------------------
}
}
}
}
//------------------------------------------------------------------------------------------
