#define PORT_SBC_SHUTDOWN 3789

#define PASSPHRASE "cS8XU:DpHq;dpCSA>wllge+gc9p2Xkjk;~a2OXahm0hFZDaXJ6C}hJ6cvB-WEp,"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "UDPReceiver.hpp"

int main()
{
    FILE *in;

    UDPReceiver receiver(PORT_SBC_SHUTDOWN);
    receiver.init_connection();

    uint16_t packet_length;
    uint8_t *array;

    while(1)
    {
        packet_length = receiver.listen();
        array = new uint8_t[packet_length];
        receiver.get_packet(array);

        if((packet_length == strlen(PASSPHRASE)) && (strncmp((char *)array, PASSPHRASE, strlen(PASSPHRASE)) == 0)) {
            printf("Valid shutdown passphrase received\n");
            in = popen("shutdown -h now", "r");
            pclose(in);
        } else {
            printf("Invalid shutdown passphrase received: %s\n", array);
        }
        delete array;
    }

    return 0;
}
