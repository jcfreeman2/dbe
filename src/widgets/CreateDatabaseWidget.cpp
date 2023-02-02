/// Including QT Headers
#include<QFileDialog>
#include<QMessageBox>
/// Including DBE
#include"CreateDatabaseWidget.h"
#include"StyleUtility.h"
/// Including config headers
#include "config/ConfigObject.hpp"
#include "config/Configuration.hpp"
#include "config/Schema.hpp"
#include "Exceptions.h"
#include "messenger.h"

dbe::CreateDatabaseWidget::CreateDatabaseWidget ( QWidget * parent, bool Include,
                                                  const QString & CreateDir )
  : QWidget ( parent ),
    StatusBar ( nullptr ),
    DirToCreate ( CreateDir ),
    CreateToInclude ( Include )
{
  setupUi ( this );

  connect ( SchemaButton, SIGNAL ( clicked() ), this, SLOT ( DefineSchema() ),
            Qt::UniqueConnection );
  connect ( SelectButton, SIGNAL ( clicked() ), this, SLOT ( DefineDatabaseFile() ),
            Qt::UniqueConnection );

  if ( !CreateToInclude )
  {
    connect ( CreateDatabaseButton, SIGNAL ( clicked() ), this,
              SLOT ( CreateDatabaseFileLoad() ),
              Qt::UniqueConnection );
    connect ( CreateNoLoadDatabaseButton, SIGNAL ( clicked() ), this,
              SLOT ( CreateDatabaseFileNoLoad() ), Qt::UniqueConnection );
  }
  else
  {
    connect ( CreateDatabaseButton, SIGNAL ( clicked() ), this,
              SLOT ( CreateDatabaseFileInclude() ), Qt::UniqueConnection );
    CreateDatabaseButton->setText ( "Create Database File" );
    CreateNoLoadDatabaseButton->hide();
  }

  StatusBar = new QStatusBar ( StatusFrame );
  StatusBar->setSizeGripEnabled ( false );
  StatusFrame->setFrameStyle ( QFrame::NoFrame );
  StatusLayout->addWidget ( StatusBar );

  StatusBar->setAutoFillBackground ( true );
  StatusBar->showMessage ( "Select schema files and define new database file!" );
  CreateDatabaseButton->setDisabled ( true );
  CreateNoLoadDatabaseButton->setDisabled ( true );

  DatabaseName->setReadOnly ( true );
}

void dbe::CreateDatabaseWidget::DefineSchema()
{
  QString SchemaFilePath = QFileDialog::getOpenFileName ( this, tr ( "Open Schema File" ),
                                                          DirToCreate.append("./"),
                                                          tr ( "XML files (*.xml)" ) );
  QFileInfo SchemaFile = QFileInfo ( SchemaFilePath );

  if ( !SchemaFile.isFile() )
  {
    StatusBar->setPalette ( StyleUtility::AlertStatusBarPallete );
    StatusBar->showMessage ( QString ( "The file is not accessible. Check before usage" ) );
  }
  else
  {
    if ( SchemaFile.fileName().contains ( "schema" ) ) SchemaCombo->addItem (
        SchemaFile.absoluteFilePath() );
    else
    {
      StatusBar->setPalette ( StyleUtility::AlertStatusBarPallete );
      StatusBar->showMessage (
        QString ( "The file %1 is not a schema file" ).arg ( SchemaFile.absoluteFilePath() ) );
    }
  }
}

void dbe::CreateDatabaseWidget::DefineDatabaseFile()
{
  QString Dir;

  if ( DirToCreate.isEmpty() )
  {
    Dir = QString ( "./NewDatabaseFile.data.xml" );
  }
  else
  {
    Dir = DirToCreate.append ( "/NewDatabaseFile.data.xml" );
  }

  QString DatabaseFilePath = QFileDialog::getSaveFileName ( this,
                                                            tr ( "Save to Database File" ),
                                                            Dir,
                                                            tr ( "XML files (*.xml)" ) );
  DatabaseFile = QFileInfo ( DatabaseFilePath );

  DatabaseName->setText ( DatabaseFile.absoluteFilePath() );
  CreateDatabaseButton->setEnabled ( true );
  CreateNoLoadDatabaseButton->setEnabled ( true );
}

void dbe::CreateDatabaseWidget::CreateDatabaseFileLoad()
{
  Configuration db ( "oksconfig" );

  try
  {
    std::list<std::string> Includes;

    for ( int i = 0; i < SchemaCombo->count(); ++i )
    {
      QString Item = SchemaCombo->itemText ( i );
      Includes.push_back ( Item.toStdString() );
    }

    const std::string DatabaseName = DatabaseFile.absoluteFilePath().toStdString();
    db.create ( DatabaseName, Includes );
    db.commit();

    QMessageBox::information (
      0, tr ( "DBE" ),
      QString ( "Database was created.\nNow the DB will be loaded into the Editor!\n" ),
      QMessageBox::Ok );
    emit CanLoadDatabase ( DatabaseFile.absoluteFilePath() );
    close();
  }
  catch ( daq::config::Exception const & ex )
  {
    FAIL ( "Database creation failure", dbe::config::errors::parse ( ex ).c_str() );
  }
}

void dbe::CreateDatabaseWidget::CreateDatabaseFileNoLoad()
{
  Configuration db ( "oksconfig" );

  try
  {
    std::list<std::string> Includes;

    for ( int i = 0; i < SchemaCombo->count(); ++i )
    {
      QString Item = SchemaCombo->itemText ( i );
      Includes.push_back ( Item.toStdString() );
    }

    const std::string DatabaseName = DatabaseFile.absoluteFilePath().toStdString();
    db.create ( DatabaseName, Includes );
    db.commit();

    QMessageBox::information ( 0, tr ( "DBE" ), QString ( "The Database was created.\n" ),
                               QMessageBox::Ok );
    close();
  }
  catch ( daq::config::Exception const & ex )
  {
    FAIL ( "Database creation failure", dbe::config::errors::parse ( ex ).c_str() );
  }
}

void dbe::CreateDatabaseWidget::CreateDatabaseFileInclude()
{
  Configuration db ( "oksconfig" );

  try
  {
    std::list<std::string> Includes;

    for ( int i = 0; i < SchemaCombo->count(); ++i )
    {
      QString Item = SchemaCombo->itemText ( i );
      Includes.push_back ( Item.toStdString() );
    }

    const std::string DatabaseName = DatabaseFile.absoluteFilePath().toStdString();
    db.create ( DatabaseName, Includes );
    db.commit();

    QMessageBox::information ( 0, tr ( "DBE" ), QString ( "The Database was created.\n" ),
                               QMessageBox::Ok );
    emit CanIncludeDatabase ( DatabaseFile.absoluteFilePath() );
    close();
  }
  catch ( daq::config::Exception const & ex )
  {
    FAIL ( "Database creation error", dbe::config::errors::parse ( ex ).c_str() );
  }
}
