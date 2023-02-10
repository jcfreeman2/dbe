#ifndef SCHEMARELATIONSHIPEDITOR_H
#define SCHEMARELATIONSHIPEDITOR_H

#include <memory>
#include <QWidget>
#include "oks/relationship.hpp"

namespace dbse
{
namespace Ui
{
class SchemaRelationshipEditor;
}  // namespace Ui

class SchemaRelationshipEditor: public QWidget
{
  Q_OBJECT
public:
  ~SchemaRelationshipEditor();

  explicit SchemaRelationshipEditor ( OksClass * Class, OksRelationship * Relationship,
                                      QWidget * parent = nullptr );

  explicit SchemaRelationshipEditor ( OksClass * Class, QWidget * parent = nullptr );

  explicit SchemaRelationshipEditor ( OksClass * Class, QString ClassType, QWidget * parent =
                                        nullptr );

  void InitialSettings();
  void SetController();
  void ParseToSave();
  void ParseToCreate();
protected:
  void FillInfo();
private:
  dbse::Ui::SchemaRelationshipEditor * ui;
  OksRelationship * SchemaRelationship;
  OksClass * SchemaClass;
  bool UsedNew;
  bool GraphScene;
private slots:
  void ProxySlot();
  void UpdateClassCombo();
  void ClassUpdated( QString className );
signals:
  void RebuildModel();
  void MakeGraphConnection ( QString ClassName1, QString ClassName2,
                             QString RelationshipName );
};

}  // namespace dbse
#endif // SCHEMARELATIONSHIPEDITOR_H
