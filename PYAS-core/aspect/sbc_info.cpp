#include <iostream>

#include <unistd.h>
#include <sys/io.h>

#include "Packet.hpp"
#include "UDPSender.hpp"

#define EC_INDEX 0x6f0
#define EC_DATA  0x6f1

int main()
{
    signed char ambtemp, start;
    unsigned long p2v5, p1v05, p3v3, p5v0, p12v0;
    UDPSender sender("127.0.0.1", 3456);

    if (iopl(3)) {
        std::cerr << "Failed to get I/O access permissions!\n";
        return 1;
    }

    outb(0x40, EC_INDEX);  //access to start/stop register
    start = 0x01 | inb(EC_DATA);
    outb(start, EC_DATA);  //activate monitor mode

    while (1) {
        outb(0x26, EC_INDEX);
        ambtemp = inb(EC_DATA);

        outb(0x21, EC_INDEX);
        p1v05 = (inb(EC_DATA)*2000)/255;

        outb(0x20, EC_INDEX);
        p2v5 = (inb(EC_DATA)*3320)/255;

        outb(0x22, EC_INDEX);
        p3v3 = (inb(EC_DATA)*4380)/255;

        outb(0x23, EC_INDEX);
        p5v0 = (inb(EC_DATA)*6640)/255;

        outb(0x24, EC_INDEX);
        p12v0 = (inb(EC_DATA)*1600)/255;

        //std::cout << (int)ambtemp << " " << (float)p1v05/1000. << " " << (float)p2v5/1000. << " " << (float)p3v3/1000. << " " << (float)p5v0/1000. << " " << (float)p12v0/100. << std::endl;

        Packet packet;
        packet << (int8_t)ambtemp << (float)(p1v05/1000.) << (float)(p2v5/1000.) << (float)(p3v3/1000.) << (float)(p5v0/1000.) << (float)(p12v0/100.);

        //std::cout << packet << std::endl;
        sender.send(&packet);

        sleep(1);
    }
    return 0;
}
