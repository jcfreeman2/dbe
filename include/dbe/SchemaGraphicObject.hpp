#ifndef SCHEMAGRAPHICOBJECT_H
#define SCHEMAGRAPHICOBJECT_H

/// Including QT Headers
#include <QGraphicsObject>
#include <QFont>

/// Including Oks Headers
#include "oks/class.hpp"

namespace dbse
{

class SchemaGraphicBrokenArrow;

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
  void set_inherited_properties_visibility( bool visible ) { m_inherited_properties_visible = visible; }
  QRectF boundingRect() const;
  QPainterPath shape() const;
  void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option,
               QWidget * widget );
  /// Arrow API
  void AddArrow ( SchemaGraphicBrokenArrow * Arrow );
  void RemoveArrow ( SchemaGraphicBrokenArrow * Arrow );
  void RemoveArrows();
  bool HasArrow ( SchemaGraphicObject * Dest ) const;
protected:
  QVariant itemChange ( GraphicsItemChange change, const QVariant & value );
private:
  dunedaq::oks::OksClass * m_class_info;
  QString ClassObjectName;
  QStringList m_class_attributes;
  QStringList m_class_methods;
  QStringList m_class_relationhips;

  QStringList m_class_inherited_attributes;
  QStringList m_class_inherited_relationhips;
  QStringList m_class_inherited_methods;

  bool m_inherited_properties_visible;  
  QFont m_font;
  double LineOffsetX;
  double LineOffsetY;
  QList<SchemaGraphicBrokenArrow *> Arrows;
private slots:
  void UpdateObject ( QString Name );
  void RemoveObject ( QString Name );
};

}  // namespace dbse
#endif // SCHEMAGRAPHICOBJECT_H
