CC=g++
OPENCVDIR = /usr/include/opencv2/
CCFITSDIR = /usr/local/include/CCfits/
CFLAGS=-c -Wall
AVT = -lPvAPI
PUREGEV_ROOT = /opt/pleora/ebus_sdk
IMPERX =-L$(PUREGEV_ROOT)/lib/		\
	-I$(PUREGEV_ROOT)/include/	\
	-lPvBase             		\
	-lPvDevice          		\
	-lPvBuffer          		\
	-lPvPersistence      		\
	-lPvGenICam          		\
	-lPvStreamRaw        		\
	-lPvStream 
OPENCV = -I$(OPENCVDIR) -lopencv_core -lopencv_highgui -lopencv_imgproc
CCFITS = -I$(CCFITSDIR) -lCCfits
THREAD = -lpthread -std=gnu++0x
OBJ = -I.

all: fullDemo

fullDemo: fullDemo.cpp processing.o utilities.o ImperxStream.o
	$(CC) fullDemo.cpp processing.o utilities.o ImperxStream.o -o fullDemo $(OBJ) $(OPENCV) $(THREAD) $(IMPERX) -pg

fidDemo: fidDemo.cpp processing.o compression.o
	$(CC) fidDemo.cpp processing.o compression.o -o fidDemo $(OBJ) $(OPENCV) $(CCFITS) -pg

threadDemo: threadDemo.cpp processing.o utilities.o
	$(CC) threadDemo.cpp processing.o utilities.o -o threadDemo $(OBJ) $(OPENCV) $(THREAD) -pg

packetDemo: packetDemo.cpp TelemetryStream.o ImperxStream.o utilities.o
	$(CC) packetDemo.cpp TelemetryStream.o lib_crc.o ImperxStream.o utilities.o -o packetDemo $(OBJ) $(OPENCV) $(IMPERX) $(THREAD)

commandingDemo: commandingDemo.cpp Commanding.o 
	$(CC) commandingDemo.cpp Commanding.o lib_crc.o -o commandingDemo

processing.o: processing.cpp
	$(CC) $(CFLAGS) processing.cpp -o processing.o $(OBJ) $(OPENCV)

camera.o: camera.cpp
	$(CC) $(CFLAGS) camera.cpp $(OBJ) $(AVT)

ImperxStream.o: ImperxStream.cpp utilities.o
	$(CC) $(CFLAGS) ImperxStream.cpp utilities.o -o ImperxStream.o $(OBJ) $(THREAD) $(IMPERX) $(OPENCV)

compression.o: compression.cpp
	$(CC) $(CFLAGS) compression.cpp -o compression.o $(OBJ) $(OPENCV) $(CCFITS)

TelemetryStream.o: TelemetryStream.cpp ./lib_crc/lib_crc.c
	$(CC) $(CFLAGS) TelemetryStream.cpp ./lib_crc/lib_crc.c -I./lib_crc/

Commanding.o: Commanding.cpp ./lib_crc/lib_crc.c
	$(CC) $(CFLAGS) Commanding.cpp ./lib_crc/lib_crc.c -I./lib_crc/

utilities.o: utilities.cpp
	$(CC) $(CFLAGS) utilities.cpp -o utilities.o $(OBJ) $(THREAD)

clean:
	rm -rf *.o fullDemo fidDemo threadDemo packetDemo commandingDemo *.out
