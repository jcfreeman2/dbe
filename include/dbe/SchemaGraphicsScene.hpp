#ifndef SCHEMAGRAPHICSSCENE_H
#define SCHEMAGRAPHICSSCENE_H

/// Include QT Headers
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QAction>
/// Including Schema Editor
#include "dbe/SchemaGraphicObject.hpp"

namespace dbse
{

class SchemaGraphicsScene: public QGraphicsScene
{
  Q_OBJECT
public:
  explicit SchemaGraphicsScene ( QObject * parent = nullptr );
  ~SchemaGraphicsScene();
  void CreateActions();
  /// Drag & Drop
  void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );
  void dragMoveEvent ( QGraphicsSceneDragDropEvent * event );
  void dropEvent ( QGraphicsSceneDragDropEvent * event );
  void contextMenuEvent ( QGraphicsSceneContextMenuEvent * event );
  void AddItemToScene ( QStringList SchemaClasses, QList<QPointF> Positions );
  void CleanItemMap();
  void RemoveClassObject ( SchemaGraphicObject * Object );
protected:
  void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
  void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
  void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
private slots:
  void AddClassSlot();
  void EditClassSlot();
  void ToggleIndirectInfos();
  void AddSuperClassesSlot();
  void AddSubClassesSlot();
  void AddDirectRelationshipClassesSlot();
  void AddAllRelationshipClassesSlot();
  void RemoveClassSlot();
  void RemoveArrowSlot();
  void DrawArrow ( QString ClassName, QString RelationshipType, QString RelationshipName );
private:
  QMap<QString, SchemaGraphicObject *> ItemMap;
  QGraphicsLineItem * line;
  QMenu * ContextMenu;
  QAction * AddClass;
  QAction * EditClass;
  QAction * m_toggle_indirect_infos;
  QAction * m_add_super_classes;
  QAction * m_add_sub_classes;
  QAction * m_add_direct_relationship_classes;
  QAction * m_add_all_relationship_classes;
  QAction * RemoveClass;
  QAction * RemoveArrow;
  SchemaGraphicObject * CurrentObject;
  SchemaGraphicArrow * CurrentArrow;

  bool m_indirects_visible;
};

}  // namespace dbse
#endif // SCHEMAGRAPHICSSCENE_H
