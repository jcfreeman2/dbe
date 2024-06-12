/// Including QT Headers
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QWidget>
#include <QPainter>
#include <QMenu>
#include <QApplication>
/// Including Schema Editor
#include "dbe/SchemaGraphicsScene.hpp"
#include "dbe/SchemaGraphicObject.hpp"
#include "dbe/SchemaGraphicSegmentedArrow.hpp"
#include "dbe/SchemaKernelWrapper.hpp"
#include "dbe/SchemaClassEditor.hpp"
#include "dbe/SchemaRelationshipEditor.hpp"

using namespace dunedaq::oks;

dbse::SchemaGraphicsScene::SchemaGraphicsScene ( QObject * parent )
  : QGraphicsScene ( parent ),
    line ( nullptr ),
    m_context_menu ( nullptr ),
    CurrentObject ( nullptr ),
    m_current_arrow ( nullptr ),
    m_inherited_properties_visible(false)
{
  CreateActions();
  setSceneRect ( QRectF ( 0, 0, 10000, 10000 ) );
}

dbse::SchemaGraphicsScene::~SchemaGraphicsScene()
{
}

void dbse::SchemaGraphicsScene::CreateActions()
{
  // Add new class
  AddClass = new QAction ( "&Add new class", this );
  AddClass->setShortcut ( tr ( "Ctrl+A" ) );
  AddClass->setShortcutContext ( Qt::WidgetShortcut );
  connect ( AddClass, SIGNAL ( triggered() ), this, SLOT ( AddClassSlot() ) );

  // Edit current class
  EditClass = new QAction ( "&Edit class", this );
  EditClass->setShortcut ( tr ( "Ctrl+E" ) );
  EditClass->setShortcutContext ( Qt::WidgetShortcut );
  connect ( EditClass, SIGNAL ( triggered() ), this, SLOT ( EditClassSlot() ) );

  // Show superclasses of the current class
  m_toggle_indirect_infos = new QAction ( "&Toggle inherited properties", this );
  m_toggle_indirect_infos->setShortcut ( tr ( "Ctrl+T" ) );
  m_toggle_indirect_infos->setShortcutContext ( Qt::WidgetShortcut );
  connect ( m_toggle_indirect_infos, SIGNAL ( triggered() ), this, SLOT ( ToggleIndirectInfos() ) );

  // Show superclasses of the current class
  m_add_direct_super_classes = new QAction ( "Add direct &superclasses to view", this );
  m_add_direct_super_classes->setShortcut ( tr ( "Ctrl+S" ) );
  m_add_direct_super_classes->setShortcutContext ( Qt::WidgetShortcut );
  connect ( m_add_direct_super_classes, SIGNAL ( triggered() ), this, SLOT ( AddDirectSuperClassesSlot() ) );

  // Show relationship classes of the current clas
  m_add_direct_relationship_classes = new QAction ( "Add &direct relationship classes to view", this );
  m_add_direct_relationship_classes->setShortcut ( tr ( "Ctrl+D" ) );
  m_add_direct_relationship_classes->setShortcutContext ( Qt::WidgetShortcut );
  connect ( m_add_direct_relationship_classes, SIGNAL ( triggered() ), this, SLOT ( AddDirectRelationshipClassesSlot() ) );
  
  // Show superclasses of the current class
  m_add_all_super_classes = new QAction ( "Add all &superclasses to view", this );
  m_add_all_super_classes->setShortcut ( tr ( "Ctrl+S" ) );
  m_add_all_super_classes->setShortcutContext ( Qt::WidgetShortcut );
  connect ( m_add_all_super_classes, SIGNAL ( triggered() ), this, SLOT ( AddAllSuperClassesSlot() ) );

  // Show subclasses of the current clas
  m_add_all_sub_classes = new QAction ( "Add all s&ubclasses to view", this );
  m_add_all_sub_classes->setShortcut ( tr ( "Ctrl+S" ) );
  m_add_all_sub_classes->setShortcutContext ( Qt::WidgetShortcut );
  connect ( m_add_all_sub_classes, SIGNAL ( triggered() ), this, SLOT ( AddAllSubClassesSlot() ) );

  // Show indirect relationship classes of the current clas
  m_add_all_relationship_classes = new QAction ( "Add a&ll relationship classes to view", this );
  m_add_all_relationship_classes->setShortcut ( tr ( "Ctrl+D" ) );
  m_add_all_relationship_classes->setShortcutContext ( Qt::WidgetShortcut );
  connect ( m_add_all_relationship_classes, SIGNAL ( triggered() ), this, SLOT ( AddAllRelationshipClassesSlot() ) );

  // Remove class
  RemoveClass = new QAction ( "&Remove Class from view", this );
  RemoveClass->setShortcut ( tr ( "Ctrl+R" ) );
  RemoveClass->setShortcutContext ( Qt::WidgetShortcut );
  connect ( RemoveClass, SIGNAL ( triggered() ), this, SLOT ( RemoveClassSlot() ) );

  // Remove arrow
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

  if ( m_context_menu == nullptr )
  {
    m_context_menu = new QMenu();
    m_context_menu->addAction ( AddClass );
    m_context_menu->addAction ( m_toggle_indirect_infos );
    m_context_menu->addSeparator();
    m_context_menu->addAction ( EditClass );
    m_context_menu->addAction ( m_add_direct_super_classes );
    m_context_menu->addAction ( m_add_direct_relationship_classes );
    m_context_menu->addAction ( m_add_all_super_classes );
    m_context_menu->addAction ( m_add_all_sub_classes );
    m_context_menu->addAction ( m_add_all_sub_classes );
    m_context_menu->addAction ( m_add_all_relationship_classes );
    m_context_menu->addAction ( RemoveClass );
    m_context_menu->addAction ( RemoveArrow );
  }

  if ( !itemAt ( event->scenePos(), QTransform() ) )
  {
    m_context_menu->actions().at ( 0 )->setVisible ( true );
    m_context_menu->actions().at ( 1 )->setVisible ( true );
    m_context_menu->actions().at ( 2 )->setVisible ( false );
    m_context_menu->actions().at ( 3 )->setVisible ( false );
    m_context_menu->actions().at ( 4 )->setVisible ( false );
    m_context_menu->actions().at ( 5 )->setVisible ( false );
    m_context_menu->actions().at ( 6 )->setVisible ( false );
    m_context_menu->actions().at ( 7 )->setVisible ( false );
    m_context_menu->actions().at ( 8 )->setVisible ( false );
    m_context_menu->actions().at ( 9 )->setVisible ( false );
    m_context_menu->actions().at ( 10 )->setVisible ( false );
  }
  else
  {
    if ( dynamic_cast<SchemaGraphicObject *> ( itemAt ( event->scenePos(), QTransform() ) ) )
    {
      m_context_menu->actions().at ( 0 )->setVisible ( true );
      m_context_menu->actions().at ( 1 )->setVisible ( true );
      m_context_menu->actions().at ( 2 )->setVisible ( true );
      m_context_menu->actions().at ( 3 )->setVisible ( true );
      m_context_menu->actions().at ( 4 )->setVisible ( true );
      m_context_menu->actions().at ( 5 )->setVisible ( true );
      m_context_menu->actions().at ( 6 )->setVisible ( true );
      m_context_menu->actions().at ( 7 )->setVisible ( true );
      m_context_menu->actions().at ( 8 )->setVisible ( true );
      m_context_menu->actions().at ( 9 )->setVisible ( true );
      m_context_menu->actions().at ( 10 )->setVisible ( false );

      CurrentObject =
        dynamic_cast<SchemaGraphicObject *> ( itemAt ( event->scenePos(), QTransform() ) );
    }
    else if ( dynamic_cast<SchemaGraphicSegmentedArrow *> ( itemAt ( event->scenePos(),
                                                            QTransform() ) ) )
    {
      m_context_menu->actions().at ( 0 )->setVisible ( false );
      m_context_menu->actions().at ( 1 )->setVisible ( false );
      m_context_menu->actions().at ( 1 )->setVisible ( false );
      m_context_menu->actions().at ( 2 )->setVisible ( false );
      m_context_menu->actions().at ( 3 )->setVisible ( false );
      m_context_menu->actions().at ( 4 )->setVisible ( false );
      m_context_menu->actions().at ( 5 )->setVisible ( false );
      m_context_menu->actions().at ( 6 )->setVisible ( false );
      m_context_menu->actions().at ( 7 )->setVisible ( false );
      m_context_menu->actions().at ( 8 )->setVisible ( false );
      m_context_menu->actions().at ( 9 )->setVisible ( false );
      m_context_menu->actions().at ( 10 )->setVisible ( true );
      m_current_arrow = dynamic_cast<SchemaGraphicSegmentedArrow *> ( itemAt ( event->scenePos(),
                                                                   QTransform() ) );
    }
  }

  m_context_menu->exec ( event->screenPos() );
}

void dbse::SchemaGraphicsScene::AddItemToScene ( QStringList SchemaClasses,
                                                 QList<QPointF> Positions )
{
  for ( QString & ClassName : SchemaClasses )
  {
    if ( !ItemMap.contains ( ClassName ) )
    {

      if ( !KernelWrapper::GetInstance().FindClass ( ClassName.toStdString() ) ) {
          std::cout << "ERROR: class " << ClassName.toStdString()  << " not found" << std::endl;
          continue;
      } else {
        auto k = KernelWrapper::GetInstance().FindClass ( ClassName.toStdString());
        std::cout << "Adding class " << ClassName.toStdString() << " " << (void*)k << std::endl;
      }

      SchemaGraphicObject * Object = new SchemaGraphicObject ( ClassName );
      Object->setPos ( Positions.at ( SchemaClasses.indexOf ( ClassName ) ) );
      Object->set_inherited_properties_visibility(m_inherited_properties_visible);
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
          SchemaGraphicSegmentedArrow * NewArrow = new SchemaGraphicSegmentedArrow (
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
          SchemaGraphicSegmentedArrow * NewArrow = new SchemaGraphicSegmentedArrow ( ItemMap[ClassName],
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
        SchemaGraphicSegmentedArrow * newArrow = new SchemaGraphicSegmentedArrow ( startItem, endItem, Inheritance,
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

void dbse::SchemaGraphicsScene::ToggleIndirectInfos() {
  m_inherited_properties_visible = !m_inherited_properties_visible;

  for ( SchemaGraphicObject * item : ItemMap.values() ) {
    item->set_inherited_properties_visibility(m_inherited_properties_visible);
  }

  this->update();

}

void dbse::SchemaGraphicsScene::AddDirectSuperClassesSlot() {

  QString class_name = QString::fromStdString ( CurrentObject->GetClass()->get_name() );
  OksClass * class_info = KernelWrapper::GetInstance().FindClass ( class_name.toStdString() );
  
  QStringList super_class_list;
  QList<QPointF> positions;

  const std::list<std::string *>* direct_classes = class_info->direct_super_classes();
  if(direct_classes != nullptr) {
      for(std::string * cl_name : *direct_classes) {
          super_class_list.push_back(QString::fromStdString(*cl_name));
          positions.push_back({0,0});
      }
  }

  this->AddItemToScene ( super_class_list, positions );

}

void dbse::SchemaGraphicsScene::AddAllSuperClassesSlot() {

  QString class_name = QString::fromStdString ( CurrentObject->GetClass()->get_name() );
  OksClass * class_info = KernelWrapper::GetInstance().FindClass ( class_name.toStdString() );
  
  QStringList super_class_list;
  QList<QPointF> positions;

  const OksClass::FList* all_classes = class_info->all_super_classes();
  if(all_classes != nullptr) {
      for(const OksClass* cl : *all_classes) {
          super_class_list.push_back(QString::fromStdString(cl->get_name()));
          positions.push_back({0,0});
      }
  }


  this->AddItemToScene ( super_class_list, positions );

}

void dbse::SchemaGraphicsScene::AddAllSubClassesSlot() {

  QString class_name = QString::fromStdString ( CurrentObject->GetClass()->get_name() );
  OksClass * class_info = KernelWrapper::GetInstance().FindClass ( class_name.toStdString() );
  
  QStringList sub_class_list;
  QList<QPointF> positions;

  const OksClass::FList* all_classes = class_info->all_sub_classes();
  if(all_classes != nullptr) {
      for(const OksClass* cl : *all_classes) {
          sub_class_list.push_back(QString::fromStdString(cl->get_name()));
          positions.push_back({0,0});
      }
  }

  this->AddItemToScene ( sub_class_list, positions );

}

void dbse::SchemaGraphicsScene::AddDirectRelationshipClassesSlot() {

  QString class_name = QString::fromStdString ( CurrentObject->GetClass()->get_name() );
  OksClass * class_info = KernelWrapper::GetInstance().FindClass ( class_name.toStdString() );
  
  QStringList relationship_classes;
  QList<QPointF> positions;

  const std::list<OksRelationship *> * direct_relationship_list = class_info->direct_relationships();
  if ( direct_relationship_list != nullptr ) {
      for(const OksRelationship* rl : *direct_relationship_list) {
          relationship_classes.push_back(QString::fromStdString(rl->get_type()));
          positions.push_back({0,0});
      }

  }

  this->AddItemToScene ( relationship_classes, positions );

}

void dbse::SchemaGraphicsScene::AddAllRelationshipClassesSlot() {

  QString class_name = QString::fromStdString ( CurrentObject->GetClass()->get_name() );
  OksClass * class_info = KernelWrapper::GetInstance().FindClass ( class_name.toStdString() );
  
  QStringList relationship_classes;
  QList<QPointF> positions;

  const std::list<OksRelationship *> * all_relationship_list = class_info->all_relationships();
  if ( all_relationship_list != nullptr ) {
      for(const OksRelationship* rl : *all_relationship_list) {
          relationship_classes.push_back(QString::fromStdString(rl->get_type()));
          positions.push_back({0,0});
      }

  }

  this->AddItemToScene ( relationship_classes, positions );
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
  removeItem ( m_current_arrow );
  m_current_arrow->GetStartItem()->RemoveArrow ( m_current_arrow );
  m_current_arrow->GetEndItem()->RemoveArrow ( m_current_arrow );
  m_current_arrow->RemoveArrow();
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
    SchemaGraphicSegmentedArrow * newArrow = new SchemaGraphicSegmentedArrow (
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
