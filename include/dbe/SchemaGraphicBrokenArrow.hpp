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
  void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option,
               QWidget * widget = 0 );
private:
  SchemaGraphicObject * m_start_item;
  SchemaGraphicObject * m_end_item;
  QPolygonF ArrowHead;
  bool m_inheritance;
  bool m_composite;
  QString Name;
  QString Cardinality;
  QGraphicsSimpleTextItem * m_label;
  double LastDegree;
  double LastRotation;
  //QString LabelString;
};

}  // namespace dbse
#endif // SCHEMABROKENGRAPHICARROW_H
