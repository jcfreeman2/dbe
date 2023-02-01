#############################
Model-View structure in "dbe"
#############################

The model-view design pattern is implemented in "dbe" using the Qt classes.

In particular:

Models:
-------

   * under ``dbe/structure`` there is a number of classes implementing the models needed to handle data:
      * ``partitionmodel.h``
      * ``resourcemodel.h``
   * there is also a class implementing a common interface for all models underlain:
      * ``treemodelinterface.h``


   * in ``dbe/internal/utility.h`` there's an implementation of the getIndexOf(), which returns a "model index"::

      inline QModelIndex getIndexOf(TreeItem* item,QAbstractItemModel* model,QModelIndex rootItem)





Views:
------
   * in ``src/structure/customtableview.cpp``:
      * QTableView
   * in ``src/structure/customtreeview.cpp``:
      * QTreeView


Delegates:
----------

   * in ``dbe/structure/tabledelegate.h`` we have the class ``class RelationShipDelegate : public QItemDelegate``:
      * the ``createEditor()`` function is reimplemented.
      * the function ``RelationShipDelegate::eventFilter()`` switches off the hitting on the "Return" key in order to commit changes if we are editing values in string, numeric, multi-values, relationship custom widgets.




Usage:
~~~~~~

   * in ``mainwindow.cpp`` models are set to views in order to display items, or to clean the view (with a NULL):
      * ``void MainWindow::initTreeTableStructure()``

