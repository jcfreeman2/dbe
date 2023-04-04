#include <QMessageBox>
/// Including Schema
#include "dbe/SchemaMethodImplementationEditor.hpp"
#include "dbe/SchemaKernelWrapper.hpp"
#include "ui_SchemaMethodImplementationEditor.h"

using namespace dunedaq::oks;

dbse::SchemaMethodImplementationEditor::~SchemaMethodImplementationEditor() = default;

dbse::SchemaMethodImplementationEditor::SchemaMethodImplementationEditor (
  OksClass * Class, OksMethod * Method, OksMethodImplementation * Implementation, QWidget * parent )
  : QWidget ( parent ),
    ui ( new Ui::SchemaMethodImplementationEditor ),
    SchemaClass ( Class ),
    SchemaMethod ( Method ),
    SchemaImplementation ( Implementation ),
    UsedNew ( false )
{
  QWidget::setAttribute(Qt::WA_DeleteOnClose);
  ui->setupUi ( this );
  InitialSettings();
  SetController();
}

dbse::SchemaMethodImplementationEditor::SchemaMethodImplementationEditor (
  OksClass * Class,
  OksMethod * Method,
  QWidget * parent )
  : QWidget ( parent ),
    ui ( new Ui::SchemaMethodImplementationEditor ),
    SchemaClass ( Class ),
    SchemaMethod ( Method ),
    SchemaImplementation ( nullptr ),
    UsedNew ( true )
{
  QWidget::setAttribute(Qt::WA_DeleteOnClose);
  ui->setupUi ( this );
  InitialSettings();
  SetController();
}

void dbse::SchemaMethodImplementationEditor::SetController()
{
  connect ( ui->SaveButton, SIGNAL ( clicked() ), this, SLOT ( ProxySlot() ) );
  connect ( &KernelWrapper::GetInstance(), SIGNAL ( ClassUpdated ( QString ) ), this,
            SLOT ( ClassUpdated ( QString ) ) );
}

void dbse::SchemaMethodImplementationEditor::ClassUpdated( QString ClassName )
{
  if(!UsedNew && ClassName.toStdString() == SchemaClass->get_name()) {
      if(SchemaMethod->find_implementation(SchemaImplementation->get_language()) != nullptr) {
          FillInfo();
      } else {
          QWidget::close();
      }
  }
}

void dbse::SchemaMethodImplementationEditor::FillInfo()
{
  setObjectName ( QString::fromStdString ( SchemaImplementation->get_language() ) );
  setWindowTitle (
    QString ( "Method Implementation Editor : %1" ).arg (
      SchemaImplementation->get_language().c_str() ) );
  ui->MethodImplementationLanguage->setText (
    QString::fromStdString ( SchemaImplementation->get_language() ) );
  ui->MethodImplementationPrototype->setText (
    QString::fromStdString ( SchemaImplementation->get_prototype() ) );
  ui->MethodImplementationDescription->setPlainText (
    QString::fromStdString ( SchemaImplementation->get_body() ) );
}

void dbse::SchemaMethodImplementationEditor::InitialSettings()
{
  setWindowTitle ( "New Method Implementation" );
  setObjectName ( "NEW" );

  if ( !UsedNew )
  {
      FillInfo();
  }
}

void dbse::SchemaMethodImplementationEditor::ParseToSave()
{
  bool changed = false;
  std::string MethodLanguage = ui->MethodImplementationLanguage->text().toStdString();
  std::string MethodPrototype = ui->MethodImplementationPrototype->text().toStdString();
  std::string MethodDescription = ui->MethodImplementationDescription->toPlainText()
                                  .toStdString();

  if ( MethodLanguage != SchemaImplementation->get_language() )
  {
    KernelWrapper::GetInstance().PushSetMethodImplementationLanguage ( SchemaClass,
                                                                       SchemaMethod,
                                                                       SchemaImplementation,
                                                                       MethodLanguage );
    changed = true;
  }

  if ( MethodPrototype != SchemaImplementation->get_prototype() )
  {
    KernelWrapper::GetInstance().PushSetMethodImplementationPrototype ( SchemaClass,
                                                                        SchemaMethod,
                                                                        SchemaImplementation,
                                                                        MethodPrototype );
    changed = true;
  }

  if ( MethodDescription != SchemaImplementation->get_body() )
  {
    KernelWrapper::GetInstance().PushSetMethodImplementationBody ( SchemaClass,
                                                                   SchemaMethod,
                                                                   SchemaImplementation,
                                                                   MethodDescription );
    changed = true;
  }

  if ( changed )
  {
    emit RebuildModel();
  }

  close();
}

void dbse::SchemaMethodImplementationEditor::ParseToCreate()
{
  if ( ui->MethodImplementationLanguage->text().isEmpty() )
  {
    QMessageBox::warning (
      0, "Schema editor",
      QString ( "Please Provide a Language for the method implementation !" ) );
    return;
  }

  if ( ui->MethodImplementationPrototype->text().isEmpty() )
  {
    QMessageBox::warning (
      0, "Schema editor",
      QString ( "Please Provide a prototype for the method implementation !" ) );
    return;
  }

  std::string MethodLanguage = ui->MethodImplementationLanguage->text().toStdString();
  std::string MethodPrototype = ui->MethodImplementationPrototype->text().toStdString();
  std::string MethodDescription = ui->MethodImplementationDescription->toPlainText()
                                  .toStdString();

  KernelWrapper::GetInstance().PushAddMethodImplementationComand ( SchemaClass,
                                                                   SchemaMethod,
                                                                   MethodLanguage,
                                                                   MethodPrototype,
                                                                   MethodDescription );
  emit RebuildModel();
  close();
}

void dbse::SchemaMethodImplementationEditor::ProxySlot()
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
