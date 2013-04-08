#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "lib_crc/lib_crc.h"

#include "TCPReceiver.hpp"

#define MAXPENDING 5 // Maximum outstanding connection requests
#define BUFSIZE 100
#define MAX_PACKET_SIZE 500

TCPReceiver::TCPReceiver(void){
    listeningPort = 5010;
    packet_size = 416;
}

TCPReceiver::TCPReceiver( unsigned short port ){
    listeningPort = port;
    packet_size = 416;
    
}

void TCPReceiver::set_packet_size( int num_bytes ){
    if ((num_bytes > 0) && (num_bytes < MAX_PACKET_SIZE)){
        packet_size = num_bytes;}
}

int TCPReceiver::accept_packet( void ){
    memset(&payload, 0, sizeof(payload));
    
    // Wait for a client to connect
    sender_sock = accept(my_sock, (struct sockaddr *) &senderAddr, &senderAddrLen);
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
    else {printf("Unable to get client address\n");}
    
    numBytesRcvd = 0;
    
    // Receive message from client
    ssize_t bytes = recv(client_socket, payload, BUFSIZE, 0);
    printf("just received %ld\n", bytes);

    if (bytes < 0){ 
        printf("recv() failed\n");
        return numBytesRcvd;
    }
    numBytesRcvd += bytes;
    // Receive again until end of stream
    //printf("packet size %ld\n", packet_size);

    int no_bytes_count;
    no_bytes_count = 0;

    //while (numBytesRcvd < packet_size) { // 0 indicates end of stream
    // See if there is more data to receive
    //    bytes = recv(client_socket, payload, BUFSIZE, 0);
    // printf("received %i\n, %i\n", bytes, no_bytes_count);
    //if (bytes == 0){ no_bytes_count++; if (no_bytes_count > 5){ close_connection(); break; } }
    //    if (bytes < 0){ printf("recv() failed\n"); } else { numBytesRcvd += bytes; }
    // }
    //close(client_socket); // Close client socket
    return numBytesRcvd;
}

void TCPReceiver::init_listen( void ){
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
    senderAddrLen = sizeof(senderAddr);

    /* Bind to the local address */
    if (bind(my_sock, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0)
        printf("bind() failed\n");

    // Mark the socket so it will listen for incoming connections
    if (listen(my_sock, MAXPENDING) < 0){
        printf("Listen() failed\n");
    }
   
}

void TCPReceiver::get_packet( uint8_t *packet ){
    if( numBytesRcvd >= 0 ){ memcpy( packet, payload, numBytesRcvd); }
}

void TCPReceiver::close_connection( void ){
    close( sender_sock );
}

void TCPReceiver::close_listen( void ){
    close( my_sock );
}




