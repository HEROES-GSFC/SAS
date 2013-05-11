#define TARGET_ID_SAS 0x30
#define TM_SAS_GENERIC 0x70
#define IP_LOOPBACK "127.0.0.1"
#define PORT_TM 2002

#include <unistd.h>
#include <iostream>

#include "UDPSender.hpp"
#include "Telemetry.hpp"
#include "types.hpp"

uint32_t count = 0;

int main()
{
    TelemetrySender telSender(IP_LOOPBACK, PORT_TM);

    while(1)    // run forever
    {
        usleep(500000);

        count++;
        TelemetryPacket tp(TM_SAS_GENERIC, TARGET_ID_SAS);

        tp.setSAS((count % 2)+1);
        tp << (uint32_t)count;
        tp << (uint16_t)0x0;
        tp << (uint16_t)0x0;

        //Housekeeping fields, two of them
        tp << Float2B((float)((count % 20)+20));
        tp << (uint16_t)((count % 30)+30);

        //Sun center and error
        tp << Pair3B(count*100 % 966, count*100 % 966);
        tp << Pair3B(3, 4);

        //Predicted Sun center and error
        tp << Pair3B(0, 0);
        tp << Pair3B(0, 0);

        //Number of limb crossings
        tp << (uint16_t)8;

        //Limb crossings (currently 8)
        for(uint8_t j = 0; j < 8; j++) {
            tp << Pair3B(0, 0);
        }

        //Number of fiducials
        tp << (uint16_t)6;

        //Fiduicals (currently 6)
        for(uint8_t k = 0; k < 6; k++) {
            tp << Pair3B(0, 0);
        }

        //Pixel to screen conversion
        tp << (float)-3000; //X intercept
        tp << (float)6; //X slope
        tp << (float)3000; //Y intercept
        tp << (float)-6; //Y slope

        //Image max and min
        tp << (uint8_t) 255; //max
        tp << (uint8_t) 0; //min

        //Tacking on the offset numbers intended for CTL
        tp << ((double)(count % 13)-7)/700+0.5; //azimuth offset
        tp << ((double)(count % 17)-9)/900-0.5; //elevation offset

        for(int i = 0; i < 8; i++) tp << (int8_t)0;

        std::cout << tp << std::endl;

        telSender.send( &tp );
    }

    return 0;
}

