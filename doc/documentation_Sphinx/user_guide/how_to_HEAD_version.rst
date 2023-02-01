
How to use the Nightly or to compile the HEAD version
=====================================================

If you want to test the nightly or the HEAD version of the DBE database editor.

How to setup the `nightly`:
---------------------------

In the ``nightly`` version of the DBE you can find the most recent bugfixes. The ``nightly`` is a stable version. When a certain  number of bug-fixes or new features has been collected and tested in the nightly, it is usually gathered in a patch and applied to the release.

To use the nightly version, setup the ``nightly`` release and start the ``nightly`` version of the DBE:

.. code-block:: bash

  source /afs/cern.ch/atlas/project/tdaq/cmt/bin/cmtsetup.sh nightly
  dbe



How to checkout and compile the `HEAD` version:
-----------------------------------------------

In the ``HEAD`` version you can find work in progress bug-fixes and new features. The HEAD usually compiles, but it is not intended to be stable. 

*Use it at your own risk!*

To use the HEAD version, start setting up the release: 

.. code-block:: bash

  source /afs/cern.ch/atlas/project/tdaq/cmt/bin/cmtsetup.sh tdaq-04-00-01

Checkout the HEAD version of the "dbe" package:

.. code-block:: bash

  $TDAQ_DIR/cmt/bin/getpkg dbe

configure and compile:

.. code-block:: bash

  cd dbe/cmt/
  cmt config
  source setup.sh
  gmake -j2; gmake inst

Start the editor at command line:  

.. code-block:: bash

  dbe

To see all the command-line options (*more to be added*) type:

.. code-block:: bash

  dbe --help

