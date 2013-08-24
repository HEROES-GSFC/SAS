/*
Service to handle relay control

Currently no ability to report relay bank
e.g., relays.getRelay(i, state);
*/

#define PORT_RELAY_CONTROL 4567

#define PASSPHRASE "tAzh0Sh?$:dGo4t8j$8ceh^,d;2#ob}j_VEHXtWrI_AL*5C3l/edTMoO2Q8FY&K"

#include <stdio.h>
#include <string.h>

#include "UDPReceiver.hpp"
#include "pmm/DiamondPMM.h"

int main()
{
    UDPReceiver receiver(PORT_RELAY_CONTROL);
    receiver.init_connection();

    DiamondPMM relayBoard;
    DiamondPMMStateRelay relays(relayBoard);

    uint16_t packet_length;
    uint8_t *array;

    //Energizing the camera-power relays by default
    relays.setRelay(0, true);
    relays.setRelay(1, true);

    while(1)
    {
        packet_length = receiver.listen();
        array = new uint8_t[packet_length];
        receiver.get_packet(array);

        if((packet_length == strlen(PASSPHRASE))+2 && (strncmp((char *)array, PASSPHRASE, strlen(PASSPHRASE)) == 0)) {
            uint8_t relay_number = array[packet_length-2];
            bool on_if_true = array[packet_length-1];
            printf("Valid relay-control passphrase received: %d %s\n", relay_number, (on_if_true ? "ON" : "OFF"));
            relays.setRelay(relay_number, on_if_true);
        } else {
            printf("Invalid relay-control passphrase received: %s\n", array);
        }
        delete array;
    }

    return 0;
}
