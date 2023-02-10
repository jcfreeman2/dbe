/*
 * view_common_interface.h
 *
 *  Created on: Nov 17, 2015
 *      Author: lgeorgop
 */

#ifndef DBE_VIEW_COMMON_INTERFACE_H_
#define DBE_VIEW_COMMON_INTERFACE_H_

namespace dbe
{
/*
 * This is an abstract interface for custom views, to provide unified means of accessing them
 */
class view_common_interface
{
public:
  virtual ~view_common_interface() = default;

  virtual void slot_delete_objects() = 0;
  virtual void slot_create_object() = 0;
  virtual void slot_edit_object() = 0;
  virtual void slot_copy_object() = 0;

};

}  // namespace dbe

#endif /* DBE_VIEW_COMMON_INTERFACE_H_ */
