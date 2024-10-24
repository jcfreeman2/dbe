/// Including QT Headers
#include <QPainter>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsView>
/// Include oks
#include"oks/method.hpp"
/// Including SchemaEditor
#include "dbe/SchemaGraphicObject.hpp"
#include "dbe/SchemaGraphicSegmentedArrow.hpp"
#include "dbe/SchemaGraphicsScene.hpp"
#include "dbe/SchemaKernelWrapper.hpp"

using namespace dunedaq::oks;

dbse::SchemaGraphicObject::SchemaGraphicObject ( QString & ClassName,
                                                 QGraphicsObject * parent )
  : QGraphicsObject ( parent ),
    m_inherited_properties_visible(true),
    LineOffsetX ( 0 ),
    LineOffsetY ( 0 )
{
  m_font = QFont( "Helvetica [Cronyx]", 9);
  m_bold_font = QFont( "Helvetica [Cronyx]", 9, QFont::DemiBold);
  m_default_color = QColor ( 0x1e1b18 );
  m_highlight_color = QColor ( 0x14aaff );
  m_opaque_color = QColor ( 0x5d5b59 );

  setFlag ( ItemIsMovable );
  setFlag ( ItemSendsGeometryChanges, true );
  setFlag ( ItemSendsScenePositionChanges, true );

  /// Connecting Signals
  connect ( &KernelWrapper::GetInstance(), SIGNAL ( ClassUpdated ( QString ) ), this,
            SLOT ( UpdateObject ( QString ) ) );
  connect ( &KernelWrapper::GetInstance(), SIGNAL ( ClassRemoved ( QString ) ), this,
            SLOT ( RemoveObject ( QString ) ) );
  /// Getting Class
  m_class_info = KernelWrapper::GetInstance().FindClass ( ClassName.toStdString() );
  m_class_object_name = ClassName;
  /// Getting class info
  GetInfo();
}

dbse::SchemaGraphicObject::~SchemaGraphicObject()
{
}

OksClass * dbse::SchemaGraphicObject::GetClass() const
{
  return m_class_info;
}

QString dbse::SchemaGraphicObject::GetClassName() const
{
  return m_class_object_name;
}

void dbse::SchemaGraphicObject::GetInfo()
{
  m_class_attributes.clear();
  m_class_relationhips.clear();
  m_class_methods.clear();

  std::list<OksAttribute *> direct_attributes = {};
  if (m_class_info->direct_attributes()) direct_attributes = *m_class_info->direct_attributes();

  std::list<OksMethod *> direct_methods = {};
  if (m_class_info->direct_methods()) direct_methods = *m_class_info->direct_methods();

  std::list<OksRelationship *> direct_relationships = {};
  if (m_class_info->direct_relationships()) direct_relationships = *m_class_info->direct_relationships();

  std::list<OksAttribute *> all_attributes = {};
  if (m_class_info->all_attributes()) all_attributes = *m_class_info->all_attributes();

  std::list<OksMethod *> all_methods = {};
  if (m_class_info->all_methods()) all_methods = *m_class_info->all_methods();

  std::list<OksRelationship *> all_relationships = {};
  if (m_class_info->all_relationships()) all_relationships = *m_class_info->all_relationships();

  // Prepare indirect relatonship list
  std::list<OksAttribute *> indirect_attributes = all_attributes;
  std::list<OksMethod *> indirect_methods = all_methods;
  std::list<OksRelationship *> indirect_relationships = all_relationships;

  for ( OksAttribute * attribute : direct_attributes ) {
    indirect_attributes.remove(attribute);
  }

  for ( OksMethod * method : direct_methods ) {
    indirect_methods.remove(method);
  } 

  for ( OksRelationship * relationship : direct_relationships ) {
    indirect_relationships.remove(relationship);
  } 

  std::map<OksRelationship::CardinalityConstraint, std::string> m = {
    {OksRelationship::Zero, "0"},
    {OksRelationship::One, "1"},
    {OksRelationship::Many, "n"}
  };

  /// Getting direct Attributes
  for ( OksAttribute * attribute : direct_attributes )
  {
    QString AttributeString (
      QString::fromStdString ( attribute->get_name() ) + " : "
      + QString::fromStdString ( attribute->get_type() ) + (attribute->get_is_multi_values() ? "[]" : "") );
    m_class_attributes.append ( AttributeString );
  }

  /// Getting direct Relationships
  for ( OksRelationship * relationship : direct_relationships )
  {
    QString relationship_string ( QString::fromStdString ( relationship->get_name() ) + " : " 
    + QString::fromStdString ( relationship->get_type() ) + " - "
    + QString::fromStdString ( m[ relationship->get_low_cardinality_constraint() ] ) + ":"
    + QString::fromStdString ( m[ relationship->get_high_cardinality_constraint() ] )  );
    m_class_relationhips.append ( relationship_string );
  }

  /// Getting direct Methods
  for ( OksMethod * Method : direct_methods )
  {
    QString MethodString ( QString::fromStdString ( Method->get_name() ) + "()" );
    m_class_methods.append ( MethodString );
  }

  if (m_inherited_properties_visible) {
    /// Getting indirect Attributes
    for ( OksAttribute * attribute : indirect_attributes )
    {
      QString attribute_string (
        QString::fromStdString ( attribute->get_name() ) + " : "
        + QString::fromStdString ( attribute->get_type() ) );
      m_class_inherited_attributes.append ( attribute_string );
    }

    /// Getting indirect Relationships
    for ( OksRelationship * relationship : indirect_relationships )
    {
      QString relationship_string ( QString::fromStdString ( relationship->get_name() ) + " : " 
      + QString::fromStdString ( relationship->get_type() ) + " - "
      + QString::fromStdString ( m[ relationship->get_low_cardinality_constraint() ] ) + ":"
      + QString::fromStdString ( m[ relationship->get_high_cardinality_constraint() ] )  );
      m_class_inherited_relationhips.append ( relationship_string );
    }

    /// Getting indirect Methods
    for ( OksMethod * method : indirect_methods )
    {
      QString method_string ( QString::fromStdString ( method->get_name() ) + "()" );
      m_class_inherited_methods.append ( method_string );
    }

  }

}

QRectF dbse::SchemaGraphicObject::boundingRect() const
{
  double SpaceX = 3;
  double TotalBoundingHeight = 0;
  double TotalBoundingWidth = 0;

  QFontMetrics FontMetrics ( m_font );

  TotalBoundingHeight += SpaceX * 5;
  TotalBoundingHeight += FontMetrics.boundingRect ( m_class_object_name ).height();
  TotalBoundingWidth += FontMetrics.boundingRect ( m_class_object_name ).width();

  for ( auto & AttributeName : m_class_attributes )
  {
    TotalBoundingHeight += FontMetrics.boundingRect ( AttributeName ).height();

    if ( FontMetrics.boundingRect ( AttributeName ).width() > TotalBoundingWidth )
      TotalBoundingWidth =
        FontMetrics.boundingRect ( AttributeName ).width();
  }

  if (m_inherited_properties_visible) {
    for ( auto & AttributeName : m_class_inherited_attributes )
    {
      TotalBoundingHeight += FontMetrics.boundingRect ( AttributeName ).height();

      if ( FontMetrics.boundingRect ( AttributeName ).width() > TotalBoundingWidth )
        TotalBoundingWidth =
          FontMetrics.boundingRect ( AttributeName ).width();
    }
  }

  for ( auto & relationship_name : m_class_relationhips )
  {
    TotalBoundingHeight += FontMetrics.boundingRect ( relationship_name ).height();

    if ( FontMetrics.boundingRect ( relationship_name ).width() > TotalBoundingWidth )
      TotalBoundingWidth =
        FontMetrics.boundingRect ( relationship_name ).width();
  }

  if (m_inherited_properties_visible) {
    for ( auto & relationship_name : m_class_inherited_relationhips )
    {
      TotalBoundingHeight += FontMetrics.boundingRect ( relationship_name ).height();

      if ( FontMetrics.boundingRect ( relationship_name ).width() > TotalBoundingWidth )
        TotalBoundingWidth =
          FontMetrics.boundingRect ( relationship_name ).width();
    }
  }

  for ( auto & MethodName : m_class_methods )
  {
    TotalBoundingHeight += FontMetrics.boundingRect ( MethodName ).height();

    if ( FontMetrics.boundingRect ( MethodName ).width() > TotalBoundingWidth )
      TotalBoundingWidth =
        FontMetrics.boundingRect ( MethodName ).width();
  }

  if (m_inherited_properties_visible) {
    for ( auto & MethodName : m_class_inherited_methods )
    {
      TotalBoundingHeight += FontMetrics.boundingRect ( MethodName ).height();

      if ( FontMetrics.boundingRect ( MethodName ).width() > TotalBoundingWidth )
        TotalBoundingWidth =
          FontMetrics.boundingRect ( MethodName ).width();
    }
  }

  TotalBoundingWidth += 15;
  return QRectF ( 0, 0, TotalBoundingWidth, TotalBoundingHeight );
}

QPainterPath dbse::SchemaGraphicObject::shape() const
{
  QPainterPath path;
  path.addRect ( boundingRect() );
  return path;
}

void dbse::SchemaGraphicObject::paint ( QPainter * painter,
                                        const QStyleOptionGraphicsItem * option,
                                        QWidget * widget )
{
  Q_UNUSED ( widget )
  Q_UNUSED ( option )

  double SpaceX = 3;
  double SpaceY = 3;

  const QPen bounding_box_pen = QPen(m_default_color, 2.5);
  const QPen inner_line_pen = QPen(m_default_color, 1.5);

  painter->setFont ( m_font );
  painter->setPen ( bounding_box_pen );
  painter->drawRect ( boundingRect() );
  // painter->setPen ( m_default_color );

  QFontMetrics FontMetrics = painter->fontMetrics();
  QRectF ClassNameBoundingRect = FontMetrics.boundingRect ( m_class_object_name );
  QRectF ObjectBoundingRect = boundingRect();

  double HeightOffset = ClassNameBoundingRect.height() + SpaceY;
  int ClassNamePosition = ( ObjectBoundingRect.width() - ClassNameBoundingRect.width() ) / 2;
  painter->setFont ( m_font );
  painter->drawText ( ClassNamePosition, ClassNameBoundingRect.height(), m_class_object_name );
  painter->setFont ( m_font );
  painter->drawLine ( 0, HeightOffset, ObjectBoundingRect.width(), HeightOffset );

  for ( QString & AttributeName : m_class_attributes )
  {
    QRectF AttributeBoundingRect = FontMetrics.boundingRect ( AttributeName );
    HeightOffset += AttributeBoundingRect.height();
    painter->drawText ( SpaceX, HeightOffset, AttributeName );
  }

  if (m_inherited_properties_visible) {
    painter->setPen ( m_opaque_color );
    for ( QString & AttributeName : m_class_inherited_attributes )
    {
      QRectF AttributeBoundingRect = FontMetrics.boundingRect ( AttributeName );
      HeightOffset += AttributeBoundingRect.height();
      painter->drawText ( SpaceX, HeightOffset, AttributeName );
    }
    painter->setPen ( m_default_color );
  }

  HeightOffset += SpaceY;
  painter->setPen ( inner_line_pen );
  painter->drawLine ( 0, HeightOffset, ObjectBoundingRect.width(), HeightOffset );

  for ( QString & relationship_name : m_class_relationhips )
  {
    QRectF relationship_bounding_rect = FontMetrics.boundingRect ( relationship_name );
    HeightOffset += relationship_bounding_rect.height();
    painter->drawText ( SpaceX, HeightOffset, relationship_name );
  }

  if (m_inherited_properties_visible) {
    painter->setPen ( m_opaque_color );
    for ( QString & relationship_name : m_class_inherited_relationhips )
    {
      QRectF relationship_bounding_rect = FontMetrics.boundingRect ( relationship_name );
      HeightOffset += relationship_bounding_rect.height();
      painter->drawText ( SpaceX, HeightOffset, relationship_name );
    }
    painter->setPen ( m_default_color );
  }


  HeightOffset += SpaceY;
  painter->setPen ( inner_line_pen );
  painter->drawLine ( 0, HeightOffset, ObjectBoundingRect.width(), HeightOffset );
  
  for ( QString & MethodName : m_class_methods )
  {
    QRectF AttributeBoundingRect = FontMetrics.boundingRect ( MethodName );
    HeightOffset += AttributeBoundingRect.height();
    painter->drawText ( SpaceX, HeightOffset, MethodName );
  }

  if (m_inherited_properties_visible) {
    painter->setPen ( m_opaque_color );
    for ( QString & MethodName : m_class_inherited_methods )
    {
      QRectF AttributeBoundingRect = FontMetrics.boundingRect ( MethodName );
      HeightOffset += AttributeBoundingRect.height();
      painter->drawText ( SpaceX, HeightOffset, MethodName );
    }
    painter->setPen ( m_default_color );
  }
}

void dbse::SchemaGraphicObject::AddArrow ( SchemaGraphicSegmentedArrow * Arrow )
{
  m_arrows.append ( Arrow );
}

void dbse::SchemaGraphicObject::RemoveArrow ( SchemaGraphicSegmentedArrow * Arrow )
{
  int index = m_arrows.indexOf ( Arrow );

  if ( index != -1 )
  {
    m_arrows.removeAt ( index );
  }
}

void dbse::SchemaGraphicObject::RemoveArrows()
{
  foreach ( SchemaGraphicSegmentedArrow * arrow, m_arrows )
  {
    arrow->GetStartItem()->RemoveArrow ( arrow );
    arrow->GetEndItem()->RemoveArrow ( arrow );
    scene()->removeItem ( arrow );
  }
}

bool dbse::SchemaGraphicObject::HasArrow ( SchemaGraphicObject * Dest ) const
{
  if ( m_arrows.isEmpty() )
  {
    return false;
  }

  for ( SchemaGraphicSegmentedArrow * Arrow : m_arrows )
  {
    SchemaGraphicObject * ArrowSource = Arrow->GetStartItem();
    SchemaGraphicObject * ArrowDest = Arrow->GetEndItem();

    if ( ( ArrowSource == this ) && ( ArrowDest == Dest ) )
    {
      return true;
    }
  }

  return false;
}

void dbse::SchemaGraphicObject::set_inherited_properties_visibility( bool visible ) {
  
  m_inherited_properties_visible = visible; 
  for ( SchemaGraphicSegmentedArrow * arrow : m_arrows )
    {
      arrow->UpdatePosition();
    }
}



QVariant dbse::SchemaGraphicObject::itemChange ( GraphicsItemChange change,
                                                 const QVariant & value )
{
  if ( change == ItemPositionChange ) 
    for ( SchemaGraphicSegmentedArrow * arrow : m_arrows )
    {
      arrow->UpdatePosition();
    }

  return value;
}

void dbse::SchemaGraphicObject::UpdateObject ( QString Name )
{
  if ( Name != m_class_object_name )
  {
    return;
  }

  /// Updating object info
  GetInfo();
  /// Updating object representation
  SchemaGraphicsScene * Scene = dynamic_cast<SchemaGraphicsScene *> ( scene() );

  if ( Scene )
  {
    QStringList ClassesList;
    ClassesList.append ( Name );

    QList<QPointF> ClassesPositions;
    QPointF ClassPosition ( this->scenePos() );
    ClassesPositions.append ( ClassPosition );

    Scene->RemoveClassObject ( this );
    Scene->AddItemsToScene ( QStringList ( m_class_object_name ), ClassesPositions );
  }

  /// Repainting object
  update();
}

void dbse::SchemaGraphicObject::RemoveObject ( QString Name )
{
  if ( Name != m_class_object_name )
  {
    return;
  }

  /// Updating object representation
  SchemaGraphicsScene * Scene = dynamic_cast<SchemaGraphicsScene *> ( scene() );

  if ( Scene )
  {
    Scene->RemoveClassObject ( this );
  }
}
