#include "UDPSender.hpp"

UDPSender::UDPSender(void) : sendPort(7000)
{
    char ip[] = "10.1.49.140";
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

void UDPSender::init_connection( void )
{
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        printf("UDPSender: socket() failed\n");

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
    printf("UDPSender: Sending to %s\n", sendtoIP);
    
    uint8_t *payload = new uint8_t[packet->getLength()];
    packet->outputTo(payload);

    /* Send the string to the server */
    if (sendto(sock, payload, packet->getLength(), 0, (struct sockaddr *)
               &sendAddr, sizeof(sendAddr)) != packet->getLength())
        printf("UDPSender: sendto() sent a different number of bytes than expected\n");
    close_connection();
}

TelemetrySender::TelemetrySender( const char *ip, unsigned short port )
  : UDPSender(ip, port) { }

void TelemetrySender::send( TelemetryPacket *packet )
{
    init_connection();
    // update the frame number every time we send out a packet
    printf("UDPSender: Sending to %s\n", sendtoIP);
    
    uint8_t *payload = new uint8_t[packet->getLength()];
    packet->outputTo(payload);

    /* Send the string to the server */
    if (sendto(sock, payload, packet->getLength(), 0, (struct sockaddr *)
               &sendAddr, sizeof(sendAddr)) != packet->getLength())
        printf("TelemetrySender: sendto() sent a different number of bytes than expected\n");
    close_connection();
}

CommandSender::CommandSender( const char *ip, unsigned short port )
  : UDPSender(ip, port) { }

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
        printf("CommandSender: sendto() sent a different number of bytes than expected\n");
    close_connection();
}
