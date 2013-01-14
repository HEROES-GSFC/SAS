#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <time.h>       /* for time_t */
#include "lib_crc/lib_crc.h"

#define PAYLOAD_SIZE 9     /* Longest string to echo */

class TelemetryStream {
    private:
        int sock;                       /* Socket descriptor */
        struct sockaddr_in ServAddr; /* Echo server address */
        struct sockaddr_in fromAddr;     /* Source address of echo */
        unsigned short ServPort;     /* Echo server port */
        unsigned int fromSize;           /* In-out of address size for recvfrom() */
        char *servIP;                     /* IP address of server */
        size_t payloadLen;               /* Length of payload */
        int frame_sequence_number;
        uint16_t syncWord;

        uint16_t temperatureInDegrees;
        
        void updateSequenceNumber( void );
        void resetSequenceNumber( void );
        void Dochecksum( void );
        void closeSocket( void );
        void buildHeader( void );
        void buildPayload( void );
        void sendPacket( void );
    public:
        TelemetryStream(void);
        bool testChecksum( void );
        void setTemperature( uint16_t temperature );
        void send( void );
        void initSocket( void );
        void printPacket( void );
        uint8_t payload[PAYLOAD_SIZE];      /* payload to send to server */
};
