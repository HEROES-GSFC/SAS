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

//#define PAYLOAD_SIZE 200     /* Longest string to echo */
//#define DEFAULT_PORT 5000 /* The default port to send on */
#define MAX_PARAMS 10

class Command_Parser {
    private:
        int sock;                       /* Socket descriptor */
        struct sockaddr_in localAddr; /* Echo server address */
        struct sockaddr_in remoteAddr;     /* Source address of echo */
        unsigned short localPort;     /* Echo server port */
        unsigned int remote_length;           /* In-out of address size for recvfrom() */
        
        uint8_t buffer[MAX_PAYLOAD];
        int packet_length, payload_length;
        
        int seq_number, seq_num_prev
        uint16_t command_key;
        uint16_t parameters[MAX_PARAMS];
        int num_params;
        bool format_valid, checksum_valid, first_packet, lost_packets, connection_active, parse_complete;
        
        void parse_packet(uint8_t *);
        void do_checksum( void );
        void parse_packet(void);
        
    public:
        CommandParser(void);
        
        bool test_checksum( void );
       	int init(void);
       	int close(void);
       	int TestGetOneCmd(void);
        
        bool packet_status(void);
        bool checksum_status(void);
        bool packets_lost(void);
      	bool connection_status(void);
      	bool parse_status(void);
      	uint16_t get_command_key();
      	(uint16_t *) get_params();
      	int get_num_params();
        
};
