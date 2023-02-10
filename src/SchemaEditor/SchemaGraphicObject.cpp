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

dbse::SchemaGraphicObject::SchemaGraphicObject ( QString & ClassName,
                                                 QGraphicsObject * parent )
  : QGraphicsObject ( parent ),
    LineOffsetX ( 0 ),
    LineOffsetY ( 0 )
{
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

  const std::list<OksAttribute *> * DirectAttributes = ClassInfo->direct_attributes();
  const std::list<OksMethod *> * DirectMethods = ClassInfo->direct_methods();

  if ( DirectAttributes != nullptr )
  {
    /// Getting All Attributes
    for ( OksAttribute * Attribute : * ( DirectAttributes ) )
    {
      QString AttributeString (
        QString::fromStdString ( Attribute->get_name() ) + " : "
        + QString::fromStdString ( Attribute->get_type() ) );
      ClassAttributes.append ( AttributeString );
    }
  }

  if ( DirectMethods != nullptr )
  {
    /// Getting All Methods
    for ( OksMethod * Method : * ( DirectMethods ) )
    {
      QString MethodString ( QString::fromStdString ( Method->get_name() ) + "()" );
      ClassMethods.append ( MethodString );
    }
  }
}

QRectF dbse::SchemaGraphicObject::boundingRect() const
{
  double SpaceX = 3;
  double TotalBoundingHeight = 0;
  double TotalBoundingWidth = 0;

  QFont Font ( "Helvetica [Cronyx]", 10 );
  QFontMetrics FontMetrics ( Font );

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

  for ( auto & MethodName : ClassMethods )
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

  double SpaceX = 1;
  double SpaceY = 3;

  QFont Font ( "Helvetica [Cronyx]", 10 );
  painter->setFont ( Font );
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

  HeightOffset += SpaceY;
  painter->drawLine ( 0, HeightOffset, ObjectBoundingRect.width(), HeightOffset );

  for ( QString & MethodName : ClassMethods )
  {
    QRectF AttributeBoundingRect = FontMetrics.boundingRect ( MethodName );
    HeightOffset += AttributeBoundingRect.height();
    painter->drawText ( SpaceX, HeightOffset, MethodName );
  }
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
