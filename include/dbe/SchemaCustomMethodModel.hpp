#ifndef CUSTOMMETHODMODEL_H
#define CUSTOMMETHODMODEL_H

/// Include Qt
#include <QStringList>
/// Including Schema
#include "dbe/SchemaCustomModelInterface.hpp"
/// Include oks
#include "oks/class.hpp"
#include "oks/method.hpp"

namespace dbse
{

class CustomMethodModel: public CustomModelInterface
{
public:
  CustomMethodModel ( dunedaq::oks::OksClass * ClassInfo, QStringList Headers, bool Derived = false );
  ~CustomMethodModel();
  void setupModel();
private:
  dunedaq::oks::OksClass * SchemaClass;
  bool SchemaDerived;
};

}  // namespace dbse

#endif // CUSTOMMETHODMODEL_H
