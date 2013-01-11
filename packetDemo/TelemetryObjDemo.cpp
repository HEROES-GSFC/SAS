#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "lib_crc/lib_crc.h"

#define PAYLOAD_SIZE 9     /* Longest string to echo */
#define DEFAULT_PORT 7000 /* The default port to send on */

class TelemetryStream {
    private:
        int sock;                       /* Socket descriptor */
        struct sockaddr_in ServAddr; /* Echo server address */
        struct sockaddr_in fromAddr;     /* Source address of echo */
        unsigned short ServPort;     /* Echo server port */
        unsigned int fromSize;           /* In-out of address size for recvfrom() */
        char *servIP;                     /* IP address of server */
        size_t payloadLen;               /* Length of payload */
        int frame_number;
        void update_frame_number( void );
        void reset_frame_number( void );
        void do_checksum( void );
        void close_connection( void );

    public:
        TelemetryStream(void);
        void set_temperature( unsigned short int temperature );
        void send( void );
        void make_test_packet( int packet_sequence_number );
        void init_socket( void );
        void print_packet( void );
        char payload[PAYLOAD_SIZE];      /* payload to send to server */
};


TelemetryStream::TelemetryStream(void)
{
    servIP = new char[11 + 1];
    servIP = "10.1.49.140";
    ServPort = DEFAULT_PORT;  /* 7 is the well-known port for the echo service */
    memset(&payload, 0, sizeof(payload));    /* Zero out structure */
    frame_number = 0;
    
    // build the HEROES Command Packet Header
    // uint16 - the sync word, split into two 8 bit chars
    payload[0] = (unsigned short int) 0xc39a & 0xFF;
    payload[1] = (unsigned short int) (0xc39a & 0xFF00) >> 8;
}

void TelemetryStream::init_socket( void )
{
   
}

void TelemetryStream::reset_frame_number( void )
{
    frame_number = 0;
}

void TelemetryStream::do_checksum( void )
{
    // initialize check sum variable
    unsigned short checksum;
    checksum = 0xffff;
    
    // calculate the checksum but leave out the last value as it contains the checksum
    for(int i = 0; i < sizeof(payload)-2; i++){
            checksum = update_crc_16( checksum, payload[i] );}
    payload[8] = checksum;
    printf("checksum is %u\n", checksum); 
}

void TelemetryStream::print_packet( void )
{
    printf("Packet\n");
    for(int i = 0; i < sizeof(payload)-1; i++)
        {
            printf("%u ", (uint8_t) payload[i]);
        }
    printf("\n");
}

void TelemetryStream::update_frame_number( void )
{
    frame_number++;
}

void TelemetryStream::close_connection( void )
{
    close(sock);
}

void TelemetryStream::send( void )
{
    // update the frame number every time we send out a packet
    update_frame_number();    
    printf("Sending to %s\n", servIP);
    
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        printf("socket() failed");

    /* Construct the server address structure */
    memset(&ServAddr, 0, sizeof(ServAddr));    /* Zero out structure */
    ServAddr.sin_family = AF_INET;                 /* Internet addr family */
    ServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    ServAddr.sin_port   = htons(ServPort);     /* Server port */

    /* Send the string to the server */
    if (sendto(sock, payload, payloadLen, 0, (struct sockaddr *)
               &ServAddr, sizeof(ServAddr)) != payloadLen)
        printf("sendto() sent a different number of bytes than expected");  

    close(sock);
}

void TelemetryStream::set_temperature( unsigned short int temperature )
{
    payload[2] = temperature;
}

void TelemetryStream::make_test_packet( int packet_sequence_number )
{
        // build the HEROES Command Packet Header
        // uint16 - the sync word, split into two 8 bit chars
        payload[0] = (unsigned short int) 0xc39a & 0xFF;
        payload[1] = (unsigned short int) (0xc39a & 0xFF00) >> 8;
        // uint8 target system ID
        // 0 corresponds to Flight data recorder
        // see Table 6-2 in HEROES Telemetry and Command Interface Description doc
        payload[2] = 0;
        // uint8 - payload length (in bytes)
        payload[3] = PAYLOAD_SIZE;
        // uint16 - packet sequence number
        payload[4] = (unsigned short int) packet_sequence_number & 0xFF;
        payload[5] = (unsigned short int) (packet_sequence_number & 0xFF00) >> 8;
        // uint16 - CRC-16 checksum
        // need to actually calculate the check sum here, use lib_crc
        uint16_t check_sum = 2345;
        
        payload[6] = (unsigned short int) check_sum & 0xFF;
        payload[7] = (unsigned short int) (check_sum & 0xFF00) >> 8;
        
        do_checksum();
}       

int main()
{
    TelemetryStream *tStream;
    tStream = new TelemetryStream;

    tStream->make_test_packet( 4 );
    tStream->print_packet();
    tStream->init_socket();
    tStream->send();
  
    delete tStream;
    exit(0);
}
