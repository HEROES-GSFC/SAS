#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define PAYLOAD_SIZE 9     /* Longest string to echo */
#define DEFAULT_PORT 7000 /* The default port to send on */

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *servIP;                    /* IP address of server */
    char payload[PAYLOAD_SIZE];                	/* payload to send to server */
    size_t payloadLen;               /* Length of payload */

    unsigned short crc_16, crc_16_modbus, crc_ccitt_ffff, crc_ccitt_0000, crc_ccitt_1d0f, crc_dnp, crc_sick, crc_kermit;
	crc_16_modbus  = 0xffff;


    //if ((argc < 3) || (argc > 4))    /* Test for correct number of arguments */
    //{
    //    fprintf(stderr,"Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
    //    exit(1);
    //}

    servIP = argv[1];           /* First arg: server IP address (dotted quad) */
    //payload = argv[2];       /* Second arg: string to echo */
	//*payload = 32;
	memset(&payload, 0, sizeof(payload));    /* Zero out structure */


	// build the HEROES Command Packet Header
	// uint16 - the sync word, split into two 8 bit chars
	payload[0] = (unsigned short int) 0xc39a & 0xFF;
	payload[1] = (unsigned short int) (0xc39a & 0xFF00) >> 8;
	// uint8 target system ID
	// 0 corresponds to Flight data recorder
	// see Table 6-2 in HEROES Telemetry and Command Interface Description doc
	payload[2] = 0;
	// uint8 - payload length (in bytes)
	payload[3] = 10;
	// uint16 - packet sequence number
	uint16_t packet_sequence_number = 345;
	payload[4] = (unsigned short int) packet_sequence_number & 0xFF;
	payload[5] = (unsigned short int) (packet_sequence_number & 0xFF00) >> 8;
	// uint16 - CRC-16 checksum
	uint16_t check_sum = 2345;
	payload[6] = (unsigned short int) check_sum & 0xFF;
	payload[7] = (unsigned short int) (check_sum & 0xFF00) >> 8;
	payload[8] = '\0';

    if ((payloadLen = sizeof(payload)) > PAYLOAD_SIZE)  /* Check input length */
        DieWithError("Echo word too long");

	printf("sending %zu bytes\n", payloadLen);
	int i;
	for(i = 0; i < sizeof(payload)-1; i++)
		{printf("Sending message %u\n", (uint8_t) payload[i]);}


    if (argc == 4)
        echoServPort = atoi(argv[3]);  /* Use given port, if any */
    else
        echoServPort = DEFAULT_PORT;  /* 7 is the well-known port for the echo service */

    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port   = htons(echoServPort);     /* Server port */


    /* Send the string to the server */
    if (sendto(sock, payload, payloadLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != payloadLen)
        DieWithError("sendto() sent a different number of bytes than expected");
  
    /* Recv a response */
    //fromSize = sizeof(fromAddr);
    //if ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, 
    //     (struct sockaddr *) &fromAddr, &fromSize)) != payloadLen)
    //    DieWithError("recvfrom() failed");

    //if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    //{
    //    fprintf(stderr,"Error: received a packet from unknown source.\n");
    //    exit(1);
    //}

    /* null-terminate the received data */
    //echoBuffer[respStringLen] = '\0';
    //printf("Received: %s\n", echoBuffer);    /* Print the echoed arg */
    
    close(sock);
    exit(0);
}
