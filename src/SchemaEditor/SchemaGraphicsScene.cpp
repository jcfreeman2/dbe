/// Including QT Headers
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QWidget>
#include <QPainter>
#include <QMenu>
#include <QApplication>
/// Including Schema Editor
#include "SchemaGraphicsScene.h"
#include "SchemaGraphicObject.h"
#include "SchemaGraphicArrow.h"
#include "SchemaKernelWrapper.h"
#include "SchemaClassEditor.h"
#include "SchemaRelationshipEditor.h"

dbse::SchemaGraphicsScene::SchemaGraphicsScene ( QObject * parent )
  : QGraphicsScene ( parent ),
    line ( nullptr ),
    ContextMenu ( nullptr ),
    CurrentObject ( nullptr ),
    CurrentArrow ( nullptr )
{
  CreateActions();
  setSceneRect ( QRectF ( 0, 0, 10000, 10000 ) );
}

dbse::SchemaGraphicsScene::~SchemaGraphicsScene()
{
}

void dbse::SchemaGraphicsScene::CreateActions()
{
  AddClass = new QAction ( "&Add new class", this );
  AddClass->setShortcut ( tr ( "Ctrl+A" ) );
  AddClass->setShortcutContext ( Qt::WidgetShortcut );
  connect ( AddClass, SIGNAL ( triggered() ), this, SLOT ( AddClassSlot() ) );

  EditClass = new QAction ( "&Edit class", this );
  EditClass->setShortcut ( tr ( "Ctrl+E" ) );
  EditClass->setShortcutContext ( Qt::WidgetShortcut );
  connect ( EditClass, SIGNAL ( triggered() ), this, SLOT ( EditClassSlot() ) );

  RemoveClass = new QAction ( "&Remove Class from view", this );
  RemoveClass->setShortcut ( tr ( "Ctrl+R" ) );
  RemoveClass->setShortcutContext ( Qt::WidgetShortcut );
  connect ( RemoveClass, SIGNAL ( triggered() ), this, SLOT ( RemoveClassSlot() ) );

  RemoveArrow = new QAction ( "&Remove Arrow", this );
  RemoveArrow->setShortcut ( tr ( "Ctrl+R" ) );
  RemoveArrow->setShortcutContext ( Qt::WidgetShortcut );
  connect ( RemoveArrow, SIGNAL ( triggered() ), this, SLOT ( RemoveArrowSlot() ) );
}

void dbse::SchemaGraphicsScene::dragEnterEvent ( QGraphicsSceneDragDropEvent * event )
{
  if ( event->mimeData()->hasFormat ( "application/vnd.text.list" ) )
  {
    event->accept();
  }
}

void dbse::SchemaGraphicsScene::dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
{
  if ( event->mimeData()->hasFormat ( "application/vnd.text.list" ) )
  {
    event->accept();
  }
}

void dbse::SchemaGraphicsScene::dropEvent ( QGraphicsSceneDragDropEvent * event )
{
  QByteArray encodedData = event->mimeData()->data ( "application/vnd.text.list" );
  QDataStream stream ( &encodedData, QIODevice::ReadOnly );
  QStringList SchemaClasses;

  while ( !stream.atEnd() )
  {
    QString ClassName;
    stream >> ClassName;
    SchemaClasses.append ( ClassName );
  }

  QList<QPointF> Positions;

  for ( int i = 0; i < SchemaClasses.size(); ++i )
  {
    Positions.push_back ( event->scenePos() );
  }

  AddItemToScene ( SchemaClasses, Positions );
}

void dbse::SchemaGraphicsScene::contextMenuEvent ( QGraphicsSceneContextMenuEvent * event )
{
  if ( !KernelWrapper::GetInstance().IsActive() )
  {
    return;
  }

  if ( ContextMenu == nullptr )
  {
    ContextMenu = new QMenu();
    ContextMenu->addAction ( AddClass );
    ContextMenu->addAction ( EditClass );
    ContextMenu->addAction ( RemoveClass );
    ContextMenu->addAction ( RemoveArrow );
  }

  if ( !itemAt ( event->scenePos(), QTransform() ) )
  {
    ContextMenu->actions().at ( 0 )->setVisible ( true );
    ContextMenu->actions().at ( 1 )->setVisible ( false );
    ContextMenu->actions().at ( 2 )->setVisible ( false );
    ContextMenu->actions().at ( 3 )->setVisible ( false );
  }
  else
  {
    if ( dynamic_cast<SchemaGraphicObject *> ( itemAt ( event->scenePos(), QTransform() ) ) )
    {
      ContextMenu->actions().at ( 0 )->setVisible ( true );
      ContextMenu->actions().at ( 1 )->setVisible ( true );
      ContextMenu->actions().at ( 2 )->setVisible ( true );
      ContextMenu->actions().at ( 3 )->setVisible ( false );
      CurrentObject =
        dynamic_cast<SchemaGraphicObject *> ( itemAt ( event->scenePos(), QTransform() ) );
    }
    else if ( dynamic_cast<SchemaGraphicArrow *> ( itemAt ( event->scenePos(),
                                                            QTransform() ) ) )
    {
      ContextMenu->actions().at ( 0 )->setVisible ( false );
      ContextMenu->actions().at ( 1 )->setVisible ( false );
      ContextMenu->actions().at ( 2 )->setVisible ( false );
      ContextMenu->actions().at ( 3 )->setVisible ( true );
      CurrentArrow = dynamic_cast<SchemaGraphicArrow *> ( itemAt ( event->scenePos(),
                                                                   QTransform() ) );
    }
  }

  ContextMenu->exec ( event->screenPos() );
}

void dbse::SchemaGraphicsScene::AddItemToScene ( QStringList SchemaClasses,
                                                 QList<QPointF> Positions )
{
  for ( QString & ClassName : SchemaClasses )
  {
    if ( !ItemMap.contains ( ClassName ) )
    {
      SchemaGraphicObject * Object = new SchemaGraphicObject ( ClassName );
      Object->setPos ( Positions.at ( SchemaClasses.indexOf ( ClassName ) ) );
      addItem ( Object );
      /// Updating item list
      ItemMap.insert ( ClassName, Object );
    }
  }

  for ( QString & ClassName : ItemMap.keys() )
  {
    OksClass * ClassInfo = KernelWrapper::GetInstance().FindClass ( ClassName.toStdString() );

    const std::list<OksRelationship *> * DirectRelationshipList =
      ClassInfo->direct_relationships();
    const std::list<std::string *> * DirectSuperClassesList = ClassInfo->direct_super_classes();

    //// PLotting relationships
    if ( DirectRelationshipList != nullptr )
    {
      for ( OksRelationship * ClassRelationship : * ( DirectRelationshipList ) )
      {
        QString RelationshipClassType = QString::fromStdString (
                                          ClassRelationship->get_class_type()->get_name() );

        if ( ItemMap.contains ( RelationshipClassType ) && !ItemMap[ClassName]->HasArrow (
               ItemMap[RelationshipClassType] ) )
        {
          QString SchemaCardinality =
            KernelWrapper::GetInstance().GetCardinalityStringRelationship ( ClassRelationship );
          SchemaGraphicArrow * NewArrow = new SchemaGraphicArrow (
            ItemMap[ClassName], ItemMap[RelationshipClassType], false,
            ClassRelationship->get_is_composite(),
            QString::fromStdString ( ClassRelationship->get_name() ), SchemaCardinality );
          ItemMap[ClassName]->AddArrow ( NewArrow );
          ItemMap[RelationshipClassType]->AddArrow ( NewArrow );
          addItem ( NewArrow );
          //NewArrow->SetLabelScene(this);
          NewArrow->setZValue ( -1000.0 );
          NewArrow->UpdatePosition();
        }
      }
    }

    /// Plotting the superclasses
    if ( DirectSuperClassesList != nullptr )
    {
      for ( std::string * SuperClassNameStd : * ( DirectSuperClassesList ) )
      {
        QString SuperClassName = QString::fromStdString ( *SuperClassNameStd );

        if ( ItemMap.contains ( SuperClassName ) && !ItemMap[ClassName]->HasArrow (
               ItemMap[SuperClassName] ) )
        {
          SchemaGraphicArrow * NewArrow = new SchemaGraphicArrow ( ItemMap[ClassName],
                                                                   ItemMap[SuperClassName], true,
                                                                   false, "", "" );
          ItemMap[ClassName]->AddArrow ( NewArrow );
          ItemMap[SuperClassName]->AddArrow ( NewArrow );
          addItem ( NewArrow );
          //NewArrow->SetLabelScene(this);
          NewArrow->setZValue ( -1000.0 );
          NewArrow->UpdatePosition();
        }
      }
    }
  }
}

void dbse::SchemaGraphicsScene::RemoveClassObject ( SchemaGraphicObject * Object )
{
  if ( Object == nullptr )
  {
    return;
  }

  Object->RemoveArrows();
  removeItem ( Object );
  ItemMap.remove ( Object->GetClassName() );
}

void dbse::SchemaGraphicsScene::CleanItemMap()
{
  ItemMap.clear();
}

void dbse::SchemaGraphicsScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
  if ( mouseEvent->button() != Qt::LeftButton )
  {
    return;
  }

  if ( mouseEvent->widget()->cursor().shape() == Qt::CrossCursor )
  {
    line = new QGraphicsLineItem ( QLineF ( mouseEvent->scenePos(), mouseEvent->scenePos() ) );
    line->setPen ( QPen ( Qt::black, 2 ) );
    addItem ( line );
    return;
  }

  QGraphicsScene::mousePressEvent ( mouseEvent );
}

void dbse::SchemaGraphicsScene::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
  if ( line != nullptr )
  {
    QLineF newLine ( line->line().p1(), mouseEvent->scenePos() );
    line->setLine ( newLine );
  }
  else
  {
    QGraphicsScene::mouseMoveEvent ( mouseEvent );
  }
}

void dbse::SchemaGraphicsScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
  if ( line != nullptr )
  {
    QList<QGraphicsItem *> startItems = items ( line->line().p1() );

    if ( startItems.count() && startItems.first() == line )
    {
      startItems.removeFirst();
    }

    QList<QGraphicsItem *> endItems = items ( line->line().p2() );

    if ( endItems.count() && endItems.first() == line )
    {
      endItems.removeFirst();
    }

    removeItem ( line );
    delete line;

    if ( startItems.count() > 0 && endItems.count() > 0
         && startItems.first() != endItems.first() )
    {

      bool Inheritance = KernelWrapper::GetInstance().GetInheritanceMode();
      SchemaGraphicObject * startItem = qgraphicsitem_cast<SchemaGraphicObject *> (
                                          startItems.first() );
      SchemaGraphicObject * endItem = qgraphicsitem_cast<SchemaGraphicObject *> (
                                        endItems.first() );

      if ( Inheritance )
      {
        startItem->GetClass()->add_super_class ( endItem->GetClassName().toStdString() );
        /// Create arrow
        SchemaGraphicArrow * newArrow = new SchemaGraphicArrow ( startItem, endItem, Inheritance,
                                                                 true, "", "" );
        startItem->AddArrow ( newArrow );
        endItem->AddArrow ( newArrow );
        newArrow->setZValue ( -1000.0 );
        addItem ( newArrow );
        //newArrow->SetLabelScene(this);
        newArrow->UpdatePosition();
      }
      else
      {
        SchemaRelationshipEditor * Editor = new SchemaRelationshipEditor (
          startItem->GetClass(), endItem->GetClassName() );
        connect ( Editor, SIGNAL ( MakeGraphConnection ( QString, QString, QString ) ), this,
                  SLOT ( DrawArrow ( QString, QString, QString ) ) );
        Editor->show();
      }
    }
  }

  line = nullptr;
  QGraphicsScene::mouseReleaseEvent ( mouseEvent );
}

void dbse::SchemaGraphicsScene::AddClassSlot()
{
  SchemaClassEditor::createNewClass();
}

void dbse::SchemaGraphicsScene::EditClassSlot()
{
  bool WidgetFound = false;
  QString ClassName = QString::fromStdString ( CurrentObject->GetClass()->get_name() );

  for ( QWidget * Editor : QApplication::allWidgets() )
  {
    SchemaClassEditor * Widget = dynamic_cast<SchemaClassEditor *> ( Editor );

    if ( Widget != nullptr )
    {
      if ( ( Widget->objectName() ).compare ( ClassName ) == 0 )
      {
        Widget->raise();
        Widget->setVisible ( true );
        Widget->activateWindow();
        WidgetFound = true;
      }
    }
  }

  if ( !WidgetFound )
  {
    SchemaClassEditor * Editor = new SchemaClassEditor ( CurrentObject->GetClass() );
    Editor->show();
  }
}

void dbse::SchemaGraphicsScene::RemoveClassSlot()
{
  if ( CurrentObject == nullptr )
  {
    return;
  }

  CurrentObject->RemoveArrows();
  removeItem ( CurrentObject );
  ItemMap.remove ( CurrentObject->GetClassName() );
}

void dbse::SchemaGraphicsScene::RemoveArrowSlot()
{
  removeItem ( CurrentArrow );
  CurrentArrow->GetStartItem()->RemoveArrow ( CurrentArrow );
  CurrentArrow->GetEndItem()->RemoveArrow ( CurrentArrow );
  CurrentArrow->RemoveArrow();
}

void dbse::SchemaGraphicsScene::DrawArrow ( QString ClassName, QString RelationshipType,
                                            QString RelationshipName )
{
  if ( !ItemMap.contains ( ClassName ) || !ItemMap.contains ( RelationshipType ) )
  {
    return;
  }

  SchemaGraphicObject * startItem = ItemMap[ClassName];
  SchemaGraphicObject * endItem = ItemMap[RelationshipType];

  OksClass * SchemaClass = KernelWrapper::GetInstance().FindClass ( ClassName.toStdString() );
  OksRelationship * SchemaRelationship = SchemaClass->find_direct_relationship (
                                           RelationshipName.toStdString() );

  if ( SchemaRelationship != nullptr )
  {
    QString RelationshipCardinality =
      KernelWrapper::GetInstance().GetCardinalityStringRelationship ( SchemaRelationship );
    SchemaGraphicArrow * newArrow = new SchemaGraphicArrow (
      startItem, endItem, false, SchemaRelationship->get_is_composite(),
      QString::fromStdString ( SchemaRelationship->get_name() ), RelationshipCardinality );
    startItem->AddArrow ( newArrow );
    endItem->AddArrow ( newArrow );
    newArrow->setZValue ( -1000.0 );
    addItem ( newArrow );
    //newArrow->SetLabelScene(this);
    newArrow->UpdatePosition();
  }
}
