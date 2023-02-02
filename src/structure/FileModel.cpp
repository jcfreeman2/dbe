/// Including QT Headers
#include <confaccessor.h>
#include <ui_constants.h>

#include <QFileInfo>
#include <QDir>
/// Including DBE Headers
#include "FileModel.h"
#include "config_api_get.h"
//#include "MainWindow.h"

dbe::FileModel::FileModel ( QObject * parent )
  : QAbstractTableModel ( parent ),
    Headers
{ tr ( "Name" ), tr ( "Folder" ), tr ( "Permission" ) }
{
  initpaths();
  initconnections();
  initmodel();
}

dbe::FileModel::FileModel ( QList<QStringList> const & FileList, QObject * parent )
  : QAbstractTableModel ( parent ),
    IncludedFiles ( FileList ),
    Headers
{ tr ( "Name" ), tr ( "Folder" ), tr ( "Permission" ) }
{
  initconnections();
}

dbe::FileModel::~FileModel()
{}

int dbe::FileModel::rowCount ( const QModelIndex & parent ) const
{
  if ( !parent.isValid() )
  {
    return IncludedFiles.size();
  }

  return 0;
}

int dbe::FileModel::columnCount ( const QModelIndex & parent ) const
{
  Q_UNUSED ( parent )
  return Headers.size();
}

QVariant dbe::FileModel::data ( const QModelIndex & index, int role ) const
{
  if ( index.isValid() )
  {
    if ( role == Qt::DisplayRole )
      return QVariant (
               IncludedFiles.at ( index.row() ).at ( index.column() ) );
  }

  return QVariant();
}

QVariant dbe::FileModel::headerData ( int section, Qt::Orientation orientation,
                                      int role ) const
{
  if ( role == Qt::DisplayRole )
  {
    if ( orientation == Qt::Horizontal )
    {
      return Headers.at ( section );
    }

    if ( orientation == Qt::Vertical )
    {
      return section + 1;
    }
  }

  return QVariant();
}

Qt::ItemFlags dbe::FileModel::flags ( const QModelIndex & index ) const
{
  if ( IncludedFiles.at ( index.row() ).at ( static_cast<int>
                                             ( tablepositions::filepermission ) ) == "RW" )
  {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }

  return Qt::ItemIsSelectable;
}

void dbe::FileModel::initpaths()
{
  QString TDAQ_DB_REPOSITORY = getenv ( "TDAQ_DB_REPOSITORY" );
  if(TDAQ_DB_REPOSITORY.isEmpty() == false) {
      QString TDAQ_DB_USER_REPOSITORY = getenv ( "TDAQ_DB_USER_REPOSITORY" );
      FolderPathList = TDAQ_DB_USER_REPOSITORY.split ( ":", QString::SkipEmptyParts );

      //FolderPathList << dbe::MainWindow::findthis()->find_db_repository_dir();
  } else {
      QString TDAQ_DB_PATH = getenv ( "TDAQ_DB_PATH" );
      FolderPathList = TDAQ_DB_PATH.split ( ":", QString::SkipEmptyParts );
  }

  for ( QString & PathName : FolderPathList )
  {
    if ( !PathName.endsWith ( "/" ) )
    {
      PathName.append ( "/" );
    }
  }
}

void dbe::FileModel::initconnections()
{
  connect ( this, SIGNAL ( FileCacheReady ( QList<QStringList> & ) ), &confaccessor::ref(),
            SLOT ( GetFileCache ( QList<QStringList> & ) ) );
}

QString dbe::FileModel::GetFullFileName ( QString & FileName )
{
  QFileInfo FileInfo ( FileName );

  if ( FileInfo.isRelative() )
  {

    for ( const QString & Folder : FolderPathList )
    {
      QDir TdaqFolder ( Folder );

      if ( TdaqFolder.exists ( FileName ) )
      {
        FileName = TdaqFolder.path() + "/" + FileName;
        break;
      }
    }
  }

  return FileName;
}

QList<QStringList> dbe::FileModel::GetFilesInfo() const
{
  return IncludedFiles;
}

void dbe::FileModel::initmodel()
{
  QStringList sources = dbe::config::api::get::file::inclusions (
  { confaccessor::dbfullname() } );

  for ( QString const & source : sources )
  {

    QFileInfo srcinfo ( source );
    QString fn = srcinfo.filePath();

    for ( QString const & path : FolderPathList )
    {
      if ( fn.startsWith ( path ) )
      {
        fn = fn.replace ( 0, path.size(), "" );
        break;
      }
    }

    IncludedFiles.append ( QStringList{ srcinfo.fileName(), fn, confaccessor::check_file_rw ( source ) ? "RW" : "RO" } );
  }

  if ( IncludedFiles.size() )
  {
    emit FileCacheReady ( IncludedFiles );
  }
}
