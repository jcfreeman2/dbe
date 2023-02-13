/*
 * dbe_test_defs.hpp
 *
 *  Created on: 25 May 2016
 *      Author: Leonidas Georgopoulos
 */

#ifndef DBE_TEST_DEFS_HPP_
#define DBE_TEST_DEFS_HPP_

#include "dbe/dbinfo.hpp"
#include <unistd.h>
#include <string>
#include <cstdlib>

namespace dbe {
namespace test {

/**
 * Base class for test fixes defines oks database location for using in unit tests
 */
struct oksfix {
        const ::dbe::dbinfo dbtype = ::dbe::dbinfo::oks;
        const std::string fn = "dbe_unittest.data.xml";
        const std::string cdbpath = std::string(getenv("DBE_SHARE")) + "/schema/dbe/";
};

}
}

#endif /* DBE_TEST_DEFS_HPP_ */
