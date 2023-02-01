# dbe

Package: [dbe](https://gitlab.cern.ch/atlas-tdaq-software/dbe)  
Jira: [ATLASDBE](https://its.cern.ch/jira/browse/ATLASDBE)  

## tdaq-09-03-00

-   The GIT checkout directory is properly identified so that the user can use it to add new files;
-   The object editor widget is refreshed in case of a database reload;
-   Unit tests retrieve configuration from `GIT`.

## tdaq-09-02-01

-   Schema editor: the schema file to be loaded can be passed as a command line argument using the `-f` option;
-   Support for new archival mechanism based on the `OKS-GIT` back-end ([ADTCC-241](https://its.cern.ch/jira/browse/ADTCC-241)): 
    
    Configurations archived in the `GIT` repository can be loaded using the `-v` command line option and specifying the `GIT` hash corresponding the desired configuration. 

## tdaq-09-00-00

-   Improved filtering and searching [ATLASDBE-144](https://its.cern.ch/jira/browse/ATLASDBE-144):  

    The filter (or the auto-completion) in drop-down menus now works on *tokens* (*i.e.,* if `Computers` have names like `pc-tdq-onl-*` or `pc-tdq-tpu-*`, in order to get only tpu-like nodes, typing *tpu* is enough).
  
-   Fixing problem building table after the move to `Qt5` [ATLASDBE-241](https://its.cern.ch/jira/browse/ATLASDBE-241);
-   Improved sorting for table [ATLASDBE-246](https://its.cern.ch/jira/browse/ATLASDBE-246);
-   Several improvements to the schema editor [ATLASDBE-239](https://its.cern.ch/jira/browse/ATLASDBE-239).

