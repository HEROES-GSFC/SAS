/*
 * StateRelay.cpp
 *
 *  Created on: Dec 12, 2012
 *      Author: kdietz
 */

#include "StateRelay.h"

StateRelay::StateRelay( int numPort, int portWidth, int* relayMask )
{
    numPort = numPort > 1 ? numPort : 1;
    portWidth = portWidth > 1 ? portWidth : 1;
    this->numPort = numPort;
    this->portWidth = portWidth;
    this->numBits = numPort * portWidth;

    mask = new unsigned int[numPort];
    bits = new unsigned int[numPort];

    if ( relayMask != NULL )
    {
        for ( int i = 0; i < numPort; i++ )
            mask[i] = relayMask[i];
    }
    else
    {
        unsigned int mval = (1 << portWidth) - 1;
        for ( int i = 0; i < numPort; i++ )
            mask[i] = mval;
    }
}

StateRelay::~StateRelay()
{
    delete []mask;
    delete []bits;
};
