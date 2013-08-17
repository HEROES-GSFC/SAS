/*
Send command to relay-control service
Arguments:
    IP
    relay number (0-15)
    1 or 0 (on or off)
*/
#define PORT_RELAY_CONTROL 4567

#define PASSPHRASE_RELAY_CONTROL "tAzh0Sh?$:dGo4t8j$8ceh^,d;2#ob}j_VEHXtWrI_AL*5C3l/edTMoO2Q8FY&K"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "Packet.hpp"
#include "UDPSender.hpp"

int main(int argc, char *argv[])
{
    UDPSender out(argv[1], PORT_RELAY_CONTROL);

    Packet pkt((const uint8_t *)PASSPHRASE_RELAY_CONTROL, strlen(PASSPHRASE_RELAY_CONTROL));
    pkt << (uint8_t)atoi(argv[2]) << (uint8_t)atoi(argv[3]);

    printf("Sending relay-control command to %s\n", argv[1]);

    out.send(&pkt);

    return 0;
}
