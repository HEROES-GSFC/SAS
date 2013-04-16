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

struct Command
{
	int seq_number, num_params;
    uint16_t key;
    uint16_t parameters[MAX_PARAMS];
    bool format_valid, checksum_valid;
    Command * next;
};    

class Command_Parser {
    private:
        int sock;                       /* Socket descriptor */
        struct sockaddr_in localAddr; /* Echo server address */
        struct sockaddr_in remoteAddr;     /* Source address of echo */
        unsigned short localPort;     /* Echo server port */
        unsigned int remote_length;           /* In-out of address size for recvfrom() */
        
        Command * first;
        int listsize;
        bool connection_active, verbose;
        
        uint8_t buffer[MAX_PAYLOAD];
        
        void parse_packet(uint8_t *);
        void do_checksum( void );
        void parse_packet(void);
        void insert(command *);
        
    public:
        CommandParser(void);
        
        bool test_checksum( void );
       	int init(void);
       	int close(void);
       	void listen(void);
       	void listenOnce(void);
        
        bool connection_status();
       	bool empty(void);
        uint16_t get_key();
      	int get_num_params();
        uint16_t get_params(int);
      	uint16_t get_seqnum();
      	bool set_verbose(bool);
        
};    