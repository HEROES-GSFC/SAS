/// \file DioDevice.h
///   C++ header defining the interface class DioDevice.

#ifndef DIODEVICE_H
#define DIODEVICE_H

//#include <Parallel.h>
//#include <HwBaseErrorCodes.h>

/**
 *  Interface class for common multiport Digital I/O operations.
 */
class DioDevice
{
public:
    DioDevice() {/*mutex = new Mutex();*/};

    /**
     * Initializes the DIO device with the number of bits and width of the ports.
     * \param[in]   numBits     Total number of DIO bits in device.
     * \param[in]   portWidth   Number of bits (nominally) in each port.
     */
    DioDevice( int numBits, int portWidth )
        {
            //mutex = new Mutex();
            NUM_BIT = numBits > 1 ? numBits : 1;
            NUM_PORTBIT = portWidth > 1 ? portWidth : 1;
            int add = (NUM_BIT % NUM_PORTBIT) > 0 ? 1 : 0;
            NUM_PORT = (NUM_BIT / NUM_PORTBIT) + add;
        }

    virtual ~DioDevice()
        {
            //delete mutex;
        };

    // Full-byte functions.
    /**
     *  Sets 8-bits of digital output to the given port.
     *  \param[in]    port    DIO port to write to.
     *  \param[in]    value   8-bit value to write.
     */
    virtual int setByte( int port, int value ) {return -1;};

    /**
     *  Reads 8-bits of digital output from the given port.
     *  \param[in]    port    DIO port to read from.
     *  \param[out]   value   8-bit value read.
     */
    virtual int getByte( int port, int &value ) {return -1;};

    /**
     * Reads a full port's worth of DIO bits from the device.
     * \param[in]     port    DIO port number to read from.
     * \param[out]    value   Value read from port (portWidth bits wide).
     */
    virtual int getPort( int port, int &value) {return -1;};

    // Single-bit functions.
    virtual int setBit( int bitnum );
    virtual int clearBit( int bitnum );
    virtual int setBitState( int bitnum, bool state );
    virtual int getBit( int bitnum, int &value );

    // Single-bit per-port functions.
    virtual int setPortBit( int port, int bitnum ) {return -1;};
    virtual int clearPortBit( int port, int bitnum ) {return -1;};
    virtual int setPortBitState( int port, int bitnum, bool state );
    virtual int getPortBit( int port, int bitnum, int &value ) {return -1;};

    // Single-bit pulse functions.
    virtual int pulseBitHigh( int bitnum, int widthUs );
    virtual int pulseBitLow( int bitnum, int widthUs );

    // Single-bit per-port pulse functions.
    virtual int pulsePortBitHigh( int port, int bitnum, int widthUs );
    virtual int pulsePortBitLow( int port, int bitnum, int widthUs );

    // Locking functions.
    //virtual int lock() {return mutex->lock();};
    //virtual int unlock() {return mutex->unlock();};

protected:
    int     NUM_BIT;      ///< Number of I/O bits.
    int     NUM_PORT;     ///< Number of I/O ports.
    int     NUM_PORTBIT;  ///< Number of I/O bits per port.
    //Mutex*  mutex;        ///< Mutex;

    virtual int verifyPort( int port );
    virtual int verifyBitnum( int bitnum );
    virtual int verifyPortBit( int port, int bitnum );
};

inline int DioDevice::pulseBitLow(int bitnum, int width)
{
    int retVal = this->clearBit(bitnum);

    if(retVal != 0)
        return retVal;

    usleep(width);

    return this->setBit(bitnum);

}

inline int DioDevice::pulseBitHigh(int bitnum, int width)
{
    int retVal = this->setBit(bitnum);

    if(retVal != 0)
        return retVal;

    usleep(width);

    return this->clearBit(bitnum);
}

inline int DioDevice::pulsePortBitLow(int port, int bitnum, int width)
{
    int retVal = this->clearPortBit(port, bitnum);

    if(retVal != 0)
        return retVal;

    usleep(width);

    return this->setPortBit(port, bitnum);

}

inline int DioDevice::pulsePortBitHigh(int port, int bitnum, int width)
{
    int retVal = this->setPortBit(port, bitnum);

    if(retVal != 0)
        return retVal;

    usleep(width);

    return this->clearPortBit(port, bitnum);
}

//	getBit:
//		Reads one of 48 bits from the first 6 ports by calling getPortBit.
inline int DioDevice::getBit( int bitnum, int &value )
{
    return getPortBit( bitnum/NUM_PORTBIT, bitnum % NUM_PORTBIT, value );
}

//	setBit:
//		Writes one of 48 bits by calling setPortBit.
inline int DioDevice::setBitState( int bitnum, bool state )
{
    return setPortBitState( bitnum/NUM_PORTBIT, bitnum % NUM_PORTBIT, state );
}

inline int DioDevice::setPortBitState( int portnum, int bitnum, bool state )
{
    if ( state )
        return setPortBit( portnum, bitnum );
    else
        return clearPortBit( portnum, bitnum );
}

inline int DioDevice::setBit( int bitnum )
{
    return setPortBit( bitnum/NUM_PORTBIT, bitnum %NUM_PORTBIT );
}

inline int DioDevice::clearBit( int bitnum )
{
    return clearPortBit( bitnum/NUM_PORTBIT, bitnum % NUM_PORTBIT );
}

inline int DioDevice::verifyBitnum( int bitnum )
{
    if ( bitnum > -1 && bitnum < NUM_BIT )
        return 0;

    return DIODEV_ERR_BITINVALID;
}

inline int DioDevice::verifyPort( int port )
{
    if ( port > -1 && port < NUM_PORT )
        return 0;

    return DIODEV_ERR_PORTINVALID;
}

inline int DioDevice::verifyPortBit( int port, int bitnum )
{
    if ( port > -1 && port < NUM_PORT )
    {
        if ( (NUM_BIT % NUM_PORTBIT) > 0 && port == NUM_PORT-1 && bitnum > -1 &&
             bitnum < (NUM_BIT % NUM_PORTBIT) )
            return 0;
        else if ( bitnum > -1 && bitnum < NUM_PORTBIT )
            return 0;

        return DIODEV_ERR_BITINVALID;
    }

    return DIODEV_ERR_PORTINVALID;
}

#endif // #ifndef DIODEVICE_H
