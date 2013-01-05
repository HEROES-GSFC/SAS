// *****************************************************************************
//
//     Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// To receive images from a multi-source device using PvPipeline
//

#include <PvSampleUtils.h>
#include <PvDeviceFinderWnd.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvStream.h>
#include <PvGenStateStack.h>

#include <iostream>
#include <iomanip>
#include <list>
using namespace std;

PV_INIT_SIGNAL_HANDLER();

#define BUFFER_COUNT ( 16 )

const char sDoodle[] = "|\\-|-/";
const int sDoodleLength = 6;


//
// Simple class used to control a source and get images from it
//

class Source
{
public:

    // Constructor
    Source( PvDevice *aDevice, const PvString &aIPAddress, const PvString &aSource )
        : mDevice( aDevice )
        , mIPAddress( aIPAddress )
        , mSource( aSource )
        , mPipeline( NULL )
        , mDoodleIndex( 0 )
    {
        // Dynamically allocate pipeline (required PvStream pointer)
        mPipeline = new PvPipeline( &mStream );
    }

    // Destructor
    ~Source()
    {
        // Free pipeline
        if ( mPipeline != NULL )
        {
            delete mPipeline;
            mPipeline = NULL;
        }
    }

    // Open source: opens the stream, set destination, starts pipeline
    void Open()
    {
        cout << "Opening source " << mSource.GetAscii() << endl;

        // Select source (if applicable)
        PvGenStateStack lStack( mDevice->GetGenParameters() );
        SelectSource( &lStack );

        // Retrieve the source channel
        cout << "  Reading source channel on device" << endl;
        PvInt64 lSourceChannel = 0;
        PvResult lResult = mDevice->GetGenParameters()->GetIntegerValue( "SourceStreamChannel", lSourceChannel );
        PvUInt32 lChannel = static_cast<PvUInt32>( lSourceChannel );

        // Open stream
        cout << "  Opening stream to device" << endl;
        mStream.Open( mIPAddress, 0, lChannel );

        PvString lLocalIP = mStream.GetLocalIPAddress();
        PvUInt32 lLocalPort = mStream.GetLocalPort();

        // Set source destination on IP engine
        cout << "  Setting source destination on device (channel " << lChannel << ") " << endl;
        cout << "    to " << lLocalIP.GetAscii() << " port " << lLocalPort << endl;
        mDevice->SetStreamDestination( lLocalIP, lLocalPort, lChannel );

        // Reading payload size from device
        PvInt64 lSize = 0;
        mDevice->GetGenParameters()->GetIntegerValue( "PayloadSize", lSize );

        // Set the Buffer size and the Buffer count
        mPipeline->SetBufferSize( static_cast<PvUInt32>( lSize ) );
        mPipeline->SetBufferCount( BUFFER_COUNT ); // Increase for high frame rate without missing block IDs

        // Start pipeline thread
        cout << "  Starting pipeline thread" << endl;
        mPipeline->Start();
    }

    // Close source: closes the stream, reset destination
    void Close()
    {
        cout << "Closing source " << mSource.GetAscii() << endl;

        // Stop pipeline thread
        cout << "  Stopping pipeline thread" << endl;
        mPipeline->Stop();

        // Close stream
        cout << "  Closing stream" << endl;
        mStream.Close();
    }

    // Start acquisition: set payload size, reset pipeline and stats
    void StartAcquisition()
    {
        cout << "Start acquisition" << mSource.GetAscii() << endl;

        // Select source (if applicable)
        PvGenStateStack lStack( mDevice->GetGenParameters() );
        SelectSource( &lStack );

        // TLParamsLocked is optional but when present, it MUST be set to 1
        // before sending the AcquisitionStart command
        cout << "  Setting TLParamsLocked to 1" << endl;
	    mDevice->GetGenParameters()->SetIntegerValue( "TLParamsLocked", 1 );

        // The pipeline is already "armed", we just have to tell the device
        // to start sending us images
        cout << "  Sending AcquisitionStart command to device" << endl;
	    mDevice->GetGenParameters()->ExecuteCommand( "AcquisitionStart" );
    }

    // Stop acquisition: stop pipeline
    void StopAcquisition()
    {
        cout << "Stop acquisition " << mSource.GetAscii() << endl;

        // Select source (if applicable)
        PvGenStateStack lStack( mDevice->GetGenParameters() );
        SelectSource( &lStack );

        // The pipeline is already "armed", we just have to tell the device
        // to start sending us images
        cout << "  Sending AcquisitionStop command to device" << endl;
	    mDevice->GetGenParameters()->ExecuteCommand( "AcquisitionStop" );

        // TLParamsLocked is optional but when present, it MUST be set to 0
        // before sending the AcquisitionStart command
        cout << "  Setting TLParamsLocked to 0" << endl;
	    mDevice->GetGenParameters()->SetIntegerValue( "TLParamsLocked", 0 );
    }

    // Pumps all the images out of the pipeline and return
    void RetrieveImages( PvUInt32 aTimeout )
    {
        // Set first wait timeout
        PvUInt32 lTimeout = aTimeout;

        // Loop for as long as images are available
        for ( ;; )
        {
            PvBuffer *lBuffer = NULL;
            PvResult lOperationResult;

            PvResult lResult = mPipeline->RetrieveNextBuffer( &lBuffer, aTimeout, &lOperationResult );
            if ( !lResult.IsOK() )
            {
                return;
            }

            // We got a buffer, check the acquisition result
            if ( lOperationResult.IsOK() )
            {
                // Here you would typically do your image manipulation/processing
                mDoodleIndex++;
                mDoodleIndex %= sDoodleLength;
            }

            // We made it here, at least release the buffer back to the pipeline
            mPipeline->ReleaseBuffer( lBuffer );

            // Set timeout to 0 for next RetrieveNextBufer operations
            lTimeout = 0;
        }
    }

    // Returns statistics in a string for stream of the source
    void PrintStatistics()
    {
        // Get frame rate
        double lFPS = 0.0;
        mStream.GetParameters()->GetFloatValue( "AcquisitionRate", lFPS );

        // Get bandwidth, convert in Mb/s
        double lBandwidth = 0.0;
        mStream.GetParameters()->GetFloatValue( "Bandwidth", lBandwidth );
        lBandwidth /= 1000000.0;

        // Display the source name (if available)
        if ( mSource.GetLength() > 0 )
        {
            cout << mSource.GetAscii() << ": ";
        }

        // Print spinning doodle
        cout << sDoodle[ mDoodleIndex ] << " ";

        // Display FPS, bandwidth
        cout << fixed;
        cout << setprecision( 1 ) << lFPS << " FPS ";
        cout << setprecision( 1 ) << lBandwidth << " Mb/s ";
    }

    // Returns recommended timeout when calling RetrieveNextBuffer
    double GetRecommendedTimeout()
    {
        // Get frame rate
        double lFPS = 0.0;
        mStream.GetParameters()->GetFloatValue( "AcquisitionRate", lFPS );

        // If no frame rate, recommend 1s timeout
        if ( lFPS == 0.0 )
        {
            return 1.0;
        }

        // Convert from frequency to period, s to ms
        double lTimeout = ( 1.0 / lFPS ) * 1000.0;

        // Be a bit more aggressive
        lTimeout /= 2.0;

        // Make sure we have at least 1 ms
        if ( lTimeout < 1.0 )
        {
            lTimeout = 1.0;
        }

        return lTimeout;
    }

protected:

    void SelectSource( PvGenStateStack *aStack )
    {
        // If no source defined, there is likely no source selector, nothing to select
        if ( mSource.GetLength() <= 0 )
        {
            return;
        }

        // Select source. When stack goes out of scope, the previous value will be restored
        aStack->SetEnumValue( "SourceSelector", mSource );
    }

private:

    PvDevice *mDevice;
    PvStream mStream;
    PvPipeline *mPipeline;

    PvString mIPAddress;
    PvString mSource;

    int mDoodleIndex;
};


// Vector of sources
typedef std::list<Source *> SourceList;


//
// Shows how to use a PvPipeline object to acquire images from a device
//

bool AcquireImages()
{
    // Create a GEV Device finder dialog
    PvDeviceFinderWnd lDeviceFinderWnd;

    // Prompt the user to select a GEV Device
    lDeviceFinderWnd.ShowModal();

    // Get the connectivity information for the selected GEV Device
    PvDeviceInfo* lDeviceInfo = lDeviceFinderWnd.GetSelected();

    // If no device is selected, abort
    if( lDeviceInfo == NULL )
    {
        cout << "No device selected." << endl;
        return false;
    }

    PvString lMACAddress = lDeviceInfo->GetMACAddress();
    PvString lIPAddress = lDeviceInfo->GetIPAddress();

    // Connect to the GEV Device
    PvDevice lDevice;
    cout << "Connecting to " << lMACAddress.GetAscii() << endl;
    // if ( !lDevice.Connect( lDeviceInfo ).IsOK() )
    if ( !lDevice.Connect( lDeviceInfo ).IsOK() )
    {
        cout << "Unable to connect to " << lMACAddress.GetAscii() << endl;
        return false;
    }
    cout << "Successfully connected to " << lMACAddress.GetAscii() << endl;

    cout << endl;

    SourceList lSources;

    // Get source selector
    PvGenEnum *lSourceSelector = lDevice.GetGenParameters()->GetEnum( "SourceSelector" );
    if ( lSourceSelector != NULL )
    {
        // Go through all sources, create source objects
        PvInt64 lCount = 0;
        lSourceSelector->GetEntriesCount( lCount );
        for ( PvInt64 i = 0; i < lCount; i++ )
        {
            // Get source enum entry
            const PvGenEnumEntry *lEE = NULL;
            lSourceSelector->GetEntryByIndex( i, &lEE );

            // If available, create source
            if ( ( lEE != NULL ) && lEE->IsAvailable() )
            {
                // Get source name
                PvString lSourceName;
                lEE->GetName( lSourceName );

                // Create source
                Source *lSource = new Source( &lDevice, lIPAddress, lSourceName );
                lSource->Open();

                // Add to sources list
                lSources.push_back( lSource );

                cout << endl;
            }
        }
    }
    else
    {
        // If no source selector, just create a single source
        Source *lSource = new Source( &lDevice, lIPAddress, "" );
        lSource->Open();

        // Add to sources list
        lSources.push_back( lSource );

        cout << endl;
    }

    // Start the acquisiton on all sources
    SourceList::iterator lIt = lSources.begin();
    while ( lIt != lSources.end() )
    {
        ( *( lIt++ ) )->StartAcquisition();
        cout << endl;
    }

    // Aggressive initial value, will be adjusted vs frame rate
    PvUInt32 lTimeout = 1;

    // Acquire images until the user instructs us to stop
    cout << "<press a key to stop streaming>" << endl;
    while ( !PvKbHit() )
    {
        double lNewTimeout = 1000.0;

        lIt = lSources.begin();
        while ( lIt != lSources.end() )
        {
            ( *lIt )->RetrieveImages( lTimeout );
            ( *lIt )->PrintStatistics();

            // Always use the smallest recommended timeout
            double lRecommendedTimeout = ( *lIt )->GetRecommendedTimeout();
            if ( lRecommendedTimeout < lNewTimeout )
            {
                lNewTimeout = lRecommendedTimeout;
            }

            lIt++;
        }

        // Update timeout for next round - smallest recommended divided by number of sources
        lTimeout = static_cast<PvUInt32>( lNewTimeout / static_cast<double>( lSources.size() ) + 0.5 );

        cout << "\r";
    }

    PvGetChar(); // Flush key buffer for next stop
    cout << endl << endl;

    // Stop the acquisiton on all sources
    lIt = lSources.begin();
    while ( lIt != lSources.end() )
    {
        ( *( lIt++ ) )->StopAcquisition();
        cout << endl;
    }

    // Close and delete sources
    lIt = lSources.begin();
    while ( lIt != lSources.end() )
    {
        ( *lIt )->Close();
        cout << endl;

        delete *lIt;

        lIt++;
    }

    // Finally disconnect the device. Optional, still nice to have
    cout << "Disconnecting device" << endl;
    lDevice.Disconnect();

    return true;
}


//
// Main function
//

int main()
{
    // PvPipeline used to acquire images from a device
    cout << "1. PvMultiSourceSample - image acquisition from a multi-source device" << endl << endl;
    AcquireImages();

    cout << "<press a key to exit>" << endl;
    PvWaitForKeyPress();

    return 0;
}

