/// Including QT Headers
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QWidget>

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
    m_line ( nullptr ),
    m_context_menu ( nullptr ),
    CurrentObject ( nullptr ),
    m_current_arrow ( nullptr ),
    m_inherited_properties_visible(false),
    m_modified(false)
{
  CreateActions();
  setSceneRect ( QRectF ( 0, 0, 10000, 10000 ) );
}

dbse::SchemaGraphicsScene::~SchemaGraphicsScene()
{
}

void dbse::SchemaGraphicsScene::CreateActions()
{
  // Edit current class
  EditClass = new QAction ( "&Edit class", this );
  connect ( EditClass, SIGNAL ( triggered() ), this, SLOT ( EditClassSlot() ) );

  // Toggle inherited properties of all classes in view
  m_toggle_indirect_infos = new QAction ( "&Toggle inherited properties", this );
  connect ( m_toggle_indirect_infos, SIGNAL ( triggered() ), this, SLOT ( ToggleIndirectInfos() ) );

  // Show superclasses of the current class
  m_add_direct_super_classes = new QAction ( "Add direct &superclasses to view", this );
  connect ( m_add_direct_super_classes, SIGNAL ( triggered() ), this, SLOT ( AddDirectSuperClassesSlot() ) );

  // Show relationship classes of the current clas
  m_add_direct_relationship_classes = new QAction ( "Add &direct relationship classes to view", this );
  connect ( m_add_direct_relationship_classes, SIGNAL ( triggered() ), this, SLOT ( AddDirectRelationshipClassesSlot() ) );
  
  // Show superclasses of the current class
  m_add_all_super_classes = new QAction ( "Add all &superclasses to view", this );
  connect ( m_add_all_super_classes, SIGNAL ( triggered() ), this, SLOT ( AddAllSuperClassesSlot() ) );

  // Show subclasses of the current clas
  m_add_all_sub_classes = new QAction ( "Add all s&ubclasses to view", this );
  connect ( m_add_all_sub_classes, SIGNAL ( triggered() ), this, SLOT ( AddAllSubClassesSlot() ) );

  // Show indirect relationship classes of the current class
  m_add_all_relationship_classes = new QAction ( "Add a&ll relationship classes to view", this );
  connect ( m_add_all_relationship_classes, SIGNAL ( triggered() ), this, SLOT ( AddAllRelationshipClassesSlot() ) );

  // Remove class
  RemoveClass = new QAction ( "&Remove Class from view", this );
  connect ( RemoveClass, SIGNAL ( triggered() ), this, SLOT ( RemoveClassSlot() ) );

  // Remove arrow
  RemoveArrow = new QAction ( "&Remove Arrow", this );
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

  AddItemsToScene ( SchemaClasses, Positions );
}

void dbse::SchemaGraphicsScene::contextMenuEvent ( QGraphicsSceneContextMenuEvent * event )
{
  if ( m_context_menu == nullptr )
  {
    m_context_menu = new QMenu();
    // m_context_menu->addAction ( AddClass );
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
    m_context_menu->actions().at ( 1 )->setVisible ( false );
    m_context_menu->actions().at ( 2 )->setVisible ( false );
    m_context_menu->actions().at ( 3 )->setVisible ( false );
    m_context_menu->actions().at ( 4 )->setVisible ( false );
    m_context_menu->actions().at ( 5 )->setVisible ( false );
    m_context_menu->actions().at ( 6 )->setVisible ( false );
    m_context_menu->actions().at ( 7 )->setVisible ( false );
    m_context_menu->actions().at ( 8 )->setVisible ( false );
    m_context_menu->actions().at ( 9 )->setVisible ( false );
  }
  else
  {
    if ( dynamic_cast<SchemaGraphicObject *> ( itemAt ( event->scenePos(), QTransform() ) ) )
    {
      CurrentObject =
        dynamic_cast<SchemaGraphicObject *> ( itemAt ( event->scenePos(), QTransform() ) );
      auto filename =
        CurrentObject->GetClass()->get_file()->get_full_file_name();
      bool writable = KernelWrapper::GetInstance().IsFileWritable ( filename );
      m_context_menu->actions().at ( 0 )->setVisible ( true );
      m_context_menu->actions().at ( 1 )->setVisible ( true );
      if ( writable ) {
        m_context_menu->actions().at ( 2 )->setVisible ( true );
      }
      else {
        m_context_menu->actions().at ( 2 )->setVisible ( false );
      }
      m_context_menu->actions().at ( 3 )->setVisible ( true );
      m_context_menu->actions().at ( 4 )->setVisible ( true );
      m_context_menu->actions().at ( 5 )->setVisible ( true );
      m_context_menu->actions().at ( 6 )->setVisible ( true );
      m_context_menu->actions().at ( 7 )->setVisible ( true );
      m_context_menu->actions().at ( 8 )->setVisible ( true );
      m_context_menu->actions().at ( 9 )->setVisible ( false );

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
      m_context_menu->actions().at ( 9 )->setVisible ( true );
      m_current_arrow =
        dynamic_cast<SchemaGraphicSegmentedArrow *> ( itemAt ( event->scenePos(),
                                                               QTransform() ) );
    }
  }

  m_context_menu->exec ( event->screenPos() );
}

QStringList dbse::SchemaGraphicsScene::AddItemsToScene (
  QStringList SchemaClasses,
  QList<QPointF> Positions )
{
  QStringList missingItems{};

  for ( QString & ClassName : SchemaClasses )
  {
    if ( !ItemMap.contains ( ClassName ) )
    {

      if ( !KernelWrapper::GetInstance().FindClass ( ClassName.toStdString() ) ) {
          std::cout << "ERROR: class " << ClassName.toStdString()  << " not found" << std::endl;
          missingItems.append(ClassName);
          continue;
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

    int arrow_num=0;
    //// PLotting relationships
    if ( DirectRelationshipList != nullptr )
    {
      for ( OksRelationship * ClassRelationship : * ( DirectRelationshipList ) )
      {
        QString RelationshipClassType = QString::fromStdString (
                                          ClassRelationship->get_class_type()->get_name() );

        if ( ItemMap.contains ( RelationshipClassType ) ) //&& !ItemMap[ClassName]->HasArrow (
          //ItemMap[RelationshipClassType] ) )
        {
          QString SchemaCardinality =
            KernelWrapper::GetInstance().GetCardinalityStringRelationship ( ClassRelationship ) + " ";
          SchemaGraphicSegmentedArrow * NewArrow = new SchemaGraphicSegmentedArrow (
            ItemMap[ClassName], ItemMap[RelationshipClassType],
            arrow_num,
            false,
            ClassRelationship->get_is_composite(),
            QString::fromStdString ( ClassRelationship->get_name() ), SchemaCardinality );
          ItemMap[ClassName]->AddArrow ( NewArrow );
          ItemMap[RelationshipClassType]->AddArrow ( NewArrow );
          addItem ( NewArrow );
          //NewArrow->SetLabelScene(this);
          NewArrow->setZValue ( -1000.0 );
          NewArrow->UpdatePosition();
          arrow_num++;
        }
      }
    }

    /// Plotting the superclasses
    if ( DirectSuperClassesList != nullptr )
    {
      for ( std::string * SuperClassNameStd : * ( DirectSuperClassesList ) )
      {
        QString SuperClassName = QString::fromStdString ( *SuperClassNameStd );

        if ( ItemMap.contains ( SuperClassName ) ) // && !ItemMap[ClassName]->HasArrow (
          // ItemMap[SuperClassName] ) )
        {
          SchemaGraphicSegmentedArrow * NewArrow = new SchemaGraphicSegmentedArrow (
            ItemMap[ClassName],
            ItemMap[SuperClassName],
            arrow_num,
            true,
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
  m_modified = true;
  return missingItems;
}

void dbse::SchemaGraphicsScene::RemoveItemFromScene ( QGraphicsItem* item ) {
  removeItem ( item );
  m_modified = true;
}


void dbse::SchemaGraphicsScene::RemoveClassObject ( SchemaGraphicObject * Object )
{
  if ( Object == nullptr )
  {
    return;
  }

  Object->RemoveArrows();
  RemoveItemFromScene ( Object );
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
    m_line = new QGraphicsLineItem ( QLineF ( mouseEvent->scenePos(), mouseEvent->scenePos() ) );
    m_line->setPen ( QPen ( Qt::black, 2 ) );
    addItem ( m_line );
    return;
  }

  QGraphicsScene::mousePressEvent ( mouseEvent );
}

void dbse::SchemaGraphicsScene::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
  if ( m_line != nullptr )
  {
    QLineF newLine ( m_line->line().p1(), mouseEvent->scenePos() );
    m_line->setLine ( newLine );
  }
  else
  {
    QGraphicsScene::mouseMoveEvent ( mouseEvent );
  }
  m_modified = true;
}

void dbse::SchemaGraphicsScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
  if ( m_line != nullptr )
  {
    QList<QGraphicsItem *> startItems = items ( m_line->line().p1() );

    if ( startItems.count() && startItems.first() == m_line )
    {
      startItems.removeFirst();
    }

    QList<QGraphicsItem *> endItems = items ( m_line->line().p2() );

    if ( endItems.count() && endItems.first() == m_line )
    {
      endItems.removeFirst();
    }

    RemoveItemFromScene ( m_line );
    delete m_line;

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
        SchemaGraphicSegmentedArrow * newArrow = new SchemaGraphicSegmentedArrow (
          startItem, endItem,
          0,
          Inheritance,
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

  m_line = nullptr;
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

  this->AddItemsToScene ( super_class_list, positions );

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


  this->AddItemsToScene ( super_class_list, positions );

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

  this->AddItemsToScene ( sub_class_list, positions );

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

  this->AddItemsToScene ( relationship_classes, positions );

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

  this->AddItemsToScene ( relationship_classes, positions );
}

void dbse::SchemaGraphicsScene::RemoveClassSlot()
{
  if ( CurrentObject == nullptr )
  {
    return;
  }

  CurrentObject->RemoveArrows();
  RemoveItemFromScene ( CurrentObject );
  ItemMap.remove ( CurrentObject->GetClassName() );
}

void dbse::SchemaGraphicsScene::RemoveArrowSlot()
{
  RemoveItemFromScene ( m_current_arrow );
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
      startItem, endItem,
      0,
      false, SchemaRelationship->get_is_composite(),
      QString::fromStdString ( SchemaRelationship->get_name() ), RelationshipCardinality );
    startItem->AddArrow ( newArrow );
    endItem->AddArrow ( newArrow );
    newArrow->setZValue ( -1000.0 );
    addItem ( newArrow );
    //newArrow->SetLabelScene(this);
    newArrow->UpdatePosition();
  }
}
