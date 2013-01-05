CC=g++
OPENCVDIR = /usr/include/opencv2/
CCFITSDIR = /usr/local/include/CCfits/
CFLAGS=-c -Wall
PVAPI = -lPvAPI
OPENCV = -I$(OPENCVDIR) -lopencv_core -lopencv_highgui -lopencv_imgproc
CCFITS = -I$(CCFITSDIR) -lCCfits
THREAD = -lpthread -std=gnu++0x
OBJ = -I.

all: fullDemo

fullDemo: fullDemo.cpp processing.o camera.o 
	$(CC) fullDemo.cpp camera.o processing.o -o fullDemo $(OBJ) $(OPENCV) $(PVAPI) -pg

fidDemo: fidDemo.cpp processing.o compression.o
	$(CC) fidDemo.cpp processing.o compression.o -o fidDemo $(OBJ) $(OPENCV) $(CCFITS) -pg

threadDemo: threadDemo.cpp processing.o semaphore.o
	$(CC) threadDemo.cpp processing.o semaphore.o -o threadDemo $(OBJ) $(OPENCV) $(THREAD)

processing.o: processing.cpp
	$(CC) $(CFLAGS) processing.cpp $(OBJ) $(OPENCV)

camera.o: camera.cpp
	$(CC) $(CFLAGS) camera.cpp $(OBJ) $(PVAPI)

compression.o: compression.cpp
	$(CC) $(CFLAGS) compression.cpp $(OBJ) $(OPENCV) $(CCFITS)
semaphore.o: semaphore.cpp
	$(CC) $(CFLAGS) semaphore.cpp $(OBJ) $(THREAD)

clean:
	rm -rf *.o fullDemo fidDemo threadDemo *.out
