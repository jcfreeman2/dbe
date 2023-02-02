/// Including QT#include <QHeaderView>
#include <tableselection.h>

#include <confaccessor.h>
#include <QScrollBar>
#include <QMenu>
#include <QLabel>
#include <QHBoxLayout>
#include <QContextMenuEvent>
#include <QDialog>
#include <QLineEdit>
#include <QVariant>
#include <QHeaderView>

/// Including DBE
#include "CustomTableView.h"
#include "ObjectEditor.h"
#include "ObjectCreator.h"
#include "messenger.h"

//#include "MainWindow.h"

//-----------------------------------------------------------------------------------------------------
dbe::CustomTableView::CustomTableView ( QWidget * parent )
  : QTableView ( parent ),
    ContextMenu ( nullptr ),
    FindObject ( nullptr ),
    editObject ( nullptr ),
    deleteObjectAc ( nullptr ),
    refByAc ( nullptr ),
    refByAcOnlyComp ( nullptr ),
    copyObjectAc ( nullptr ),
    FindFileDialog ( nullptr ),
    LineEdit ( nullptr ),
    NextButton ( nullptr ),
    GoButton ( nullptr ),
    ListIndex ( 0 )
{
  verticalHeader()->setVisible(true);
  verticalHeader()->setSectionResizeMode ( QHeaderView::Interactive );
  verticalHeader()->setMinimumSectionSize(40);
  horizontalHeader()->setSectionResizeMode( QHeaderView::Interactive );
  horizontalHeader()->setDefaultSectionSize(250);
  setSortingEnabled ( true );
  setAlternatingRowColors ( true );
  setSelectionMode ( SelectionMode::SingleSelection );
  setHorizontalScrollMode(ScrollMode::ScrollPerPixel);
  setVerticalScrollMode(ScrollMode::ScrollPerPixel);
  setWordWrap(true);
  setTextElideMode(Qt::ElideRight);
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::contextMenuEvent ( QContextMenuEvent * Event )
{
  if ( ContextMenu == nullptr )
  {
    ContextMenu = new QMenu ( this );
    CreateActions();
  }

  QModelIndex Index = indexAt ( Event->pos() );

  if ( Index.isValid() )
  {
    ContextMenu->exec ( Event->globalPos() );
  }
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::FindObjectSlot()
{
  if ( FindFileDialog != nullptr )
  {
    delete FindFileDialog;
    FindFileDialog = nullptr;
  }

  FindFileDialog = new QDialog ( this );
  FindFileDialog->setSizePolicy ( QSizePolicy::Preferred, QSizePolicy::Preferred );
  FindFileDialog->setToolTip ( "Type string to edit line and press Enter." );
  FindFileDialog->setWindowTitle ( "Search for an object in the table" );

  QHBoxLayout * Layout = new QHBoxLayout ( FindFileDialog );
  QLabel * Label = new QLabel ( QString ( "Find Object:" ), FindFileDialog );

  NextButton = new QPushButton ( "Next" );
  GoButton = new QPushButton ( "Go !" );

  LineEdit = new QLineEdit ( FindFileDialog );
  LineEdit->setToolTip ( "Type string and press Enter" );

  Layout->addWidget ( Label );
  Layout->addWidget ( LineEdit );
  Layout->addWidget ( GoButton );
  Layout->addWidget ( NextButton );

  FindFileDialog->setLayout ( Layout );
  FindFileDialog->show();
  NextButton->setDisabled ( true );

  connect ( LineEdit, SIGNAL ( textEdited ( QString ) ), this,
            SLOT ( EditedSearchString ( QString ) ) );
  connect ( LineEdit, SIGNAL ( returnPressed() ), this, SLOT ( GoToFile() ) );
  connect ( GoButton, SIGNAL ( clicked() ), this, SLOT ( GoToFile() ) );
  connect ( NextButton, SIGNAL ( clicked() ), this, SLOT ( GoToNext() ) );
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::GoToFile()
{
  ListIndex = 0;
  ListOfMatch.clear();

  QString UserType = LineEdit->text();

  if ( UserType.isEmpty() )
  {
    return;
  }

  QAbstractItemModel * Model = model();

  if ( Model != nullptr )
  {
    QVariant StringCriterium = QVariant ( UserType );
    QModelIndex WhereToStartSearch = Model->index ( 0, 0 );
    ListOfMatch = Model->match ( WhereToStartSearch, Qt::DisplayRole, StringCriterium, 1000,
                                 Qt::MatchContains | Qt::MatchWrap );

    if ( ListOfMatch.size() > 0 )
    {
      ListIndex = 0;
      scrollTo ( ListOfMatch.value ( ListIndex ), QAbstractItemView::EnsureVisible );
      selectRow ( ListOfMatch.value ( ListIndex ).row() );
      resizeColumnToContents ( ListIndex );

      GoButton->setDisabled ( true );
      NextButton->setEnabled ( true );

      disconnect ( LineEdit, SIGNAL ( returnPressed() ), this, SLOT ( GoToFile() ) );
      connect ( LineEdit, SIGNAL ( returnPressed() ), this, SLOT ( GoToNext() ) );
    }
  }
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::GoToNext()
{
  if ( ( LineEdit->text() ).isEmpty() )
  {
    ListIndex = 0;
    ListOfMatch.clear();
    return;
  }

  if ( ListOfMatch.size() > 0 )
  {
    if ( ( ++ListIndex ) < ListOfMatch.size() )
    {
      scrollTo ( ListOfMatch.value ( ListIndex ), QAbstractItemView::EnsureVisible );
      selectRow ( ListOfMatch.value ( ListIndex ).row() );
      resizeColumnToContents ( ListIndex );
    }
    else
    {
      ListIndex = 0;
      scrollTo ( ListOfMatch.value ( ListIndex ), QAbstractItemView::EnsureVisible );
      selectRow ( ListOfMatch.value ( ListIndex ).row() );
      resizeColumnToContents ( ListIndex );
    }
  }
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::EditedSearchString ( QString Text )
{
  Q_UNUSED ( Text )

  connect ( LineEdit, SIGNAL ( returnPressed() ), this, SLOT ( GoToFile() ) );
  disconnect ( LineEdit, SIGNAL ( returnPressed() ), this, SLOT ( GoToNext() ) );

  GoButton->setEnabled ( true );
  NextButton->setDisabled ( true );
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::slot_edit_object()
{
  QModelIndex const & index = this->currentIndex();

  if ( index.isValid() )
  {
    if ( models::tableselection const * selectionmodel =
           dynamic_cast<models::tableselection const *> ( index.model() ) )
    {
      if ( models::table const * srcmodel =
             dynamic_cast<const dbe::models::table *> ( selectionmodel->sourceModel() ) )
      {
        tref obj = srcmodel->GetTableObject ( selectionmodel->mapToSource ( index ).row() );

        if ( not obj.is_null() )
        {
          emit OpenEditor ( obj );
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::referencedBy_OnlyComposite()
{
  QModelIndex const & Index = this->currentIndex();

  const dbe::models::tableselection * Model =
    dynamic_cast<const dbe::models::tableselection *> ( Index.model() );

  const dbe::models::table * SourceModel = dynamic_cast<const dbe::models::table *> ( Model
                                                                                      ->sourceModel() );

  if ( !Index.isValid() || !Model )
  {
    return;
  }

  tref obj = SourceModel->GetTableObject ( Model->mapToSource ( Index ).row() );
  referencedBy ( obj, true );
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::referencedBy_All()
{
  QModelIndex const & Index = this->currentIndex();
  const dbe::models::tableselection * Model =
    dynamic_cast<const dbe::models::tableselection *> ( Index.model() );

  dbe::models::table const * SourceModel = dynamic_cast<const dbe::models::table *> ( Model
                                                                                      ->sourceModel() );

  if ( !Index.isValid() || !Model )
  {
    return;
  }

  tref obj = SourceModel->GetTableObject ( Model->mapToSource ( Index ).row() );
  referencedBy ( obj, false );
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::slot_copy_object()
{
  QModelIndex const & Index = this->currentIndex();

  dbe::models::tableselection const * Model =
    dynamic_cast<const dbe::models::tableselection *> ( Index.model() );

  dbe::models::table const * SourceModel = dynamic_cast<const dbe::models::table *> ( Model
                                                                                      ->sourceModel() );

  if ( Index.isValid() and Model )
  {
    tref obj = SourceModel->GetTableObject ( Model->mapToSource ( Index ).row() );

    if ( not obj.is_null() )
    {
      ( new ObjectCreator ( obj ) )->show();
    }
  }
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::slot_create_object()
{
  // TODO implement dbe::CustomTableView::slot_create_object
  throw std::string ( " dbe::CustomTableView::slot_create_object is not yet implemented " );
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::slot_delete_objects()
{
  if ( this->model() != nullptr )
  {
    QModelIndexList qindices = this->selectedIndexes();
    std::vector<QModelIndex> indices;

    for ( QModelIndex q : qindices )
    {
      if ( q.isValid() )
      {
        indices.push_back ( q );
      }
    }

    if ( dbe::models::tableselection * casted =
           dynamic_cast<dbe::models::tableselection *> ( this->model() ) )
    {
      casted->delete_objects ( indices.begin(), indices.end() );
    }
    else if ( dbe::models::table * casted = dynamic_cast<dbe::models::table *>
                                            ( this->model() ) )
    {
      casted->delete_objects ( indices.begin(), indices.end() );
    }
    else
    {
      WARN ( "Object Deleting",
             "Object deletion failed due to the internal model being in invalid state",
             "You are advised to restart the application before proceeding any further" );
    }
  }

}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::CreateActions()
{
  FindObject = new QAction ( tr ( "Find Object" ), this );
  FindObject->setShortcutContext ( Qt::WidgetShortcut );
  connect ( FindObject, SIGNAL ( triggered() ), this, SLOT ( FindObjectSlot() ) );
  ContextMenu->addAction ( FindObject );

  editObject = new QAction ( tr ( "&Edit Object" ), this );
  editObject->setShortcut ( tr ( "Ctrl+E" ) );
  editObject->setShortcutContext ( Qt::WidgetShortcut );
  connect ( editObject, SIGNAL ( triggered() ), this, SLOT ( slot_edit_object() ) );
  ContextMenu->addAction ( editObject );

  deleteObjectAc = new QAction ( tr ( "&Delete Object" ), this );
  deleteObjectAc->setShortcut ( tr ( "Ctrl+D" ) );
  deleteObjectAc->setShortcutContext ( Qt::WidgetShortcut );
  connect ( deleteObjectAc, SIGNAL ( triggered() ), this, SLOT ( slot_delete_objects() ) );
  ContextMenu->addAction ( deleteObjectAc );

  refByAc = new QAction ( tr ( "Referenced B&y (All objects)" ), this );
  refByAc->setShortcut ( tr ( "Ctrl+Y" ) );
  refByAc->setShortcutContext ( Qt::WidgetShortcut );
  refByAc->setToolTip ( "Find all objects which reference the selected object" );
  refByAc->setStatusTip ( refByAc->toolTip() );
  connect ( refByAc, SIGNAL ( triggered() ), this, SLOT ( referencedBy_All() ) );
  ContextMenu->addAction ( refByAc );

  refByAcOnlyComp = new QAction ( tr ( "Referenced B&y (Only Composite)" ), this );
  refByAcOnlyComp->setShortcut ( tr ( "Ctrl+Shift+Y" ) );
  refByAcOnlyComp->setShortcutContext ( Qt::WidgetShortcut );
  refByAcOnlyComp->setToolTip (
    "Find objects (ONLY Composite ones) which reference the selected object" );
  refByAcOnlyComp->setStatusTip ( refByAcOnlyComp->toolTip() );
  connect ( refByAcOnlyComp, SIGNAL ( triggered() ), this,
            SLOT ( referencedBy_OnlyComposite() ) );
  ContextMenu->addAction ( refByAcOnlyComp );

  copyObjectAc = new QAction ( tr ( "Copy This Object Into A &New One" ), this );
  copyObjectAc->setShortcut ( tr ( "Ctrl+Shift+N" ) );
  copyObjectAc->setShortcutContext ( Qt::WidgetShortcut );
  connect ( copyObjectAc, SIGNAL ( triggered() ), this, SLOT ( slot_copy_object() ) );
  ContextMenu->addAction ( copyObjectAc );
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
void dbe::CustomTableView::referencedBy ( tref obj, bool onlyComposite )
{
  if ( not obj.is_null() )
  {
    int i = 89;
    //MainWindow::findthis()->get_view()->referencedBy ( onlyComposite, obj );
  }
}
//-----------------------------------------------------------------------------------------------------

