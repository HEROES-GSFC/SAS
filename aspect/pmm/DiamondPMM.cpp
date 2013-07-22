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
// DiamondPMMDio class definitions.

DiamondPMMDio::DiamondPMMDio( DiamondPMM& pmm) : DioDevice( 16, 8 )
{
    this->dev = &pmm;
}

int DiamondPMMDio::getBit(int bit, int& value)
{
    int retVal;

//  cout << "DiamondPMMDio::getBit: Verifying bit number " << bit << endl;

    // Verify the port and bit number are valid.
    if ( (retVal = verifyBitnum( bit )) )
        return retVal;

    unsigned char val;
    unsigned char chanID = 1+ bit;

//  cout << "DiamondPMMDio::getBit: chanID = " << (int)chanID << endl;

    retVal = ::dscGetRelay(dev->handle, chanID, &val);
    value = (int)val;

    return retVal;
}

int DiamondPMMDio::getPortBit(int port, int bit, int& value)
{
    int retVal;

//  cout << "DiamondPMMDio::getPortBit: Verifying port " << port << " and bit number " << bit << endl;

    // Verify the port and bit number are valid.
    if ( (retVal = verifyPortBit( port, bit )) )
        return retVal;

    unsigned char val;
    unsigned char chanID = (unsigned char)(1 + (port * NUM_PORTBIT) + bit);

//  cout << "DiamondPMMDio::getPortBit: chanID = " << (int)chanID << endl;

    retVal = ::dscGetRelay(dev->handle, chanID, &val);
    value = (int)val;

    return retVal;
}

int DiamondPMMDio::getPort(int port, int& value)
{
    int retVal;

//  cout << "DiamondPMMDio::getPort: verifying port " << port << endl;

    if ( (retVal = verifyPort( port )) )
        return retVal;

    unsigned char val;
    value = 0;
    int base = (port * NUM_PORTBIT) + 1;
    for ( int i = 0; i < NUM_PORTBIT; i++ )
    {
//    cout << "DiamondPMMDio::getPort: Channel ID port = " << port << ", bit = " << i;
//    cout << ", chanID = " << base + i << endl;

        retVal = ::dscGetRelay( dev->handle, base+i, &val);
        value |= (val & 1) << i;
    }

    return retVal;
}

///////////////////////////////////////////////////////////////////////////////
// DiamondPMMStateRelay class definitions.

DiamondPMMStateRelay::DiamondPMMStateRelay( DiamondPMM &pmm ) :
StateRelay( 2, 8, NULL )
{
    // Set the device pointer.
    this->dev = &pmm;
}

int DiamondPMMStateRelay::setPort( int port, int val )
{
//  cout << "DiamondPMMDio::getPort: verifying port " << port << endl;

    if ( !verifyPort( port ) )
        return STRLYERR_PORT;

    int chanID = 1 + (port * portWidth);
    int bit, rval=0;

    for ( int i = 0; i < portWidth; i++ )
    {
        // If the mask bit is set, set the state.
        bit = 1 << i;
        if ( ( bit & mask[port] ) > 0 )
        {
//      cout << "DiamondPMMStateRelay::setPort: Channel ID port = " << port << ", bit = " << i;
//      cout << ", chanID = " << chanID + i << endl;

            // Set relay.
            if ( (bit & val) > 0 )
                rval = ::dscSetRelay( dev->handle, chanID + i, 0 );
            // Clear relay.
            else
                rval = ::dscSetRelay( dev->handle, chanID + i, 1 );
        }
    }

    return rval;
}

int DiamondPMMStateRelay::setRelay( int relayID, bool relayOn )
{
    if ( !verifyRelay( relayID ) )
        return STRLYERR_RELAY;

    int port = relayID / portWidth;
    int bit = relayID % portWidth;

    // Make sure the bit is masked in.
    if ( ((1 << bit) & mask[port]) > 0 )
    {
        // Set relay.
        if ( relayOn )
            ::dscSetRelay( dev->handle, relayID+1 , (char) 1);
        // Clear relay.
        else
            ::dscSetRelay( dev->handle, relayID+1, (char) 0);
    }

    return 0;
}

int DiamondPMMStateRelay::getPort( int port, int &val )
{
    if ( !verifyPort( port ) )
        return STRLYERR_PORT;

    int rval, chanID = 1 + (port * portWidth);
    val = 0;

    for ( int i = 0; i < portWidth; i++ )
    {
        unsigned char cval;
        rval = ::dscGetRelay( dev->handle, chanID + i, &cval );
        val |= (cval > 0) ? 1 << i : 0;
    }

    return rval;
}

int DiamondPMMStateRelay::getRelay( int relayID, bool &relayOn )
{
    if ( !verifyRelay( relayID ) )
        return STRLYERR_RELAY;

    unsigned char cval=0;
    int rval = ::dscGetRelay( dev->handle, relayID + 1, &cval );

    relayOn = cval > 0;

    return rval;
}
