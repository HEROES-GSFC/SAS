#include <cstdio>
#include <iostream>
#include <cmath>

#include "Command.hpp"
#include "Telemetry.hpp"
#include "UDPSender.hpp"
#include "UDPReceiver.hpp"

int main()
{
    uint32_t exposure;
    uint16_t command_sequence_number = 0;
    float mean_image_max, stddev_image_max;
    uint8_t image_max = 0;

    uint16_t packet_length;
    uint8_t *array;

    CommandSender cs("192.168.2.21", 2000);
    TelemetryReceiver tr(2004);
    

    CommandPacket cp(NULL);

    /*
    for (int j = 1; j <= 1024; j++) {
        cp = CommandPacket(0x30, ++command_sequence_number);
        cp << (uint16_t)0x10FF << (uint16_t)0x3151 << (uint16_t)j;
        cs.send(&cp);
        usleep(100000);
    }
    */

    for (int preamp = -3; preamp <=6; preamp += 3) { // 4 steps
        
        for (int analog = 0; analog < 1024; analog += 50) { // 21 steps

            //Send commands to set preamp and analog gains
            cp = CommandPacket(0x30, ++command_sequence_number);

            cp << (uint16_t)0x10FF << (uint16_t)0x3191 << (uint16_t)preamp;
            cp << (uint16_t)0x10FF << (uint16_t)0x3181 << (uint16_t)analog;

            cs.send(&cp);

            mean_image_max = 0.;
            exposure = 128;
            do {

                cp = CommandPacket(0x30, ++command_sequence_number);
                cp << (uint16_t)0x10FF << (uint16_t)0x3151 << (uint16_t)exposure;

                cs.send(&cp);

                sleep(2);

                tr.init_connection();

                mean_image_max = 0.;
                stddev_image_max = 0.;
                int i = 0;
                do {

                    packet_length = tr.listen();
                    array = new uint8_t[packet_length];
                    tr.get_packet(array);

                    TelemetryPacket packet(array, packet_length);

                    if (packet_length >= 106) {
                        i++;
                        packet.readAtTo(104, image_max);
                        mean_image_max += image_max;
                        stddev_image_max += pow(image_max, 2);
                        //printf("%3d ", image_max);
                    }
                } while (i < 4);

                tr.close_connection();

//                printf("\n");
                mean_image_max /= i;
                stddev_image_max = sqrt((stddev_image_max - i*pow(mean_image_max, 2))/(i-1));

                if (mean_image_max >= 8) {
                    printf("%2d %4d %5d %6.2f %6.3f\n", preamp, analog, exposure, mean_image_max, stddev_image_max);
                }

                //This will speed up sampling
                if (mean_image_max < 2) exposure *= 2;
                if (mean_image_max < 4) exposure *= 2;
                if (mean_image_max < 8) exposure *= 2;
            } while ((mean_image_max < 128) && ((exposure *= 2) < 38000)); // stepping through as many exposure settings as possible
        }
    }

    return 0;
}
