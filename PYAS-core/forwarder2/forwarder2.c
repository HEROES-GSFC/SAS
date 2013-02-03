#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <pthread.h>    /* for multithreading */

#define ECHOMAX 1024     /* Longest string to echo */

void printWithError(char *errorMessage)
{
    printf("%s", errorMessage);
}

void *listenandForward1(void *arg)
{
    // This thread listens on port 5000 for packets from SAS 2
    // and forwards them to the correct place
    int sock;                           /* Socket */
    struct sockaddr_in myAddr;          /* Local address */
    struct sockaddr_in senderAddr;      /* Sender address */
    struct sockaddr_in forwarding1Addr; /* Forwarding address 1 (myself on port 5010)*/
    struct sockaddr_in forwarding2Addr; /* Forwarding address 1 (other IP on port 5000)*/
    
    unsigned int cliAddrLen;            /* Length of incoming message */
    char echoBuffer[ECHOMAX];           /* Buffer for echo string */
    unsigned short listeningPort;       /* The port to listen to */
    unsigned short forwardingPort;     /* port 1 to forward to */
    int recvMsgSize;                    /* Size of received message */

    // listen to port 5000 for UDP packets and forward them to 192.168.2.2
    // char *forwadingIP = "192.168.2.1"; /* IP address to forward to */
        
    listeningPort = 5000;

    /* Create socket for receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        printWithError("socket() failed");

    /* Construct local address structure */
    memset(&myAddr, 0, sizeof(myAddr));         /* Zero out structure */
    myAddr.sin_family = AF_INET;                /* Internet address family */
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    myAddr.sin_port = htons(listeningPort);     /* Local port */

   /* Construct one forwarding address structure */
    //memset(&forwarding1Addr, 0, sizeof(myAddr));    /* Zero out structure */
    //forwarding1Addr.sin_family = AF_INET;                 /* Internet addr family */
    //forwarding1Addr.sin_addr.s_addr = inet_addr(forwadingIP1);  /* Server IP address */
    //forwarding1Addr.sin_port   = htons(forwardingPort1);     /* Port to forward on is same as listening port*/

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0)
        printWithError("bind() failed");
  
	while(1)    // run forever
	{
         /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(senderAddr);
    
        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, echoBuffer, sizeof(echoBuffer), 0,
            (struct sockaddr *) &senderAddr, &cliAddrLen)) < 0)
            printWithError("recvfrom() failed");
        printf("received packet on port %i from %s\n", listeningPort, inet_ntoa(senderAddr.sin_addr));
	}

    /* NEVER REACHED */
	return NULL;
}

void *listenandForward2(void *arg)
{
    // and forwards them to the correct place
    int sock;                           /* Socket */
    struct sockaddr_in myAddr;          /* Local address */
    struct sockaddr_in senderAddr;      /* Sender address */
    struct sockaddr_in forwarding1Addr; /* Forwarding address 1 (myself on port 5010)*/
    struct sockaddr_in forwarding2Addr; /* Forwarding address 1 (other IP on port 5000)*/
    
    unsigned int cliAddrLen;            /* Length of incoming message */
    char echoBuffer[ECHOMAX];           /* Buffer for echo string */
    unsigned short listeningPort;       /* The port to listen to */
    unsigned short forwardingPort;     /* port 1 to forward to */
    int recvMsgSize;                    /* Size of received message */

    // listen to port 5000 for UDP packets and forward them to 192.168.2.2
    // char *forwadingIP = "192.168.2.1"; /* IP address to forward to */
        
    listeningPort = 5002;

    /* Create socket for receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        printWithError("socket() failed");

    /* Construct local address structure */
    memset(&myAddr, 0, sizeof(myAddr));         /* Zero out structure */
    myAddr.sin_family = AF_INET;                /* Internet address family */
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    myAddr.sin_port = htons(listeningPort);     /* Local port */

   /* Construct one forwarding address structure */
    //memset(&forwarding1Addr, 0, sizeof(myAddr));    /* Zero out structure */
    //forwarding1Addr.sin_family = AF_INET;                 /* Internet addr family */
    //forwarding1Addr.sin_addr.s_addr = inet_addr(forwadingIP1);  /* Server IP address */
    //forwarding1Addr.sin_port   = htons(forwardingPort1);     /* Port to forward on is same as listening port*/

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0)
        printWithError("bind() failed");
  
	while(1)    // run forever
	{
         /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(senderAddr);
    
        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, echoBuffer, sizeof(echoBuffer), 0,
            (struct sockaddr *) &senderAddr, &cliAddrLen)) < 0)
            printWithError("recvfrom() failed");
        printf("received packet on port %i from %s\n", listeningPort, inet_ntoa(senderAddr.sin_addr));
	}

    /* NEVER REACHED */
	return NULL;
}

int main(void)
{
	pthread_t pth1;	// this is our thread identifier
	pthread_t pth2;	// this is our thread identifier

	/* Create worker threads */
	
	pthread_create(&pth1,NULL,listenandForward1,"processing...");   // listen on port 5000
	pthread_create(&pth2,NULL,listenandForward2,"processing...");   // listen on port 5002

	while(1){} /* never stop */

    /* NEVER REACHED */
	return 0;
}

