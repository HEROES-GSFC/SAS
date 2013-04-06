// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <PvSampleTransmitterConfig.h>


// Default values
#define DEFAULT_DEVICE_ADDRESS ( "" )


// Application config
class Config : public PvSampleTransmitterConfig
{
public:

    Config()
    {
        SetDefaults();
    }

    ~Config()
    {
    }

    const char *GetDeviceAddress() const { return mDeviceAddress.c_str(); }

    void SetDefaults()
    {
        PvSampleTransmitterConfig::SetDefaults();

        // Set static defaults
        mDeviceAddress = DEFAULT_DEVICE_ADDRESS;
    }

    void ParseCommandLine( int aCount, const char **aArgs )
    {
        if ( ParseOptionFlag( aCount, aArgs, "--help" ) )
        {
            PrintHelp();
            exit( 0 );
        }

        PvSampleTransmitterConfig::ParseCommandLine( aCount, aArgs );

        ParseOption<string>( aCount, aArgs, "--deviceaddress", mDeviceAddress );
    }

    void PrintHelp()
    {
        cout << "This utility receives an image stream from a GigE Vision device, writes text on it and retransmits it using the GigEVision Streaming Protocol (GVSP)." << endl << endl << endl;

        PvSampleTransmitterConfig::PrintHelp();

        cout << "--deviceaddress=<address of the device from which to receive>" << endl;
        cout << "Default behaviour opens the device finder window to allow the user to select a device." << endl << endl;
    }

private:

    string mDeviceAddress;
};


#endif // __CONFIGURATION_H__

