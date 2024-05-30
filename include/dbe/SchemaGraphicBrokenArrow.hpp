#ifndef SCHEMABROKENGRAPHICARROW_H
#define SCHEMABROKENGRAPHICARROW_H

/// Including QT Headers
#include <QGraphicsLineItem>
/// Including Schema Editor
#include "dbe/SchemaGraphicObject.hpp"
#include "dbe/SchemaGraphicsScene.hpp"

namespace dbse
{

class SchemaBrokenGraphicArrow: public QGraphicsLineItem
{
public:
  SchemaBrokenGraphicArrow ( SchemaGraphicObject * StartItem, SchemaGraphicObject * EndItem,
                       bool IsInheritance, bool IsComposite, QString ArrowName,
                       QString ArrowCardinality, QGraphicsItem * parent = nullptr );
  ~SchemaBrokenGraphicArrow();
  QRectF boundingRect() const;
  QPainterPath shape() const;
  void UpdatePosition();
  SchemaGraphicObject * GetStartItem() const;
  SchemaGraphicObject * GetEndItem() const;
  bool GetInheritanceMode();
  void RemoveArrow();
  void SetLabelScene ( SchemaGraphicsScene * Scene );
protected:
  //void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
  //void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
  //void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
  void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option,
               QWidget * widget = 0 );
private:
  SchemaGraphicObject * Start;
  SchemaGraphicObject * End;
  QPolygonF ArrowHead;
  bool Inheritance;
  bool Composite;
  QString Name;
  QString Cardinality;
  QGraphicsSimpleTextItem * Label;
  double LastDegree;
  double LastRotation;
  //QString LabelString;
};

}  // namespace dbse
#endif // SCHEMABROKENGRAPHICARROW_H
