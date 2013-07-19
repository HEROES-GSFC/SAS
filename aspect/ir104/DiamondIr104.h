/**
 * \file DiamondIr104.h
 * \date Dec 7, 2012
 * \author Kurt Dietz
 */

#ifndef DIAMONDIR104_H_
#define DIAMONDIR104_H_

#include <StateRelay.h>
#include <DioDevice.h>

#ifdef _WIN32
#include <DiamondBoard.h>
#else
#include <DiamondBoard.h>
#include <DioDevice.h>
#endif



class DiamondIr104Dio;
class DiamondIr104StateRelay;

/**
 * Basic class to initialize and control a Diamond Systems IR-104 board.
 */
class DiamondIr104 : public DiamondBoard
{
public:
    friend class DiamondIr104Dio;
    friend class DiamondIr104StateRelay;

    /**
     * Initializes the library, and initializes the IR-104 board.
     */
    DiamondIr104();

    virtual ~DiamondIr104() {};
};

/**
 * Standard DIO interface to the IR-104 board.
 */
class DiamondIr104Dio : public DioDevice
{
public:
    /**
     * Sets IR104 device, number of DIO bits, ports, and bits per port.
     * \param[in]   ir104     Diamond IR-104 device object.
     */
    DiamondIr104Dio( DiamondIr104& ir104 );

    virtual ~DiamondIr104Dio() {};

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
    DiamondIr104*   dev;      ///< Pointer to device object.
};

/**
 * State Relay interface to the IR-104 board.
 */
class DiamondIr104StateRelay : public StateRelay
{
public:
    /**
     * Sets the number and ID of each channel.
     * \param[in]   ir104     Diamond IR-104 device object.
     */
    DiamondIr104StateRelay( DiamondIr104& ir104 );

    virtual ~DiamondIr104StateRelay() {};

    // Base class over-rides.
    virtual int setPort( int port, int val );
    virtual int setRelay( int relayID, bool relayOn );

    virtual int getPort( int port, int& val );
    virtual int getRelay( int relayID, bool& relayOn );

protected:
    DiamondIr104*   dev;      ///< Pointer to device object.
};

#endif /* DIAMONDIR104_H_ */
