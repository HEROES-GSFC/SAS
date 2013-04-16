#include "Command_Parser.hpp"

#define MAX_PAYLOAD 4096     /* Longest string to echo */
#define DEFAULT_PORT 7000 /* The default port to send on */
#define SAS_IP_ADDR "10.1.49.140"

Command_Parser::Command_Parser(void)
{
    
	localPort = DEFAULT_PORT;
	
	listsize = 0;	
	/*
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
	while ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
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

int Command_Parser::listen(void);
/*This is intended to be a free-running background function that will populate the command list
  It should be called within it's own thread.
  
  !!Need to consider how this function is to be shut down... need to look into how the recvfrom() 
  function can be aborted...*/
{                                      
	uint16_t * packet = (uint16_t *) buffer;
	int packet_length, payload_length;
			
    while (1)
    {
		ListenOnce
	}
}

void Command_Parser::ListenOnce()
	while ((packet_length = recvfrom(sock, buffer, MAX_PAYLOAD, 0, (struct sockaddr *)&remoteAddr, &remote_length)) < 0) 
    {
	    printf("Failed to get datagram... exiting"); //Get datagram, sleep & retry on failure (not sure if good idea)
    	return 0;
    }
    	
    command * newcommand = new command;  //allocate space for the new command
		    	
    //Add code here to enable searching for packet header
    /*while((packet[0] != 0xc39a) || (buffer[2] != 0x30))
    {
		    
	    *buffer = *buffer + 1;   //find the packet header, discard invalid words and count the number discarded
	    packet = (uint16_t *) buffer;
	    discards++;
    }*/
    newcommand->format_valid = (packet[0] != 0xc39a) || (buffer[2] != 0x30);
    payload_length = buffer[3];  //payload length is in bytes and must be even, therefore divide by 2 gives number of words
    newcommand->num_params = payload_length/2 - 1;
    newcommand->seq_num = packet[2];
    newcommand->checksum_valid = (do_checksum((char *)buffer, packet_length) == 0);
    newcommand->key = packet[4];
    for(i = 0;i < newcommand->num_params;i++)
    {
	    newcommand->parameters[i] = packet[i+5];  //copy parameters into command data structure
    )
    	
	/*  Add code here to handle chopping/splicing of SAS packets within UDP framework
	if (payload_length == (packet_length - 8)) {
	}//no spare bytes
	else if (payload_length < (packet_length - 8)) {
	}//incomplete packet
	else {
			//spare bytes
	}*/
		 
	if (verbose = TRUE)
	{
		printf("Packet recieved:\nPacket Length: x%x\nPayload Length: x%x\nSequence Number: x%x\n Checksum Status: x%x\nCommand Key: x%x", packet_length, payload_length, seq_num, checksum_valid, command_key);
		printf("Packet contents:\n");
		printf("uint16_t       uint8_t");
		for(int i = 0;i < packet_length/2;i++)
		{
			printf("x%4.4x          x%2.2x x%2.2x",(int) packet[i]);
		}
	}
	
		insert(newcommand);
	}
}

void Command_Parser::insert(command * newcommand)
{
	newcommand * searchpointer;
	
	if (listsize == 0)
	{
		first = newcommand;
		newcommand->next = newcommand;  //pointer to self indicates last in list
	)
	else if (first->seqnum > newcommand->seqnum)
	{
			newcommand->next = first;
			first = newcommand;
	}
	else
	{	
		searchpointer = first;
		while ((searchpointer->next->seqnum > newcommand->seqnum) && (searchpointer->next != searchpointer))
		{
			newcommand->next = newcommand->next->next;   //step through the list to find the appropriate location (or end of lest)
		}
		newcommand->next = searchpointer->next;    //insert the new command into the list
		searchpointer->next = newcommand;
	}
	listsize++;
}

bool Command_Parser::dispatch()  //returns empty() after the top command is dispatched
{
	if listsize = 0
	{
		return TRUE;
	}
	else if listsize = 1
	{
		listsize = 0;
		delete first;
		first = NULL;
	}
	else
		listsize--;
		Command * temp = first;
		first = first->next;
		delete temp;
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

bool Command_Parser::empty(void)
{
	if listsize = 0
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool Command_Parser::connection_status(void)
{
	return connection_active;
}

uint16_t Command_Parser::get_key(void)
{
	return first->key;
}

int Command_Parser::get_num_params()
{
	return num_params;
}

uint16_t Command_Parser::get_params(int index)
{
	if (index < 0) or (index >= num_params)
	{
		return -1;
	}
	else
	{
		return first->parameters[index];
	}
}

uint16_t Command_Parser::get_seqnum()
{
	return first->seq_number;
}

bool set_verbose(bool newsetting)
{
	verbose = newsetting;
	return verbose;
}