#include "Command.hpp"
#include "UDPSender.hpp"

#define CTL_CMD_PORT 2001 /* The default port to send on */

int main(int argc, char *argv[])
{
	int onoff = atoi(argv[2]);
	char *servIP;                    /* IP address of server */
    unsigned short port;     /* Echo server port */

	uint16_t command;
	if (onoff == 1){ command = 0x1000; }
	if (onoff == 0){ command = 0x1001; }
    servIP = argv[1];           /* First arg: server IP address (dotted quad) */

	if (argc == 4)
        port = atoi(argv[3]);  /* Use given port, if any */
    else
        port = CTL_CMD_PORT;  /* 7 is the well-known port for the echo service */

	CommandSender cmdsender = CommandSender( servIP, port );

	CommandPacket cp(0x30, 1); //target ID and sequence number
	cp << (uint16_t) command;
	
	printf("Sending to %s on port %u, message is %u\n", servIP, port, command);
	
    cmdsender.send( &cp );
    cmdsender.close_connection();
	exit(0);
}
