/*
 * DiamondPMM.cpp
 *
 *  Created on: Jul 22, 2013
 *      Author: acramer
 *  Copied heavily from Kurt Dietz IR104 driver
 */

#include "DiamondPMM.h"

//#include <ErrorCodes.h>
#include <iostream>
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// DiamondPMM class definitions.

// Set up the default settings.
static DSCCB defaultDsccb = {0, 0, 0x240, 5};

DiamondPMM::DiamondPMM() : DiamondBoard( DSC_PMM, &defaultDsccb )
{
}

///////////////////////////////////////////////////////////////////////////////
// DiamondPMMStateRelay class definitions.

DiamondPMMStateRelay::DiamondPMMStateRelay( DiamondPMM &pmm ) :
StateRelay( 2, 8, NULL )
{
    // Set the device pointer.
    this->dev = &pmm;
    
    int val;
    // Read in current state of PMM board 
    for ( int port = 0; port < numPort; port++)
        getPort(port, val);

}

int DiamondPMMStateRelay::setPort( int port, int val )
{
//  cout << "DiamondPMMDio::getPort: verifying port " << port << endl;

    if ( !verifyPort( port ) )
        return STRLYERR_PORT;

    int rval = 0;

    // Only set bits which are not currently 
    val = (bits[port] & (~mask[port])) | (mask[port] & val);
    bits[port] = val;
    rval = ::dscSetRelayMulti(dev->handle, port, val); 
    

    return rval;
}

int DiamondPMMStateRelay::setRelay( int relayID, bool relayOn )
{
    if ( !verifyRelay( relayID ) )
        return STRLYERR_RELAY;

    int port = relayID / portWidth;
    int bit = relayID % portWidth;

    int rval = 0;

    if (port < numPort)
    {
        // Make sure the bit is masked in.
        if ( ((1 << bit) & mask[port]) > 0 )
        {
            bits[port] = (bits[port] & (~(1 << bit))) | 
                         (bits[port] & (relayOn << bit));
            
            rval = ::dscSetRelay( dev->handle, relayID, relayOn);
        }
    }

    return rval;
}

int DiamondPMMStateRelay::getPort( int port, int &val )
{
    if ( !verifyPort( port ) )
        return STRLYERR_PORT;
    BYTE cval;
    int rval = ::dscGetRelayMulti( dev->handle, port, &cval );
    std::cout << "JUST READ VALUE: " << std::hex << cval << std::endl;
    val = cval;
    bits[port] = val;

    return rval;
}

int DiamondPMMStateRelay::getRelay( int relayID, bool &relayOn )
{
    if ( !verifyRelay( relayID ) )
        return STRLYERR_RELAY;

    BYTE cval;
    int rval = ::dscGetRelay( dev->handle, relayID, &cval );
    std::cout << "JUST READ VALUE: " << std::hex << cval << std::endl;

    int port = relayID / portWidth;
    int bit = relayID % portWidth;

    relayOn = cval > 0;

    bits[port] = (bits[port] & (~(1 << bit))) | (bits[port] & (relayOn << bit));
    
    return rval;
}
