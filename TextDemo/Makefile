CC=g++
OPENCVDIR = /usr/include/opencv2/
CFLAGS=-c -Wall
PVAPI = -lPvAPI
OPENCV = -I$(OPENCVDIR) -lopencv_core -lopencv_highgui
CHORD = -I.

all: chordDemo

chordDemo: chord.o
	$(CC) main.cpp chord.o -o demo $(CHORD) $(PVAPI)
	
chord.o: chord.cpp
	$(CC) $(CFLAGS) chord.cpp $(CHORD)

clean:
	rm -rf *o demo
