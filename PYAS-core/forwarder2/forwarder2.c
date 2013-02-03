#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define ECHOMAX 1024     /* Longest string to echo */

/* This program listens for packet on UDP port 5000 and forwards them to forwardingIP */

void DieWithError(char *errorMessage)
{
    printf("%s", errorMessage);
}

int main()
{
    int sock;                        /* Socket */
    struct sockaddr_in myAddr; /* Local address */
    struct sockaddr_in senderAddr; /* Sender address */
    struct sockaddr_in forwardingAddr; /* Forwarding address */
    
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short listeningPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */

    // listen to port 5000 for UDP packets and forward them to 192.168.2.2
    char *forwadingIP = "192.168.2.2";     /* IP address to forward to */
	listeningPort = 5000;

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&myAddr, 0, sizeof(myAddr));   /* Zero out structure */
    myAddr.sin_family = AF_INET;                /* Internet address family */
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    myAddr.sin_port = htons(listeningPort);      /* Local port */

    /* Construct the forwarding address structure */
    memset(&forwardingAddr, 0, sizeof(myAddr));    /* Zero out structure */
    forwardingAddr.sin_family = AF_INET;                 /* Internet addr family */
    forwardingAddr.sin_addr.s_addr = inet_addr(forwadingIP);  /* Server IP address */
    forwardingAddr.sin_port   = htons(listeningPort);     /* Port to forward on is same as listening port*/

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

        /* Forward UDP packet */
        if (sendto(sock, echoBuffer, recvMsgSize, 0, 
             (struct sockaddr *) &forwardingAddr, sizeof(forwardingAddr)) != recvMsgSize)
            DieWithError("sendto() sent a different number of bytes than expected");
    }
    /* NOT REACHED */
}
