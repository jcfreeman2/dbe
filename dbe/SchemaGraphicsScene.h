#ifndef SCHEMAGRAPHICSSCENE_H
#define SCHEMAGRAPHICSSCENE_H

/// Include QT Headers
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QAction>
/// Including Schema Editor
#include "SchemaGraphicObject.h"

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
  void RemoveClassSlot();
  void RemoveArrowSlot();
  void DrawArrow ( QString ClassName, QString RelationshipType, QString RelationshipName );
private:
  QMap<QString, SchemaGraphicObject *> ItemMap;
  QGraphicsLineItem * line;
  QMenu * ContextMenu;
  QAction * AddClass;
  QAction * EditClass;
  QAction * RemoveClass;
  QAction * RemoveArrow;
  SchemaGraphicObject * CurrentObject;
  SchemaGraphicArrow * CurrentArrow;
};

}  // namespace dbse
#endif // SCHEMAGRAPHICSSCENE_H
