# Pattern matching in make files
#   $^ matches all the input dependencies
#   $< matches the first input dependency
#   $@ matches the output

CC=g++
OPENCVDIR = /usr/include/opencv2/
CCFITSDIR = /usr/local/include/CCfits/
CFLAGS=-Wall
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
	$(CC) $(CFLAGS) $^ -o $@ $(OBJ) $(OPENCV) $(THREAD) $(IMPERX) -pg

fidDemo: fidDemo.cpp processing.o compression.o
	$(CC) $(CFLAGS) $^ -o $@ $(OBJ) $(OPENCV) $(CCFITS) -pg

threadDemo: threadDemo.cpp processing.o utilities.o
	$(CC) $(CFLAGS) $^ -o $@ $(OBJ) $(OPENCV) $(THREAD) -pg

packetDemo: packetDemo.cpp TelemetrySender.o ImperxStream.o utilities.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@ $(OBJ) $(OPENCV) $(IMPERX) $(THREAD)

commandingDemo: commandingDemo.cpp Commanding.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@

networkDemo: networkDemo.cpp Packet.o UDPSender.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@

test: test.cpp Packet.o UDPSender.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@

#This pattern matching will catch all "simple" object dependencies
%.o: %.cpp %.hpp
	$(CC) -c $(CFLAGS) $< -o $@

processing.o: processing.cpp
	$(CC) -c $(CFLAGS) $< -o $@ $(OBJ) $(OPENCV)

camera.o: camera.cpp
	$(CC) -c $(CFLAGS) $< -o $@ $(OBJ) $(AVT)

ImperxStream.o: ImperxStream.cpp utilities.o
	$(CC) -c $(CFLAGS) $< -o $@ $(OBJ) $(THREAD) $(IMPERX) $(OPENCV)

compression.o: compression.cpp
	$(CC) -c $(CFLAGS) $< -o $@ $(OBJ) $(OPENCV) $(CCFITS)

utilities.o: utilities.cpp
	$(CC) -c $(CFLAGS) $< -o $@ $(OBJ) $(THREAD)

lib_crc.o: lib_crc/lib_crc.c lib_crc/lib_crc.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o fullDemo fidDemo threadDemo packetDemo commandingDemo networkDemo *.out
