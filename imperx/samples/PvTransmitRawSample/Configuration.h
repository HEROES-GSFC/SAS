// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <PvSampleTransmitterConfig.h>


// Default values
#define DEFAULT_NO_PATTERN ( false )
#define DEFAULT_DATA_SIZE ( 1000000 )


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

    bool GetNoPattern() const { return mNoPattern; }
    PvUInt32 GetDataSize() const { return mDataSize; }

    void SetDefaults()
    {
        PvSampleTransmitterConfig::SetDefaults();

        // Set static defaults
        mNoPattern = DEFAULT_NO_PATTERN;
        mDataSize = DEFAULT_DATA_SIZE;
    }

    void ParseCommandLine( int aCount, const char **aArgs )
    {
        if ( ParseOptionFlag( aCount, aArgs, "--help" ) )
        {
            PrintHelp();
            exit( 0 );
        }

        PvSampleTransmitterConfig::ParseCommandLine( aCount, aArgs );

        ParseOptionFlag( aCount, aArgs, "--nopattern", &mNoPattern );
        ParseOption<PvUInt32>( aCount, aArgs, "--datasize", mDataSize );
    }

    void PrintHelp()
    {
        cout << "This utility transmits a raw data test pattern to a given destination using the GigEVision Streaming Protocol (GVSP)." << endl << endl << endl;

        PvSampleTransmitterConfig::PrintHelp();

        cout << "--nopattern" << endl;
        cout << "Disables the test pattern." << endl;
        cout << "By default, each frame is populated with test data - this takes a little bit of CPU power so for pure benchmark purposes it may be advisable to disable this behaviour." << endl << endl;

        cout << "--datasize=<size of each payload in bytes>" << endl;
        cout << "Default: 1000000" << endl << endl;
    }

private:

    bool mNoPattern;
    PvUInt32 mDataSize;
};


#endif // __CONFIGURATION_H__

