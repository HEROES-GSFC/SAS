#include "pmm/DiamondPMM.h"
#include <iostream>

int main(int argc, char* argv[])
{
    DiamondPMM relayBoard;
    DiamondPMMStateRelay relays(relayBoard);

    int relayID;
    int value;
    
    for (int k = 0; k < 2; k++)
    {
        relays.getPort(k, value);
        std::cout << "Port " << k << " reads " << value << std::endl;
    }
    relays.setPort(0, 0xAA);
    relays.setPort(1, 0x00);

    sleep(10);
    for (int k = 0; k < 2; k++)
    {
        relays.getPort(k, value);
        std::cout << "Port " << k << " reads " << value << std::endl;
    }

    for (int k = 0; k < 8; k++)
    {
        relays.setRelay(k, 0);
    }

    sleep(10);    
    for (int k = 0; k < 2; k++)
    {
        relays.getPort(k,value);
        std::cout << "Port " << k << " reads " << value << std::endl;
    }
}
