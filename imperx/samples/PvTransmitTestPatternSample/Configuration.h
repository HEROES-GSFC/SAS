// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <PvSampleTransmitterConfig.h>


// Default values
#define DEFAULT_WIDTH ( 640 )
#define DEFAULT_HEIGHT ( 480 )
#define DEFAULT_NO_PATTERN ( false )


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

    PvUInt32 GetWidth() const { return mWidth; }
    PvUInt32 GetHeight() const { return mHeight; }
    bool GetNoPattern() const { return mNoPattern; }

    void SetDefaults()
    {
        PvSampleTransmitterConfig::SetDefaults();

        // Set static defaults
        mWidth = DEFAULT_WIDTH;
        mHeight = DEFAULT_HEIGHT;
        mNoPattern = DEFAULT_NO_PATTERN;
    }

    void ParseCommandLine( int aCount, const char **aArgs )
    {
        if ( ParseOptionFlag( aCount, aArgs, "--help" ) )
        {
            PrintHelp();
            exit( 0 );
        }

        PvSampleTransmitterConfig::ParseCommandLine( aCount, aArgs );

        ParseOption<PvUInt32>( aCount, aArgs, "--width", mWidth );
        ParseOption<PvUInt32>( aCount, aArgs, "--height", mHeight );
        ParseOptionFlag( aCount, aArgs, "--nopattern", &mNoPattern );
    }

    void PrintHelp()
    {
        cout << "This utility transmits a test pattern to a given destination using the GigEVision Streaming Protocol (GVSP)." << endl << endl << endl;

        PvSampleTransmitterConfig::PrintHelp();

        cout << "--width=<width in pixels>" << endl;
        cout << "Default: " << DEFAULT_WIDTH << endl << endl;

        cout << "--height=<height in pixels>" << endl;
        cout << "Default: " << DEFAULT_HEIGHT << endl << endl;

        cout << "--nopattern" << endl;
        cout << "Disables the test pattern." << endl;
        cout << "By default, each frame is populated with test data - this takes a little bit of CPU power so for pure benchmark purposes it may be advisable to disable this behaviour." << endl << endl;
    }

private:

    PvUInt32 mWidth;
    PvUInt32 mHeight;
    bool mNoPattern;
};


#endif // __CONFIGURATION_H__

