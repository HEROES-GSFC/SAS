#define TARGET_ID_SAS 0x30
#define TM_SAS_GENERIC 0x70
#define IP_LOOPBACK "127.0.0.1"
#define PORT_TM 2002

#include <unistd.h>
#include <iostream>
#include <stdlib.h>

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

        //Sync word
        tp.setSAS((count % 2)+1);

        //Frame counter
        tp << (uint32_t)count/2;

        //Status bitfield
        tp << (uint8_t)(count % 2 == 0 ? 0xFA : 0xD5);

        //Command echo
        tp << (uint16_t)0x1234;

        float value;

        //Housekeeping field 0 (SBC and I2C temperatures)
        switch (count/2 % 8) {
            case 7:
                tp << (uint16_t)true;
                break;
            default:
                value = ((count % 30)+30) * 10;
                tp << Float2B((float)value);
        }

        //Housekeeping field 1 (camera temperature, SBC voltages, and flag)
        switch (count/2 % 8) {
            case 0:
            case 1:
                value = ((count % 20)+20+(count/2 % 8)) * 10;
                tp << Float2B((float)value);
                break;
            case 2:
            	value = (1.05 + 0.6* (rand() / (float)RAND_MAX - 0.5)) * 500;
            	tp << Float2B((float)value);
            	break;
            case 3:
            	value = (2.5 + 0.6* (rand() / (float)RAND_MAX  - 0.5)) * 500;
            	tp << Float2B((float)value);
            	break;
            case 4:
            	value = (3.3 + 1.0* (rand() / (float)RAND_MAX - 0.5)) * 500;
            	tp << Float2B((float)value);
            	break;
            case 5:
            	value = (5.0 + 3.2* (rand() / (float)RAND_MAX - 0.5)) * 500;
            	tp << Float2B((float)value);
            	break;
            case 6:
                value = (12.0 + 5.2*(rand() / (float)RAND_MAX - 0.5)) * 500;
            	tp << Float2B((float)value);
                break;
            case 7:
                tp << (uint16_t)true;
                break;
        }

        //Sun center and error
        tp << Pair3B(count*100 % 966, count*100 % 966);
        tp << Pair3B(3, 4);

        //Limb crossings (currently 8)
        for(uint8_t j = 0; j < 8; j++) {
            tp << Pair3B(0, 0);
        }

        //Number of fiducials
        tp << (uint8_t)6;

        //Number of limb crossings
        tp << (uint8_t)8;

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

        //Tacking on the offset numbers intended for CTL
        tp << (float)(((float)(count % 13)-7)/700+0.5); //azimuth offset
        tp << (float)(((float)(count % 17)-9)/900-0.5); //elevation offset

        //Fiduical IDs (currently 6)
        //7 is added to the ID number (Which ranges from -7 to 7)
        for(uint8_t j = 0; j < 6; j++) {
            uint8_t temp = 0;
            bitwrite(&temp, 0, 4, 1+7);
            bitwrite(&temp, 4, 4, 1+7);
            tp << (uint8_t)temp;
        }

        tp.setTimeAndFinish();

        std::cout << tp << std::endl;
        std::cout << "Packet size: " << tp.getReadIndex()+tp.remainingBytes() << std::endl;

        telSender.send( &tp );
    }

    return 0;
}

