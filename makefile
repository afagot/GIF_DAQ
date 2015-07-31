#file: Makefile
#
# test program for CMS RPC
#
# 14/01/14  Created by Y.Benhammou


DAQ_BIN_DIR  := bin
DAQ_INC_DIR  := include
DAQ_SRC_DIR  := src
DAQ_OBJ_DIR  := obj

ROOT_INC     := $(ROOTSYS)/include
ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)

LFLAGS       := -Llib -L/usr/lib \
                $(ROOTLIBS) 

CFLAGS       := -ggdb -fPIC -DLINUX -Wall -funsigned-char \
                -I$(DAQ_INC_DIR) -I$(ROOT_INC) -I$(ROOTCFLAGS)

all: daq

daq: 	daq.o v1718.o v1190a.o DataReader.o IniFile.o
	g++ $(CFLAGS) $(DAQ_OBJ_DIR)/daq.o \
	$(DAQ_OBJ_DIR)/v1718.o \
	$(DAQ_OBJ_DIR)/v1190a.o \
	$(DAQ_OBJ_DIR)/DataReader.o \
	$(DAQ_OBJ_DIR)/IniFile.o \
        -o $(DAQ_BIN_DIR)/daq \
        $(LFLAGS)  \
        -l CAENVME -l curses

daq.o:
	g++ -std=c++11 -c $(CFLAGS) $(DAQ_SRC_DIR)/daq.cxx -o $(DAQ_OBJ_DIR)/daq.o
v1718.o:
	g++ -std=c++11 -c $(CFLAGS) $(DAQ_SRC_DIR)/v1718.cxx -o $(DAQ_OBJ_DIR)/v1718.o
v1190a.o:
	g++ -std=c++11 -c $(CFLAGS) $(DAQ_SRC_DIR)/v1190a.cxx -o $(DAQ_OBJ_DIR)/v1190a.o
DataReader.o:
	g++ -std=c++11 -c $(CFLAGS) $(DAQ_SRC_DIR)/DataReader.cxx -o $(DAQ_OBJ_DIR)/DataReader.o
IniFile.o:
	g++ -std=c++11 -c $(CFLAGS) $(DAQ_SRC_DIR)/IniFile.cxx -o $(DAQ_OBJ_DIR)/IniFile.o

$(DAQ_BIN_DIR):
	mkdir -p $(DAQ_BIN_DIR)

$(DAQ_OBJ_DIR):
	mkdir -p $(DAQ_OBJ_DIR)

clean:
	rm -rf $(DAQ_BIN_DIR)/
	rm -rf $(DAQ_OBJ_DIR)/

remove:
	rm -rf $(DAQ_BIN_DIR)/
	rm -rf $(DAQ_OBJ_DIR)/

