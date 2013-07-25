/**
 * \file DiamondPMM.h
 * \date Dec 7, 2012
 * \author Kurt Dietz
 */

#ifndef DIAMONDPMM_H_
#define DIAMONDPMM_H_

#include "StateRelay.h"
#include "DiamondBoard.h"
#include "DioDevice.h"

class DiamondPMMStateRelay;

/**
 * Basic class to initialize and control a Diamond Systems Pearl-MM board.
 */
class DiamondPMM : public DiamondBoard
{
public:
    friend class DiamondPMMStateRelay;

    /**
     * Initializes the library, and initializes the Pearl-MM board.
     */
    DiamondPMM();

    virtual ~DiamondPMM() {};
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
