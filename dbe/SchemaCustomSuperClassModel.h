#ifndef CUSTOMSUPERCLASSMODEL_H
#define CUSTOMSUPERCLASSMODEL_H

/// Include oks
#include <oks/class.h>
#include <oks/relationship.h>
/// Including Schema
#include "SchemaCustomModelInterface.h"

namespace dbse
{

class CustomSuperClassModel: public CustomModelInterface
{
public:
  CustomSuperClassModel ( OksClass * ClassInfo, QStringList Headers, bool Derived = false );
  ~CustomSuperClassModel();
  void setupModel();
private:
  OksClass * SchemaClass;
  bool SchemaDerived;
};

}  // namespace dbse
#endif // CUSTOMSUPERCLASSMODEL_H
