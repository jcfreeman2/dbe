#ifndef CUSTOMMETHODIMPLEMENTATION_H
#define CUSTOMMETHODIMPLEMENTATION_H

/// Including Schema
#include "dbe/SchemaCustomModelInterface.hpp"
/// Include oks
#include "oks/method.hpp"

namespace dbse
{

class CustomMethodImplementationModel: public CustomModelInterface
{
public:
  CustomMethodImplementationModel ( dunedaq::oks::OksMethod * Method, QStringList Headers );
  ~CustomMethodImplementationModel();
  void setupModel();
private:
  dunedaq::oks::OksMethod * SchemaMethod;
};

}  // namespace dbse
#endif // CUSTOMMETHODIMPLEMENTATION_H
