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

fullDemo: fullDemo.cpp processing.o camera.o 
	$(CC) fullDemo.cpp camera.o processing.o -o fullDemo $(OBJ) $(OPENCV) $(AVT) -pg

fidDemo: fidDemo.cpp processing.o compression.o
	$(CC) fidDemo.cpp processing.o compression.o -o fidDemo $(OBJ) $(OPENCV) $(CCFITS) -pg

threadDemo: threadDemo.cpp processing.o semaphore.o
	$(CC) threadDemo.cpp processing.o semaphore.o -o threadDemo $(OBJ) $(OPENCV) $(THREAD)

ImperxStream: ImperxStream.o utilities.o
	$(CC) ImperxStream.o utilities.o $(OBJ) $(THREAD) $(IMPERX)
processing.o: processing.cpp
	$(CC) $(CFLAGS) processing.cpp -o processing.o $(OBJ) $(OPENCV)

camera.o: camera.cpp
	$(CC) $(CFLAGS) camera.cpp $(OBJ) $(AVT)

ImperxStream.o: ImperxStream.cpp utilities.o
	$(CC) $(CFLAGS) ImperxStream.cpp utilities.o -o ImperxStream.o $(OBJ) $(THREAD) $(IMPERX)

compression.o: compression.cpp
	$(CC) $(CFLAGS) compression.cpp -o compression.o $(OBJ) $(OPENCV) $(CCFITS)

utilities.o: utilities.cpp
	$(CC) $(CFLAGS) utilities.cpp -o utilities.o $(OBJ) $(THREAD)

clean:
	rm -rf *.o fullDemo fidDemo threadDemo *.out
