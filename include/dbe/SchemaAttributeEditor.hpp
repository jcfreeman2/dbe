#ifndef SCHEMAATTRIBUTEEDITOR_H
#define SCHEMAATTRIBUTEEDITOR_H

#include<memory>
#include <QWidget>
#include "oks/attribute.hpp"

namespace dbse
{

namespace Ui
{
class SchemaAttributeEditor;
}

class SchemaAttributeEditor: public QWidget
{
  Q_OBJECT
public:
  ~SchemaAttributeEditor();

  explicit SchemaAttributeEditor ( OksClass * ClassInfo, OksAttribute * AttributeData,
                                   QWidget * parent = nullptr );

  explicit SchemaAttributeEditor ( OksClass * ClassInfo, QWidget * parent = nullptr );

  void InitialSettings();
  void SetController();
  void ParseToSave();
  void ParseToCreate();
protected:
  void FillInfo();
private:
  std::unique_ptr<dbse::Ui::SchemaAttributeEditor> ui;
  OksClass * SchemaClass;
  OksAttribute * SchemaAttribute;
  bool UsedNew;
private slots:
  void ProxySlot();
  void ToggleFormat ( int );
  void ClassUpdated( QString ClassName);
signals:
  void RebuildModel();
};
} //end namespace okse
#endif // SCHEMAATTRIBUTEEDITOR_H
