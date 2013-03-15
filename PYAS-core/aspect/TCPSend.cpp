#define NUM_THREADS 6
#define SAS_TARGET_ID 0x30
#define SAS_TM_TYPE 0x70
#define SAS_IMAGE_TYPE 0x82
#define SAS_SYNC_WORD 0xEB90
#define SAS_CM_ACK_TYPE 0x01

#define numXpixels 1392
#define numYpixels 1090

#include <stdio.h>      /* for printf() and fprintf() */
#include <pthread.h>    /* for multithreading */
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for sleep()  */
#include <signal.h>     /* for signal() */
#include <math.h>       /* for testing only, remove when done */

//#include "UDPSender.hpp"
//#include "UDPReceiver.hpp"
#include "TCPSender.hpp"
//#include "Command.hpp"
#include "Telemetry.hpp"

unsigned int stop_message[NUM_THREADS];
uint16_t command_sequence_number = 0;
uint16_t latest_sas_command_key = 0x0000;
uint32_t tm_frame_sequence_number = 0;

// loopback IP, just talking to myself
char ip[] = "127.0.0.1";



int main(void)
{  
    TCPSender tcpSndr(ip, (unsigned short) 5010);
    tcpSndr.init_connection();

    uint32_t dimx = numXpixels;
    uint32_t dimy = numYpixels;

    uint32_t img_index;
    
    for( int i = 0; i < 100; i++ ){
        printf("%d\n", i);
        img_index = i * (dimx * dimy)/10;
        TelemetryPacket tp(0x70, 0x30);
        //tp << (uint16_t)img_index;
        //for( int j = 0; j < 50; j++){ tp << (uint8_t) j; }
        std::cout << tp << std::endl;
        tcpSndr.send_packet( &tp );
        tcpSndr.send_packet( &tp );
        sleep(1);
    }
    tcpSndr.close_connection();
    
    return 0;
}
