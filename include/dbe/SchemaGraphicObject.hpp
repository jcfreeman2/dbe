#ifndef SCHEMAGRAPHICOBJECT_H
#define SCHEMAGRAPHICOBJECT_H

/// Including QT Headers
#include <QGraphicsObject>
/// Including Oks Headers
#include "oks/class.hpp"

namespace dbse
{

class SchemaGraphicArrow;

class SchemaGraphicObject: public QGraphicsObject
{
  Q_OBJECT
public:
  explicit SchemaGraphicObject ( QString & ClassName, QGraphicsObject * parent = nullptr );
  ~SchemaGraphicObject();
  dunedaq::oks::OksClass * GetClass() const;
  QString GetClassName() const;
  void GetInfo();
  /// Graphic API
  QRectF boundingRect() const;
  QPainterPath shape() const;
  void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option,
               QWidget * widget );
  /// Arrow API
  void AddArrow ( SchemaGraphicArrow * Arrow );
  void RemoveArrow ( SchemaGraphicArrow * Arrow );
  void RemoveArrows();
  bool HasArrow ( SchemaGraphicObject * Dest ) const;
protected:
  QVariant itemChange ( GraphicsItemChange change, const QVariant & value );
private:
  dunedaq::oks::OksClass * ClassInfo;
  QString ClassObjectName;
  QStringList ClassAttributes;
  QStringList ClassMethods;
  double LineOffsetX;
  double LineOffsetY;
  QList<SchemaGraphicArrow *> Arrows;
private slots:
  void UpdateObject ( QString Name );
  void RemoveObject ( QString Name );
};

}  // namespace dbse
#endif // SCHEMAGRAPHICOBJECT_H
