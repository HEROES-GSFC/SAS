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
    /* Create a datagram/TCP socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) < 0){
        printf("TCPSender: socket() failed\n");
        return -1;
    }

    /* Construct the server address structure */
    memset(&sendAddr, 0, sizeof(sendAddr));    /* Zero out structure */
    sendAddr.sin_family = AF_INET;                 /* Internet addr family */
    int rtnVal = inet_pton(AF_INET, sendtoIP, &sendAddr.sin_addr.s_addr);
    if (rtnVal <= 0){
        printf("inet_pton() failed", "invalid address string");
        return -1;
    }
    sendAddr.sin_addr.s_addr = inet_addr(sendtoIP);  /* Server IP address */
    sendAddr.sin_port = htons(sendPort);     /* Server port */
    
    if (connect(sock, (struct sockaddr *) &sendAddr, sizeof(sendAddr)) < 0){
        printf("Connect() failed");
        return -1;
    }
    
    return sock;
}

void TCPSender::close_connection( void )
{
    close(sock);
}

void TCPSender::send( TelemetryPacket *packet )
{    
    int bytesSent;

    if( init_connection() >= 0){
        // update the frame number every time we send out a packet
        printf("TCPSender: Sending to %s\n", sendtoIP);
        
        uint8_t *payload = new uint8_t[packet->getLength()];
        packet->outputTo(payload);
        
        bytesSent = send(sock, payload, packet->getLength(), 0);
        
        if (bytesSent != packet->getLength()){
                printf("CommandSender: sendto() sent a different number of bytes (%u)than expected\n", bytesSent);
            }
        if (bytesSent == -1){ printf("CommandSender: sendto() failed!\n"); }
    }
    close_connection();
}

// TelemetrySender::TelemetrySender( const char *ip, unsigned short port )
//   : TCPSender(ip, port) { }
// 
// void TelemetrySender::send( TelemetryPacket *packet )
// {
//     int bytesSent;
// 
//     if( init_connection() >= 0){
//         // update the frame number every time we send out a packet
//         printf("TCPSender: Sending to %s\n", sendtoIP);
//         
//         uint8_t *payload = new uint8_t[packet->getLength()];
//         packet->outputTo(payload);
//     
//         bytesSent = sendto(sock, payload, packet->getLength(), 0, (struct sockaddr *)
//                    &sendAddr, sizeof(sendAddr));
//         if (bytesSent != packet->getLength()){
//                 printf("CommandSender: sendto() sent a different number of bytes (%u)than expected\n", bytesSent);
//             }
//         if (bytesSent == -1){ printf("CommandSender: sendto() failed!\n"); }
//     }
//     close_connection();
// }
// 
// CommandSender::CommandSender( const char *ip, unsigned short port )
//   : TCPSender(ip, port) { }
// 
// void CommandSender::send( CommandPacket *packet )
// {
//     int bytesSent;
//     
//     if( init_connection() >= 0){
//         // update the frame number every time we send out a packet
//         printf("Sending to %s\n", sendtoIP);
//         
//         uint8_t *payload = new uint8_t[packet->getLength()];
//         packet->outputTo(payload);
//     
//         /* Send the string to the server */
//         
//         bytesSent = sendto(sock, payload, packet->getLength(), 0, (struct sockaddr *)
//                    &sendAddr, sizeof(sendAddr));
//         if (bytesSent != packet->getLength()){
//                 printf("CommandSender: sendto() sent a different number of bytes (%u)than expected\n", bytesSent);
//             }
//         if (bytesSent == -1){ printf("CommandSender: sendto() failed!\n"); }
//         close_connection();
//     }      
// }
