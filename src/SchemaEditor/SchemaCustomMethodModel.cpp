#include "dbe/SchemaCustomMethodModel.hpp"

dbse::CustomMethodModel::CustomMethodModel ( OksClass * ClassInfo, QStringList Headers,
                                             bool Derived )
  : CustomModelInterface ( Headers ),
    SchemaClass ( ClassInfo ),
    SchemaDerived ( Derived )
{
  setupModel();
}

void dbse::CustomMethodModel::setupModel()
{
  Data.clear();
  const std::list<OksMethod *> * MethodList;

  if ( SchemaDerived )
  {
    MethodList = SchemaClass->all_methods();
  }
  else
  {
    MethodList = SchemaClass->direct_methods();
  }

  if ( MethodList )
  {
    for ( OksMethod * Method : *MethodList )
    {
      QStringList Row;
      Row.append ( QString::fromStdString ( Method->get_name() ) );
      Data.append ( Row );
    }
  }
}

dbse::CustomMethodModel::~CustomMethodModel() = default;
