# Parameters
# SRC_CS: The source C files to compie
# SRC_CPPS: The source CPP files to compile
# EXEC: The executable name

ifeq ($(SRC_CS) $(SRC_CPPS),)
  $(error No source files specified)
endif

ifeq ($(EXEC),)
  $(error No executable file specified)
endif

CC                  = gcc
CPP                 = g++
LD                  = $(CPP)
SRC_MOC             =
MOC			        =
RCC					=

PUREGEV_ROOT        ?= /opt/pleora/ebus_sdk
CFLAGS              += -I$(PUREGEV_ROOT)/include
CPPFLAGS            += -I$(PUREGEV_ROOT)/include
LDFLAGS             += -L$(PUREGEV_ROOT)/lib	\
						-lPvBase             	\
             			-lPvDevice          	\
             			-lPvBuffer          	\
             			-lPvGUIUtils         	\
             			-lPvGUI              	\
             			-lPvPersistence      	\
             			-lPvGenICam          	\
             			-lPvStreamRaw        	\
             			-lPvStream           	\
             			-lPvTransmitterRaw   	\
             			-lPvVirtualDevice
PV_LIBRARY_PATH      = $(PUREGEV_ROOT)/lib

FILES_MOC            = $(shell grep -l Q_OBJECT *)
ifeq ($(FILES_MOC),)
else
	# This is a sample compiling Qt code
        QT_IS_PRESENT = $( shell which qmake-qt4 &> /dev/null ; echo $?)
	ifeq ($(QT_IS_PRESENT), "1")
		# We cannot compile the sample without the Qt SDK!
 		$(error The sample $(EXEC) requires the Qt SDK to be compiled. See share/samples/Readme.txt for more details)
	else
        	# Query qmake to find out the folder required to compile
        	QT_SDK_BIN        = $(shell qmake-qt4 -query QT_INSTALL_BINS)
        	QT_SDK_LIB        = $(shell qmake-qt4 -query QT_INSTALL_LIBS)
        	QT_SDK_INC        = $(shell qmake-qt4 -query QT_INSTALL_HEADERS)
		
    		# We have a full Qt SDK installed, so we can compile the sample
		CFLAGS 	         += -I$(QT_SDK_INC)
		CPPFLAGS         += -I$(QT_SDK_INC)
		LDFLAGS          += -L$(QT_SDK_LIB)
        	QT_LIBRARY_PATH   = $(QT_SDK_LIB)

		SRC_MOC           = $(FILES_MOC:%h=moc_%cxx)
		FILES_QRC         = $(shell ls *.qrc)
   		SRC_QRC           = $(FILES_QRC:%qrc=qrc_%cxx)

		OBJS             += $(SRC_MOC:%.cxx=%.o)
		OBJS		 += $(SRC_QRC:%.cxx=%.o)

		MOC               = $(QT_SDK_BIN)/moc
		RCC     	  = $(QT_SDK_BIN)/rcc
	endif
endif

FILES_OPENCV              = $(shell grep -l opencv2 *)
ifneq ($(FILES_OPENCV),)
	# OpenCV code
	ifeq ($(wildcard /usr/local/include/opencv2),)
		# We cannot compile the sample without the OpenCV SDK!
 		$(error The sample $(EXEC) requires the OpenCV SDK to be compiled. See share/samples/Readme.txt for more details)
	else
		CFLAGS 	           += -I/usr/local/include/opencv2
		CPPFLAGS           += -I/usr/local/include/opencv2

		LDFLAGS            += -L/usr/local/lib			\
								-lopencv_core			\
								-lopencv_legacy
		OPENCV_LIBRARY_PATH = /usr/local/lib
	endif
endif

ifneq ($(shell uname -m),x86_64)
	LDFLAGS            += -L$(PUREGEV_ROOT)/lib/genicam/bin/Linux32_i86
	GEN_LIB_PATH        =  $(PUREGEV_ROOT)/lib/genicam/bin/Linux32_i86
else
	LDFLAGS            += -L$(PUREGEV_ROOT)/lib/genicam/bin/Linux64_x64
	GEN_LIB_PATH        =  $(PUREGEV_ROOT)/lib/genicam/bin/Linux64_x64
endif

LD_LIBRARY_PATH        = $(GEN_LIB_PATH):$(OPENCV_LIBRARY_PATH):$(QT_LIBRARY_PATH):$(PV_LIBRARY_PATH)
export LD_LIBRARY_PATH

ifdef _DEBUG
    CFLAGS    += -g -D_DEBUG
    CPPFLAGS  += -g -D_DEBUG
else
    CFLAGS    += -O3
    CPPFLAGS  += -O3
endif

CFLAGS    += -D_UNIX_ -D_LINUX_
CPPFLAGS  += -D_UNIX_ -D_LINUX_

OBJS      += $(SRC_CPPS:%.cpp=%.o)
OBJS      += $(SRC_CS:%.c=%.o)

all: $(EXEC)

clean:
	rm -rf $(OBJS) $(EXEC) $(SRC_MOC) $(SRC_QRC)

moc_%.cxx: %.h
	$(MOC) $< -o $@ 

qrc_%.cxx: %.qrc
	$(RCC) $< -o $@

%.o: %.cxx
	$(CPP) -c $(CPPFLAGS) -o $@ $<

%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) -o $@ $<

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(EXEC): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LDFLAGS)

.PHONY: all clean
