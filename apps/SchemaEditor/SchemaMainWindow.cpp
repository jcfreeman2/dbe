/// Including Schema Editor
#include "dbe/SchemaMainWindow.hpp"
#include "dbe/SchemaKernelWrapper.hpp"
#include "dbe/SchemaTab.hpp"
#include "dbe/SchemaClassEditor.hpp"
#include "dbe/SchemaRelationshipEditor.hpp"
#include "dbe/SchemaMethodImplementationEditor.hpp"
/// Including Auto-Generated Files
#include "ui_SchemaMainWindow.h"
/// Including QT Headers
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QPushButton>
#include <QGraphicsScene>
#include <QCloseEvent>
#include <QPrinter>
#include <QPrintDialog>

//#include <format>
#include <sstream>

using namespace dunedaq;
using namespace dunedaq::oks;

dbse::SchemaMainWindow::SchemaMainWindow ( QWidget * parent )
  : QMainWindow ( parent ),
    ui ( new Ui::SchemaMainWindow ),
    FileModel ( nullptr ),
    TableModel ( nullptr ),
    proxyModel ( new QSortFilterProxyModel() ),
    ContextMenuFileView ( nullptr ),
    ContextMenuTableView ( nullptr )
{
  InitialSettings();
  InitialTab();
  InitialTabCorner();
  SetController();
}

dbse::SchemaMainWindow::SchemaMainWindow ( QString SchemaFile, QWidget * parent )
    : SchemaMainWindow(parent)
{
    OpenSchemaFile(SchemaFile);
}

dbse::SchemaMainWindow::~SchemaMainWindow() = default;

void dbse::SchemaMainWindow::InitialSettings()
{
  ui->setupUi ( this );
  setWindowTitle ( "SchemaEditor : The new TDAq schema editor" );
  ui->UndoView->setStack ( KernelWrapper::GetInstance().GetUndoStack() );
  ui->ClassTableView->horizontalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
  ui->ClassTableView->setDragEnabled ( true );
  ui->ClassTableView->setContextMenuPolicy ( Qt::ContextMenuPolicy::CustomContextMenu );
  ui->FileView->horizontalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
  ui->FileView->setContextMenuPolicy ( Qt::ContextMenuPolicy::CustomContextMenu );
  ui->FileView->setSelectionBehavior ( QAbstractItemView::SelectRows );
  ui->TabWidget->setTabsClosable ( true );
  ui->ClassTableSearchLine->setProperty ( "placeholderText",
                                          QVariant ( QString ( "Type to search" ) ) );
}

void dbse::SchemaMainWindow::InitialTab()
{
  ui->TabWidget->addTab ( new SchemaTab(), "Schema View" );
  ui->TabWidget->removeTab ( 0 );
}

void dbse::SchemaMainWindow::InitialTabCorner()
{
  QPushButton * RightButton = new QPushButton ( "+" );
  ui->TabWidget->setCornerWidget ( RightButton, Qt::TopLeftCorner );
  connect ( RightButton, SIGNAL ( clicked() ), this, SLOT ( AddTab() ) );
}

void dbse::SchemaMainWindow::SetController()
{
  connect ( ui->OpenFileSchema, SIGNAL ( triggered() ), this, SLOT ( OpenSchemaFile() ) );
  connect ( ui->LoadFileSchema, SIGNAL ( triggered() ), this, SLOT ( OpenSchemaFile() ) );
  connect ( ui->CreateNewSchema, SIGNAL ( triggered() ), this, SLOT ( CreateNewSchema() ) );
  connect ( ui->SaveSchema, SIGNAL ( triggered() ), this, SLOT ( SaveSchema() ) );
  connect ( ui->SetRelationship, SIGNAL ( triggered ( bool ) ), this,
            SLOT ( ChangeCursorRelationship ( bool ) ) );
  connect ( ui->SetInheritance, SIGNAL ( triggered ( bool ) ), this,
            SLOT ( ChangeCursorInheritance ( bool ) ) );
  connect ( ui->SaveView, SIGNAL ( triggered() ), this, SLOT ( SaveView() ) );
  connect ( ui->LoadView, SIGNAL ( triggered() ), this, SLOT ( LoadView() ) );
  connect ( ui->Exit, SIGNAL ( triggered() ), this, SLOT ( close() ) );
  connect ( ui->ClassTableView, SIGNAL ( doubleClicked ( QModelIndex ) ), this,
            SLOT ( LaunchClassEditor ( QModelIndex ) ) );
  connect ( &KernelWrapper::GetInstance(), SIGNAL ( ClassCreated() ), this,
            SLOT ( BuildTableModelSlot() ) );
  connect ( &KernelWrapper::GetInstance(), SIGNAL ( ClassRemoved ( QString ) ), this,
            SLOT ( BuildTableModelSlot ( QString ) ) );
  connect ( ui->TabWidget, SIGNAL ( tabCloseRequested ( int ) ), this,
            SLOT ( RemoveTab ( int ) ) );
  connect ( ui->FileView, SIGNAL ( customContextMenuRequested ( QPoint ) ), this,
            SLOT ( CustomContextMenuFileView ( QPoint ) ) );
  connect ( ui->ClassTableView, SIGNAL ( customContextMenuRequested ( QPoint ) ), this,
            SLOT ( CustomContextMenuTableView ( QPoint ) ) );
  connect ( ui->PrintView, SIGNAL ( triggered() ), this, SLOT ( PrintCurrentView() ) );
  connect ( ui->ClassTableSearchLine, SIGNAL( textChanged ( QString ) ), proxyModel, SLOT( setFilterRegExp( QString ) ) );
}

void dbse::SchemaMainWindow::BuildFileModel()
{
  QStringList Headers
  { "File Name", "Access" };

  if ( FileModel == nullptr )
  {
    FileModel = new CustomFileModel ( Headers );
  }
  else
  {
    delete FileModel;
    FileModel = new CustomFileModel ( Headers );
  }

  ui->FileView->setModel ( FileModel );
}

void dbse::SchemaMainWindow::BuildTableModel()
{
  QStringList Headers
  { "Class Name" };

  if ( TableModel == nullptr )
  {
    TableModel = new CustomTableModel ( Headers );
  }
  else
  {
    delete TableModel;
    TableModel = new CustomTableModel ( Headers );
  }

  proxyModel->setSourceModel(TableModel);
  ui->ClassTableView->setModel ( proxyModel );
}

int dbse::SchemaMainWindow::ShouldSaveChanges() const
{
  // if ( KernelWrapper::GetInstance().GetUndoStack()->isClean() )
  auto modified = KernelWrapper::GetInstance().ModifiedSchemaFiles();
  if (modified.empty())
  {
    return QMessageBox::Discard;
  }

  std::string msg = "There are unsaved changes in the following files:\n\n"
    + modified + "Do you want to save the changes in the schema?\n";
  return QMessageBox::question (
           0, tr ( "DBE" ),
           QString ( msg.c_str() ),
           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save );
}

void dbse::SchemaMainWindow::AddNewClass()
{
    SchemaClassEditor::createNewClass();
}

void dbse::SchemaMainWindow::RemoveClass()
{
  QModelIndex Index = ui->ClassTableView->currentIndex();
  QModelIndex proxyIndex = proxyModel->mapToSource( Index );
  QStringList Row = TableModel->getRowFromIndex ( proxyIndex );
  OksClass * SchemaClass = KernelWrapper::GetInstance().FindClass ( Row.at (
                                                                      0 ).toStdString() );

  if ( SchemaClass->all_sub_classes()->size() != 0 )
  {
    QMessageBox::warning ( 0, "Schema editor",
                           QString ( "Cannot delete class because it has sub-classes." ) );
    return;
  }
  else if ( KernelWrapper::GetInstance().AnyClassReferenceThis ( SchemaClass ) )
  {
    QMessageBox::warning ( 0, "Schema editor",
                           QString ( "Cannot delete class because some classes references it." ) );
    return;
  }
  else
  {
    QString Name = QString::fromStdString ( SchemaClass->get_name() );
    KernelWrapper::GetInstance().PushRemoveClassCommand ( SchemaClass, SchemaClass->get_name(),
                                                          SchemaClass->get_description(),
                                                          SchemaClass->get_is_abstract() );
  }
}

void dbse::SchemaMainWindow::SetSchemaFileActive()
{
  QModelIndex Index = ui->FileView->currentIndex();
  QStringList Row = FileModel->getRowFromIndex ( Index );
  KernelWrapper::GetInstance().SetActiveSchema ( Row.at ( 0 ).toStdString() );
}

void dbse::SchemaMainWindow::PrintCurrentView()
{
  SchemaTab * CurrentTab = dynamic_cast<SchemaTab *> ( ui->TabWidget->currentWidget() );

  QPrinter printer;

  if ( QPrintDialog ( &printer ).exec() == QDialog::Accepted )
  {
    QPainter painter ( &printer );
    painter.setRenderHint ( QPainter::Antialiasing );

    SchemaGraphicsScene * Scene = CurrentTab->GetScene();
    QGraphicsView * View = CurrentTab->GetView();
    Scene->render ( &painter, QRectF(), View->viewport()->rect() );
  }
}

void dbse::SchemaMainWindow::closeEvent ( QCloseEvent * event )
{
  int UserChoice = ShouldSaveChanges();

  if ( UserChoice == QMessageBox::Save )
  {
    try
    {
      int nsaved = KernelWrapper::GetInstance().SaveModifiedSchema();
      std::ostringstream ostream;
      ostream << nsaved << " schema files successfully saved";
      std::string msg = ostream.str();
      QMessageBox::information ( 0, "Schema editor",
                                 QString ( msg.c_str() ) );
      KernelWrapper::GetInstance().CloseAllSchema();
    }
    catch ( oks::exception & Ex )
    {
      QMessageBox::warning ( 0, "Schema editor",
                             QString ( "Could not save schemas.\n\n%1" ).arg ( QString ( Ex.what() ) ) );
    }
  }
  else if ( UserChoice == QMessageBox::Discard )
  {
    KernelWrapper::GetInstance().CloseAllSchema();
  }
  else if ( UserChoice == QMessageBox::Cancel )
  {
    event->ignore();
    return;
  }

  for ( QWidget * Widget : QApplication::allWidgets() )
  {
    Widget->close();
  }

  event->accept();
}

void dbse::SchemaMainWindow::OpenSchemaFile(QString SchemaFile) {
    if(!SchemaFile.isEmpty()) {
            try {
                KernelWrapper::GetInstance().LoadSchema(SchemaFile.toStdString());
                KernelWrapper::GetInstance().SetActiveSchema(SchemaFile.toStdString());

                BuildTableModel();
                BuildFileModel();

#ifdef QT_DEBUG
          /// KernelWrapper::GetInstance().ShowSchemaClasses();
#endif
            }
            catch(oks::exception &Ex) {
                QMessageBox::warning(0,
                                     "Load Schema",
                                     QString("Could not load schema!\n\n").append(QString(Ex.what())),
                                     QMessageBox::Ok);
            }
    }
}

void dbse::SchemaMainWindow::OpenSchemaFile()
{
  QFileDialog FileDialog ( this, tr ( "Open File" ), ".", tr ( "XML files (*.xml);;All files (*)" ) );
  FileDialog.setAcceptMode ( QFileDialog::AcceptOpen );
  FileDialog.setFileMode ( QFileDialog::AnyFile );
  FileDialog.setViewMode ( QFileDialog::Detail );
  QStringList FilesSelected;
  QString SchemaPath;

  if ( FileDialog.exec() )
  {
    FilesSelected = FileDialog.selectedFiles();
  }

  if ( FilesSelected.size() )
  {
    SchemaPath = FilesSelected.value ( 0 );
  }

  OpenSchemaFile(SchemaPath);
}

void dbse::SchemaMainWindow::SaveSchema()
{
    try
    {
      int nsaved = KernelWrapper::GetInstance().SaveModifiedSchema();
      //std::format msg("{} schema files successfully saved", nsaved)
      std::ostringstream ostream;
      ostream << nsaved << " schema files successfully saved";
      std::string msg = ostream.str();
      QMessageBox::information ( 0, "Schema editor",
                                 QString ( msg.c_str() ) );

    }
    catch ( oks::exception & Ex )
    {
      QMessageBox::warning ( 0, "Schema editor",
                             QString ( "Could not save schemas.\n\n%1" ).arg ( QString ( Ex.what() ) ) );
    }
}

void dbse::SchemaMainWindow::CreateNewSchema()
{
  QString FileName = QFileDialog::getSaveFileName ( this, tr ( "Save File" ) );

  if ( FileName.isEmpty() )
  {
    QMessageBox::warning ( 0, "Schema editor",
                           QString ( "Please provide a name for the schema !" ) );
    return;
  }

  if ( !FileName.endsWith ( ".schema.xml" ) )
  {
    FileName.append ( ".schema.xml" );
  }

  QFile FileInfo ( FileName );
  std::string FileNameStd = FileInfo.fileName().toStdString();

  try
  {
    KernelWrapper::GetInstance().CreateNewSchema ( FileNameStd );

    BuildTableModel();
    BuildFileModel();
  }
  catch ( oks::exception & Ex )
  {
    QMessageBox::warning (
      0,
      "Schema editor",
      QString ( "Could not create file : %1.\n\n%2" ).arg ( QString::fromStdString (
                                                              FileNameStd ) ).arg (
        QString ( Ex.what() ) ) );
  }
}

void dbse::SchemaMainWindow::ChangeCursorRelationship ( bool State )
{
  if ( ui->SetInheritance->isChecked() )
  {
    ui->SetInheritance->setChecked ( false );
  }

  if ( State )
  {
    ui->TabWidget->setCursor ( Qt::CrossCursor );
  }
  else
  {
    ui->TabWidget->setCursor ( Qt::ArrowCursor );
  }

  KernelWrapper::GetInstance().SetInheritanceMode ( false );
}

void dbse::SchemaMainWindow::ChangeCursorInheritance ( bool State )
{
  if ( ui->SetRelationship->isChecked() )
  {
    ui->SetRelationship->setChecked ( false );
  }

  if ( State )
  {
    ui->TabWidget->setCursor ( Qt::CrossCursor );
  }
  else
  {
    ui->TabWidget->setCursor ( Qt::ArrowCursor );
  }

  KernelWrapper::GetInstance().SetInheritanceMode ( true );
}

void dbse::SchemaMainWindow::AddTab()
{
  ui->TabWidget->addTab ( new SchemaTab(), "Schema View" );
}

void dbse::SchemaMainWindow::SaveView()
{
  SchemaTab * CurrentTab = dynamic_cast<SchemaTab *> ( ui->TabWidget->currentWidget() );

  if ( CurrentTab->GetScene()->items().size() != 0 )
  {
    QString FileName = QFileDialog::getSaveFileName ( this, tr ( "Save View" ) );

    if ( !FileName.endsWith ( ".view" ) )
    {
      FileName.append ( ".view" );
    }

    QFile ViewFile ( FileName );
    ViewFile.open ( QIODevice::WriteOnly );

    for ( QGraphicsItem * Item : CurrentTab->GetScene()->items() )
    {
      if ( dynamic_cast<SchemaGraphicObject *> ( Item ) )
      {
        SchemaGraphicObject * SchemaObject = dynamic_cast<SchemaGraphicObject *> ( Item );
        QString ObjectDescription = SchemaObject->GetClassName() + ","
                                    + QString::number ( SchemaObject->scenePos().x() ) + ","
                                    + QString::number ( SchemaObject->scenePos().y() ) + "\n";
        ViewFile.write ( ObjectDescription.toUtf8() );
      }
    }

    ViewFile.close();
  }
}

void dbse::SchemaMainWindow::LoadView()
{
  QFileDialog FileDialog ( this, tr ( "Open File" ), ".", tr ( "View files (*.view)" ) );
  FileDialog.setAcceptMode ( QFileDialog::AcceptOpen );
  FileDialog.setFileMode ( QFileDialog::AnyFile );
  FileDialog.setViewMode ( QFileDialog::Detail );
  QStringList FilesSelected;
  QString ViewPath;

  if ( FileDialog.exec() )
  {
    FilesSelected = FileDialog.selectedFiles();
  }

  if ( FilesSelected.size() )
  {
    ViewPath = FilesSelected.value ( 0 );
  }

  if ( !ViewPath.isEmpty() )
  {
    SchemaTab * CurrentTab = dynamic_cast<SchemaTab *> ( ui->TabWidget->currentWidget() );

    if ( CurrentTab->GetScene()->items().size() != 0 )
    {
      int UserAnswer =
        QMessageBox::question (
          0,
          tr ( "DBE" ),
          QString (
            "If you load this view the current view will be lost.\n\nDo you want to load it anyway?\n" ),
          QMessageBox::Save | QMessageBox::Cancel, QMessageBox::Save );

      if ( UserAnswer == QMessageBox::Cancel )
      {
        return;
      }
    }

    CurrentTab->GetScene()->clear();

    QFile ViewFile ( ViewPath );
    ViewFile.open ( QIODevice::ReadOnly );

    QStringList ClassesNames;
    QList<QPointF> Positions;

    while ( !ViewFile.atEnd() )
    {
      QString Line ( ViewFile.readLine() );
      QStringList ObjectDescription = Line.split ( "," );
      ClassesNames.append ( ObjectDescription.at ( 0 ) );
      QPointF Position;
      Position.setX ( ObjectDescription.at ( 1 ).toInt() );
      Position.setY ( ObjectDescription.at ( 2 ).toInt() );
      Positions.append ( Position );
    }

    CurrentTab->GetScene()->CleanItemMap();
    CurrentTab->GetScene()->AddItemToScene ( ClassesNames, Positions );
    ViewFile.close();
  }
}

void dbse::SchemaMainWindow::LaunchClassEditor ( QModelIndex Index )
{
  QModelIndex proxyIndex = proxyModel->mapToSource( Index );
  QStringList Row = TableModel->getRowFromIndex ( proxyIndex );

  if ( !Row.isEmpty() )
  {
    bool WidgetFound = false;
    QString ClassName = Row.at ( 0 );
    OksClass * ClassInfo = KernelWrapper::GetInstance().FindClass ( ClassName.toStdString() );

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
      SchemaClassEditor * Editor = new SchemaClassEditor ( ClassInfo );
      Editor->show();
    }
  }
}

void dbse::SchemaMainWindow::BuildTableModelSlot ( QString ClassName )
{
  Q_UNUSED ( ClassName )

  BuildTableModel();
}

void dbse::SchemaMainWindow::BuildTableModelSlot()
{
  BuildTableModel();
}

void dbse::SchemaMainWindow::RemoveTab ( int i )
{
  if ( i == -1 || ( ( ui->TabWidget->count() == 1 ) && i == 0 ) )
  {
    return;
  }

  QWidget * Widget = ui->TabWidget->widget ( i );
  ui->TabWidget->removeTab ( i );
  delete Widget;
  Widget = nullptr;
}

void dbse::SchemaMainWindow::CustomContextMenuFileView ( QPoint Pos )
{
  if ( ContextMenuFileView == nullptr )
  {
    ContextMenuFileView = new QMenu ( this );

    QAction * Add = new QAction ( tr ( "&Set Active Schema" ), this );
    Add->setShortcut ( tr ( "Ctrl+S" ) );
    Add->setShortcutContext ( Qt::WidgetShortcut );
    connect ( Add, SIGNAL ( triggered() ), this, SLOT ( SetSchemaFileActive() ) );

    ContextMenuFileView->addAction ( Add );
  }

  QModelIndex Index = ui->FileView->currentIndex();

  if ( Index.isValid() )
  {
    ContextMenuFileView->exec ( ui->FileView->mapToGlobal ( Pos ) );
  }
}

void dbse::SchemaMainWindow::CustomContextMenuTableView ( QPoint Pos )
{
  if ( ContextMenuTableView == nullptr )
  {
    ContextMenuTableView = new QMenu ( this );

    QAction * Add = new QAction ( tr ( "&Add New Class" ), this );
    Add->setShortcut ( tr ( "Ctrl+A" ) );
    Add->setShortcutContext ( Qt::WidgetShortcut );
    connect ( Add, SIGNAL ( triggered() ), this, SLOT ( AddNewClass() ) );

    QAction * Remove = new QAction ( tr ( "&Remove Selected Class" ), this );
    Remove->setShortcut ( tr ( "Ctrl+R" ) );
    Remove->setShortcutContext ( Qt::WidgetShortcut );
    connect ( Remove, SIGNAL ( triggered() ), this, SLOT ( RemoveClass() ) );

    ContextMenuTableView->addAction ( Add );
    ContextMenuTableView->addAction ( Remove );
  }

  QModelIndex Index = ui->ClassTableView->currentIndex();

  if ( Index.isValid() )
  {
    ContextMenuTableView->exec ( ui->ClassTableView->mapToGlobal ( Pos ) );
  }
}

