// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <PvSampleTransmitterConfig.h>


// Default values
#define DEFAULT_MOVIE ( "sample.mp4" )
#define DEFAULT_CAPTURE_DEVICE ( -1 )


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

    const char *GetVideoFile() const { return mVideoFile.c_str(); }
    PvInt32 GetCaptureDevice() const { return mCaptureDevice; }

    void SetDefaults()
    {
        PvSampleTransmitterConfig::SetDefaults();

        // Set static defaults
        mVideoFile = DEFAULT_MOVIE;
        mCaptureDevice = DEFAULT_CAPTURE_DEVICE;
    }

    void ParseCommandLine( int aCount, const char **aArgs )
    {
        if ( ParseOptionFlag( aCount, aArgs, "--help" ) )
        {
            PrintHelp();
            exit( 0 );
        }

        PvSampleTransmitterConfig::ParseCommandLine( aCount, aArgs );

        ParseOption<PvInt32>( aCount, aArgs, "--capturedevice", mCaptureDevice );
        ParseOption<string>( aCount, aArgs, "--videofile", mVideoFile );
    }

    void PrintHelp()
    {
        cout << "This utility transmits a video file or webcam to a given destination using the GigEVision Streaming Protocol (GVSP)." << endl << endl << endl;

        PvSampleTransmitterConfig::PrintHelp();

        cout << "--capturedevice=<index of capture device on the system> " << endl;
        cout << "Use this option to transmit from a capture device (such as a video capture card or webcam)." << endl << endl;

        cout << "--videofile=<video file to transmit> " << endl;
        cout << "By default, the sample video file residing in the folder containing this utility is used." << endl << endl;
    }

private:

    string mVideoFile;
    PvInt32 mCaptureDevice;
};


#endif // __CONFIGURATION_H__

