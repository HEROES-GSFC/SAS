#include "Telemetry.hpp"
#include "UDPReceiver.hpp"

#define SRV_TM_PORT 2003 /* The default port to send on */

int main(int argc, char *argv[])
{
    UDPReceiver udpreceiver = UDPReceiver( SRV_TM_PORT );

    udpreceiver.init_connection();
    while(1){
        //printf("Started to listen\n");
        uint16_t packet_length = udpreceiver.listen();
        //printf("Finished listening. Packet length was %u\n", packet_length);
        if( packet_length != 0){
            uint8_t *packet = new uint8_t[packet_length];
            udpreceiver.get_packet( packet );
        
            TelemetryPacket cp = TelemetryPacket( packet, packet_length);
            //if (cp.valid() && (cp.getSourceID() == 0x30)){
            if (cp.valid()){
                std::cout << cp << std::endl;
            }
            delete packet;
        }
    }
    exit(0);
}
