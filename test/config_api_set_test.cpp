/*
 * config_api_test.cpp
 *
 *  Created on: Nov 2, 2015
 *      Author: Leonidas Georgopoulos
 */

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "dbe_test_defs.hpp"
#include "config_api_info.h"
#include "config_api_set.h"
#include "config_api.hpp"
#include "confaccessor.h"

#include <boost/test/unit_test.hpp>

#include <algorithm>

namespace dbe
{
namespace config
{
namespace test
{
//------------------------------------------------------------------------------------------
struct class_facilities_fix:	dbe::test::oksfix
{
		std::string const aclass = "IPCServiceApplication";
		std::string const object_to_modify = "CHIP";
		std::string const relation_to_modify = "InitializationDependsFrom";

		class_facilities_fix()
		{
			confaccessor::setdbinfo(QString::fromStdString(cdbpath + fn), dbtype);
			confaccessor::load();
		}
};

BOOST_FIXTURE_TEST_SUITE(info_class_facilities_suite, class_facilities_fix)

BOOST_AUTO_TEST_CASE(unset_multi_relation)
{
	BOOST_TEST_MESSAGE("=====================================================");
	BOOST_TEST_MESSAGE("OKS DB required at :");
	BOOST_TEST_MESSAGE(cdbpath.c_str());
	BOOST_TEST_MESSAGE("=====================================================");

	BOOST_CHECK(confaccessor::is_database_loaded());
	{

		using namespace dbe::config::api;

		// Returns an empty object
		dunedaq::config::class_t definition =
		{ info::onclass::definition(aclass, false) };

		dbe::tref oref = dbe::inner::dbcontroller::get({object_to_modify,aclass});

		std::vector<dbe::cokey> empty {};
		
		dunedaq::config::relationship_t relation = info::relation::match(relation_to_modify, aclass);
		BOOST_CHECK_EQUAL(relation_to_modify,relation.p_name );
		BOOST_CHECK_NO_THROW(set::noactions::relation(oref, relation , empty));
	}
}

BOOST_AUTO_TEST_SUITE_END()
//------------------------------------------------------------------------------------------

}/* namespace test */
} /* namespace config */
} /* namespace dbe */
