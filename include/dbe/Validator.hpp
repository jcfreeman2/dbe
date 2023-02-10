#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QValidator>
#include <QStringList>

namespace dbe
{
class ValidatorAcceptMatch: public QValidator
{
  Q_OBJECT
public:
  ValidatorAcceptMatch ( QVariant & Storage, QObject * parent = 0 );
  QValidator::State validate ( QString & Input, int & Position ) const;
private:
  QStringList List;
};

class ValidatorAcceptNoMatch: public QValidator
{
  Q_OBJECT
public:
  ValidatorAcceptNoMatch ( QVariant & Storage, QObject * parent = 0 );
  QValidator::State validate ( QString & Input, int & Position ) const;
private:
  QStringList List;
};

} //end namespace dbe
#endif // VALIDATOR_H
