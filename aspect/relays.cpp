#include "pmm/DiamondPMM.h"
#include <iostream>

int main(int argc, char* argv[])
{
    DiamondPMM relayBoard;
    DiamondPMMStateRelay relays(relayBoard);

    int value;
    
    for (int k = 0; k < 2; k++)
    {
        relays.getPort(k, value);
        std::cout << "Port " << k << " reads " << std::hex << value << std::endl;
    }
    relays.setPort(0, 0x00);
    relays.setPort(1, 0xB0);

    sleep(1);
    for (int k = 0; k < 2; k++)
    {
        relays.getPort(k, value);
        std::cout << "Port " << k << " reads " << std::hex << value << std::endl;
    }

    for (int k = 0; k < 2; k++)
    {
        relays.setPort(k, 0x00);
    }

    sleep(1);    
    for (int k = 0; k < 2; k++)
    {
        relays.getPort(k,value);
        std::cout << "Port " << k << " reads " << std::hex << value << std::endl;
  }
	for (int k = 0; k < 16; k++)
{relays.setRelay(k, 1);
sleep(1);
}

for (int k = 0; k < 16; k++)
{
relays.setRelay(k,0);
sleep(1);
}
}
