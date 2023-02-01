
============================
Developing and compiling DBE
============================

Setting up your workspace
=========================

At the time of writing we are using **TDAQ rel. 04-00-01**.

You can follow the TDAQ development guidelines to setup your working space here: http://atlas-tdaq-sw.web.cern.ch/atlas-tdaq-sw/cmt.html


Basically:

   * source the TDAQ release
   * create a folder for your working space, e.g. ``mkdir work-tdaq401/``
   * inside ``work-tdaq401/`` create a folder for the global CMT settings: ``mkdir cmt``
   * create a file inside the ``cmt/`` folder to tell CMT which release you want to use: ``echo "use tdaq tdaq-04-00-01"  > cmt/project.cmt``.
     Use ``"use tdaq nightly"`` if you want to compile DBE against the nightly release.
   * get the package: ``$TDAQ_DIR/cmt/bin/getpkg dbe``.
     This will check out the HEAD version in a new folder ``dbe/``.



Compiling DBE
=============

Enter the DBE folder and compile it:

   * ``cd dbe/cmt/``
   * Instruct CMT to compile your private copy of DBE:
     ``cmt config``
     ``source setup.sh``
   * Compile DBE:
     ``gmake -j2; gmake inst``
        * ``gmake -j2`` compiles the package (using 2 threads)
        * ``gmake inst`` install the binaries, the libraries, the python scripts, the examples, the documentation. You can see what ``gmake inst`` install in the last part of the ``requirements`` file.

**Please Notice:** every time you change the ``requirements`` file, you have to repeat the whole sequence here above (from ``cmt config`` included).

**Hint:** If you did not change the ``requirements`` file (usually you don't need) and if you already run ``gmake`` once at least, you can use this command for the following compilations: ``gmake -j2 QUICK=1; gmake inst QUICK=1``, this speeds up the compilation time.

**Please Notice:** When you add a new file to the package (e.g. a new class) you have to add a new line in the ``.h``, ``.cpp`` and ``.moc`` list of files in the ``requirements`` file. Notice alos that the ``.moc`` files are automatically **generated** by Qt when compiling. That's why you don't find them in the package itself.


Browsing the code
=================

You can browse the code of all TDAQ packages here: http://atlas-tdaq-sw.web.cern.ch/atlas-tdaq-sw/

and here: http://pcatd12.cern.ch/source/

The DBE code is here: http://pcatd12.cern.ch/source/xref/tdaq/tdaq-04-00-01/dbe/

You can also access to a private DBE Doxygen doc here: https://atlasdaq.cern.ch/dbe/doxygen_doc/html/

