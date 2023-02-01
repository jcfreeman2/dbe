#ifndef SCHEMAMETHODEDITOR_H
#define SCHEMAMETHODEDITOR_H

#include <memory>
#include <QWidget>
#include <oks/class.h>
#include <oks/method.h>
#include "SchemaCustomMethodImplementationModel.h"



namespace dbse
{

namespace Ui
{
class SchemaMethodEditor;
}  // namespace Ui

class SchemaMethodEditor: public QWidget
{
  Q_OBJECT
public:
  ~SchemaMethodEditor();
  explicit SchemaMethodEditor ( OksClass * ClassInfo, OksMethod * Method, QWidget * parent =
                                  nullptr );
  explicit SchemaMethodEditor ( OksClass * ClassInfo, QWidget * parent = nullptr );

  void SetController();
  void InitialSettings();
  void ParseToSave();
  void ParseToCreate();
  void BuildModels();
protected:
  void FillInfo();
private:
  std::unique_ptr<dbse::Ui::SchemaMethodEditor> ui;
  OksClass * SchemaClass;
  OksMethod * SchemaMethod;
  CustomMethodImplementationModel * ImplementationModel;
  bool UsedNew;
  bool ShouldOpenMethodImplementationEditor ( QString Name );
private slots:
  void ProxySlot();
  void AddNewMethodImplementation();
  void OpenMethodImplementationEditor ( QModelIndex Index );
  void BuildModelImplementationSlot();
  void ClassUpdated( QString ClassName);
signals:
  void RebuildModel();
};

}  // namespace dbse
#endif // SCHEMAMETHODEDITOR_H
