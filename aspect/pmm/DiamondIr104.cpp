/*
 * DiamondIr104.cpp
 *
 *  Created on: Dec 7, 2012
 *      Author: kdietz
 */

#include "DiamondIr104.h"

//#include <ErrorCodes.h>
#include <iostream>
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// DiamondIr104 class definitions.

// Set up the default settings.
static DSCCB defaultDsccb = {0, 0, 0x240, 5};

DiamondIr104::DiamondIr104() : DiamondBoard( DSC_IR104, &defaultDsccb )
{
}

///////////////////////////////////////////////////////////////////////////////
// DiamondIr104Dio class definitions.

DiamondIr104Dio::DiamondIr104Dio( DiamondIr104& ir104) : DioDevice( 20, 10 )
{
    this->dev = &ir104;
}

int DiamondIr104Dio::getBit(int bit, int& value)
{
    int retVal;

//  cout << "DiamondIr104Dio::getBit: Verifying bit number " << bit << endl;

    // Verify the port and bit number are valid.
    if ( (retVal = verifyBitnum( bit )) )
        return retVal;

    unsigned char val;
    unsigned char chanID = 1+ bit;

//  cout << "DiamondIr104Dio::getBit: chanID = " << (int)chanID << endl;

    retVal = ::dscIR104OptoInput(dev->handle, chanID, &val);
    value = (int)val;

    return retVal;
}

int DiamondIr104Dio::getPortBit(int port, int bit, int& value)
{
    int retVal;

//  cout << "DiamondIr104Dio::getPortBit: Verifying port " << port << " and bit number " << bit << endl;

    // Verify the port and bit number are valid.
    if ( (retVal = verifyPortBit( port, bit )) )
        return retVal;

    unsigned char val;
    unsigned char chanID = (unsigned char)(1 + (port * NUM_PORTBIT) + bit);

//  cout << "DiamondIr104Dio::getPortBit: chanID = " << (int)chanID << endl;

    retVal = ::dscIR104OptoInput(dev->handle, chanID, &val);
    value = (int)val;

    return retVal;
}

int DiamondIr104Dio::getPort(int port, int& value)
{
    int retVal;

//  cout << "DiamondIr104Dio::getPort: verifying port " << port << endl;

    if ( (retVal = verifyPort( port )) )
        return retVal;

    unsigned char val;
    value = 0;
    int base = (port * NUM_PORTBIT) + 1;
    for ( int i = 0; i < NUM_PORTBIT; i++ )
    {
//    cout << "DiamondIr104Dio::getPort: Channel ID port = " << port << ", bit = " << i;
//    cout << ", chanID = " << base + i << endl;

        retVal = ::dscIR104OptoInput( dev->handle, base+i, &val);
        value |= (val & 1) << i;
    }

    return retVal;
}

///////////////////////////////////////////////////////////////////////////////
// DiamondIr104StateRelay class definitions.

DiamondIr104StateRelay::DiamondIr104StateRelay( DiamondIr104 &ir104 ) :
StateRelay( 2, 10, NULL )
{
    // Set the device pointer.
    this->dev = &ir104;
}

int DiamondIr104StateRelay::setPort( int port, int val )
{
//  cout << "DiamondIr104Dio::getPort: verifying port " << port << endl;

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
//      cout << "DiamondIr104StateRelay::setPort: Channel ID port = " << port << ", bit = " << i;
//      cout << ", chanID = " << chanID + i << endl;

            // Set relay.
            if ( (bit & val) > 0 )
                rval = ::dscIR104SetRelay( dev->handle, chanID + i );
            // Clear relay.
            else
                rval = ::dscIR104ClearRelay( dev->handle, chanID + i );
        }
    }

    return rval;
}

int DiamondIr104StateRelay::setRelay( int relayID, bool relayOn )
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
            ::dscIR104SetRelay( dev->handle, relayID+1 );
        // Clear relay.
        else
            ::dscIR104ClearRelay( dev->handle, relayID+1 );
    }

    return 0;
}

int DiamondIr104StateRelay::getPort( int port, int &val )
{
    if ( !verifyPort( port ) )
        return STRLYERR_PORT;

    int rval, chanID = 1 + (port * portWidth);
    val = 0;

    for ( int i = 0; i < portWidth; i++ )
    {
        unsigned char cval;
        rval = ::dscIR104RelayInput( dev->handle, chanID + i, &cval );
        val |= (cval > 0) ? 1 << i : 0;
    }

    return rval;
}

int DiamondIr104StateRelay::getRelay( int relayID, bool &relayOn )
{
    if ( !verifyRelay( relayID ) )
        return STRLYERR_RELAY;

    unsigned char cval=0;
    int rval = ::dscIR104RelayInput( dev->handle, relayID + 1, &cval );

    relayOn = cval > 0;

    return rval;
}
