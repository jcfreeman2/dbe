#ifndef SCHEMAINCLUDEFILEWIDGET_H
#define SCHEMAINCLUDEFILEWIDGET_H

#include <memory>
/// Including QT Headers
#include <QFileDialog>
#include <QStatusBar>
#include <QWidget>
/// Including DBE
//#include "dbe/CreateDatabaseWidget.hpp"
#include "dbe/StyleUtility.hpp"

using dbe::StyleUtility;

namespace dbe::Ui
{
class IncludeFileWidget;
}  // namespace dbse::Ui

namespace dbse
{
class SchemaIncludeFileWidget: public QWidget
{
  Q_OBJECT
public:
  ~SchemaIncludeFileWidget();
  explicit SchemaIncludeFileWidget ( QString FilePath, QWidget * parent = 0 );

private:
  void SetRemoveComboBox();
  void SetController();

  std::unique_ptr<dbe::Ui::IncludeFileWidget> ui;

  // CreateDatabaseWidget * CreateWidget;
  QString CurrentFile;
  QString Directory;
  bool Removed;
  QStatusBar * StatusBar;
  QFileDialog * SelectFile;
  QStringList FolderPathList;
  QStringList dbPath;

private slots:
  void SelectFileToInclude();
  void AddFileToInclude();
  void AddNewFileToInclude ( const QString & File );
  void RemoveFileFromInclude();
  void RemoveFileFromInclude ( int );
  void SetDirectory ( const QString & Dir );
  void CheckInclude ();
  // void CreateFileToInclude();
};
}  // namespace dbse

#endif // SCHEMAINCLUDEFILEWIDGET_H
