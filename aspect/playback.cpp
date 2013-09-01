#define SPEED_FACTOR 2

#define PORT_TM 2003

#include <iostream>
#include <ctime>
#include <unistd.h>

#include "UDPSender.hpp"
#include "Telemetry.hpp"
#include "utilities.hpp"

int main(int argc, char *argv[])
{
    if(argc != 3) {
        std::cerr << "Calling sequence: playback <IP address> <filename>\n";
        return 1;
    }

    TelemetryPacketQueue tpq;
    tpq.filterSourceID(0x30);
    tpq.add_file(argv[2]);

    std::cout << "Playing back " << tpq.size() << " SAS telemetry packets at " << SPEED_FACTOR << "x speed\n";

    TelemetrySender telSender(argv[1], PORT_TM);

    timespec run_start, run_mark, run_diff;
    timespec tm_start, tm_mark, tm_diff;
    timespec wait;
    clock_gettime(CLOCK_MONOTONIC, &run_start);

    TelemetryPacket tp(NULL);

    tpq >> tp;

    tm_start.tv_sec = tp.getSeconds();
    tm_start.tv_nsec = tp.getNanoseconds();

    telSender.send(&tp);

    while(!tpq.empty()) {
        tpq >> tp;

        tm_mark.tv_sec = tp.getSeconds();
        tm_mark.tv_nsec = tp.getNanoseconds();
        tm_diff = TimespecDiff(tm_start, tm_mark);

        clock_gettime(CLOCK_MONOTONIC, &run_mark);
        run_diff = TimespecDiff(run_start, run_mark);
        if(SPEED_FACTOR != 1) {
            run_diff.tv_sec = run_diff.tv_sec*SPEED_FACTOR+
                              (int)(run_diff.tv_nsec*SPEED_FACTOR/1000000000);
            run_diff.tv_nsec = (run_diff.tv_nsec*SPEED_FACTOR) % 1000000000;
        }

        wait = TimespecDiff(run_diff, tm_diff);

        //std::cout << wait.tv_sec << "," << wait.tv_nsec << std::endl;
        //std::cerr << '.';

        if(wait.tv_sec >= 0) {
            sleep(wait.tv_sec);
            if(wait.tv_nsec > 0) usleep(wait.tv_nsec/1000);
        }

        telSender.send(&tp);
    }

    //std::cerr << std::endl;

    return 0;
}
