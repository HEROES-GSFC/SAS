#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "lib_crc/lib_crc.h"

#include "TCPReceiver.hpp"

#define MAXPENDING 5; // Maximum outstanding connection requests
#define BUFSIZE 100;

TCPReceiver::TCPReceiver(void){
    listeningPort = 5010;
}

TCPReceiver::TCPReceiver( unsigned short port ){
    listeningPort = port;
}

unsigned int TCPReceiver::accept( void ){

    struct sockaddr_in senderAddr; // Client address
    // Set length of client address structure (in-out parameter)
    socklen_t senderAddrLen = sizeof(senderAddr);
    
    // Wait for a client to connect
    int sender_sock = accept(my_sock, (struct sockaddr *) &senderAddr, &senderAddrLen);
    if (clntSock < 0){ printf("Accept() failed\n"); }
    
    // clntSock is connected to a client!
    char sender_name[INET_ADDRSTRLEN]; // String to contain client address
    if (inet_ntop(AF_INET, &senderAddr.sin_addr.s_addr, sender_name, sizeof(sender_name)) != NULL)
    { printf("Handling client %s/%d\n", sender_name, ntohs(senderAddr.sin_port)); }
    else {puts("Unable to get client address");}
    
    HandleTCPClient( client_sock );
  
    close_connection();
}

void TCPReceiver::init_connection( void ){
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) < 0)
        printf("socket() failed");

    /* Construct local address structure */
    memset(&myAddr, 0, sizeof(myAddr));   /* Zero out structure */
    myAddr.sin_family = AF_INET;                /* Internet address family */
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    myAddr.sin_port = htons(listeningPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0)
        printf("bind() failed");

    // Mark the socket so it will listen for incoming connections
    if (listen(my_sock, MAXPENDING) < 0){
        printf("Listen() failed\n");}
   
}

void TCPReceiver::get_packet( uint8_t *packet ){
    memcpy( packet, payload, recvMsgSize);
}

void TCPReceiver::close_connection( void ){
    close( my_sock );
}

void TCPReceiver::handle_tcpclient( int client_socket ){
    char buffer[BUFSIZE]; // Buffer for incoming packet
    // Receive message from client
    ssize_t numBytesRcvd = recv(client_socket, buffer, BUFSIZE, 0);
    if (numBytesRcvd < 0){ printf("recv() failed"); }

    // Receive again until end of stream
    while (numBytesRcvd > 0) { // 0 indicates end of stream
        // See if there is more data to receive
        numBytesRcvd = recv(client_socket, buffer, BUFSIZE, 0);
        if (numBytesRcvd < 0){ printf("recv() failed"); }
    }
    close(client_socket); // Close client socket
}




