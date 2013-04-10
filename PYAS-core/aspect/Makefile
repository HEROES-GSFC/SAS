# Pattern matching in make files
#   $^ matches all the input dependencies
#   $< matches the first input dependency
#   $@ matches the output

CC=g++
GCC_VERSION_GE_43 := $(shell expr `g++ -dumpversion | cut -f2 -d.` \>= 3)

PUREGEV_ROOT = /opt/pleora/ebus_sdk
OPENCVDIR = /usr/include/opencv2/
CCFITSDIR = /usr/include/CCfits/
INCLUDE = -I$(OPENCVDIR) -I$(PUREGEV_ROOT)/include/ -I$(CCFITSDIR)

#removed to compile on Mac -schriste
#CFLAGS = -Wall $(INCLUDE) -Wno-unknown-pragmas -lrt
CFLAGS = -Wall $(INCLUDE) -Wno-unknown-pragmas
ifeq "$(GCC_VERSION_GE_43)" "1"
    CFLAGS += -std=gnu++0x
endif

IMPERX =-L$(PUREGEV_ROOT)/lib/		\
	-lPvBase             		\
	-lPvDevice          		\
	-lPvBuffer          		\
	-lPvPersistence      		\
	-lPvGenICam          		\
	-lPvStreamRaw        		\
	-lPvStream 
OPENCV = -lopencv_core -lopencv_highgui -lopencv_imgproc
THREAD = -lpthread
CCFITS = -lCCfits

EXEC = sunDemo fullDemo packetDemo commandingDemo networkDemo test_command test_sender AspectTest sbc_info

default: sunDemo sbc_info

all: $(EXEC)

fullDemo: fullDemo.cpp processing.o utilities.o ImperxStream.o compression.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(THREAD) $(IMPERX) $(CCFITS) -pg

packetDemo: packetDemo.cpp ImperxStream.o utilities.o
	$(CC) $(CFLAGS) $^ -o $@ $(IMPERX)

CTLCommandSimulator: CTLCommandSimulator.cpp UDPSender.o Command.o Packet.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@
	
CTLSimulator: CTLSimulator.cpp UDPReceiver.o Command.o Packet.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@

commandingDemo: commandingDemo.cpp Commanding.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@

networkDemo: networkDemo.cpp Packet.o Command.o Telemetry.o UDPSender.o lib_crc.o UDPReceiver.o TCPSender.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD) -pg

sunDemo: sunDemo.cpp Packet.o Command.o Telemetry.o UDPSender.o lib_crc.o UDPReceiver.o processing.o utilities.o ImperxStream.o compression.o types.o Transform.o TCPSender.o Image.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD) $(OPENCV) $(IMPERX) $(CCFITS) -pg

tcpDemo: tcpDemo.cpp TCPReceiver.o Packet.o lib_crc.o TCPSender.o
	$(CC) $(CFLAGS) $^ -o $@

tcpSend: tcpSend.cpp Telemetry.o Packet.o lib_crc.o TCPSender.o
	$(CC) $(CFLAGS) $^ -o $@ -pg

test_command: test_command.cpp Packet.o Command.o lib_crc.o UDPSender.o
	$(CC) $(CFLAGS) $^ -o $@

test_sender: test_sender.cpp UDPSender.o Packet.o lib_crc.o Telemetry.o
	$(CC) $(CFLAGS) $^ -o $@

AspectTest: AspectTest.cpp processing.o utilities.o compression.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(CCFITS)

AspectVideo: AspectVideo.cpp processing.o utilities.o compression.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(CCFITS)

#This executable need to be copied to /usr/local/bin/ after it is built
sbc_info: sbc_info.cpp Packet.o lib_crc.o UDPSender.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

#This pattern matching will catch all "simple" object dependencies
%.o: %.cpp %.hpp
	$(CC) -c $(CFLAGS) $< -o $@

lib_crc.o: lib_crc/lib_crc.c lib_crc/lib_crc.h
	$(CC) -c $(CFLAGS) $< -o $@

Transform.o: Transform.cpp Transform.hpp spa/spa.c spa/spa.h
	$(CC) -c $(CFLAGS) $< -o $@

install: sbc_info
	sudo systemctl stop sbc_info
	sudo cp $^ /usr/local/bin
	sudo systemctl start sbc_info

clean:
	rm -rf *.o *.out $(EXEC)
