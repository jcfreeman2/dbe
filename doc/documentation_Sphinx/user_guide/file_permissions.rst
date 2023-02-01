
File permissions
================

In the "Files" table displayed in the "Info Tabs" (at the bottom of the main window, if the default layout is used by the user)  you can see the list of all files containing the Database objects. For each of the files the third column displays the user's file access permissions. 
Please Notice:
If AccessManager is ON and the database file is under its control (tipically at P1), DBE uses it to check the file permissions.
Otherwise, if the database file is not under AccessManager control, and it's stored locally or on a distributed file system (like AFS), in that case file permissions are only checked at folder level, i.e. DBE only checks if the folder containing the file is writable by the user. 
According to the permissions and the file system the flag is set to:

   * if the DB file is at P1 and the AcessManager is ON: 
      * **"AM: user (RW)"**
      * **"AM: user (RO)"**

   * if the DB file is on other file systems or it's a local file:
      * **"RW"**
      * **"RO"**

..   * if the DB file is on AFS:
      * **"RW (AFS)"**
      * **"RO (AFS)"**



Please notice that, given the check at folder level, a file marked as "wr" (read-write) by its Unix flag but stored, e.g., in an AFS folder where the user has no AFS "write" permission, will be correctly flagged as "Read-Only" by the DBE. 
For the same reason a file which has a "r" (read-only) Unix flag is actually still writable by the user if stored in AFS where the user has AFS "write" permission; and it will be correctly displayed as a "Read-Write" file [#f1]_.

The *read-only* files (as well as the "schema" files) in the list are disabled (grayed-out) to prevent the user to edit them, but they are still searchable with the "Find File" command in the context-menu.

.. rubric:: Footnotes

.. [#f1] The decision of only checking write-permissions at folder level was made to ensure an enough fast operation of the DBE; otherwise a complete check of the fyle-system type should have been performed; which would have been resulted in a slow building of the file table. Unfortunately the cons of this decision is that files with a "r" Unix flag, stored in a local folder which is writable by the user, are wrongly shown as "RW", even if it's not true. Of course the user will get warned about that, but only after he/she tried to edit it. 


