

How to use DBE at P1
====================

At P1 (ATLAS site) the configuration files are handled by the OKS-Server. 
The files are stored in a CVS repository under ``/atlas/oks/tdaq-*``, and by default they are not writable by users.
Configuration files stored there have to be checked-out, edited and then commited back to the CVS repository. For details, see `OKS-server documentation and User's Guide <https://twiki.cern.ch/twiki/bin/viewauth/Atlas/DaqHltOks>`_


**DBE can transparently manage this**, delegating the necessary machinery to the underlain Config package. But the environment has to be setup as described here below.


First of all, check your environment:

.. code-block:: bash

  printenv | grep TDAQ_DB

if you see something like this:

.. code-block:: bash

  TDAQ_DB_REPOSITORY=/atlas/oks/tdaq-04-00-01
  TDAQ_DB_USER_REPOSITORY=/atlas-home/0/rbianchi/db/tdaq-04-00-01
  TDAQ_DB_PATH=/atlas/oks/tdaq-04-00-01

it means that you have set a working area folder (the directory pointed by the variable TDAQ_DB_USER_REPOSITORY). In this case you have to checkout and commit files from/to OKS-Server manually, as you would do using any other editor (VI, Emacs, ...).
For details, see `OKS-server documentation and User's Guide <https://twiki.cern.ch/twiki/bin/viewauth/Atlas/DaqHltOks>`_

If you want that DBE will take care of the checkout and commit phases (via the Config plugin), you have to unset the USER environment variable. So let's unset them with:

.. code-block:: bash

  unset TDAQ_DB_USER_REPOSITORY
  unset TDAQ_DB_PATH

and in the end you should have something like this:

.. code-block:: bash

  printenv | grep TDAQ_DB
  TDAQ_DB_REPOSITORY=/atlas/oks/tdaq-04-00-01

With this setup DBE commit your changes to the repository in a transparent way for the final user (delegating the checkout/commit to the OKS plugin layer).

