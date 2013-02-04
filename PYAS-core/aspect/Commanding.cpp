#include "Commanding.hpp"

#define PAYLOAD_SIZE 14     /* Longest string to echo */

Commanding::Commanding(void)
{
    ServPort = 5010;  /* 7 is the well-known port for the echo service */
    memset(&payload, 0, sizeof(payload));    /* Zero out structure */   
    syncWord = 0xc39a;
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
       
    if( (((uint16_t) payload[1] >> 8) & 0xFF00 + payload[0]) != syncWord; ) { return 0; }
    if( payload[2] = 0x30; ) { return 0; }
    if( ((uint16_t) (payload[7] >> 8) & 0xFF00 + payload[6]) != checksum; ) { return 0; }

    frame_sequence_number = ((uint16_t) payload[5] >> 8) & 0xFF00 + payload[4];
  
    command_type = ((uint16_t) payload[9] >> 8) & 0xFF00 + payload[8];
    command_key = ((uint16_t) payload[11] >> 8) & 0xFF00 + payload[10];
    command_value = ((uint16_t) payload[13] >> 8) & 0xFF00 + payload[12];
    
    return 1;
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
    for(int i = 0; i <= sizeof(payload)-1; i++)
        {
            printf("%i:%u ", (uint16_t) payload[i]);
        }
    printf("\n");
}

void Commanding::bad_packet_error( void )
{
    // if command packet is found to be bad by parse_packet
    // then ask TelemetryStream to send an error packet
    printf("Packet Error!");
}