#ifndef SUBTREEPROXYMODEL_H
#define SUBTREEPROXYMODEL_H

/// Including QT Headers
#include <QUuid>
#include <QSortFilterProxyModel>

/// Including DBE Headers
#include "TreeModelInterface.h"
#include "model_common_interface.h"

#include <vector>
#include "datahandler.h"

namespace dbe
{

namespace models
{

class subtree_proxy:
  public QSortFilterProxyModel,
  public TreeModelInterface,
  public model_common_impl<subtree_proxy>,
  public model_common_async_operations<subtree_proxy>
{
  Q_OBJECT

  MODEL_COMMON_IMPL_REQ_DEF ( subtree_proxy )

public    :

  explicit subtree_proxy ( const QString & Name, const QStringList & Default,
                           QObject * parent = nullptr );
  ~subtree_proxy();

  void ResetModel();

  void check_view_type();

  void LoadClasses();

  treenode * getnode ( const QModelIndex & index ) const;

protected:

  bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;

  bool lessThan (
    const QModelIndex & left, const QModelIndex & right ) const;

private:
  bool ApplyDefaultFilter ( int source_row, const QModelIndex & source_parent ) const;

  bool ApplyUserFilter (
    int source_row, const QModelIndex & source_parent ) const;

  QString this_gui_windowname;
  QStringList this_default_filter;
  Window this_window_config;

  bool this_apply_default_filter;

private slots:
  void slot_create_object ( QString const & src, dref const & obj );
  void slot_remove_object ( QString const & src, dref const & obj );
  void slot_update_object ( QString const & src, dref const & obj );
  void slot_rename_object ( QString const & src, dref const & obj );

};
}
// namespace models
}// namespace dbe
#endif // SUBTREEPROXYMODEL_H
