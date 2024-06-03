#ifndef SchemaGraphicSegmentedArrow_H
#define SchemaGraphicSegmentedArrow_H

/// Including QT Headers
#include <QGraphicsPathItem>
/// Including Schema Editor
#include "dbe/SchemaGraphicObject.hpp"
#include "dbe/SchemaGraphicsScene.hpp"

namespace dbse
{

class SchemaGraphicSegmentedArrow: public QGraphicsPathItem
{
public:
  SchemaGraphicSegmentedArrow ( SchemaGraphicObject * StartItem, SchemaGraphicObject * EndItem,
                       bool IsInheritance, bool IsComposite, QString ArrowName,
                       QString ArrowCardinality, QGraphicsItem * parent = nullptr );
  ~SchemaGraphicSegmentedArrow();
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

  QPointF p1() const { return path().elementAt(0); }
  QPointF p2() const { return path().elementAt(path().elementCount()-1); }
  qreal dx() const { return p2().x() - p1().x(); } 
  qreal dy() const { return p2().y() - p1().y(); } 


  SchemaGraphicObject * m_start_item;
  SchemaGraphicObject * m_end_item;
  QPolygonF m_arrow_head;
  bool m_inheritance;
  bool m_composite;
  QString m_name;
  QString m_cardinality;
  QGraphicsSimpleTextItem * m_label;
  double LastDegree;
  double LastRotation;
  //QString LabelString;
};

}  // namespace dbse
#endif // SchemaGraphicSegmentedArrow_H
