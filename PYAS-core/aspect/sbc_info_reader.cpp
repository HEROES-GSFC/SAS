#define PORT_SBC_INFO 3456 //

#include <stdlib.h>
#include <stdio.h>

#include "UDPReceiver.hpp"
#include "Packet.hpp"

int8_t sbc_temperature;
float sbc_v105, sbc_v25, sbc_v33, sbc_v50, sbc_v120;

int main()
{
    UDPReceiver receiver(PORT_SBC_INFO);
    receiver.init_connection();

    uint16_t packet_length;
    uint8_t *array;

    while(1)
    {
        //This call will block forever if the service is not running
        packet_length = receiver.listen();
        array = new uint8_t[packet_length];
        receiver.get_packet(array);

        Packet packet( array, packet_length );
        packet >> sbc_temperature >> sbc_v105 >> sbc_v25 >> sbc_v33 >> sbc_v50 >> sbc_v120;

        printf("CPU temperature: %d deg C    Voltages: %5.3f V | %5.3f V | %5.3f V | %5.3f V | %5.2f V\n", sbc_temperature, sbc_v105, sbc_v25, sbc_v33, sbc_v50, sbc_v120);
    }

    return 0;
}
