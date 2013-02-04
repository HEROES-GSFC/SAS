#include "UDPSender.hpp"

UDPSender::UDPSender(void)
{
    sendtoIP = new char[11 + 1];
    sendtoIP = "10.1.49.140";
    sendPort = 7000;
}

UDPSender::UDPSender( char *ip, unsigned short port )
{
    sendtoIP = ip;
    sendPort = port;
}

void UDPSender::init_connection( void )
{
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        printf("socket() failed");

    /* Construct the server address structure */
    memset(&sendAddr, 0, sizeof(sendAddr));    /* Zero out structure */
    sendAddr.sin_family = AF_INET;                 /* Internet addr family */
    sendAddr.sin_addr.s_addr = inet_addr(sendtoIP);  /* Server IP address */
    sendAddr.sin_port   = htons(sendPort);     /* Server port */
}

void UDPSender::close_connection( void )
{
    close(sock);
}

void UDPSender::send( TelemetryPacket *packet )
{
    init_connection();
    // update the frame number every time we send out a packet
    printf("Sending to %s\n", sendtoIP);
    
    uint8_t *payload = new uint8_t[packet->getLength()];
    packet->outputTo(payload);

    /* Send the string to the server */
    if (sendto(sock, payload, packet->getLength(), 0, (struct sockaddr *)
               &sendAddr, sizeof(sendAddr)) != packet->getLength())
        printf("sendto() sent a different number of bytes than expected");
    close_connection();
}

TelemetrySender::TelemetrySender( char *ip, unsigned short port )
{
    sendtoIP = ip;
    sendPort = port;
}

void TelemetrySender::send( TelemetryPacket *packet )
{
    init_connection();
    // update the frame number every time we send out a packet
    printf("Sending to %s\n", sendtoIP);
    
    uint8_t *payload = new uint8_t[packet->getLength()];
    packet->outputTo(payload);

    /* Send the string to the server */
    if (sendto(sock, payload, packet->getLength(), 0, (struct sockaddr *)
               &sendAddr, sizeof(sendAddr)) != packet->getLength())
        printf("sendto() sent a different number of bytes than expected");
    close_connection();
}

CommandSender::CommandSender( char *ip, unsigned short port )
{
    sendtoIP = ip;
    sendPort = port;
}

void CommandSender::send( CommandPacket *packet )
{
    init_connection();
    // update the frame number every time we send out a packet
    printf("Sending to %s\n", sendtoIP);
    
    uint8_t *payload = new uint8_t[packet->getLength()];
    packet->outputTo(payload);

    /* Send the string to the server */
    if (sendto(sock, payload, packet->getLength(), 0, (struct sockaddr *)
               &sendAddr, sizeof(sendAddr)) != packet->getLength())
        printf("sendto() sent a different number of bytes than expected");
    close_connection();
}