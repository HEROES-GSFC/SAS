#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "lib_crc/lib_crc.h"

#include "UDPReceiver.hpp"

UDPReceiver::UDPReceiver(void){
    listeningPort = 5000;
}

UDPReceiver::UDPReceiver( unsigned short port ){
    listeningPort = port;
}

unsigned int UDPReceiver::listen( void ){
    /* Block until receive message from a client */
    recvMsgSize = recvfrom(sock, payload, sizeof(payload), 0,
                           (struct sockaddr *) &senderAddr, &cliAddrLen);
        
    if (recvMsgSize < 0){
        //printf("recvfrom() failed");
        return 0;
    } else {
        //printf("received  %u bytes\n", recvMsgSize);
        return recvMsgSize;
    }
}

void UDPReceiver::init_connection( void ){
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        printf("socket() failed");

    /* Construct local address structure */
    memset(&myAddr, 0, sizeof(myAddr));   /* Zero out structure */
    myAddr.sin_family = AF_INET;                /* Internet address family */
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    myAddr.sin_port = htons(listeningPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0)
        printf("bind() failed");

    /* Set the size of the in-out parameter */
    cliAddrLen = sizeof(senderAddr);
}

void UDPReceiver::get_packet( uint8_t *packet ){
    memcpy( packet, payload, recvMsgSize);
}

void UDPReceiver::close_connection( void ){
    close( sock );
}

CommandReceiver::CommandReceiver( unsigned short port ){
    listeningPort = port;
}

TelemetryReceiver::TelemetryReceiver( unsigned short port ){
    listeningPort = port;
}




