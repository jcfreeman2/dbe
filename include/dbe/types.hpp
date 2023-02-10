/*
 * types.h
 *
 *  Created on: Nov 3, 2015
 *      Author: lgeorgop
 */

#ifndef DBE_TYPES_H_
#define DBE_TYPES_H_

#include <mutex>

namespace dbe
{
namespace types
{
namespace common
{

typedef std::mutex type_mutex;
typedef std::lock_guard<type_mutex> type_lock;

}
}
}

#endif /* DBE_TYPES_H_ */
