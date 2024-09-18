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

namespace Ui
{
class SchemaMainWindow;
}  // namespace Ui

class SchemaMainWindow: public QMainWindow
{
  Q_OBJECT
public:
  ~SchemaMainWindow();
  explicit SchemaMainWindow ( QWidget * parent = nullptr );
  explicit SchemaMainWindow ( QString SchemaFile, QWidget * parent = nullptr );

private:
  std::unique_ptr<dbse::Ui::SchemaMainWindow> ui;

  CustomFileModel * FileModel;
  CustomTableModel * TableModel;
  QSortFilterProxyModel * proxyModel;
  QMenu * ContextMenuFileView;
  QMenu * ContextMenuTableView;
  void InitialSettings();
  void InitialTab();
  void InitialTabCorner();
  void SetController();
  void BuildFileModel();
  void BuildTableModel();
  int ShouldSaveChanges() const;
protected:
  void closeEvent ( QCloseEvent * event );
  void focusInEvent( QFocusEvent * event ) override;
  void OpenSchemaFile( QString SchemaFile);
private slots:
  void OpenSchemaFile();
  void CreateNewSchema();
  void LaunchIncludeEditor();
  void LaunchIncludeEditorActiveSchema();
  void SaveSchema();
  void SaveModifiedSchema();
  void ChangeCursorRelationship ( bool State );
  void ChangeCursorInheritance ( bool State );
  void AddTab();
  void SaveView();
  void LoadView();
  void LaunchClassEditor ( QModelIndex Index );
  void BuildTableModelSlot();
  void BuildTableModelSlot ( QString ClassName );
  void RemoveTab ( int i );
  void CustomContextMenuFileView ( QPoint Pos );
  void CustomContextMenuTableView ( QPoint Pos );
  void AddNewClass();
  void RemoveClass();
  void SetSchemaFileActive();
  void PrintCurrentView();
};

}  // namespace dbse
#endif // MAINWINDOW_H
