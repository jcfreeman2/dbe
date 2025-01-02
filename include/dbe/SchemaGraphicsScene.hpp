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
  QStringList AddItemsToScene ( QStringList SchemaClasses, QList<QPointF> Positions );
  void CleanItemMap();
  void RemoveClassObject ( SchemaGraphicObject * Object );
  [[nodiscard]] bool IsModified () const {return m_modified;};
  void ClearModified() {m_modified = false;};
protected:
  void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
  void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
  void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
  void RemoveItemFromScene ( QGraphicsItem* item );
private slots:
  void AddClassSlot();
  void EditClassSlot();
  void ToggleIndirectInfos();
  void AddDirectSuperClassesSlot();
  void AddAllSuperClassesSlot();
  void AddAllSubClassesSlot();
  void AddDirectRelationshipClassesSlot();
  void AddAllRelationshipClassesSlot();
  void RemoveClassSlot();
  void RemoveArrowSlot();
  void DrawArrow ( QString ClassName, QString RelationshipType, QString RelationshipName );
private:
  QMap<QString, SchemaGraphicObject *> ItemMap;
  QGraphicsLineItem * m_line;
  QMenu * m_context_menu;
  QAction * AddClass;
  QAction * EditClass;
  QAction * m_toggle_indirect_infos;
  QAction * m_add_direct_super_classes;
  QAction * m_add_direct_relationship_classes;
  QAction * m_add_all_super_classes;
  QAction * m_add_all_sub_classes;
  QAction * m_add_all_relationship_classes;
  QAction * RemoveClass;
  QAction * RemoveArrow;
  SchemaGraphicObject * CurrentObject;
  SchemaGraphicSegmentedArrow * m_current_arrow;

  bool m_inherited_properties_visible;
  bool m_modified;
};

}  // namespace dbse
#endif // SCHEMAGRAPHICSSCENE_H
