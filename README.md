* DBE (DataBase Editor)

** Intro

The DBE package provides a GUI interface to the OKS suite, allowing you edit both schema files and data files. Note that these instructions will assume you have a basic familiarity with OKS as [described here](https://github.com/DUNE-DAQ/dal#readme). They also assume that you [know how to download and build repositories](https://dune-daq-sw.readthedocs.io/en/latest/packages/daq-buildtools/) and are able to display windows from your command line. 

While DBE was [originally written as part of the ATLAS TDAQ effort](https://gitlab.cern.ch/atlas-tdaq-software/dbe.git), it has been modified to build within the DUNE DAQ framework. To get started working with DBE, you'll want to set up a work area based on release NT23-01-30, which among other things provides the core of the OKS suite and the Qt GUI package DBE depends on. However, you'll also need to download and build the `dbe` repository as well as the `qtutils` repository which it depends on. It is recommended that you build with the `--unittest` option and check that the `dbe` unit tests are working. 

** Walkthrough

Now that you've built these two repos, let's get started seeing what functionality it provides. Assuming that you've run `source env.sh` since you downloaded the repos, you can do the following:
```
tutorial.py   # Generates a data file which we can use for the purposes of a DBE tutorial
dbe_main -f tutorial.data.xml
```
`tutorial.data.xml` describes a very simple DAQ system which consists of one run control, one TPC readout application and one photon readout application; it's covered more thoroughly [here](https://github.com/DUNE-DAQ/dal#readme). When DBE launches, you'll see a subwindow on the left side of the DBE window titled "Class View". As you can see, this lists the classes available to the data file you provided DBE, as well as the number of objects of each class type. Click on `ReadoutApplication`, and you'll see the following:
```
Main window here
```
...i.e., we have a toy readout application for photon detection and a toy TPC readout application. 

Let's open a second terminal and edit the `tutorial.data.xml` file. Take the `TPCReadout` object at the bottom of the file, i.e.
```
<obj class="ReadoutApplication" id="TPCReadout">
 <attr name="Name" type="string" val="/full/pathname/of/readout/executable"/>
 <attr name="SubDetector" type="enum" val="WireChamber"/>
</obj>
```
and cut-and-paste a copy of it. Then for the second copy, make sure to give it a unique ID, e.g. changing `TPCReadout` into `TPCReadout2`; OKS requires that each object has a unique ID. Save the file. You should see two things have changed in the GUI: (1) a message saying `Database reloaded due external changes` and (2) the addition of the second TPC readout application which you created. DBE keeps track of edits to the XML database file, and notices when it's been changed. Hit `OK` in the message to close it. 

Now let's perform some edits within the GUI. First, as an example of how you can change a variable across multiple objects, do the following: click on the text box titled `Table Filter` below the list of readout object. Our three objects have the IDs `PhotonReadout`, `TPCReadout` and `TPCReadout2`. Let's pretend we want to change the name of the TPC readout executable. Type `TPC` and you'll see that DBE is now displaying only the two objects whose names begin with those three letters. Click on the `Edit` option in the upper left of the window and select `Batch Change Table`. You'll see a small window like the following:
```
Batch Change Table window here
```
While there's a dropdown menu for `New Attribute Value`, lets stay with the `Name` attribute value for editing purposes. In the text box directly below the dropdown menu, you can enter any string, e.g. `/new/name/of/tpc/readout/executable`. Then (not sure why ATLAS put this in), check the checkbox `Check if you want to change attribute` and click the button `Make change` at the bottom of the window. You'll see the value update in the main DBE window. If you delete the `TPC` from the `Table Filter` text box you can return to the full listing of readout objects. 

Let's now make changes to the run control application. 
