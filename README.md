#GIF++ DAQ v3.0
####Author: Alexis Fagot
####email : alexis.fagot@ugent.be
####Tel.: +32 9 264 65 69
####Mobile: +32 4 77 91 61 31
####Mobile: +33 6 66 89 02 90

##Compilation

This application requires 3 non-optional dependencies:

* The **[CAEN USB Driver](http://www.caen.it/csite/CaenProd.jsp?idmod=417&parent=11)** to mount the VME hardware
* **[CAEN VME Library](http://www.caen.it/csite/CaenProd.jsp?idmod=689&parent=38)** to communicate with the VME hardware
* **[ROOT](https://root.cern.ch/downloading-root) to organize the collected data into a TTree**

The CAEN VME library will not be packaged by distributions and will need to be installed manually.

To compile the gif daq project, simply do :

    make

## Usage

Note that this new version of the DAQ is not meant to be used as a standalone program anymore.
A web interface has been designed to automate the data taking of an entire scan. This web interface launches the DAQ and sends START signals at the beginning of every run and shut downs the DAQ via a STOP signal when the scan is completed.

