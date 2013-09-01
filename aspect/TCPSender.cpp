#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#include "TCPSender.hpp"

TCPSender::TCPSender(void) : sendPort(7000)
{
    char ip[] = "192.168.1.114";
    sendtoIP = new char[strlen(ip)+1];
    strcpy(sendtoIP, ip);
    sock = 0;
}

TCPSender::TCPSender( const char *ip, unsigned short port ) : sendPort(port)
{
    sendtoIP = new char[strlen(ip)+1];
    strcpy(sendtoIP, ip);
}

TCPSender::~TCPSender()
{
    delete sendtoIP;
}

int TCPSender::init_connection( void )
{
    
    // Create a reliable, stream socket using TCP
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        printf("socket() failed");
    } else {

        // Construct the server address structure
        struct sockaddr_in servAddr; // Server address
        memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
        servAddr.sin_family = AF_INET; // IPv4 address family
        // Convert address
        int rtnVal = inet_pton(AF_INET, sendtoIP, &servAddr.sin_addr.s_addr);
        if (rtnVal == 0){ printf("inet_pton() failed, invalid address string"); }
        else if (rtnVal < 0){ printf("inet_pton() failed"); }
        servAddr.sin_port = htons(sendPort); // Server port

        // Establish the connection to the echo server
        if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
            printf("connect() failed");
            sock = -1;
        }
    }

    return sock;
}

void TCPSender::close_connection( void )
{
    close(sock);
}

void TCPSender::send_packet( Packet *packet )
{    
    int bytesSent;

    if( sock > 0){
        // update the frame number every time we send out a packet
        printf("TCPSender: Sending to %s\n", sendtoIP);
        
        uint8_t *payload = new uint8_t[packet->getLength()];
        packet->outputTo(payload);
        
        bytesSent = send(sock, payload, packet->getLength(), 0);
        
        if (bytesSent != packet->getLength()){
            printf("TCPSender: sendto() sent a different number of bytes (%u)than expected\n", bytesSent);
        }
        if (bytesSent == -1){ printf("TCPSender: sendto() failed!\n"); }
        delete payload;
    }
}
