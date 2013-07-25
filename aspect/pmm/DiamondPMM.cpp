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
    // Zero the PMM board just in case.
    for ( int port = 0; port < numPort; port++)
        setPort(port, 0x00);

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
    
    //std::cout << "setPort: " << dscGetErrorString(rval) << std::endl;
    return rval;
}

int DiamondPMMStateRelay::setRelay( int relayID, bool relayOn )
{
    if ( !verifyRelay( relayID ) )
        return STRLYERR_RELAY;

    int rval = 0;

    int port = relayID / portWidth;
    int bit = relayID % portWidth;


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

    //std::cout << "setRelay: " << dscGetErrorString(rval) << std::endl;
    return rval;
}

int DiamondPMMStateRelay::getPort( int port, int &val )
{
    if ( !verifyPort( port ) )
        return STRLYERR_PORT;

    val = bits[port];
    //std::cout << "getPort: " << dscGetErrorString(rval) << std::endl;
    return 0;
}

int DiamondPMMStateRelay::getRelay( int relayID, bool &relayOn )
{
    if ( !verifyRelay( relayID ) )
        return STRLYERR_RELAY;


    int port = relayID / portWidth;
    int bit = relayID % portWidth;

    relayOn = (bits[port] & (1 << bit)) > 0;

    //std::cout << "getRelay: " << dscGetErrorString(rval) << std::endl;
    return 0;
}
