/*
 * change_enum.hpp
 *
 *  Created on: Nov 10, 2015
 *      Author: lgeorgop
 */

#ifndef DBE_CHANGE_ENUM_HPP_
#define DBE_CHANGE_ENUM_HPP_

#include "confaccessor.h"
#include "config_api_get.h"
#include "config_api_set.h"
#include "Command.h"
#include "Exceptions.h"
#include "messenger.h"

#include "config/ConfigObject.hpp"
#include "config/Errors.hpp"
#include "config/Schema.hpp"

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

#include <string>

namespace dbe
{
namespace actions
{
//------------------------------------------------------------------------------------------
template<typename T>
class ChangeEnum:
  public onobject
{
public:
  ChangeEnum ( tref Object, daq::config::attribute_t AttributeData, T NewValueData,
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
ChangeEnum<T>::ChangeEnum ( tref Object, daq::config::attribute_t AttributeData,
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
  catch ( daq::config::Exception const & e )
  {
    Q_UNUSED ( e )
  }

  setText (
    QObject::tr ( "Attribute %1 of object %2 was updated." ).arg (
      AttributeData.p_name.c_str() ).arg ( Object.UID().c_str() ) );
}

template<typename T>
void ChangeEnum<T>::undo()
{
  try
  {
    if ( isvalid() )
    {
      failed();
      dbe::config::api::set::noactions::anenum ( this->checkedref(), Attribute, OldValue );
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
void ChangeEnum<T>::redo()
{
  try
  {
    if ( isvalid() )
    {
      failed();

      QStringList Data
      { dbe::config::api::get::attribute::list<QStringList> ( this->checkedref(), Attribute ) };

      OldValue = convert::to<T> ( Data, Attribute.p_int_format );

      dbe::config::api::set::noactions::anenum ( this->checkedref(), Attribute, NewValue );

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
} // namespace dbe
#endif /* DBE_CHANGE_ENUM_HPP_ */
