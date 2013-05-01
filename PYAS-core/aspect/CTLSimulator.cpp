#include "Command.hpp"
#include "UDPReceiver.hpp"

#define CTL_CMD_PORT 2000 /* The default port to send on */

int main(int argc, char *argv[])
{
    UDPReceiver udpreceiver = UDPReceiver( CTL_CMD_PORT );

    udpreceiver.init_connection();
    while(1){
        //printf("Started to listen\n");
        uint16_t packet_length = udpreceiver.listen();
        //printf("Finished listening. Packet length was %u\n", packet_length);
        if( packet_length != 0){
            uint8_t *packet = new uint8_t[packet_length];
            udpreceiver.get_packet( packet );
        
            CommandPacket cp = CommandPacket( packet, packet_length);
            if (cp.valid()){
                std::cout << cp << std::endl;
            }
            delete packet;
        }
    }
    exit(0);
}
