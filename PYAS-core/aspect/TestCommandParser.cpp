#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <sys/types.h>  
#include <netinet/in.h>
//#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <time.h>       /* for sleep() and usleep() */
#include "lib_crc/lib_crc.h"


#define MAX_PAYLOAD 4096     /* Longest string to echo */
#define DEFAULT_PORT 7000 /* The default port to send on */
#define SAS_IP_ADDR "10.1.49.140"

void main()
{
	int sock;                       /* Socket descriptor */
    struct sockaddr_in localAddr; /* Echo server address */
    struct sockaddr_in remoteAddr;     /* Source address of echo */
    unsigned short localPort;     /* Echo server port */
    unsigned int remote_length;           /* In-out of address size for recvfrom() */
    uint16_t seq_number, seq_num_prev
    uint16_t command_key;
    uint16_t parameters[MAX_PARAMS];
    uint8_t num_params;
    bool format_valid, checksum_valid, first_packet, lost_packets, connection_active, parse_complete;
        
	
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
	
	while ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {printf("Socket fail... retrying in 1 sec..."); sleep(1);}  //Create a socket or wait 1 sec and retry on failure
    
    //Construct the local address structure
    bzero((char *)&localAddr, sizeof(localAddr));  //zero out structure
    ServAddr.sin_family = AF_INET;	//Internet address family
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//Any incoming interface
    ServAddr.sin_port = htons(ServPort);	//Local port
    while (bind(sock, (struct sockaddr *) &localAddr, sizeof(localAddr)) == -1) {printf("Bind fail... retrying in 1 sec..."); sleep(1);}  //Bind address to the socket... wait 1 sec and retry on failure (not sure if this is a good idea)
    
    remote_length = sizeof(remoteAddr);
    
    connection_active = TRUE;  //set connection status to active
    
    //while(1){
    while ((packet_length = recvfrom(sock, buffer, MAX_PAYLOAD, 0, (struck sockaddr *)&remoteaddr, &remote_length)) < 0) {printf("Failed to get datagram... retrying in 1 sec..."; sleep(1);}  //Get datagram, sleep on failure (not sure if good idea)
        
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
    checksum_valid = (do_checksum(packet, packet_length) == 0);
    command_key = packet[4];
    
    if (payload_length == (packet_length - 8)) {
	    //no spare bytes
	    else if (payload_length < (packet_length - 8)) {
		    //incomplete packet
		    else {
			    //spare bytes
	 }}}
	 
	 printf("Packet recieved:\nPayload Length: %x\nSequence Number: %x\n Checksum Status: %x\n Command Key: %x", payload_length, seq_num, checksum_valid, command_key);
	//}//end of while loop
	 	 
	 close(sock);
	 
	 return;
    
}

uint16_t do_checksum(char * payload, int payload_length )
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