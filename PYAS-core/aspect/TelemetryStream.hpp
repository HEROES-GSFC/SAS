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
        bool test_checksum( void );
        void set_temperature( unsigned short int temperature );
        void send( void );
        void make_test_packet( int packet_sequence_number );
        void init_socket( void );
        void print_packet( void );
        unsigned short int payload[PAYLOAD_SIZE];      /* payload to send to server */
};
