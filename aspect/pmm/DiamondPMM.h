/**
 * \file DiamondPMM.h
 * \date Dec 7, 2012
 * \author Kurt Dietz
 */

#ifndef DIAMONDPMM_H_
#define DIAMONDPMM_H_

#include "StateRelay.h"
#include "DioDevice.h"
#include "DiamondBoard.h"


class DiamondPMMDio;
class DiamondPMMStateRelay;

/**
 * Basic class to initialize and control a Diamond Systems Pearl-MM board.
 */
class DiamondPMM : public DiamondBoard
{
public:
    friend class DiamondPMMDio;
    friend class DiamondPMMStateRelay;

    /**
     * Initializes the library, and initializes the Pearl-MM board.
     */
    DiamondPMM();

    virtual ~DiamondPMM() {};
};

/**
 * Standard DIO interface to the Pearl-MM board.
 */
class DiamondPMMDio : public DioDevice
{
public:
    /**
     * Sets Pearl-MM device, number of DIO bits, ports, and bits per port.
     * \param[in]   pmm     Diamond Pearl-MM device object.
     */
    DiamondPMMDio( DiamondPMM& pmm );

    virtual ~DiamondPMMDio() {};

    // Base class over-rides.


    // Full-byte functions.
    virtual int getByte( int port, int &value )
    {
        return getPort( port, value );
    };

    virtual int getPort( int port, int &value );

    // Single-bit per-port functions.
    virtual int getBit( int bitnum, int &value );
    virtual int getPortBit( int port, int bitnum, int &value );

protected:
    DiamondPMM*   dev;      ///< Pointer to device object.
};

/**
 * State Relay interface to the Pearl-MM board.
 */
class DiamondPMMStateRelay : public StateRelay
{
public:
    /**
     * Sets the number and ID of each channel.
     * \param[in]   pmm     Diamond Pearl-MM device object.
     */
    DiamondPMMStateRelay( DiamondPMM& pmm );

    virtual ~DiamondPMMStateRelay() {};

    // Base class over-rides.
    virtual int setPort( int port, int val );
    virtual int setRelay( int relayID, bool relayOn );

    virtual int getPort( int port, int& val );
    virtual int getRelay( int relayID, bool& relayOn );

protected:
    DiamondPMM*   dev;      ///< Pointer to device object.
};

#endif /* DIAMONDIR104_H_ */
