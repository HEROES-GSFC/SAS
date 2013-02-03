#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define ECHOMAX 1024     /* Longest string to echo */

/* This program listens for packet on UDP port 5000 and forwards them to 
    itself on port 5010 and
    forwardingIP on port 5000
*/

void DieWithError(char *errorMessage)
{
    printf("%s", errorMessage);
}

int main()
{
    int sock;                        /* Socket */
    struct sockaddr_in myAddr; /* Local address */
    struct sockaddr_in senderAddr; /* Sender address */
    struct sockaddr_in forwarding1Addr; /* Forwarding address 1 (myself on port 5010)*/
    struct sockaddr_in forwarding2Addr; /* Forwarding address 1 (other IP on port 5000)*/
    
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short listeningPort;     /* The port to listen to */
    unsigned short forwardingPort1;    /* port 1 to forward to */
    unsigned short forwardingPort2;    /* port 2 to forward to */
    int recvMsgSize;                 /* Size of received message */

    // listen to port 5000 for UDP packets and forward them to 192.168.2.2
    char *forwadingIP2 = "192.168.2.2";     /* IP address to forward to */
    char *forwadingIP1 = "192.168.2.1";     /* IP address to forward to */
        
	listeningPort = 5000;
	forwardingPort1 = 5010;
	forwardingPort2 = listeningPort;
	
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&myAddr, 0, sizeof(myAddr));   /* Zero out structure */
    myAddr.sin_family = AF_INET;                /* Internet address family */
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    myAddr.sin_port = htons(listeningPort);      /* Local port */

   /* Construct one forwarding address structure */
    memset(&forwarding1Addr, 0, sizeof(myAddr));    /* Zero out structure */
    forwarding1Addr.sin_family = AF_INET;                 /* Internet addr family */
    forwarding1Addr.sin_addr.s_addr = inet_addr(forwadingIP1);  /* Server IP address */
    forwarding1Addr.sin_port   = htons(forwardingPort1);     /* Port to forward on is same as listening port*/

    /* Construct one forwarding address structure */
    memset(&forwarding2Addr, 0, sizeof(myAddr));    /* Zero out structure */
    forwarding2Addr.sin_family = AF_INET;                 /* Internet addr family */
    forwarding2Addr.sin_addr.s_addr = inet_addr(forwadingIP2);  /* Server IP address */
    forwarding2Addr.sin_port   = htons(forwardingPort2);     /* Port to forward on is same as listening port*/
    
    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0)
        DieWithError("bind() failed");
  
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(senderAddr);

        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, echoBuffer, sizeof(echoBuffer), 0,
            (struct sockaddr *) &senderAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");

        // now forward two packets, one to itself on port 5010 and another to the
        // forwarding address on port 5000
        
        /* Forward UDP packet */
        if (sendto(sock, echoBuffer, recvMsgSize, 0, 
             (struct sockaddr *) &forwarding1Addr, sizeof(forwarding1Addr)) != recvMsgSize)
            DieWithError("sendto() sent a different number of bytes than expected");
            
        /* Forward UDP packet */
        if (sendto(sock, echoBuffer, recvMsgSize, 0, 
             (struct sockaddr *) &forwarding2Addr, sizeof(forwarding2Addr)) != recvMsgSize)
            DieWithError("sendto() sent a different number of bytes than expected");

    }
    /* NOT REACHED */
}
