/// Including QT Headers
#include <QPainter>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsView>
/// Include oks
#include"oks/method.hpp"
/// Including SchemaEditor
#include "dbe/SchemaGraphicObject.hpp"
#include "dbe/SchemaGraphicArrow.hpp"
#include "dbe/SchemaGraphicsScene.hpp"
#include "dbe/SchemaKernelWrapper.hpp"

using namespace dunedaq::oks;

dbse::SchemaGraphicObject::SchemaGraphicObject ( QString & ClassName,
                                                 QGraphicsObject * parent )
  : QGraphicsObject ( parent ),
    LineOffsetX ( 0 ),
    LineOffsetY ( 0 )
{
  m_font = QFont( "Helvetica [Cronyx]", 9 );

  setFlag ( ItemIsMovable );
  setFlag ( ItemSendsGeometryChanges, true );
  setFlag ( ItemSendsScenePositionChanges, true );

  /// Connecting Signals
  connect ( &KernelWrapper::GetInstance(), SIGNAL ( ClassUpdated ( QString ) ), this,
            SLOT ( UpdateObject ( QString ) ) );
  connect ( &KernelWrapper::GetInstance(), SIGNAL ( ClassRemoved ( QString ) ), this,
            SLOT ( RemoveObject ( QString ) ) );
  /// Getting Class
  ClassInfo = KernelWrapper::GetInstance().FindClass ( ClassName.toStdString() );
  ClassObjectName = ClassName;
  /// Getting class info
  GetInfo();
}

dbse::SchemaGraphicObject::~SchemaGraphicObject()
{
}

OksClass * dbse::SchemaGraphicObject::GetClass() const
{
  return ClassInfo;
}

QString dbse::SchemaGraphicObject::GetClassName() const
{
  return ClassObjectName;
}

void dbse::SchemaGraphicObject::GetInfo()
{
  ClassAttributes.clear();
  ClassMethods.clear();

  std::list<OksAttribute *> direct_attributes = {};
  if (ClassInfo->direct_attributes()) direct_attributes = *ClassInfo->direct_attributes();

  std::list<OksMethod *> direct_methods = {};
  if (ClassInfo->direct_methods()) direct_methods = *ClassInfo->direct_methods();

  std::list<OksRelationship *> direct_relationships = {};
  if (ClassInfo->direct_relationships()) direct_relationships = *ClassInfo->direct_relationships();

  std::list<OksAttribute *> all_attributes = {};
  if (ClassInfo->all_attributes()) all_attributes = *ClassInfo->all_attributes();

  std::list<OksMethod *> all_methods = {};
  if (ClassInfo->all_methods()) all_methods = *ClassInfo->all_methods();

  std::list<OksRelationship *> all_relationships = {};
  if (ClassInfo->all_relationships()) all_relationships = *ClassInfo->all_relationships();

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

  /// Getting All Attributes
  for ( OksAttribute * Attribute : direct_attributes )
  {
    QString AttributeString (
      QString::fromStdString ( Attribute->get_name() ) + " : "
      + QString::fromStdString ( Attribute->get_type() ) );
    ClassAttributes.append ( AttributeString );
  }

  /// Getting All Relationships
  for ( OksRelationship * relationship : direct_relationships )
  {
    QString reltionship_string ( QString::fromStdString ( relationship->get_name() ) + " : " 
    + QString::fromStdString ( relationship->get_type() ) + " - "
    + QString::fromStdString ( m[ relationship->get_low_cardinality_constraint() ] ) + ":"
    + QString::fromStdString ( m[ relationship->get_high_cardinality_constraint() ] )  );
    m_class_relationhips.append ( reltionship_string );
  }

  /// Getting All Methods
  for ( OksMethod * Method : direct_methods )
  {
    QString MethodString ( QString::fromStdString ( Method->get_name() ) + "()" );
    ClassMethods.append ( MethodString );
  }



  /// Getting All Attributes
  for ( OksAttribute * attribute : indirect_attributes )
  {
    QString attribute_string (
      QString::fromStdString ( attribute->get_name() ) + " : "
      + QString::fromStdString ( attribute->get_type() ) );
    m_class_indirect_attributes.append ( attribute_string );
  }

  /// Getting All Relationships
  for ( OksRelationship * relationship : indirect_relationships )
  {
    QString reltionship_string ( QString::fromStdString ( relationship->get_name() ) + " : " 
    + QString::fromStdString ( relationship->get_type() ) + " - "
    + QString::fromStdString ( m[ relationship->get_low_cardinality_constraint() ] ) + ":"
    + QString::fromStdString ( m[ relationship->get_high_cardinality_constraint() ] )  );
    m_class_indirect_relationhips.append ( reltionship_string );
  }

  /// Getting All Methods
  for ( OksMethod * method : indirect_methods )
  {
    QString method_string ( QString::fromStdString ( method->get_name() ) + "()" );
    m_class_indirect_methods.append ( method_string );
  }


}

QRectF dbse::SchemaGraphicObject::boundingRect() const
{
  double SpaceX = 3;
  double TotalBoundingHeight = 0;
  double TotalBoundingWidth = 0;

  QFontMetrics FontMetrics ( m_font );

  TotalBoundingHeight += SpaceX * 5;
  TotalBoundingHeight += FontMetrics.boundingRect ( ClassObjectName ).height();
  TotalBoundingWidth += FontMetrics.boundingRect ( ClassObjectName ).width();

  for ( auto & AttributeName : ClassAttributes )
  {
    TotalBoundingHeight += FontMetrics.boundingRect ( AttributeName ).height();

    if ( FontMetrics.boundingRect ( AttributeName ).width() > TotalBoundingWidth )
      TotalBoundingWidth =
        FontMetrics.boundingRect ( AttributeName ).width();
  }


  for ( auto & AttributeName : m_class_indirect_attributes )
  {
    TotalBoundingHeight += FontMetrics.boundingRect ( AttributeName ).height();

    if ( FontMetrics.boundingRect ( AttributeName ).width() > TotalBoundingWidth )
      TotalBoundingWidth =
        FontMetrics.boundingRect ( AttributeName ).width();
  }

  for ( auto & relationship_name : m_class_relationhips )
  {
    TotalBoundingHeight += FontMetrics.boundingRect ( relationship_name ).height();

    if ( FontMetrics.boundingRect ( relationship_name ).width() > TotalBoundingWidth )
      TotalBoundingWidth =
        FontMetrics.boundingRect ( relationship_name ).width();
  }

  for ( auto & relationship_name : m_class_indirect_relationhips )
  {
    TotalBoundingHeight += FontMetrics.boundingRect ( relationship_name ).height();

    if ( FontMetrics.boundingRect ( relationship_name ).width() > TotalBoundingWidth )
      TotalBoundingWidth =
        FontMetrics.boundingRect ( relationship_name ).width();
  }

  for ( auto & MethodName : ClassMethods )
  {
    TotalBoundingHeight += FontMetrics.boundingRect ( MethodName ).height();

    if ( FontMetrics.boundingRect ( MethodName ).width() > TotalBoundingWidth )
      TotalBoundingWidth =
        FontMetrics.boundingRect ( MethodName ).width();
  }

  for ( auto & MethodName : m_class_indirect_methods )
  {
    TotalBoundingHeight += FontMetrics.boundingRect ( MethodName ).height();

    if ( FontMetrics.boundingRect ( MethodName ).width() > TotalBoundingWidth )
      TotalBoundingWidth =
        FontMetrics.boundingRect ( MethodName ).width();
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

  painter->setFont ( m_font );
  painter->setPen ( QColor ( "blue" ) );
  painter->drawRect ( boundingRect() );

  QFontMetrics FontMetrics = painter->fontMetrics();
  QRectF ClassNameBoundingRect = FontMetrics.boundingRect ( ClassObjectName );
  QRectF ObjectBoundingRect = boundingRect();

  double HeightOffset = ClassNameBoundingRect.height() + SpaceY;
  int ClassNamePosition = ( ObjectBoundingRect.width() - ClassNameBoundingRect.width() ) / 2;
  painter->drawText ( ClassNamePosition, ClassNameBoundingRect.height(), ClassObjectName );
  painter->drawLine ( 0, HeightOffset, ObjectBoundingRect.width(), HeightOffset );

  for ( QString & AttributeName : ClassAttributes )
  {
    QRectF AttributeBoundingRect = FontMetrics.boundingRect ( AttributeName );
    HeightOffset += AttributeBoundingRect.height();
    painter->drawText ( SpaceX, HeightOffset, AttributeName );
  }

  painter->setPen ( QColor ( "black" ) );
  for ( QString & AttributeName : m_class_indirect_attributes )
  {
    QRectF AttributeBoundingRect = FontMetrics.boundingRect ( AttributeName );
    HeightOffset += AttributeBoundingRect.height();
    painter->drawText ( SpaceX, HeightOffset, AttributeName );
  }
  painter->setPen ( QColor ( "blue" ) );


  HeightOffset += SpaceY;
  painter->drawLine ( 0, HeightOffset, ObjectBoundingRect.width(), HeightOffset );

  for ( QString & relationship_name : m_class_relationhips )
  {
    QRectF relationship_bounding_rect = FontMetrics.boundingRect ( relationship_name );
    HeightOffset += relationship_bounding_rect.height();
    painter->drawText ( SpaceX, HeightOffset, relationship_name );
  }

  painter->setPen ( QColor ( "black" ) );
  for ( QString & relationship_name : m_class_indirect_relationhips )
  {
    QRectF relationship_bounding_rect = FontMetrics.boundingRect ( relationship_name );
    HeightOffset += relationship_bounding_rect.height();
    painter->drawText ( SpaceX, HeightOffset, relationship_name );
  }
  painter->setPen ( QColor ( "blue" ) );



  HeightOffset += SpaceY;
  painter->drawLine ( 0, HeightOffset, ObjectBoundingRect.width(), HeightOffset );
  
  for ( QString & MethodName : ClassMethods )
  {
    QRectF AttributeBoundingRect = FontMetrics.boundingRect ( MethodName );
    HeightOffset += AttributeBoundingRect.height();
    painter->drawText ( SpaceX, HeightOffset, MethodName );
  }

  painter->setPen ( QColor ( "black" ) );
  for ( QString & MethodName : m_class_indirect_methods )
  {
    QRectF AttributeBoundingRect = FontMetrics.boundingRect ( MethodName );
    HeightOffset += AttributeBoundingRect.height();
    painter->drawText ( SpaceX, HeightOffset, MethodName );
  }
  painter->setPen ( QColor ( "blue" ) );
}

void dbse::SchemaGraphicObject::AddArrow ( SchemaGraphicArrow * Arrow )
{
  Arrows.append ( Arrow );
}

void dbse::SchemaGraphicObject::RemoveArrow ( SchemaGraphicArrow * Arrow )
{
  int index = Arrows.indexOf ( Arrow );

  if ( index != -1 )
  {
    Arrows.removeAt ( index );
  }
}

void dbse::SchemaGraphicObject::RemoveArrows()
{
  foreach ( SchemaGraphicArrow * arrow, Arrows )
  {
    arrow->GetStartItem()->RemoveArrow ( arrow );
    arrow->GetEndItem()->RemoveArrow ( arrow );
    scene()->removeItem ( arrow );
  }
}

bool dbse::SchemaGraphicObject::HasArrow ( SchemaGraphicObject * Dest ) const
{
  if ( Arrows.isEmpty() )
  {
    return false;
  }

  for ( SchemaGraphicArrow * Arrow : Arrows )
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

QVariant dbse::SchemaGraphicObject::itemChange ( GraphicsItemChange change,
                                                 const QVariant & value )
{
  if ( change == ItemPositionChange ) for ( SchemaGraphicArrow * arrow : Arrows )
    {
      arrow->UpdatePosition();
    }

  return value;
}

void dbse::SchemaGraphicObject::UpdateObject ( QString Name )
{
  if ( Name != ClassObjectName )
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
    Scene->AddItemToScene ( QStringList ( ClassObjectName ), ClassesPositions );
  }

  /// Repainting object
  update();
}

void dbse::SchemaGraphicObject::RemoveObject ( QString Name )
{
  if ( Name != ClassObjectName )
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
