#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) // Test for correct number of arguments
    printf("Parameter(s)", "<Server Address> <Echo Word> [<Server Port>]");
    
    char *servIP = argv[1]; // First arg: server IP address (dotted quad)
    char *echoString = argv[2]; // Second arg: string to echo
    
    // Third arg (optional): server port (numeric). 7 is well-known echo port
    in_port_t servPort = (argc == 4) ? atoi(argv[3]) : 7;
    
    // Create a reliable, stream socket using TCP
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0){ printf("socket() failed"); }
    
    // Construct the server address structure
    struct sockaddr_in servAddr; // Server address
    memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
    servAddr.sin_family = AF_INET; // IPv4 address family
    // Convert address
    int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if (rtnVal == 0){ printf("inet_pton() failed", "invalid address string"); }
    else if (rtnVal < 0){ printf("inet_pton() failed"); }
    servAddr.sin_port = htons(servPort); // Server port
    
    // Establish the connection to the echo server
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    { printf("connect() failed"); }
    
    size_t echoStringLen = strlen(echoString); // Determine input length 45
    // Send the string to the server
    ssize_t numBytes = send(sock, echoString, echoStringLen, 0);
    if (numBytes < 0){ printf("send() failed"); }
    else if (numBytes != echoStringLen)
        { printf("send()", "sent unexpected number of bytes"); }
    
    close(sock);
    exit(0);
}