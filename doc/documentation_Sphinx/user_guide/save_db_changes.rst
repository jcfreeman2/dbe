
Saving DB changes
=================

If the "dbe" editor is closed when there are changes which have not been committed (you can check this at any time by looking at the status of the list of the Undo/Redo operations in the "Info Tabs" section of the main window), a message pops up asking the user if the changes have to be saved to the DB or discarded.


To save the changes you made to the DB-file, at any time you can click on the save-icon in the tool-box, or on the ``Edit-->Commit`` action in the ``Edit`` menu.

Before actually saving the changes you will be asked to enter a *commit message*, to leave an explanation of what you changed and why.


Accordingly to the nature of the file you are editing, the commit message will be saved in a different way.

If you are editing a **local file**, or a file in an **AFS** folder, your commit message will be stored inside the ``.xml`` DB-file, inside the ``<comments>`` tag.

If you are editing a file managed by the **OKS-Server** (like those under ``/atlas/oks/tdaq-*`` folder **in P1**), the commit message will be not saved inside the file, but it will be used as CVS commit message and you will find it in the CVS history. `You can see the history of all changes here <https://atlasop.cern.ch/cvs/viewvc.cgi/>`_.
