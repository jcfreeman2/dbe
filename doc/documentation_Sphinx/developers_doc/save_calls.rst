

When a multi-value Relationship (like "Segments") is changed from the Table View:
---------------------------------------------------------------------------------

   * The user clicks on "Save" button, in the editor widget. This makes emit the finishEdit() signal.
   * The RelationShipDelegate::commitAndClose() slot is triggered, which in turn emits the commitData() signal.
   * the RelationShipDelegate::setModelData() is triggered, which calls the method setData() of the underlain table model.
   * TableSelectionModel::setData()   --->  calling: sourceModel()->setData(sourceParent,value,role);
   * TableModel::setData()  -- here the item is recognized as RelationshipItem and set_rel_value() is called
   * TableModel::set_rel_value() calls ConfigWrapper::ChangeObjectRef(object,field,newValue,rel);
   * ConfigWrapper::ChangeObjectRef() puts in the undo stack an instance of the undo command: ChangeObjs<std::vector<std::string> > (rObj)
   * undocommands.h: the constructor of ChangeObjs() get the config objects which are the new objects, and push them in the object vector. Then its redo() function makes the actual work: it calls the ConfigWrapper::SetRels(m_obj, m_field, m_newValue2) to store the new values for the relationship.
   * ConfigWrapper::SetRels() calls the method CongigObject::set_objs() on the CongigObject obj:  obj.set_objs(field, value). Then it emits the ObjectRelationshipUpdate(obj) signal.
   * The signal is caught by MainWindow::objectRelationshipUpdated().
   * In MainWindow::objectRelationshipUpdated() the treeView push its state to store it, then the treeModel->objectRelationshipUpdated(robj) is called; after that the treeView pull its state again to go back where it was.
   * When the TreeModel::objectRelationshipUpdated() is called, the rebuild() function on the model itself is called. Nothing else.
   * This rebuild() triggers the call of TreeModel::setupModelData().


When a multi-value Relationship (like "Segments") is changed from the Tree View:
--------------------------------------------------------------------------------

