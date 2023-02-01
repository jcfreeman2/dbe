#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

/// Including QT Headers
#include <QWidget>
#include <QUuid>

/// Including DBE
#include "GraphicalClass.h"
/// Including Ui Files
#include "ui_GraphView.h"

#include <confaccessor.h>

namespace dbe
{
namespace Ui
{
class GraphView;
}  // namespace Ui

class GraphView: public QWidget, private Ui::GraphView
{
  Q_OBJECT
public:
  ~GraphView();
  explicit GraphView ( QWidget * parent = nullptr );

  void ConnectActions();
  void SetupView();
  void contextMenuEvent ( QContextMenuEvent * Event );

private slots:
  void GetWindowConfiguration();
  void RedrawObject ( tref Object );
  void RedrawObject();
  void CreateActions();
  void editThisObject();
  void deleteThisObject();
  void copyObject();
  void referencedBy_All();
  void referencedBy_OnlyComposite();

private:
  Window WindowConfiguration;
  //Qt::DropActions supportedDropActions() const;
  QMenu * ContextMenu;
  QAction * editObject;
  QAction * deleteObjectAc;
  QAction * refByAc;
  QAction * refByAcOnlyComp;
  QAction * copyObjectAc;
  GraphicalObject * ClickedItem;
  QUuid const uuid;
};
} // end namespace dbe
#endif // GRAPHVIEW_H
