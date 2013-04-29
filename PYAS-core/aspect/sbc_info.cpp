#include <iostream>

#include <unistd.h>
#include <sys/io.h>

#include "Packet.hpp"
#include "UDPSender.hpp"

#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include "smbus/smbus.h"

#define EC_INDEX 0x6f0
#define EC_DATA  0x6f1

#define I2C_ADDR 0x48           //base address for I2C temp readers

int main()
{
    signed char ambtemp, start;
    unsigned long p2v5, p1v05, p3v3, p5v0, p12v0;
    UDPSender sender("127.0.0.1", 3456);

    int file, i;
    int8_t temp[8];
    uint8_t curaddr;
    int32_t ctl_res, rd_res;

    if (iopl(3)) {
        std::cerr << "Failed to get I/O access permissions!\n";
        return 1;
    }

    if ((file = open("/dev/i2c-0", O_RDWR)) < 0) {
        std::cerr << "Failed to access /dev/i2c-0!\n";
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

	for (i=0;i<=7;i++)
        {
            curaddr = I2C_ADDR + i;
            ctl_res = ioctl(file, I2C_SLAVE, curaddr);
            rd_res = i2c_smbus_read_byte_data(file, 0x00);
            if ((ctl_res >= 0) && (rd_res >= 0))
            {
                temp[i] = (int8_t) rd_res;
            }
            else
            {
                temp[i] = 0x7f;    //return +127C when no response is received
            }
        }

        //std::cout << (int)ambtemp << " " << (float)p1v05/1000. << " " << (float)p2v5/1000. << " " << (float)p3v3/1000. << " " << (float)p5v0/1000. << " " << (float)p12v0/100. << std::endl;

        Packet packet;
        packet << (int8_t)ambtemp << (float)(p1v05/1000.) << (float)(p2v5/1000.) << (float)(p3v3/1000.) << (float)(p5v0/1000.) << (float)(p12v0/100.);
        for(i=0;i<=7;i++) { packet<<(int8_t)temp[i];}

        //std::cout << packet << std::endl;
        sender.send(&packet);

        sleep(1);
    }

    close(file);
    return 0;
}
