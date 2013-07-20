#include "ir104/DiamondIr104.h"
#include <iostream>

int main(int argc, char* argv[])
{
    DiamondIr104 relayBoard;
    DiamondIr104StateRelay relays(relayBoard);

    int relayID;
    bool value;
    for (int k = 0; k < 16; k++)
    	{
        relayID = k;

        relays.getRelay(relayID, value);
        std::cout << "Relay " << relayID << " is ";
        std::cout << value << std::endl;

	relays.setRelay(relayID, 0);

	relays.getRelay(relayID, value);
	std::cout << "Relay " << relayID << " is now ";
	std::cout << value << std::endl;
}
}
