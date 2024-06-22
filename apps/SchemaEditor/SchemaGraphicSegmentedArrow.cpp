/// Including QT Headers
#include <QPainter>
#include <QPen>
/// Including Schema Editor
#include "dbe/SchemaGraphicSegmentedArrow.hpp"
#include "dbe/SchemaKernelWrapper.hpp"
/// Including C++ Headers
#include <cmath>

namespace dbse {

SchemaGraphicSegmentedArrow::SchemaGraphicSegmentedArrow ( SchemaGraphicObject * start_item,
                                               SchemaGraphicObject * end_item, bool is_inheritance,
                                               bool is_composite, QString arrow_name,
                                               QString arrow_cardinality, QGraphicsItem * parent )
  : QGraphicsPathItem ( parent ),
    m_start_item ( start_item ),
    m_end_item ( end_item ),
    m_inheritance ( is_inheritance ),
    m_composite ( is_composite ),
    m_name ( arrow_name ),
    m_cardinality ( arrow_cardinality ),
    LastDegree ( 0 ),
    LastRotation ( 0 )
{
  //setPen(QPen(Qt::black,2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
  setFlag ( ItemIsSelectable, true );
  m_default_color = QColor ( 0x1e1b18 );
  m_label_font = QFont( "Helvetica [Cronyx]", 10);
  m_arrow_size = 20;

}

SchemaGraphicSegmentedArrow::~SchemaGraphicSegmentedArrow()
{
}

QRectF SchemaGraphicSegmentedArrow::boundingRect() const
{
  if ( m_start_item->collidesWithItem ( m_end_item ) )
  {
    return QRectF();
  }

  qreal extra = ( pen().width() + 20 ) / 2.0 + 10;
  return QRectF ( p1(),
                  QSizeF ( p2().x() - p1().x(),
                           p2().y() - p1().y() ) ).normalized().adjusted (
           -extra, -extra, extra, extra );
}

QPainterPath SchemaGraphicSegmentedArrow::shape() const
{
  QPainterPath path = QGraphicsPathItem::shape();
  // path.addPolygon ( m_arrow_head );
  path.addText ( p2() + QPoint ( 10, 10 ), QFont ( "Helvetica [Cronyx]", 10 ),
                 m_cardinality );
  path.addText ( p1() + QPoint ( -10, -10 ), QFont ( "Helvetica [Cronyx]", 10 ),
                 m_cardinality );
  return path;
}

void SchemaGraphicSegmentedArrow::UpdatePosition()
{
  QPainterPath path( mapFromItem ( m_start_item, 0, 0 ) );
  path.lineTo(mapFromItem ( m_end_item, 0, 0 ));
  setPath(path);
  // QLineF line ( mapFromItem ( m_start_item, 0, 0 ), mapFromItem ( m_end_item, 0, 0 ) );
  // setLine ( line );
}

SchemaGraphicObject * SchemaGraphicSegmentedArrow::GetStartItem() const
{
  return m_start_item;
}

SchemaGraphicObject * SchemaGraphicSegmentedArrow::GetEndItem() const
{
  return m_end_item;
}

bool SchemaGraphicSegmentedArrow::GetInheritanceMode()
{
  return m_inheritance;
}

void SchemaGraphicSegmentedArrow::RemoveArrow()
{
  if ( m_inheritance )
  {
    /// Remove Super class
  }
  else
  {
    /// Remove relationship
  }
}

QPolygonF SchemaGraphicSegmentedArrow::make_arrow_head(qreal rotation) const {

  // if (path().elementCount()<2) {
  //   return QPolygonF();
  // }

  // QLineF segment(path().elementAt(0), path().elementAt(1));

  // QPointF tip = path().elementAt(0);
  // qreal rotation = 0;

  // // Vertical line
  // if (segment.dx() > 0) {
  //   int sign = (segment.dy() > 0) - (segment.dy() < 0);
  // }
  // // Horizontal line 
  // else if (segment.dy() > 0) {

  // } else {
  //   return QPolygonF();
  // }

  float opening_angle = atan2(1, 0.5);
  qreal size = m_arrow_size;
  QPointF a1 = QPointF ( sin ( rotation + opening_angle ) * size, cos ( rotation + opening_angle ) * size );
  QPointF a2 = QPointF ( sin ( rotation + M_PI - opening_angle ) * size, cos ( rotation + M_PI - opening_angle ) * size );

  QPolygonF arrow;
  arrow << QPointF(0,0) << a1 << a2;
  return arrow;
}

QPolygonF SchemaGraphicSegmentedArrow::make_rhombus(qreal rotation) const {

  float opening_angle = M_PI / 3;
  qreal size = 0.6*m_arrow_size;

  QPointF a1 = QPointF ( sin ( rotation + opening_angle ) * size, cos ( rotation + opening_angle ) * size );
  QPointF a2 = QPointF ( sin ( rotation + M_PI - opening_angle ) * size, cos ( rotation + M_PI - opening_angle ) * size );

  QPointF m = QPointF ( ( a1.x() + a2.x() ) / 2, ( a1.y() + a2.y() ) / 2 );
  QPointF a3 = QPointF ( 2 * m.x(), 2 * m.y() );
  
  QPolygonF rhombus;
  rhombus << QPointF(0,0) << a1 << a3 << a2;
  return rhombus;
}



void SchemaGraphicSegmentedArrow::paint ( QPainter * painter,
                                       const QStyleOptionGraphicsItem * option,
                                       QWidget * widget )
{
  Q_UNUSED ( option )
  Q_UNUSED ( widget )

  if ( m_start_item->collidesWithItem ( m_end_item ) )
  {
    return;
  }

  const QPen line_pen = QPen(m_default_color, 1);
  const QPen arrow_pen = QPen(m_default_color, 1);

  QFont Font ( "Helvetica [Cronyx]", 10 );
  qreal arrowSize = 15;
  painter->setFont ( Font );
  painter->setPen ( line_pen );
  painter->setBrush ( {} );
  painter->setRenderHint(QPainter::Antialiasing);


  QLineF centerLine ( m_start_item->mapToScene ( m_start_item->boundingRect().center() ),
                      m_end_item->mapToScene ( m_end_item->boundingRect().center() ) );
  QPolygonF startPolygon = QPolygonF ( m_start_item->boundingRect() );
  QPolygonF endPolygon = QPolygonF ( m_end_item->boundingRect() );
  QPointF p1 = endPolygon.first() + m_end_item->pos();
  QPointF p2;
  QPointF intersectPointStart;
  QPointF intersectPointEnd;
  QLineF polyLine;

  for ( int i = 1; i < endPolygon.count(); ++i )
  {
    p2 = endPolygon.at ( i ) + m_end_item->pos();
    polyLine = QLineF ( p1, p2 );
    QLineF::IntersectType intersectType = polyLine.intersects ( centerLine, &intersectPointEnd );

    if ( intersectType == QLineF::BoundedIntersection )
    {
      break;

    }

    p1 = p2;
  }

  p1 = startPolygon.first() + m_start_item->pos();

  for ( int i = 1; i < startPolygon.count(); ++i )
  {
    p2 = startPolygon.at ( i ) + m_start_item->pos();
    polyLine = QLineF ( p1, p2 );
    QLineF::IntersectType intersectType = polyLine.intersects ( centerLine,
                                                               &intersectPointStart );

    if ( intersectType == QLineF::BoundedIntersection )
    {
      break;
    }

    p1 = p2;
  }

  QLineF direct_line(intersectPointEnd, intersectPointStart);

  QPainterPath path( intersectPointEnd );

  // approximate
  if ( abs(direct_line.dx()) < abs(direct_line.dy()) ) {
    path.lineTo(QPointF(direct_line.x1(), direct_line.center().y()));
    path.lineTo(QPointF(direct_line.x2(), direct_line.center().y()));
  } else {
    path.lineTo(QPointF(direct_line.center().x(), direct_line.y1()));
    path.lineTo(QPointF(direct_line.center().x(), direct_line.y2()));
  }
  path.lineTo( intersectPointStart );
  setPath(path);



  double angle = ::acos ( this->dx() / this->path().length() );
  if ( this->dy() >= 0 )
  {
    angle = ( M_PI * 2 ) - angle;
  }

  angle = int(angle/ M_PI_2+0.5)*M_PI_2;

  // QPointF arrowP1 = this->p1()
  //                   + QPointF ( sin ( angle + M_PI / 3 ) * arrowSize, cos ( angle + M_PI / 3 ) * arrowSize );
  // QPointF arrowP2 = this->p1()
  //                   + QPointF ( sin ( angle + M_PI - M_PI / 3 ) * arrowSize,
  //                               cos ( angle + M_PI - M_PI / 3 ) * arrowSize );
  // QPointF middlePoint = QPointF ( ( arrowP1.x() + arrowP2.x() ) / 2,
  //                                 ( arrowP1.y() + arrowP2.y() ) / 2 );
  // QPointF arrowP3 = QPointF ( this->p1().x() - 2 * ( this->p1().x() - middlePoint.x() ),
  //                             this->p1().y() - 2 * ( this->p1().y() - middlePoint.y() ) );
  // m_arrow_head.clear();
  // m_arrow_head << this->p1() << arrowP1 << arrowP2;

  QFontMetrics Metrics ( Font );
  Metrics.boundingRect ( m_name + " - " + m_cardinality );

  painter->drawPath ( this->path());
   
  painter->setPen ( arrow_pen );
  if ( !m_inheritance ) {
    painter->drawText(direct_line.center(), QString(m_name + " - " + m_cardinality));
  }

  if ( m_inheritance )
  {
    painter->setBrush ( Qt::white );
    painter->drawPolygon ( this->make_arrow_head(angle).translated(this->p1()));
  }
  else if ( m_composite )
  {
    /// Draw Rhombus
    painter->setBrush ( Qt::black );
    painter->drawPolygon ( this->make_rhombus(angle+M_PI).translated(this->p2()));

  } else {
    /// Draw Rhombus
    painter->setBrush ( Qt::white );
    painter->drawPolygon ( this->make_rhombus(angle+M_PI).translated(this->p2()));
  }
}

} // namespace dbse
