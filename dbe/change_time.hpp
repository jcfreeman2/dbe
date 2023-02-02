/*
 * change_time.hpp
 *
 *  Created on: Nov 10, 2015
 *      Author: lgeorgop
 */

#ifndef DBE_CHANGE_TIME_HPP_
#define DBE_CHANGE_TIME_HPP_

#include "config_api_set.h"
#include "Command.h"
#include "Exceptions.h"
#include "confaccessor.h"

#include "config/ConfigObject.hpp"
#include "config/Errors.hpp"
#include "config/Schema.hpp"

#include <QObject>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

#include <string>

namespace dbe
{
namespace actions
{
//------------------------------------------------------------------------------------------
template<typename T>
class ChangeTime:
  public onobject
{
public:
  ChangeTime ( tref Object, daq::config::attribute_t AttributeData, T NewValueData,
               QUndoCommand * parent = nullptr );
  void undo();
  void redo();

private:
  T OldValue;
  T NewValue;
  daq::config::attribute_t Attribute;
  bool Success;
};

template<typename T>
ChangeTime<T>::ChangeTime ( tref Object, daq::config::attribute_t AttributeData,
                            T NewValueData, QUndoCommand * parent )
  : onobject ( Object, parent ),
    NewValue ( NewValueData ),
    Attribute ( AttributeData ),
    Success ( true )
{
  try
  {
    QStringList Data
    { dbe::config::api::get::attribute::list<QStringList> ( Object, Attribute ) };
    OldValue = convert::to<T> ( Data );
  }
  catch ( daq::config::Exception const & )
  {
  }

  setText (
    QObject::tr ( "Attribute %1 of object %2 was updated." ).arg (
      AttributeData.p_name.c_str() ).arg ( Object.UID().c_str() ) );
}

template<typename T>
void ChangeTime<T>::undo()
{
  try
  {
    if ( undoable() )
    {
      failed();
      dbe::config::api::set::noactions::atime ( this->checkedref(), Attribute, OldValue );
      toggle();
    }
  }
  catch ( daq::config::Exception const & e )
  {
    Success = false;
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, e );
  }
  catch ( daq::dbe::config_object_retrieval_result_is_null const & ex )
  {
    FAIL ( "Operation did not complete because a lookup in the underlying database failed",
           ex.what() );
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, ex );
  }

}

template<typename T>
void ChangeTime<T>::redo()
{
  try
  {
    if ( redoable() )
    {
      failed();

      QStringList Data
      { dbe::config::api::get::attribute::list<QStringList> ( this->checkedref(), Attribute ) };

      OldValue = convert::to<T> ( Data );

      dbe::config::api::set::noactions::atime ( this->checkedref(), Attribute, NewValue );
      toggle();
    }
  }
  catch ( daq::config::Exception const & e )
  {
    Success = false;
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, e );
  }
  catch ( daq::dbe::config_object_retrieval_result_is_null const & ex )
  {
    FAIL ( "Operation did not complete because a lookup in the underlying database failed",
           ex.what() );
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, ex );
  }

}
//------------------------------------------------------------------------------------------
}
}
// namespace dbe
#endif /* DBE_CHANGE_TIME_HPP_ */
