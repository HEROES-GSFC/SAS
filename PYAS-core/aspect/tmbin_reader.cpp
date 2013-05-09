#include "Telemetry.hpp"
#import "types.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define SAS_TARGET_ID 0x30
#define SAS_TM_TYPE 0x70
#define TM_PACKET_LENGTH 122

FILE *file;
uint8_t *packet;
time_t curSec;
struct tm *curDate;
char buf[255];

int main(int argc, char *argv[])
{
    file = fopen(argv[1],"rb");
    if (!file)
    {
        printf("Unable to open %s file!", argv[1]);
        return 1;
    }
    TelemetryPacketQueue tm_packet_queue = TelemetryPacketQueue();
    tm_packet_queue.add_file(argv[1]);
    tm_packet_queue.filterTypeID(SAS_TM_TYPE);
    tm_packet_queue.filterSourceID(SAS_TARGET_ID);
    while( !tm_packet_queue.empty() ){
	    TelemetryPacket tm_packet = TelemetryPacket(NULL);
    	tm_packet_queue >> tm_packet;
		uint32_t frame_number;
		tm_packet.getSAS();
		tm_packet >> frame_number;
		//uint16_t command_count;
		uint32_t seconds;
		seconds = tm_packet.getSeconds();
		curSec = seconds;
		curDate = localtime(&curSec);
		strftime(buf, sizeof(buf), "%Y-%b-%d %H:%M:%S", curDate);
		
		uint16_t command_count;
		tm_packet >> command_count;
        uint16_t command_key;
        tm_packet >> command_key;                    
        uint16_t housekeeping1, housekeeping2;
        tm_packet >> housekeeping1 >> housekeeping2;
        std::cout << buf << ", " << frame_number << ", " << Float2B(housekeeping1).value() << ", " << (int16_t)housekeeping2 << ", ";

		for( int i = 0; i < 8; i++){
			tm_packet.setReadIndex(tm_packet.getLength() + i - 8);
			int8_t i2temp;
			tm_packet >> i2temp;
			std::cout << static_cast<int16_t>(i2temp);
			if( i != 7 ) std::cout << ", ";
		}
    	std::cout << std::endl;
    }
    exit(0);
}
