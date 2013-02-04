# Pattern matching in make files
#   $^ matches all the input dependencies
#   $< matches the first input dependency
#   $@ matches the output

CC=g++
GCC_VERSION_GE_43 := $(shell expr `g++ -dumpversion | cut -f2 -d.` \>= 3)

PUREGEV_ROOT = /opt/pleora/ebus_sdk
OPENCVDIR = /usr/include/opencv2/
CCFITSDIR = /usr/local/include/CCfits/
INCLUDE = -I$(OPENCVDIR) -I$(CCFITSDIR) -I$(PUREGEV_ROOT)/include/

CFLAGS = -Wall $(INCLUDE)
ifeq "$(GCC_VERSION_GE_43)" "1"
    CFLAGS += -std=gnu++0x
endif

#AVT = -lPvAPI
IMPERX =-L$(PUREGEV_ROOT)/lib/		\
	-lPvBase             		\
	-lPvDevice          		\
	-lPvBuffer          		\
	-lPvPersistence      		\
	-lPvGenICam          		\
	-lPvStreamRaw        		\
	-lPvStream 
OPENCV = -lopencv_core -lopencv_highgui -lopencv_imgproc
CCFITS = -lCCfits
THREAD = -lpthread

EXEC = fullDemo fidDemo threadDemo packetDemo commandingDemo networkDemo test

all: $(EXEC)

fullDemo: fullDemo.cpp processing.o utilities.o ImperxStream.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(THREAD) $(IMPERX) -pg

fidDemo: fidDemo.cpp processing.o compression.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(CCFITS) -pg

threadDemo: threadDemo.cpp processing.o utilities.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(THREAD) -pg

packetDemo: packetDemo.cpp TelemetrySender.o ImperxStream.o utilities.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(IMPERX) $(THREAD)

commandingDemo: commandingDemo.cpp Commanding.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@

networkDemo: networkDemo.cpp Packet.o UDPSender.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

test: test.cpp Packet.o UDPSender.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@

#This pattern matching will catch all "simple" object dependencies
%.o: %.cpp %.hpp
	$(CC) -c $(CFLAGS) $< -o $@

#No longer relevant?
#camera.o: camera.cpp
#	$(CC) -c $(CFLAGS) $< -o $@ $(OBJ) $(AVT)

lib_crc.o: lib_crc/lib_crc.c lib_crc/lib_crc.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o *.out $(EXEC)
