#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include "Command.hpp"
#include "Telemetry.hpp"

class UDPReceiver {
protected:
    int sock;                       /* Socket */
    struct sockaddr_in myAddr;      /* Local address */
    struct sockaddr_in senderAddr;  /* Sender address */
    
    unsigned int cliAddrLen;        /* Length of incoming message */
    char payload[PACKET_MAX_SIZE];  /* Buffer for echo string */
    unsigned short listeningPort;   /* The port to listen to */
    int recvMsgSize;                /* Size of received message */

public:
    UDPReceiver( void );
    UDPReceiver( unsigned short port );
    ~UDPReceiver();
        
    unsigned int listen( void );
    void get_packet( uint8_t *packet  );
    void init_connection( void );
    void close_connection( void );
};

class CommandReceiver: public UDPReceiver {

public:
    CommandReceiver( unsigned short port );
};

class TelemetryReceiver: public UDPReceiver {

public:
    TelemetryReceiver( unsigned short port );
};
