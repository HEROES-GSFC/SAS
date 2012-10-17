CC=g++
OPENCVDIR = /usr/include/opencv2/
CFLAGS=-c -Wall
PVAPI = -lPvAPI
OPENCV = -I$(OPENCVDIR) -lopencv_core -lopencv_highgui -lopencv_imgproc
OBJ = -I.

all: fullDemo

fullDemo: fullDemo.cpp chord.o camera.o fiducials.o
	$(CC) fullDemo.cpp chord.o camera.o fiducials.o -o fullDemo $(OBJ) $(OPENCV) $(PVAPI)

fidDemo: fidDemo.cpp chord.o fiducials.o
	$(CC) fidDemo.cpp chord.o fiducials.o -o fidDemo $(OBJ) $(OPENCV) -pg
	
chord.o: chord.cpp
	$(CC) $(CFLAGS) chord.cpp $(OBJ)
	
fiducials.o: chord.cpp
	$(CC) $(CFLAGS) fiducials.cpp $(OBJ) $(OPENCV)
	
camera.o: camera.cpp
	$(CC) $(CFLAGS) camera.cpp $(OBJ) $(PVAPI)

clean:
	rm -rf *.o fullDemo fidDemo *.out
