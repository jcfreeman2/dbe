=========
Resources
=========

All resources (like icons, stylesheets, ...) are declared in the ``resources.qrc`` file.

Once declared there, one can access them following the Qt Resource schema, e.g. with ``:/icons/redo.png`` for an icon file ``redo.png`` stored in the folder ``icons`` and declared in the ``resources.qrc`` file.

.. warning::
   Default Qt programs use ``qmake`` to compile, which also reads a ``.pro`` file where the resource file is specified.
   As we are using CMT to compile our package, the resource file is declared in the CMT ``cmt/requirements`` file, with this line::

      qt_embedimages proj=DBEimg images=../resources.qrc

   this ensures that the resource ``.qrc.`` file is read and processed in compilation file, and the resources allocated in the Qt virtual file system for the application.

