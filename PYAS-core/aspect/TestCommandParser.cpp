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
#include "Command_Parser.hpp"



#define MAX_PAYLOAD 4096     /* Longest string to echo */
#define MAX_PARAMS 10
#define DEFAULT_PORT 7000 /* The default port to send on */
#define SAS_IP_ADDR "10.1.49.140"


uint16_t do_checksum(char *, int);

int main()
{
	Command_Parser CmdService;
	
	CmdService.Init();
    CmdService.set_verbose(true);
	CmdService.ListenOnce();
    CmdService.ListenOnce();
    CmdService.ListenOnce();
    CmdService.Close();
	
    printf("Commands in queue:\n");
    for (i = 0;i < 3;i++)
    {
	    printf("Key:               x%4.4x\n",CmdService.get_key());
	    printf("Sequence Number:   x%4.4x\n",CmdService.get_seqnum());
	    printf("Num_Params:        x%4.4x\n",CmdService.get_num_params());
	    for(n = 0;n < get_num_params;n++)
	    {
		    printf("Param #%3d:        x%4.4x\n",n,CmdService.get_params(n));
	    }
	    printf("\n");
	    CmdService.dispatch();
    }
    if CmdService.empty()
    {
	    printf("Command queue empty.\n");
    }
    else
    {
	    printf("More commands in queue...\n");
    }
    
	return 0;
}
		