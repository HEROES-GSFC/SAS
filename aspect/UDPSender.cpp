#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "UDPSender.hpp"

UDPSender::UDPSender(void) : sendPort(7000)
{
    char ip[] = "192.168.1.114";
    sendtoIP = new char[strlen(ip)+1];
    strcpy(sendtoIP, ip);
}

UDPSender::UDPSender( const char *ip, unsigned short port ) : sendPort(port)
{
    sendtoIP = new char[strlen(ip)+1];
    strcpy(sendtoIP, ip);
}

UDPSender::~UDPSender()
{
    delete sendtoIP;
}

int UDPSender::init_connection( void )
{
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
        printf("UDPSender: socket() failed\n");
    }

    /* Construct the server address structure */
    memset(&sendAddr, 0, sizeof(sendAddr));    /* Zero out structure */
    sendAddr.sin_family = AF_INET;                 /* Internet addr family */
    sendAddr.sin_addr.s_addr = inet_addr(sendtoIP);  /* Server IP address */
    sendAddr.sin_port = htons(sendPort);     /* Server port */
    
    return sock;
}

void UDPSender::close_connection( void )
{
    close(sock);
}

void UDPSender::send( Packet *packet )
{    
    int bytesSent;

    if( init_connection() >= 0){
        // update the frame number every time we send out a packet
        //printf("UDPSender: Sending to %s\n", sendtoIP);
        
        uint8_t *payload = new uint8_t[packet->getLength()];
        packet->outputTo(payload);
    
        bytesSent = sendto(sock, payload, packet->getLength(), 0, (struct sockaddr *)
                           &sendAddr, sizeof(sendAddr));
        if (bytesSent != packet->getLength()){
            printf("UDPSender: sendto() sent a different number of bytes (%u)than expected\n", bytesSent);
        }
        if (bytesSent == -1){ printf("UDPSender: sendto() failed!\n"); }
        close_connection();

        delete payload;
    }
}

TelemetrySender::TelemetrySender( const char *ip, unsigned short port )
    : UDPSender(ip, port) { }

void TelemetrySender::send( TelemetryPacket *packet )
{
    int bytesSent;

    if( init_connection() >= 0){
        // update the frame number every time we send out a packet
        //printf("UDPSender: Sending to %s\n", sendtoIP);
        
        uint8_t *payload = new uint8_t[packet->getLength()];
        packet->outputTo(payload);
    
        bytesSent = sendto(sock, payload, packet->getLength(), 0, (struct sockaddr *)
                           &sendAddr, sizeof(sendAddr));
        if (bytesSent != packet->getLength()){
            printf("TelemetrySender: sendto() sent a different number of bytes (%u)than expected\n", bytesSent);
        }
        if (bytesSent == -1){ printf("TelemetrySender: sendto() failed!\n"); }
        close_connection();

        delete payload;
    }
}

CommandSender::CommandSender( const char *ip, unsigned short port )
    : UDPSender(ip, port) { }

void CommandSender::send( CommandPacket *packet )
{
    int bytesSent;
    
    if( init_connection() >= 0){
        // update the frame number every time we send out a packet
        //printf("Sending to %s\n", sendtoIP);
        
        uint8_t *payload = new uint8_t[packet->getLength()];
        packet->outputTo(payload);
    
        /* Send the string to the server */
        
        bytesSent = sendto(sock, payload, packet->getLength(), 0, (struct sockaddr *)
                           &sendAddr, sizeof(sendAddr));
        if (bytesSent != packet->getLength()){
            printf("CommandSender: sendto() sent a different number of bytes (%u)than expected\n", bytesSent);
        }
        if (bytesSent == -1){ printf("CommandSender: sendto() failed!\n"); }
        close_connection();

        delete payload;
    }      
}
