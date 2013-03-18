#include "TCPReceiver.hpp"
#include "TCPSender.hpp"
#include <time.h>

int main(void)
{    
    //TCPSender tcpsender( ip, (unsigned int) 5010 );
    //tcpsender.send_packet();
    unsigned int no_packet_count;
    
    TCPReceiver tcprecv( 5010 );
    int sock;
    tcprecv.init_connection();
    while(1){
        while((sock = tcprecv.accept_packet()) >= 0){
            while(1){
                no_packet_count = 0;
                int packet_length = tcprecv.handle_tcpclient( sock );
                if (packet_length == 0){
                    no_packet_count++;
                    printf("packet count %d\n", no_packet_count);
                    if (no_packet_count >= 1000){ break; }
                    } else {
                    uint8_t *packet;
                    packet = new uint8_t[packet_length];
                    tcprecv.get_packet(packet);
                    for(int i = 0; i < packet_length; i++){ printf("%i", packet[i]); }
                    free(packet);
                }
            }
        }
    }
}