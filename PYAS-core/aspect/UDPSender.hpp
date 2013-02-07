#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "lib_crc/lib_crc.h"
#include "Command.hpp"
#include "Telemetry.hpp"

class UDPSender {
    protected:
        int sock;                       /* Socket descriptor */
        struct sockaddr_in sendAddr;    /* Echo server address */
        unsigned int fromSize;          /* In-out of address size for recvfrom() */
        char *sendtoIP;                 /* IP address to send to */
        unsigned short sendPort;        /* Port to send on*/

    public:
        UDPSender( void );
        UDPSender( const char *ip, unsigned short port );
	~UDPSender();
        
        virtual void send(  TelemetryPacket *packet  );
        void init_connection( void );
        void close_connection( void );
};

class TelemetrySender: public UDPSender {
   
    public:
        TelemetrySender( const char *ip, unsigned short port );
        virtual void send( TelemetryPacket *packet );
};

class CommandSender: public UDPSender {

    public:
        CommandSender( const char *ip, unsigned short port );
        virtual void send( CommandPacket *packet );
};
