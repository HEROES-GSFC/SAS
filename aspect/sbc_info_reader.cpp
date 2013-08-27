#define PORT_SBC_INFO 3456 //

#include <stdlib.h>
#include <stdio.h>

#include "UDPReceiver.hpp"
#include "Packet.hpp"

int8_t sbc_temperature;
int8_t i2c_temperatures[8];
float sbc_v105, sbc_v25, sbc_v33, sbc_v50, sbc_v120;
float ntp_drift, ntp_offset_ms, ntp_stability;

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
        for (int i=0; i<8; i++) packet >> i2c_temperatures[i];
        packet >> ntp_drift;
        packet >> ntp_offset_ms;
        packet >> ntp_stability;

        printf("CPU temperature: %d deg C    Voltages: %5.3f V | %5.3f V | %5.3f V | %5.3f V | %5.2f V\n", sbc_temperature, sbc_v105, sbc_v25, sbc_v33, sbc_v50, sbc_v120);
        printf("    I2C temperatures (deg C): ");
        for (int i=0; i<7; i++) printf("%d, ", i2c_temperatures[i]);
        printf("%d\n", i2c_temperatures[7]);
        printf("NTP info: %f (offset ms), %f (stability), %f (drift)\n", ntp_offset_ms, ntp_stability, ntp_drift);

        delete array;
    }

    return 0;
}
