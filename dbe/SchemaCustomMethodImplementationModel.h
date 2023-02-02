#ifndef CUSTOMMETHODIMPLEMENTATION_H
#define CUSTOMMETHODIMPLEMENTATION_H

/// Including Schema
#include "SchemaCustomModelInterface.h"
/// Include oks
#include "oks/method.hpp"

namespace dbse
{

class CustomMethodImplementationModel: public CustomModelInterface
{
public:
  CustomMethodImplementationModel ( OksMethod * Method, QStringList Headers );
  ~CustomMethodImplementationModel();
  void setupModel();
private:
  OksMethod * SchemaMethod;
};

}  // namespace dbse
#endif // CUSTOMMETHODIMPLEMENTATION_H
