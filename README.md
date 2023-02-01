# Testing

To run the tests against the official OKS database
in git in your work area, define DBE_ROOT with a
URL prefix to access the git repo.

E.g. if you have a kerberos token:

    cmake -D DBE_ROOT=https://:@gitlab.cern.ch

or if you have setup your SSH key:

    cmake -D DBE_ROOT=ssh://git@gitlab.cern.ch:7990

Tests are automatically enabled if the CI_TOKEN 
environment variable is set in a gitlab job.

Otherwise they are disabled as their is no way to
guess how the current environment can access the
OSK database.
