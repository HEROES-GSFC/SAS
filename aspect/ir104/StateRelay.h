/**
 * \file StateRelay.h
 * \date Dec 7, 2012
 * \author Kurt Dietz
 */

#ifndef STATERELAY_H_
#define STATERELAY_H_

#include "stdio.h"


/**
 * Provides a base class for any simple state-based relay.
 */
class StateRelay
{
public:
    /**
     * Creates the relay mask, which are the only bits this relay object can set.
     * \param[in]   relayMask   Bit mask of bits this object can alter.
     */
    StateRelay( int numPort, int portWidth, int* relayMask );

    virtual ~StateRelay();

    /**
     * Sets all the relay states in a single port.
     * param[in]    port    ID of port on which to set relay states.
     * param[in]    val     Bitwise state of relay states to set.
     */
    virtual int setPort( int port, int val ) {return -1;};

    /**
     * Sets the state of a single relay.
     * \param[in]   relayID   ID of relay to set or clear.
     * \param[in]   relayOn   State to set relay (true = ON, false = OFF).
     */
    virtual int setRelay( int relayID, bool relayOn ) {return -1;};

    /**
     * Gets all the relay states from a single port.
     * param[in]    port    ID of port from which to get relay states.
     * param[out]   val     Bitwise state of relay states from this port.
     */
    virtual int getPort( int port, int& val ) {return -1;};

    /**
     * Gets the state of a single relay.
     * \param[in]   relayID   ID of relay to read state of.
     * \param[out]  relayOn   State of relay (true = ON, false = OFF).
     */
    virtual int getRelay( int relayID, bool& relayOn ) {return -1;};

protected:
    /**
     * Verifies that the port is valid.
     */
    bool verifyPort( int port )
    {
        return (port >= 0 && port < numPort );
    };

    bool verifyRelay( int bitnum )
    {
        return ( bitnum >= 0 && bitnum < numBits );
    }

    int   numBits;    ///< Number of discrete bits or relays on entire device.
    int   numPort;    ///< Number of ports supported by relay device.
    int   portWidth;  ///< Width of port (number of relays or bits per port).
    int*  bits;       ///< Image of bits set in the hardware or on disk.
    int*  mask;       ///< Masks in only bits this relay controls.
};

#endif /* STATERELAY_H_ */
