#include "CommandStream.hpp"

#define PAYLOAD_SIZE 255    /* Longest packet */
#define DEFAULT_PORT 5000   /* The default port to listen on */

Commanding::Commanding(void)
{
    ServPort = DEFAULT_PORT;  /* 7 is the well-known port for the echo service */
    memset(&payload, 0, sizeof(payload));    /* Zero out structure */    
}


void Commanding::init_socket( void )
{

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        printf("socket() failed");

    /* Construct local address structure */
    memset(&ServAddr, 0, sizeof(ServAddr));   /* Zero out structure */
    ServAddr.sin_family = AF_INET;                /* Internet address family */
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    ServAddr.sin_port = htons(ServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0)
        printf("bind() failed");
}

void Commanding::close_socket( void )
{
    close( sock );
}

void Commanding::listen( void )
{
    int ok;

    // need to start a new thread here       	
    /* Set the size of the in-out parameter */
    cliAddrLen = sizeof(ClntAddr);

    /* Block until receive message from a client */
    if ((recvMsgSize = recvfrom(sock, payload, sizeof(payload), 0,
        (struct sockaddr *) &ClntAddr, &cliAddrLen)) < 0)
        printf("recvfrom() failed");

    printf("Handling client %s\n", inet_ntoa(ClntAddr.sin_addr));

    for(int i = 0; i < sizeof(payload)-1; i++)
        {printf("Received message %u\n", (uint8_t) payload[i]);}

    ok = parse_packet();
    if (!ok) { bad_packet_error(); }
}

int Commanding::parse_packet( void )
{
    uint16_t checksum;
    checksum = calculate_checksum();
    
    if( payload[PAYLOAD_SIZE-1] != checksum ){ return 0; }
    if( payload[0] != 0xeb90 ) { return 0; }
    if( payload[1] != 0xf626 ) { return 0; }
}

uint16_t Commanding::calculate_checksum( void )
{
    // initialize check sum variable
    uint16_t checksum;
    checksum = 0xffff;
    
    // calculate the checksum but leave out the last value as it contains the checksum
    for(int i = 0; i < sizeof(payload)-2; i++){
            checksum = update_crc_16( checksum, (char) payload[i] );}
   return checksum;
}

void Commanding::print_packet( void )
{
    printf("Packet\n");
    for(int i = 0; i < sizeof(payload)-1; i++)
        {
            printf("%u ", (uint16_t) payload[i]);
        }
    printf("\n");
}

void Commanding::bad_packet_error( void )
{
    // if command packet is found to be bad by parse_packet
    // then ask TelemetryStream to send an error packet
}