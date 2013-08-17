#include "pmm/DiamondPMM.h"
#include <iostream>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    DiamondPMM relayBoard;
    DiamondPMMStateRelay relays(relayBoard);

    int value;
    bool set;
    
    relays.setRelay(atoi(argv[1]), 1);
    sleep(10);
    relays.setRelay(atoi(argv[1]), 0);
    //sleep(10);

    //relays.setRelay(3, 0);
    //sleep(10);
   return 0;
}
