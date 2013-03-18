#define numXpixels 1392
#define numYpixels 1090

#include <stdio.h>      /* for printf() and fprintf() */
#include <unistd.h>     /* for sleep()  */

#include "TCPSender.hpp"
#include "Telemetry.hpp"

// loopback IP, just talking to myself
char ip[] = "127.0.0.1";

int main(void)
{  
    TCPSender tcpSndr(ip, (unsigned short) 5010);
    tcpSndr.init_connection();
    
    int pixels_per_packet = 100;
    int num_packets = numXpixels * numYpixels / pixels_per_packet;
    long int count = 0;
    for( int i = 0; i < num_packets; i++ ){
        printf("%d\n", i);
        TelemetryPacket tp(0x70, 0x30);
        for( int j = 0; j < pixels_per_packet; j++){ tp << (uint8_t)count/(numXpixels * numYpixels); }
        tcpSndr.send_packet( &tp );
        printf("sending %d bytes\n", tp.getLength());
        count++;
    }
    tcpSndr.close_connection();
    
    return 0;
}
