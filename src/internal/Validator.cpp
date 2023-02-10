/// Including DBE
#include "dbe/Validator.hpp"

dbe::ValidatorAcceptMatch::ValidatorAcceptMatch ( QVariant & Storage, QObject * parent )
  : QValidator ( parent )
{
  List = Storage.toStringList();
}

QValidator::State dbe::ValidatorAcceptMatch::validate ( QString & Input,
                                                        int & Position ) const
{
  Q_UNUSED ( Position )

  for ( const QString & Name : List )
  {
    if ( Name.compare ( Input ) == 0 )
    {
      return QValidator::Acceptable;
    }
  }

  return QValidator::Intermediate;
}

dbe::ValidatorAcceptNoMatch::ValidatorAcceptNoMatch ( QVariant & Storage, QObject * parent )
  : QValidator ( parent )
{
  List = Storage.toStringList();
}

QValidator::State dbe::ValidatorAcceptNoMatch::validate ( QString & Input,
                                                          int & Position ) const
{
  Q_UNUSED ( Position )

  for ( const QString & Name : List )
  {
    if ( Name.compare ( Input ) == 0 )
    {
      return QValidator::Intermediate;
    }
  }

  return QValidator::Acceptable;
}
