/// Including Qt
#include <QMessageBox>
/// Including Schema
#include "dbe/SchemaMethodEditor.hpp"
#include "dbe/SchemaMethodImplementationEditor.hpp"
#include "dbe/SchemaKernelWrapper.hpp"
/// Including auto-generated ui
#include "ui_SchemaMethodEditor.h"

using namespace dunedaq::oks;

dbse::SchemaMethodEditor::~SchemaMethodEditor() = default;

dbse::SchemaMethodEditor::SchemaMethodEditor ( OksClass * ClassInfo, OksMethod * Method,
                                               QWidget * parent )
  : QWidget ( parent ),
    ui ( new Ui::SchemaMethodEditor ),
    SchemaClass ( ClassInfo ),
    SchemaMethod ( Method ),
    ImplementationModel ( nullptr ),
    UsedNew ( false )
{
  QWidget::setAttribute(Qt::WA_DeleteOnClose);
  ui->setupUi ( this );
  InitialSettings();
  BuildModels();
  SetController();
}

dbse::SchemaMethodEditor::SchemaMethodEditor ( OksClass * ClassInfo, QWidget * parent )
  : QWidget ( parent ),
    ui ( new Ui::SchemaMethodEditor ),
    SchemaClass ( ClassInfo ),
    SchemaMethod ( nullptr ),
    ImplementationModel ( nullptr ),
    UsedNew ( true )
{
  QWidget::setAttribute(Qt::WA_DeleteOnClose);
  ui->setupUi ( this );
  InitialSettings();
  SetController();
}

void dbse::SchemaMethodEditor::SetController()
{
  connect ( ui->SaveButton, SIGNAL ( clicked() ), this, SLOT ( ProxySlot() ) );
  connect ( ui->AddButton, SIGNAL ( clicked() ), this,
            SLOT ( AddNewMethodImplementation() ) );
  connect ( ui->ImplementationsView, SIGNAL ( doubleClicked ( QModelIndex ) ), this,
            SLOT ( OpenMethodImplementationEditor ( QModelIndex ) ) );
  connect ( &KernelWrapper::GetInstance(), SIGNAL ( ClassUpdated ( QString ) ), this,
            SLOT ( ClassUpdated ( QString ) ) );
}

void dbse::SchemaMethodEditor::ClassUpdated( QString ClassName)
{
    if(!UsedNew && ClassName.toStdString() == SchemaClass->get_name()) {
        if(SchemaClass->find_direct_method(SchemaMethod->get_name()) != nullptr) {
            FillInfo();
            BuildModels();
        } else {
            QWidget::close();
        }
    }
}

void dbse::SchemaMethodEditor::FillInfo()
{
    setObjectName ( QString::fromStdString ( SchemaMethod->get_name() ) );
    setWindowTitle (
      QString ( "Method Editor : %1" ).arg ( QString::fromStdString (
                                               SchemaMethod->get_name() ) ) );
    ui->MethodName->setText ( QString::fromStdString ( SchemaMethod->get_name() ) );
    ui->DescriptionTextBox->setPlainText (
      QString::fromStdString ( SchemaMethod->get_description() ) );
}

void dbse::SchemaMethodEditor::InitialSettings()
{
  if ( UsedNew )
  {
    setWindowTitle ( "New Method" );
    setObjectName ( "NEW" );
    ui->AddButton->setEnabled ( false );
  }
  else
  {
      FillInfo();
  }
}

void dbse::SchemaMethodEditor::ParseToSave()
{
  bool changed = false;
  std::string MethodName = ui->MethodName->text().toStdString();
  std::string MethodDescription = ui->DescriptionTextBox->toPlainText().toStdString();

  if ( MethodName != SchemaMethod->get_name() )
  {
    KernelWrapper::GetInstance().PushSetNameMethodCommand ( SchemaClass, SchemaMethod, MethodName );
    changed = true;
  }

  if ( MethodDescription != SchemaMethod->get_description() )
  {
    KernelWrapper::GetInstance().PushSetDescriptionMethodCommand ( SchemaClass,
                                                                   SchemaMethod,
                                                                   MethodDescription );
    changed = true;
  }

  if ( changed )
  {
    emit RebuildModel();
  }

  close();
}

void dbse::SchemaMethodEditor::ParseToCreate()
{
  bool changed = true;
  std::string MethodName = ui->MethodName->text().toStdString();
  std::string MethodDescription = ui->DescriptionTextBox->toPlainText().toStdString();
  KernelWrapper::GetInstance().PushAddMethodCommand ( SchemaClass, MethodName,
                                                      MethodDescription );

  if ( changed )
  {
    emit RebuildModel();
  }

  close();
}

void dbse::SchemaMethodEditor::BuildModels()
{
  QStringList MethodHeaders
  { "Method Name" };

  if ( ImplementationModel == nullptr ) ImplementationModel = new
    CustomMethodImplementationModel (
      SchemaMethod, MethodHeaders );
  else
  {
    delete ImplementationModel;
    ImplementationModel = new CustomMethodImplementationModel ( SchemaMethod, MethodHeaders );
  }

  ui->ImplementationsView->setModel ( ImplementationModel );
  ui->ImplementationsView->horizontalHeader()->setSectionResizeMode ( QHeaderView::Stretch );
}

bool dbse::SchemaMethodEditor::ShouldOpenMethodImplementationEditor ( QString Name )
{
  bool WidgetFound = false;

  for ( QWidget * Editor : QApplication::allWidgets() )
  {
    SchemaMethodImplementationEditor * Widget =
      dynamic_cast<SchemaMethodImplementationEditor *> ( Editor );

    if ( Widget != nullptr )
    {
      if ( ( Widget->objectName() ).compare ( Name ) == 0 )
      {
        Widget->raise();
        Widget->setVisible ( true );
        Widget->activateWindow();
        WidgetFound = true;
      }
    }
  }

  return !WidgetFound;
}

void dbse::SchemaMethodEditor::ProxySlot()
{
  if ( UsedNew )
  {
    ParseToCreate();
  }
  else
  {
    ParseToSave();
  }
}

void dbse::SchemaMethodEditor::AddNewMethodImplementation()
{
  SchemaMethodImplementationEditor * Editor = new SchemaMethodImplementationEditor (
    SchemaClass, SchemaMethod );
  connect ( Editor, SIGNAL ( RebuildModel() ), this,
            SLOT ( BuildModelImplementationSlot() ) );
  Editor->show();
}

void dbse::SchemaMethodEditor::OpenMethodImplementationEditor ( QModelIndex Index )
{
  QStringList Row = ImplementationModel->getRowFromIndex ( Index );
  bool ShouldOpen = ShouldOpenMethodImplementationEditor ( Row.at ( 1 ) );

  if ( !Row.isEmpty() && ShouldOpen )
  {
    SchemaMethodImplementationEditor * Editor = new SchemaMethodImplementationEditor (
      SchemaClass, SchemaMethod, SchemaMethod->find_implementation ( Row.at ( 1 ).toStdString() ) );
    Editor->show();
  }
}

void dbse::SchemaMethodEditor::BuildModelImplementationSlot()
{
    BuildModels();
}
