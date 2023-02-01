
Using user's script with the Python interface
=============================================

.. note:: New in ``dbe-01-05-00``


Users can use their own custom Python scripts to extend the editor functionalities through the Python interface.

The objects in the OKS database files can be accessed and modified through all functions of the TDAQ PartitionMaker tool, exactly as like as the user's script were run in stand-alone mode.

The user can also use the Qt framework, through the PyQt bindings, to create GUI panels and buttons in the script.

Python stdout redirected to DBE
-------------------------------

The output of the Python scripts, when run embedded in DBE, is caught by DBE and redirected to the `Python` tab in the `Info Tabs` window of the DBE interface. If you don't see the `Info Tab`, try reloading the standard visualization settings from the ``View`` DBE menu.


Decorated functions
-------------------

Python, Qt or PartitionMaker class methods and functions which start new applications, access or update the database files or exit the script are internally decorated in DBE to ensure a safe and smooth running of the users' scripts inside the embedded environment. 

For example: when you call ``pm.project.Project(filename)`` in your script, DBE returns to you an instance of the currently opened DB file if ``filename`` matches the path of the current DB, so the modifications you make can be then saved by DBE into the current DB file. Otherwise, if ``filename`` is the path of another DB file, DBE passes your request on to the original PartitionMaker function and a new instance of the DB file is returned to you.  

At the end of this section a list of the currently decorated functions is given.


Convenience global variables
----------------------------

Two convenience global variables are added so far by the DBE in the main Python global space, and you can use them in your scripts if you want, although they are not needed to run your scripts in DBE.

``dbe_running``
~~~~~~~~~~~~~~~

You usually don't need to know where your script is being run. Because the script runs fine inside DBE without changes. But sometimes you could want, for example, to change a GUI: let's say disable a button if you're in embedded mode. 

So if in your script you need to know if the script is being run embedded or not, you can check if the global variable ``dbe_running`` exists.
In the example here below we check it and we disable a button of the GUI while in embedded mode.

.. code-block:: python

    if ('dbe_running' in globals() ):
        print "Ehi! I'm running inside DBE! I don't need the button A."
        buttonA.setDisabled(True)
    else:
        print "I'm running in stand-alone mode, I need all buttons."


``database``
~~~~~~~~~~~~

You can use this global variable in order to directly get a ``pm.project.Project()`` instance of the DB currently opened in DBE. And you can use it directly with all PartitionMaker methods

.. code-block:: python

    database.addInclude("newfile.data.xml")
    
    database.getObject("Computer", "my_pc")



Standard PartitionMaker scripts
-------------------------------

You can run standard stand-alone PartitionMaker scripts inside the DBE to edit the DB file currently opened in the DBE.
For example you could want to add a whole ``EF`` segment to the file you are editing, and for that you could use the ``pm_part_ef.py``. Let's see how.

Click on ``Plugins-->Python script``, then choose the standard ``pm_part_ef.py`` which should be stored for example here: ``/afs/cern.ch/atlas/project/tdaq/inst/tdaq/tdaq-04-00-01/installed/share/bin/pm_part_ef.py``

Insert ``-p filename`` in the command-line argument string in the form, where ``filename`` should be the name of the DB file you are editing in DBE in that moment. This is needed because the ``pm_part_*.py`` scripts set the DB file name equal to the partition name, which is set with the ``-p`` argument.
You need to add this argument when using most of the standard ``pm_part_*.py`` scripts.


Script examples
---------------

The easiest way to show the behaviour of the DBE Python interface is giving some real examples. 

| You can find those examples installed with the DBE, or you can look at them on-line in the "examples" folder here: 
| http://pcatd12.cern.ch/lxr/source/dbe/examples/


1. Simple script using PartitionMaker
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

| We start with a simple script using PartitionMaker functions to list and modify object attributes. You can also find the script here: 
| http://pcatd12.cern.ch/lxr/source/dbe/examples/dbe_embedded_python_script_example.py

In this example you can see how to perform the basic operations on the OKS DB files: opening the DB, retrieving objects and modifying them.

The script is a normal ``pm.project`` script, and it can be run stand-alone or embedded without any modification. You can see at the beginning of the script that the DB file is open with the normal syntax:

.. code-block:: python

   print "opening the DB file through PM"
   proj = pm.project.Project("example_db.data.xml")
  
Because of the decoration of certain PartitionMaker methods, when run embedded, an instance of the DB file opened in DBE is passed to ``proj`` if the file name the user sets matches the path of the DB loaded in DBE. So all subsequent operations on it will be caught by DBE, and all updating methods (create, remove, ...) will be handled like normal DBE operations, passing the same checks. If the file name is different from the DB path opened in DBE, then the requested action will be redirected to the normal PartitionMaker instance.




2. An example of custom GUI using PyQt
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this second example you can see how to build a very simple custom GUI, with PyQt, to graphically display a list of objects retrieved from the DB.

| You can find the example here: 
| http://pcatd12.cern.ch/lxr/source/dbe/examples/dbe_embedded_python_custom_QT_gui_example.py

3. A more complete example of custom GUI using PyQt
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A second, more complete example of a custom GUI.

| You can find the example here:
| http://pcatd12.cern.ch/lxr/source/dbe/examples/dbe_embedded_python_custom_QT_gui_example_extended.py



4. An example of a custom GUI using a Model (MVC pattern)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this fourth example a more complete GUI using the Model-View-Delegate pattern (a version of the Model-View-Controller "MVC" pattern) is used. A Model is filled with objects retrieved from the DB file with PartitionMaker methods, and different properties of them are then passed to a View to be shown to the user.

| You can find the example here: 
| http://pcatd12.cern.ch/lxr/source/dbe/examples/dbe_embedded_python_custom_QT_gui_example_extended_with_Model.py





List of the decorated classes and functions
-------------------------------------------

Here you can find the list of the functions and classes currently decorated in DBE. The decorated functions redirect users' requests to the original functions, to a custom version of them when needed; for example: the ``pm.project.updateObjects(obj)``, when used on the DB file currently opened in DBE, is redirected to the DBE methods to save and update objects. If it's otherwise used on a different DB file the original ``pm`` function is used.

The redirection is done in a transparent way to the final user, so the same Python script can be used both in stand-alone and in embedded mode inside DBE.

Here below the list of the functions and classes currently decorated in DBE. More can be added if needed.


   * PartitionMaker ``pm`` modules:

      * ``pm.project.Project()`` class:
         * ``__new__()``
         * ``__init__()``
         * ``updateObjects()``
         * ``update_dal()``
         * ``addObjects()``
         * ``removeObjects()``    
         * ``addInclude()``    
         * ``removeInclude()``    
    

   * Qt framework:

      * ``QApplication()`` class:

         * Since only one QApplication instance can be created in a single application, and DBE already starts an instance of this class for its own purpose, the Qt class has been decorated to ignore all calls from the user's script when run embedded in DBE.

   * ``sys`` module:

      * ``sys.exit()``, called from embedded scripts, only shows a message on the screen informing the user that the script is exited. This prevents the possibility that the user could close the whole DBE application from the Python script.



   * ``os`` module:

      * ``os.remove()`` function has been decorated in DBE to prevent the user's script to delete the DB file opened in the DBE.


`Disclaimer`
------------

The new Python interface has been tested with the standard ``pm_part_*.py`` scripts. They work properly inside DBE without any changes.

It has been also well tested with private scripts using PartitionMaker to access and edit the DB objects, and PyQt for building custom GUIs.

I didn't tested other actions one can perform from Python scripts, like SSH connections, external DB querys and so on. If you try them please let me know. If you'll have problems I can add more decorated objects to the DBE Python interface mechanism or I can fix the problem. If, on the contrary, you'll get them working without problems...I'll can be proud of my excellent Python interface! ;-)


