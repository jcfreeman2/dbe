/************************************************************
 * rwdacc.cpp
 *
 *  Created on: Feb 4, 2016
 *      Author: Leonidas Georgopoulos
 ************************************************************/
#include "dbaccessor.h"
#include "config_direct_access.h"
#include "confobject_desc.hpp"
#include "Exceptions.h"
#include "messenger.h"
#include "config_api_set.h"
#include "Conversion.h"

#include <config/ConfigObject.h>
#include <config/Configuration.h>
#include <config/Errors.h>
#include <config/Schema.h>
#include <ers/LocalContext.h>

#include <QString>
#include <QStringList>

#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace dbe
{

namespace config
{

namespace api
{
//------------------------------------------------------------------------------------------
ConfigObject rwdacc::create_object(std::string const & fn, std::string const & cn,
																		std::string const & uid)
{
	if (info::onclass::definition(cn, false).p_abstract)
	{
		ERROR("Create Object: Was not possible to create object ",
				"Abstract classes cannot hold objects", uid, "for abstract class" , cn);

		throw daq::dbe::ObjectChangeWasNotSuccessful(ERS_HERE);
	}

	// An empty object, if its creation does not succeed this object will be returned
	ConfigObject newobj;

	try
	{
		dbaccessor::dbptr()->create(fn, cn, uid,newobj);
	}
	catch (daq::config::Exception const & ex)
	{
    FAIL ( "Object creation failed", dbe::config::errors::parse ( ex ), "\n\nObject UID:",
           uid );

		throw daq::dbe::ObjectChangeWasNotSuccessful(ERS_HERE);
	}

	return newobj;

}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
/**
 * Define a function to retrieve from a key-val container a values with of KEYTYPE keys and
 * convert them to result type RESTYPE
 */
template<typename RESTYPE, typename KEYTYPE = std::string>
using mapreader = std::function< RESTYPE (KEYTYPE const &) >;
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
tref rwdacc::set_object(tref newobj,
                        dbe::t_config_object_preimage::type_attrmap const & attributes,
                        dbe::t_config_object_preimage::type_relmap const & relations)
{
	auto relreader = [&relations ]( std::string const & name )
	{
		return relations.at(name);
	};

	auto attreader = [ & attributes ] ( std::string const & name )
	{
		return attributes.at(name);
	};

	if (not newobj.is_null())
	{
		daq::config::class_t ClassInfo = dbe::config::api::info::onclass::definition(
		        newobj.class_name(),
				false);
		std::vector<daq::config::attribute_t> AttList = ClassInfo.p_attributes;
		std::vector<daq::config::relationship_t> RelList = ClassInfo.p_relationships;

		for (daq::config::attribute_t const & att : AttList)
		{
			try
			{
			    if(att.p_is_multi_value) {
			        switch(att.p_type) {
			            case daq::config::bool_type:
			            {
			                auto data = dbe::convert::to<std::vector<bool>>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
			            case daq::config::enum_type:
			            {
			                auto data = dbe::convert::to<std::vector<std::string>>(dbe::convert::to<QStringList>(attreader(att.p_name)));
			                set::noactions::anenum(newobj, att, data, true);
			            }
			                break;
			            case daq::config::date_type:
                        {
                            auto data = dbe::convert::to<std::vector<std::string>>(dbe::convert::to<QStringList>(attreader(att.p_name)));
                            set::noactions::adate(newobj, att, data, true);
                        }
                            break;
			            case daq::config::time_type:
                        {
                            auto data = dbe::convert::to<std::vector<std::string>>(dbe::convert::to<QStringList>(attreader(att.p_name)));
                            set::noactions::atime(newobj, att, data, true);
                        }
                            break;
			            case daq::config::string_type:
                        {
                            auto data = dbe::convert::to<std::vector<std::string>>(dbe::convert::to<QStringList>(attreader(att.p_name)));
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
			            case daq::config::class_type:
			            {
                            auto data = dbe::convert::to<std::vector<std::string>>(dbe::convert::to<QStringList>(attreader(att.p_name)));
                            set::noactions::aclass(newobj, att, data, true);
			            }
			                break;
			            case daq::config::float_type:
			            {
			                auto data = dbe::convert::to<std::vector<float>>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
			            case daq::config::double_type:
			            {
			                auto data = dbe::convert::to<std::vector<double>>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
			            case daq::config::s8_type:
			            {
			                auto data = dbe::convert::to<std::vector<int8_t>>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
			            case daq::config::s16_type:
			            {
			                auto data = dbe::convert::to<std::vector<int16_t>>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
			            case daq::config::s32_type:
			            {
			                auto data = dbe::convert::to<std::vector<int32_t>>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
			            case daq::config::s64_type:
			            {
			                auto data = dbe::convert::to<std::vector<int64_t>>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
			            case daq::config::u8_type:
			            {
			                auto data = dbe::convert::to<std::vector<u_int8_t>>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
			            case daq::config::u16_type:
			            {
			                auto data = dbe::convert::to<std::vector<u_int16_t>>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
			            case daq::config::u32_type:
			            {
			                auto data = dbe::convert::to<std::vector<u_int32_t>>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
			            case daq::config::u64_type:
			            {
			                auto data = dbe::convert::to<std::vector<u_int64_t>>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
			            default:
			                break;
			        }
			    } else {
			        switch(att.p_type) {
			            case daq::config::bool_type:
			            {
			                auto data = dbe::convert::to<bool>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
			                set::noactions::attribute(newobj, att, data, true);
			            }
			                break;
                        case daq::config::enum_type:
                        {
                            auto data = dbe::convert::to<std::string>(dbe::convert::to<QStringList>(attreader(att.p_name)));
                            set::noactions::anenum(newobj, att, data, true);
                        }
                            break;
                        case daq::config::date_type:
                        {
                            auto data = dbe::convert::to<std::string>(dbe::convert::to<QStringList>(attreader(att.p_name)));
                            set::noactions::adate(newobj, att, data, true);
                        }
                            break;
                        case daq::config::time_type:
                        {
                            auto data = dbe::convert::to<std::string>(dbe::convert::to<QStringList>(attreader(att.p_name)));
                            set::noactions::atime(newobj, att, data, true);
                        }
                            break;
                        case daq::config::string_type:
                        {
                            auto data = dbe::convert::to<std::string>(dbe::convert::to<QStringList>(attreader(att.p_name)));
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
                        case daq::config::class_type:
                        {
                            auto data = dbe::convert::to<std::string>(dbe::convert::to<QStringList>(attreader(att.p_name)));
                            set::noactions::aclass(newobj, att, data, true);
                        }
                            break;
                        case daq::config::float_type:
                        {
                            auto data = dbe::convert::to<float>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
                        case daq::config::double_type:
                        {
                            auto data = dbe::convert::to<double>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
                        case daq::config::s8_type:
                        {
                            auto data = dbe::convert::to<int8_t>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
                        case daq::config::s16_type:
                        {
                            auto data = dbe::convert::to<int16_t>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
                        case daq::config::s32_type:
                        {
                            auto data = dbe::convert::to<int32_t>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
                        case daq::config::s64_type:
                        {
                            auto data = dbe::convert::to<int64_t>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
                        case daq::config::u8_type:
                        {
                            auto data = dbe::convert::to<u_int8_t>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
                        case daq::config::u16_type:
                        {
                            auto data = dbe::convert::to<u_int16_t>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
                        case daq::config::u32_type:
                        {
                            auto data = dbe::convert::to<u_int32_t>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
                        case daq::config::u64_type:
                        {
                            auto data = dbe::convert::to<u_int64_t>(dbe::convert::to<QStringList>(attreader(att.p_name)), att.p_int_format);
                            set::noactions::attribute(newobj, att, data, true);
                        }
                            break;
                        default:
                            break;
			        }
			    }
			}
			catch (std::out_of_range const & e)
			{
				// nothing to do just ignore this , it might be the case that the attribute was not defined
			}
		}

		for (daq::config::relationship_t const & rel : RelList)
		{
			try
			{
				auto data = relreader(rel.p_name);
				set::noactions::relation(newobj, rel, data);
			}
			catch (std::out_of_range const & e)
			{
				// nothing to do just ignore this , it might be the case that the relation was not defined
			}
		}
	}

	return newobj;
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
void rwdacc::destroy_object(ConfigObject & todelete)
{
	try
	{
		dbaccessor::dbptr()->destroy_obj(todelete);
	}
	catch (daq::config::Exception const & ex)
	{
		WARN("Object deletion did not succeed for object" , errors::parse(ex).c_str(),
				todelete.UID().c_str());

		throw daq::dbe::ObjectChangeWasNotSuccessful(ERS_HERE);
	}
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
ConfigObject rwdacc::get_object(std::string const & classname,
																std::string const & objectname)
{
	if (dbaccessor::is_loaded())
	{
		try
		{
			ConfigObject object;
			dbaccessor::dbptr()->get(classname, objectname, object);
			return object;
		}
		catch (daq::config::NotFound const & e)
		{
			ERS_DEBUG(3, "Object not found");
		}
		catch (daq::config::Generic const & e)
		{
			ERROR("Generic exception caught", dbe::config::errors::parse(e).c_str());
			ERS_DEBUG(3, "Generic Exception Caught");
		}
		catch (...)
		{
			ERROR("Unknown exception caught", "");
			ERS_DEBUG(3, "Unknown exception caught!");
		}
	}
	return
	{};
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
std::vector<ConfigObject> rwdacc::query_class(std::string const & classname,
																							std::string const & query)
{
	std::vector<ConfigObject> objects;
	if (dbaccessor::is_loaded())
	{
		try
		{
			dbaccessor::dbptr()->get(classname, objects, query);
		}
		catch (daq::config::NotFound const & e)
		{
			ERS_DEBUG(3, "Object not found");
		}
		catch (daq::config::Generic const & e)
		{
			ERROR("Generic exception caught", dbe::config::errors::parse(e).c_str());
			ERS_DEBUG(3, "Generic Exception Caught");
		}
		catch (...)
		{
			ERROR("Unknown exception caught", "");
			ERS_DEBUG(3, "Unknown exception caught!");
		}
	}
	return objects;
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
void rwdacc::rename_object(ConfigObject & object, std::string const & newname)
{
object.rename(newname);
}
//------------------------------------------------------------------------------------------

}// namespace api

}  // namespace config

}  // namespace dbe
