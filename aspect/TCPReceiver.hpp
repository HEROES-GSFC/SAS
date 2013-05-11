#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */

#define MAX_PACKET_SIZE 500

class TCPReceiver {
protected:
    int sender_sock;                /* Socket of someone connecting to me */
    int my_sock;                /* My Socket */
    struct sockaddr_in myAddr;      /* Local address */
    struct sockaddr_in senderAddr;  /* Sender address */
    socklen_t senderAddrLen;

    char payload[MAX_PACKET_SIZE];  /* Buffer for echo string */
    unsigned short listeningPort;   /* The port to listen to */
    int numBytesRcvd;                /* Size of received message */
    unsigned int packet_size;
public:
    TCPReceiver( unsigned short port );
    ~TCPReceiver();
    void set_packet_size( int num_bytes );
    void get_packet( uint8_t *packet  );
    unsigned int handle_tcpclient( int client_socket );
    int accept_packet();
    void init_listen( void );
    void close_connection( void );
    void close_listen( void);
};
