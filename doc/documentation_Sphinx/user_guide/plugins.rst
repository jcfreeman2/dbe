
Building User Customized Plugins
================================

The user can build custom plugins following the code of the three examples shipped with the package, which can be found under the folder ``examples/``.

Those plugins are specialized custom tests and consistency checks, to be run on databases from the `"dbe"` editor. 

For instance, the example plugin called:
   * `"P1Check"` checks whether all partition objects are using rdbconfig.
   * `"RunsOn"` checks that no application runs on a host that is marked as OFF in the database.
   * `"StartStopCheck"` performs two checks:
      1. Whether all runcontrol applications have start-at = boot
      2. Whether all runcontrol applications have stop-at = shutdown

The user can build custom plugins following the examples. After created a new folder under ``examples/``, where to save the custom code, build of the plugins can be invoked typing one of the following two equivalent commands:

   * cmt do buildPlugins
   * gmake buildPlugins

The generated ``.so`` shared libraries will be saved under the folder ``plugins/``, from where they will can be loaded into the `"dbe"` editor, from the menu `Plugins`.

.. warning::
    
   Sometimes, for some reasons, the ``qmake`` command does not work properly in combination with CMT. And this misbehaviour results  in a corrupted generated Makefile.

   If the plugins do not compile, check the Makefile.

   If you get errors like **"config/Configuration.h" does not exist"**, try to look for wrong include statement inside the automatically generated Makefile. You should remove all ``-I-I`` occurencies, and substitute them with ``-I``.

   If you get errors like this one:
   ``/usr/bin/ld: skipping incompatible /afs/.cern.ch/sw/lcg/external/qt/4.6.3/i686-slc5-gcc43-opt/lib/libQtCore.so when searching for -lQtCore``
   for libraries like:
   ``-lQtCore``, ``-lQtGui``, ``-lpthreads``
   you can safely remove those statements from the Makefile. The correct version of needed library will be taken by CMT.

   If you have problems, contact Ric at: rbianchi@cern.ch




