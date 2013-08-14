#include "pmm/DiamondPMM.h"
#include <iostream>

int main(int argc, char* argv[])
{
    DiamondPMM relayBoard;
    DiamondPMMStateRelay relays(relayBoard);

    int value;
    bool set;
    
    for (int k = 0; k < 2; k++)
    {
        relays.getPort(k, value);
        std::cout << "Port " << k << " reads " << std::hex << value << std::endl;
    }
    relays.setPort(0, 0xAB);
    relays.setPort(1, 0xCD);

    relays.setPort(0, 0x00);
    relays.setPort(1, 0x00);

    relays.setRelay(1, 1);
    sleep(10);
    relays.setRelay(2, 1);
    sleep(10);
    relays.setRelay(1, 0);
    sleep(10);

    sleep(1);
    for (int k = 0; k < 2; k++)
    {
        relays.getPort(k, value);
        std::cout << "Port " << k << " reads " << std::hex << value << std::endl;
    }

    for (int k = 0; k < 2; k++)
    {
        relays.setPort(k, 0xFF);
    }

    sleep(1);    
    for (int k = 0; k < 2; k++)
    {
        relays.getPort(k,value);
        std::cout << "Port " << k << " reads " << std::hex << value << std::endl;
    }
    for (int k = 0; k < 16; k++)
    {
        relays.getRelay(k, set);
        std::cout << "Port " << k << " is " << (set ? "ON" : "OFF") << std::endl;
        relays.setRelay(k, 1);
        std::cout << "Port " << k << " set ON" << std::endl;
        relays.getRelay(k, set);
        std::cout << "Port " << k << " is " << (set ? "ON" : "OFF") << std::endl;
        sleep(1);
    }

    for (int k = 0; k < 16; k++)
    {
        relays.setRelay(k,0);
        relays.getRelay(k,set);
        std::cout << "Port " << k << " is " << (set ? "ON" : "OFF") << std::endl;
        sleep(1);
    }
}
