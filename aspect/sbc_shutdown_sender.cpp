/*
Sends the shutdown command to a certain IP (command-line argument)
*/

#define PORT_SBC_SHUTDOWN 3789

#define PASSPHRASE "cS8XU:DpHq;dpCSA>wllge+gc9p2Xkjk;~a2OXahm0hFZDaXJ6C}hJ6cvB-WEp,"

#include <string.h>

#include "Packet.hpp"
#include "UDPSender.hpp"

int main(int argc, char *argv[])
{
    UDPSender out(argv[1], PORT_SBC_SHUTDOWN);

    Packet pkt((const uint8_t *)PASSPHRASE, strlen(PASSPHRASE));

    printf("Sending shutdown trigger to %s\n", argv[1]);

    out.send(&pkt);

    return 0;
}
