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
    
    int numYpixels = 500;
    int numXpixels = 500;
    
    int pixels_per_packet = 10;
    int num_packets = numXpixels * numYpixels / pixels_per_packet;
    long int count = 0;
    TelemetryPacket tp(0x70, 0x30);
    tp << (uint16_t)numXpixels;
    tp << (uint16_t)numYpixels;
    tcpSndr.send_packet( &tp );
    
    uint8_t *pixels = (uint8_t *)malloc(numYpixels * numXpixels);
    for( int xi = 0; xi < numXpixels; xi++)
    {
        for( int yi = 0; yi < numYpixels; yi++)
        {
            pixels[yi * numYpixels + xi] = 256 * xi / numXpixels;
        }
    }
    
    long k = 0;
    for( int i = 0; i < num_packets; i++ ){
        printf("%d\n", i);
        TelemetryPacket tp(0x70, 0x30);
        for( int j = 0; j < pixels_per_packet; j++){ //tp << (uint8_t)count/(numXpixels * numYpixels); }
            tp << (uint8_t)pixels[k];
            k++;}
        tcpSndr.send_packet( &tp );
        printf("sending %d bytes\n", tp.getLength());
        count++;
    }
    tcpSndr.close_connection();
    
    return 0;
}
