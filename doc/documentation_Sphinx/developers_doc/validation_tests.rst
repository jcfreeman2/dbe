
Validation Tests
================


Here some tests which are performed before submitting a new release:

   * Create a new DB file, with a schema and then load it into the editor. 

   * Add an include file.

   * Create two objects in parallel.
     Open the "Create New Object" window for OnlineSegment
     Open another "Create New Object" window for another OnlineSegment object, and create this second object. Then create the first one opened.

   * Create two objects with dependencies.
     Create a new Partition. While creating it, create a new OnlineSegment object, then reload the OnlineSegment list in the Partition editor window referencing the new one which has just been created.

   * Create an object with the name of another object already present in the DB. --> DBE should not accept it.

   * Create a new object, setting its new UID, then click on the exit button of the window. --> DBE should asks to you if you want discard changes.

   * Perform a bulk change of a set of objects (e.g. "Computer") and change their Attribute and Relationship values.

   * Test the "Find" on the TableView columns. Check if the selection is still in a valid state (or null state) after a sorting of the columns.

   * Run a Python script performing a consistency check in the embedded Python interface.

   * Remove a simple object. --> DBE should remove the object.

   * Remove a referenced object. --> DBE should not remove  the object and it should prompt a warning message.

   * Remove an include file from the opened DB




   
