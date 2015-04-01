#GIF++ DAQ
####Author: Alexis Fagot
####email : alexis.fagot@ugent.be
####Tel.: +32 9 264 65 69
####Mobile: +32 4 77 91 61 31
####Mobile: +33 6 66 89 02 90

##Compilation

This application requires two non-optional dependencies:

* The **[CAEN USB Driver](http://www.caen.it/csite/CaenProd.jsp?idmod=417&parent=11)** to mount the VME hardware
* **[CAEN VME Library](http://www.caen.it/csite/CaenProd.jsp?idmod=689&parent=38)** to communicate with the VME hardware

The CAEN VME library will not be packaged by distributions and will need to be installed manually.

To compile the gif daq project, first, open the makefile with a text editor, for example gedit, and modify the path to DAQ_HOME_DIR :

    DAQ_HOME_DIR = /path/to/your/working/directory

In the case of the GIF ++ computer, we have :

    DAQ_HOME_DIR = /home/gifdaq/Desktop/GIF_DAQ

Then, simply do :

    make

## Usage

To use the program, the user can modify the parameters of the TDC inside the file `daqgifpp.ini` using the **[USER'S MANUAL](http://www.caen.it/servlet/checkCaenManualFile?Id=8657)**. Then to run it, simply type :

    bin/daq

The output datafiles are saved into the directory `datarun`.
