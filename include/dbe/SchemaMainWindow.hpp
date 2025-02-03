#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QModelIndex>
#include <QMenu>
#include <QSortFilterProxyModel>
#include "dbe/SchemaCustomFileModel.hpp"
#include "dbe/SchemaCustomTableModel.hpp"

class QGraphicsView;

namespace dbse
{
  class SchemaTab;

namespace Ui
{
class SchemaMainWindow;
}  // namespace Ui

class SchemaMainWindow: public QMainWindow
{
  Q_OBJECT
public:
  ~SchemaMainWindow();
  explicit SchemaMainWindow ( QString SchemaFile, QWidget * parent = nullptr );

private:
  std::unique_ptr<dbse::Ui::SchemaMainWindow> ui;

  CustomFileModel * FileModel;
  CustomTableModel * TableModel;
  QSortFilterProxyModel * proxyModel;
  QMenu * ContextMenuFileView;
  QMenu * ContextMenuTableView;
  QString Title{"DUNE DAQ Configuration Schema editor"};
  QString m_view_dir{"."};
  void InitialSettings();
  void InitialTab();
  void InitialTabCorner();
  void SetController();
  void BuildFileModel();
  void BuildTableModel();
  void write_view_file(const QString& fn, SchemaTab* tab);
  [[nodiscard]] int ShouldSaveChanges() const;
  [[nodiscard]] int ShouldSaveViewChanges() const;
protected:
  void closeEvent ( QCloseEvent * event );
  void focusInEvent( QFocusEvent * event ) override;
  void OpenSchemaFile( QString SchemaFile);
private slots:
  void OpenSchemaFile();
  void CreateNewSchema();
  void LaunchIncludeEditor();
  void LaunchIncludeEditorActiveSchema();
  // From main menu / shortcut
  void SaveSchema();
  // From FileView 
  void SaveSchemaFile();
  void SaveModifiedSchema();
  void ChangeCursorRelationship ( bool State );
  void ChangeCursorInheritance ( bool State );
  void AddTab();
  void SaveView();
  void SaveViewAs();
  void LoadView();
  void NameView();
  void LaunchClassEditor ( QModelIndex Index );
  void BuildTableModelSlot();
  void BuildTableModelSlot ( QString ClassName );
  void RemoveTab ( int i );
  void CustomContextMenuFileView ( QPoint Pos );
  void CustomContextMenuTableView ( QPoint Pos );
  void AddNewClass();
  void RemoveClass();
  void editClass();
  void SetSchemaFileActive();
  void PrintCurrentView();
};

}  // namespace dbse
#endif // MAINWINDOW_H
