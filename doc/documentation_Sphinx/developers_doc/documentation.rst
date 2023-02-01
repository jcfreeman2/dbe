###################################
Building the Documentation
###################################


DBE has two main documentation items: the Sphinx (hosting this and the User's Manual) and the Doxygen one. You find them at:

   * Doxygen: http://atlasdaq.cern.ch/dbe/doxygen_doc/html/
   * Sphinx:  http://atlasdaq.cern.ch/dbe/

For documentation on Doxygen and Sphinx see here:

   * http://www.stack.nl/~dimitri/doxygen/index.html
   * http://sphinx.pocoo.org/



Sphinx documentation
====================

The "User's Manual" and the "Developer's Notes" are made of several ``.rst`` files which you find inside the ``doc/documentation_Sphinx/`` folder.

They are organized in two main groups: the "User's Guide" and the "Developer's Notes".

The root file of all the Sphinx documentation is the file ``doc/documentation_Sphinx/index.rst``, which is then converted, for example, into the ``index.html`` file responding at the URL http://atlasdaq.cern.ch/dbe/.

The build of the Sphinx doc is defined inside the script ``doc/documentation_Sphinx/make_html_doc_atlasdaq.sh``. It can be run by hand, or through the main script ``doc/build_documentation.sh`` (see below).

The Sphinx script also build the HTML version of the CHANGES.txt file, which contains the list of the changes introduced in DBE by the developers. 


Doxygen documentation
=====================

The private Doxygen documentation for DBE is here:
http://atlasdaq.cern.ch/dbe/doxygen_doc/html/

You can find the Doxygen configuration file for the DBE project here: ``doc/doxygen_doc/Doxyfile``.



Building the documentation
==========================

**Please Notice**: in order to build the Sphinx doc (and so the "all docs") you must have Sphinx installed on your system and no TDAQ environment sourced.

The doc is built launching the BASH script ``build_documentation.sh`` which can be found inside the ``doc/`` folder.

To run it:

.. code-block:: bash

   cd doc/
   ./build_documentation.sh all

**Options:**

   * If you want to see the options, just type ``./build_documentation.sh -h``.
   * To build the Doxygen doc only, type: ``./build_documentation.sh doxy``.
   * To build the Doxygen doc only, type: ``./build_documentation.sh sphinx``.

At the end of each step, the script will copy the output folder to the ``atlasdaq.cern.ch`` web server, in the folders:

   * ``/var/www/html/dbe/`` - the Sphinx doc
   * ``/var/www/html/dbe/doxygen_doc/`` - the Doxygen doc

**Please Notice:** You must have write permission on those folders in order to be able to run the building script smoothly. Contact the sysadmin to get write permission on those folders.
At the time of writing the ``atlasdaq`` sysadmin is *Luca*; otherwise ask *Giovanna*.


