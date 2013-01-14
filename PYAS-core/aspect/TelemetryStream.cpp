#include "TelemetryStream.hpp"

#define PAYLOAD_SIZE 9     /* Longest string to echo */
#define DEFAULT_PORT 7000 /* The default port to send on */

TelemetryStream::TelemetryStream(void)
{
    servIP = new char[11 + 1];
    servIP = "192.168.1.1"; // defined in Table 5-1
    ServPort = 5003;        // defined in Table 5-3
    memset(&payload, 0, sizeof(payload));    /* Zero out structure */
    frame_number = 0;
    syncWord = 0xc39a;
}

void TelemetryStream::initSocket( void )
{
   
}

void TelemetryStream::buildHeader( void )
{
    time_t current_time = time();
    // build the HEROES Command Packet Header (see Table 7-1)
    // uint16 - the sync word, split into two 8 bit chars
    payload[0] = (unsigned short int) (syncWord >> 8);
    payload[1] = (unsigned short int) syncWord & 0xFF;
    payload[2] = 0x70;              // Type ID of payload data (SAS)
    payload[3] = 0x30;              // Source ID
    payload[4] = (uint8_t) PAYLOAD_SIZE >> 8;                 // length of payload 2
    payload[5] = (uint8_t) PAYLOAD_SIZE & 0xff;                 // length of payload 3
    payload[6] = 0;                 // checksum
    payload[7] = 0;                 // checksum
    payload[8] = (uint8_t) (current_time.tv_nsec & 0xff000000) >> 24;  // time stamp Time stamp seconds (Linux epoch seconds since 00:00:00 Jan 1, 1970.).
    payload[9] = (uint8_t) (current_time.tv_nsec & 0x00ff0000) >> 16;  // time stamp Time stamp seconds (Linux epoch seconds since 00:00:00 Jan 1, 1970.).
    payload[10] = (uint8_t) (current_time.tv_nsec & 0x0000ff00) >> 8;   // time stamp Time stamp seconds (Linux epoch seconds since 00:00:00 Jan 1, 1970.).
    payload[11] = (uint8_t) (current_time.tv_nsec & 0x000000ff);        // time stamp Time stamp seconds (Linux epoch seconds since 00:00:00 Jan 1, 1970.).
    payload[12] = (uint8_t) (current_time.tv_sec & 0xff000000) >> 24;  // time stamp Time stamp seconds (Linux epoch seconds since 00:00:00 Jan 1, 1970.).
    payload[13] = (uint8_t) (current_time.tv_sec & 0x00ff0000) >> 16;  // time stamp Time stamp seconds (Linux epoch seconds since 00:00:00 Jan 1, 1970.).
    payload[14] = (uint8_t) (current_time.tv_sec & 0x0000ff00) >> 8;   // time stamp Time stamp seconds (Linux epoch seconds since 00:00:00 Jan 1, 1970.).
    payload[15] = (uint8_t) (current_time.tv_sec & 0x000000ff);        // time stamp Time stamp seconds (Linux epoch seconds since 00:00:00 Jan 1, 1970.).
       
}
void TelemetryStream::resetSequenceNumber( void )
{
    frame_sequence_number = 0;
}

bool TelemetryStream::testChecksum( void )
{
    // initialize check sum variable
    unsigned short checksum;
    checksum = 0xffff;
  
    char test[] = "123456789";
    for(int i = 0; i < sizeof(test)-1; i++){
            checksum = update_crc_16( checksum, (char) test[i] );}
    printf("4b37 vs calculated %x\n", checksum); 
    if (checksum == 0x4b37) return 1; else return 0;
}

void TelemetryStream::doChecksum( void )
{
    // initialize check sum variable
    unsigned short checksum;
    checksum = 0xffff;
    
    // calculate the checksum but leave out the last value as it contains the checksum
    for(int i = 0; i < sizeof(payload)-2; i++){
            checksum = update_crc_16( checksum, (char) payload[i] );}
    payload[6] = (uint8_t) checksum >> 8;
    payload[7] = checksum & 0xff;
    printf("checksum is %x\n", checksum); 
}

void TelemetryStream::printPacket( void )
{
    printf("Packet\n");
    for(int i = 0; i < sizeof(payload)-1; i++)
        {
            printf("%i:%u\n", (uint8_t) payload[i]);
        }
    printf("\n");
}

void TelemetryStream::updateSequenceNumber( void )
{
    frame_sequence_number++;
}

void TelemetryStream::closeSocket( void )
{
    close(sock);
}

void TelemetryStream::send( void )
{
    // update the frame number every time we send out a packet
    updateSequenceNumber();
    buildHeader();
    buildPayload();
    doChecksum();
    
    printf("Sending to %s\n", servIP);
    sendPacket();
}

void TelemetryStream::setTemperature( uint16_t temperature )
{
    temperatureInDegrees = temperature;
}

void TelemtryStream::buildPayload( void ){
    payload[16] = (uint8_t) temperatureInDegrees >> 8;
    payload[17] = temperatureInDegrees & 0xff;
} 

void TelemtryStream::sendPacket( void ){
    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        printf("socket() failed");

    /* Construct the server address structure */
    memset(&ServAddr, 0, sizeof(ServAddr));    /* Zero out structure */
    ServAddr.sin_family = AF_INET;                 /* Internet addr family */
    ServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    ServAddr.sin_port   = htons(ServPort);     /* Server port */

    /* Send the string to the server */
    if (sendto(sock, payload, payloadLen, 0, (struct sockaddr *)
               &ServAddr, sizeof(ServAddr)) != payloadLen)
        printf("sendto() sent a different number of bytes than expected");  
}
