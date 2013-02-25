#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "lib_crc/lib_crc.h"

#include "TCPReceiver.hpp"

#define MAXPENDING 5 // Maximum outstanding connection requests
#define BUFSIZE 100

TCPReceiver::TCPReceiver(void){
    listeningPort = 5010;
}

TCPReceiver::TCPReceiver( unsigned short port ){
    listeningPort = port;
}

int TCPReceiver::accept_packet( void ){
    memset(&payload, 0, sizeof(payload));
    
    // Wait for a client to connect
    int sender_sock = accept(my_sock, (struct sockaddr *) &senderAddr, &senderAddrLen);
    if (sender_sock < 0){ 
        printf("Accept() failed\n");
        return -1;
    }
    return sender_sock;
}

unsigned int TCPReceiver::handle_tcpclient( int client_socket ){
    
    // clntSock is connected to a client!
    char sender_name[INET_ADDRSTRLEN]; // String to contain client address
    if (inet_ntop(AF_INET, &senderAddr.sin_addr.s_addr, sender_name, sizeof(sender_name)) != NULL)
    { printf("Handling client %s/%d\n", sender_name, ntohs(senderAddr.sin_port)); }
    else {puts("Unable to get client address\n");}
    
    numBytesRcvd = 0;
    
    // Receive message from client
    ssize_t bytes = recv(client_socket, payload, BUFSIZE, 0);
    //printf("received %i\n", bytes);

    if (bytes < 0){ 
        printf("recv() failed\n");
        return numBytesRcvd;
    }
    numBytesRcvd += bytes;
    // Receive again until end of stream
    while (bytes > 0) { // 0 indicates end of stream
        // See if there is more data to receive
        bytes = recv(client_socket, payload, BUFSIZE, 0);
        //printf("received %i\n", bytes);
        numBytesRcvd += bytes;
        if (numBytesRcvd < 0){ printf("recv() failed\n"); }
    }
    //close(client_socket); // Close client socket
    return numBytesRcvd;
}

void TCPReceiver::init_connection( void ){
    /* Create socket for sending/receiving datagrams */
    if ((my_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        printf("socket() failed\n");

    /* Construct local address structure */
    struct sockaddr_in myAddr;
    memset(&myAddr, 0, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(listeningPort);

    // Set length of client address structure (in-out parameter)
    socklen_t senderAddrLen = sizeof(senderAddr);

    /* Bind to the local address */
    if (bind(my_sock, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0)
        printf("bind() failed\n");

    // Mark the socket so it will listen for incoming connections
    if (listen(my_sock, MAXPENDING) < 0){
        printf("Listen() failed\n");
    }
   
}

void TCPReceiver::get_packet( uint8_t *packet ){
    if( numBytesRcvd > 0 ){ memcpy( packet, payload, numBytesRcvd); }
}

void TCPReceiver::close_connection( void ){
    close( my_sock );
}




