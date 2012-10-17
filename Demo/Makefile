CC=g++
OPENCVDIR = /usr/include/opencv2/
CFLAGS=-c -Wall
PVAPI = -lPvAPI
OPENCV = -I$(OPENCVDIR) -lopencv_core -lopencv_highgui -lopencv_imgproc
CHORD = -I.

all: demo

demo: main.cpp chord.o
	$(CC) main.cpp chord.o -o demo $(CHORD) $(PVAPI) $(OPENCV)
	
chord.o: chord.cpp
	$(CC) $(CFLAGS) chord.cpp $(CHORD)

clean:
	rm -rf *.o demo
