#include "Command_Parser.hpp"

#define MAX_PAYLOAD 4096     /* Longest string to echo */
#define DEFAULT_PORT 7000 /* The default port to send on */
#define SAS_IP_ADDR "10.1.49.140"

Command_Parser::Command_Parser(void)
{
    int packet_length, payload_length, discards;
	uint8_t buffer[MAX_PAYLOAD];
	uint16_t * packet = buffer;
	
	localPort = DEFAULT_PORT;
	
	seq_num_prev = 0;
	packet_fail = FALSE;
	checksum_fail = FALSE;
	first_packet = TRUE;
	lost_packets = FALSE;
	connection_active = FALSE;
	
	/*
	while ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {sleep(1);}  //Create a socket or wait 1 sec and retry on failure
    
    //Construct the local address structure
    bzero((char *)&localAddr, sizeof(localAddr));  //zero out structure
    ServAddr.sin_family = AF_INET;	//Internet address family
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//Any incoming interface
    ServAddr.sin_port = htons(ServPort);	//Local port
    while (bind(sock, (struct sockaddr *) &localAddr, sizeof(localAddr)) == -1) {sleep(1);}  //Bind address to the socket... wait 1 sec and retry on failure (not sure if this is a good idea)
    
    remote_length = sizeof(remoteAddr);
    
    connection_active = TRUE;  //set connection status to active
    
    while ((packet_length = recvfrom(sock, buffer, MAX_PAYLOAD, 0, (struck sockaddr *)&remoteaddr, &remote_length)) < 0) {sleep(1);}  //Get datagram, sleep on failure (not sure if good idea)
    
    //!!!!!!!!!!!Add spawn new command parser here (using the spawn constructor)
    
    //begin parsing packet
    while((packet[0] != 0xc39a) || (buffer[2] != 0x30))
    {
	    
	    buffer++;   //find the packet header, discard invalid words and count the number discarded
	    packet = buffer;
	    discards++;
    }
    payload_length = buffer[3];  //payload length is in bytes and must be even, therefore divide by 2 gives number of words
    num_params = payload_length/2 - 1;
    seq_num = packet[2];
    checksum_valid = (do_checksum(packet, packet_length) == packet[3]);
    
    if (payload_length == (packet_length - 8)) {
	    //no spare bytes
	    else if (payload_length < (packet_length - 8)) {
		    //incomplete packet
		    else {
			    //spare bytes
	    }}}
    
    */
}

int Command_Parser::Init(void)
{
	while ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) 
	{printf("Socket fail... retrying in 1 sec..."); sleep(1);}  //Create a socket or wait 1 sec and retry on failure
    
    //Construct the local address structure
    bzero((char *)&localAddr, sizeof(localAddr));  //zero out structure
    localAddr.sin_family = AF_INET;	//Internet address family
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//Any incoming interface
    localAddr.sin_port = htons(localPort);	//Local port
    while (bind(sock, (struct sockaddr *) &localAddr, sizeof(localAddr)) == -1) 
    {printf("Bind fail... retrying in 1 sec..."); sleep(1);}  //Bind address to the socket... wait 1 sec and retry on failure (not sure if this is a good idea)
 
    remote_length = sizeof(remoteAddr);
    
    connection_active = TRUE;  //set connection status to active
    
    return 0;
}

int Command_Parser::Close(void)
{
	connection_active = FALSE;
	close(sock);
	
	return 0;
}

int Command_Parser::TestGetOneCmd(void);
{
	uint16_t * packet = (uint16_t *) buffer;
		
    while ((packet_length = recvfrom(sock, buffer, MAX_PAYLOAD, 0, (struct sockaddr *)&remoteAddr, &remote_length)) < 0) 
    {printf("Failed to get datagram... retrying in 1 sec..."); sleep(1);}  //Get datagram, sleep on failure (not sure if good idea)
    //begin parsing packet
    /*while((packet[0] != 0xc39a) || (buffer[2] != 0x30))
    {
	    
	    *buffer = *buffer + 1;   //find the packet header, discard invalid words and count the number discarded
	    packet = (uint16_t *) buffer;
	    discards++;
    }*/
    payload_length = buffer[3];  //payload length is in bytes and must be even, therefore divide by 2 gives number of words
    num_params = payload_length/2 - 1;
    seq_num = packet[2];
    checksum_valid = (do_checksum((char *)buffer, packet_length) == 0);
    command_key = packet[4];
    
    if (payload_length == (packet_length - 8)) {
	}//no spare bytes
	else if (payload_length < (packet_length - 8)) {
	}//incomplete packet
	else {
			//spare bytes
	}
	 
	printf("Packet recieved:\nPacket Length: x%x\nPayload Length: x%x\nSequence Number: x%x\n Checksum Status: x%x\nCommand Key: x%x", packet_length, payload_length, seq_num, checksum_valid, command_key);
	printf("Packet contents:\n");
	for(int i = 0;i < packet_length/2;i++)
	{
		printf("%x",(int) packet[i]);
	}	
}


bool Command_Parser::test_checksum( void )
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

uint16_t Command_Parser::do_checksum(char * payload, int payload_length )
{
    // initialize check sum variable
    unsigned short checksum;
    checksum = 0xffff;
    
    // calculate the checksum but leave out the last value as it contains the checksum
    for(int i = 0; i < payload_length; i++){
            checksum = (uint16_t) update_crc_16( checksum, payload[i] );}
    //printf("checksum is %x\n", checksum);
    return checksum;
}

bool Command_Parser::packet_status(void)
{
	return format_valid;
}

bool Command_Parser::checksum_status(void)
{
	return checksum_valid;
}

bool Command_Parser::packets_lost(void)
{
	return lost_packets;
}

bool Command_Parser::connection_status(void)
{
	return connection_active;
}

bool Command_Parser::parse_status(void)
{
	return parse_complete;
}

uint16_t Command_Parser::get_command_key(void)
{
	return command_key;
}

(uint16_t *) Command_Parser::get_params(void)
{
	return parameters;
}

uint8_t Command_Parser::get_num_params(void)
{
	return num_params;
}