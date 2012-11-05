CC=g++
OPENCVDIR = /usr/local/include/opencv2/
CCFITSDIR = /usr/local/include/CCfits/
CFLAGS=-c -Wall
PVAPI = -lPvAPI
OPENCV = -I$(OPENCVDIR) -lopencv_core -lopencv_highgui -lopencv_imgproc
CCFITS = -I$(CCFITSDIR) -lCCfits
OBJ = -I.

all: fullDemo

fullDemo: fullDemo.cpp chord.o camera.o fiducials.o
	$(CC) fullDemo.cpp chord.o camera.o fiducials.o -o fullDemo $(OBJ) $(OPENCV) $(PVAPI) -pg

fidDemo: fidDemo.cpp chord.o fiducials.o compression.o
	$(CC) fidDemo.cpp chord.o fiducials.o compression.o -o fidDemo $(OBJ) $(OPENCV) $(CCFITS) -pg
	
chord.o: chord.cpp
	$(CC) $(CFLAGS) chord.cpp $(OBJ)
	
fiducials.o: chord.cpp
	$(CC) $(CFLAGS) fiducials.cpp $(OBJ) $(OPENCV)
	
camera.o: camera.cpp
	$(CC) $(CFLAGS) camera.cpp $(OBJ) $(PVAPI)

compression.o: compression.cpp
	$(CC) $(CFLAGS) compression.cpp $(OBJ) $(OPENCV) $(CCFITS)

clean:
	rm -rf *.o fullDemo fidDemo *.out
