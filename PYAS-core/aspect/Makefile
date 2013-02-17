# Pattern matching in make files
#   $^ matches all the input dependencies
#   $< matches the first input dependency
#   $@ matches the output

CC=g++
GCC_VERSION_GE_43 := $(shell expr `g++ -dumpversion | cut -f2 -d.` \>= 3)

PUREGEV_ROOT = /opt/pleora/ebus_sdk
OPENCVDIR = /usr/include/opencv2/
INCLUDE = -I$(OPENCVDIR) -I$(PUREGEV_ROOT)/include/

CFLAGS = -Wall $(INCLUDE)
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

EXEC = fullDemo fidDemo threadDemo packetDemo commandingDemo networkDemo test_command test_sender

all: $(EXEC)

fullDemo: fullDemo.cpp processing.o utilities.o ImperxStream.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(THREAD) $(IMPERX) -pg

fidDemo: fidDemo.cpp processing.o compression.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) -pg

threadDemo: threadDemo.cpp processing.o utilities.o ImperxStream.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(THREAD) $(IMPERX) -pg

packetDemo: packetDemo.cpp TelemetrySender.o ImperxStream.o utilities.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@ $(OPENCV) $(IMPERX) $(THREAD)

commandingDemo: commandingDemo.cpp Commanding.o lib_crc.o
	$(CC) $(CFLAGS) $^ -o $@

networkDemo: networkDemo.cpp Packet.o Command.o Telemetry.o UDPSender.o lib_crc.o UDPReceiver.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD)

cdrDemo: cdrDemo.cpp Packet.o Command.o Telemetry.o UDPSender.o lib_crc.o UDPReceiver.o processing.o utilities.o ImperxStream.o
	$(CC) $(CFLAGS) $^ -o $@ $(THREAD) $(OPENCV) $(IMPERX) -pg

test_command: test_command.cpp Packet.o Command.o lib_crc.o UDPSender.o
	$(CC) $(CFLAGS) $^ -o $@

test_sender: test_sender.cpp UDPSender.o Packet.o lib_crc.o Telemetry.o
	$(CC) $(CFLAGS) $^ -o $@

#This pattern matching will catch all "simple" object dependencies
%.o: %.cpp %.hpp
	$(CC) -c $(CFLAGS) $< -o $@

lib_crc.o: lib_crc/lib_crc.c lib_crc/lib_crc.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o *.out $(EXEC)
