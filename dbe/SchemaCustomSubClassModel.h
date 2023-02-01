#ifndef CUSTOMSUBCLASSMODEL_H
#define CUSTOMSUBCLASSMODEL_H

/// Include oks
#include <oks/class.h>
#include <oks/relationship.h>
/// Including Schema
#include "SchemaCustomModelInterface.h"

namespace dbse
{

class CustomSubClassModel: public CustomModelInterface
{
public:
  CustomSubClassModel ( OksClass * ClassInfo, QStringList Headers);
  ~CustomSubClassModel();
  void setupModel();
private:
  OksClass * SchemaClass;
};

}  // namespace dbse
#endif // CUSTOMSUBCLASSMODEL_H
