# Pattern matching in make files
#   $^ matches all the input dependencies
#   $< matches the first input dependency
#   $@ matches the output

CC=g++
GCC_VERSION_GE_43 := $(shell expr `g++ -dumpversion | cut -f2 -d.` \>= 3)

PUREGEV_ROOT = /opt/pleora/ebus_sdk
OPENCVDIR = /usr/include/opencv2/
CCFITSDIR = /usr/include/CCfits/
DSCUD = /usr/local/dscud-6.02
INCLUDE = -I$(OPENCVDIR) -I$(PUREGEV_ROOT)/include/ -I$(CCFITSDIR) -I$(DSCUD)

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
DSCUD_LIBS = -L$(DSCUD) -ldscud-6.02
ifeq "$(GCC_VERSION_GE_43)" "1"
    CCFITS += -lrt
endif

TESTS = AspectTest MeasureScreen BlackFrames ClockReader
EXEC_CORE = sunDemo sbc_info sbc_shutdown relay_control
EXEC_ALL = $(EXEC_CORE) sbc_info_reader sbc_shutdown_sender relay_control_sender CTLCommandSimulator CTLSimulator SRVSimulator
RELAYS = pmm/DiamondPMM.o pmm/DiamondBoard.o pmm/StateRelay.o
PACKET = Packet.o lib_crc.o
ASPECT = processing.o AspectError.o AspectParameter.o

default: $(EXEC_CORE)

all: $(EXEC_ALL)

fullDemo: fullDemo.cpp $(ASPECT) utilities.o ImperxStream.o compression.o draw.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(THREAD) $(IMPERX) $(CCFITS)

CTLCommandSimulator: CTLCommandSimulator.cpp UDPSender.o Command.o $(PACKET)
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

CTLSimulator: CTLSimulator.cpp UDPReceiver.o Command.o $(PACKET)
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

SRVSimulator: SRVSimulator.cpp UDPReceiver.o Telemetry.o $(PACKET)
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

sunDemo: sunDemo.cpp $(PACKET) Command.o Telemetry.o UDPSender.o UDPReceiver.o utilities.o ImperxStream.o compression.o types.o Transform.o TCPSender.o Image.o $(ASPECT)
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD) $(OPENCV) $(IMPERX) $(CCFITS) -pg

test_telemetry: test_telemetry.cpp Telemetry.o $(PACKET) UDPSender.o types.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

evaluate: evaluate.cpp compression.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(CCFITS)

snap: snap.cpp ImperxStream.o compression.o processing.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(CCFITS) $(IMPERX)

playback: playback.cpp Telemetry.o $(PACKET) UDPSender.o utilities.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

#This executable need to be copied to /usr/local/bin/ after it is built
sbc_info: sbc_info.cpp $(PACKET) UDPSender.o smbus.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

#This executable need to be copied to /usr/local/bin/ after it is built
sbc_shutdown: sbc_shutdown.cpp UDPReceiver.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

#This executable need to be copied to /usr/local/bin/ after it is built
relay_control: relay_control.cpp UDPReceiver.o $(RELAYS)
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD) $(DSCUD_LIBS)

sbc_info_reader: sbc_info_reader.cpp UDPReceiver.o $(PACKET)
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

sbc_shutdown_sender: sbc_shutdown_sender.cpp $(PACKET) UDPSender.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

relay_control_sender: relay_control_sender.cpp $(PACKET) UDPSender.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

tmbin_reader: tmbin_reader.cpp $(PACKET) Telemetry.o types.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

#This pattern matching will catch all "simple" object dependencies
%.o: %.cpp %.hpp
	$(CC) -c $(CFLAGS) $< -o $@

#Relay code
relays: relays.cpp $(RELAYS)
	$(CC) $(CFLAGS) $^ -o $@ $(DSCUD_LIBS) $(THREAD)

test_relays: test_relays.cpp $(RELAYS)
	$(CC) $(CFLAGS) $^ -o $@ $(DSCUD_LIBS) $(THREAD)

pmm/%.o: pmm/%.cpp pmm/%.h
	$(CC) -c $(CFLAGS) $< -o $@ $(DSCUD_LIBS) $(THREAD)

PointingTest: PointingTest.cpp $(ASPECT) utilities.o compression.o Transform.o types.o $(PACKET) draw.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(CCFITS)

#This pattern is for any of Alex's weird test programs
$(TESTS): % : %.cpp utilities.o $(ASPECT) compression.o draw.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(CCFITS)

smbus.o: smbus/smbus.c smbus/smbus.h
	$(CC) -c $(CFLAGS) $< -o $@

lib_crc.o: lib_crc/lib_crc.c lib_crc/lib_crc.h
	$(CC) -c $(CFLAGS) $< -o $@

Transform.o: Transform.cpp Transform.hpp spa/spa.c spa/spa.h
	$(CC) -c $(CFLAGS) $< -o $@

install: $(EXEC_CORE)
	sudo systemctl stop sas
	sudo systemctl stop sbc_info
	sudo systemctl stop sbc_shutdown
	sudo systemctl stop relay_control
	sudo cp sunDemo /usr/local/bin/sas.runtime
	sudo cp sbc_info /usr/local/bin/sbc_info.runtime
	sudo cp sbc_shutdown /usr/local/bin/sbc_shutdown.runtime
	sudo cp relay_control /usr/local/bin/relay_control.runtime
	sudo systemctl start relay_control
	sudo systemctl start sbc_shutdown
	sudo systemctl start sbc_info
	sudo systemctl start sas

clean:
	rm -rf *.o *.out $(EXEC_ALL) $(TESTS) pmm/*.o
