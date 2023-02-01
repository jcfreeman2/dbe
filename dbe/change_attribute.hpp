/*
 * change_attribute.hpp
 *
 *  Created on: Nov 10, 2015
 *      Author: lgeorgop
 */

#ifndef DBE_CHANGE_ATTRIBUTE_HPP_
#define DBE_CHANGE_ATTRIBUTE_HPP_

#include "confaccessor.h"
#include "config_api_set.h"
#include "config_api_get.h"
#include "Command.h"
#include "Exceptions.h"

#include <config/ConfigObject.h>
#include <config/Errors.h>
#include <config/Schema.h>

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

#include <string>

#include  "messenger.h"

namespace dbe
{
namespace actions
{

template<typename T>
class ChangeAttribute:
  public onobject
{
public:
  ChangeAttribute ( tref Object, daq::config::attribute_t AttributeData, T NewValueData,
                    QUndoCommand * parent = nullptr );
  void undo();
  void redo();
private:
  T OldValue;
  T NewValue;
  daq::config::attribute_t Attribute;
};

template<typename T>
ChangeAttribute<T>::ChangeAttribute ( tref Object, daq::config::attribute_t AttributeData,
                                      T NewValueData, QUndoCommand * parent )
  : onobject ( Object, parent ),
    NewValue ( NewValueData ),
    Attribute ( AttributeData )
{
  try
  {
    failed();
    QStringList Data
    { dbe::config::api::get::attribute::list<QStringList> ( this->checkedref(), Attribute ) };
    OldValue = convert::to<T> ( Data, Attribute.p_int_format );
    toggle();
  }
  catch ( daq::config::Exception const & )
  {
  }
  catch ( daq::dbe::config_object_retrieval_result_is_null const & ex )
  {
    FAIL ( "Operation did not complete because a lookup in the underlying database failed",
           ex.what() );
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, ex );
  }

  setText (
    QObject::tr ( "Attribute %1 of object %2 was updated." ).arg (
      AttributeData.p_name.c_str() ).arg ( Object.UID().c_str() ) );
}

template<typename T>
void ChangeAttribute<T>::undo()
{
  try
  {
    if ( undoable() )
    {
      failed();
      dbe::config::api::set::noactions::attribute ( this->checkedref(), Attribute, OldValue );
      toggle();
    }
  }
  catch ( daq::config::Exception const & e )
  {
    failed();
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
void ChangeAttribute<T>::redo()
{
  try
  {
    if ( redoable() )
    {
      failed();

      QStringList Data
      { dbe::config::api::get::attribute::list<QStringList> ( this->checkedref(), Attribute ) };

      OldValue = convert::to<T> ( Data, Attribute.p_int_format );

      dbe::config::api::set::noactions::attribute ( this->checkedref(), Attribute, NewValue );
      toggle();
    }
  }
  catch ( daq::config::Exception const & e )
  {
    failed();
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, e );
  }
  catch ( daq::dbe::config_object_retrieval_result_is_null const & ex )
  {
    FAIL ( "Operation did not complete because a lookup in the underlying database failed",
           ex.what() );
    throw daq::dbe::ObjectChangeWasNotSuccessful ( ERS_HERE, ex );
  }

}

}
}
#endif /* DBE_CHANGE_ATTRIBUTE_HPP_ */
