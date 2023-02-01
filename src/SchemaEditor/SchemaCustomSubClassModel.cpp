#include "SchemaCustomSubClassModel.h"

dbse::CustomSubClassModel::CustomSubClassModel ( OksClass * ClassInfo,
                                                 QStringList Headers )
  : CustomModelInterface ( Headers ),
    SchemaClass ( ClassInfo )
{
  setupModel();
}

void dbse::CustomSubClassModel::setupModel()
{
  Data.clear();

  const OksClass::FList* allClasses = SchemaClass->all_sub_classes();
  if(allClasses != nullptr) {
      for(const OksClass* cl : *allClasses) {
          QStringList Row;
          Row.append ( QString::fromStdString ( cl->get_name() ) );
          Data.append ( Row );
      }
  }
}

dbse::CustomSubClassModel::~CustomSubClassModel() = default;
