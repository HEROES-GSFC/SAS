// *****************************************************************************
//
// Copyright (c) 2011, Pleora Technologies Inc., All rights reserved.
//
// -----------------------------------------------------------------------------
// This sample code illustrates how to use the PvStream and PvPipeline classes 
// to receive raw data using the eBUS SDK.
//
// Using the PvDeviceFinderWnd class, this sample enumerates available GEV 
// devices to the user and allows the user to select a GEV device. Note that the 
// selected device must transmit raw data on channel 0.
// Using the PvStream and PvPipeline classes, this sample illustrates how to 
// receive raw data from a eBUS Transmitter.
// A PvBuffer is used to hold the raw data once it is received.
//
// By default, this sample receives raw buffers passively from multicast address 
// 239.192.1.1:1042. However, it can be used in many different ways by providing 
// it with optional command line arguments. For further details on the command 
// line options that are available, run the sample with the argument --help.
//
// Also note that by default, this sample receives data passively (it connects 
// as a pure data receiver). However, if the --connectdevice command line argument 
// is supplied, a PvDevice is instantiated to control the transmitting entity and 
// start streaming. This mode of operation is not supported by all eBUS Transmitters.
//
// Refer to the readme in PvTransmitRawSample for how to test transmission and 
// reception of raw buffers by having PvReceiveRawSample receive data from 
// PvTransmitRawSample.
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvSystem.h>
#include <PvInterface.h>
#include <PvStream.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvDeviceFinderWnd.h>

#include <string>

PvUInt64 GetTickCountMs();
void SleepMs( PvUInt32 aSleepTime );
void PrintHelp();

PV_INIT_SIGNAL_HANDLER();

int main( int aCount, const char ** aArgs )
{
    char lDeviceAddress[1024];
    char lMulticastAddress[1024];
    char lLocalAddress[1024];

    memset( lLocalAddress, 0, 1024 );
    sprintf( lMulticastAddress, "239.192.1.1" );
    memset( lDeviceAddress, 0, 1024 );

    bool lPassive = true;
    PvUInt32 lChannel = 0;
    PvUInt16 lHostPort = 1042;
    PvResult lResult;
    for ( int i=1; i<aCount; i++ )
	{
		std::string lString = aArgs[i];
        if ( lString.find( "--hostport" ) != std::string::npos )
        {
            sscanf( aArgs[i], "--hostport=%d", &lHostPort );
        }
        else if ( lString.find( "--localaddress" ) != std::string::npos )
        {
            sscanf( aArgs[i], "--localaddress=%s", lLocalAddress );
        }
        else if ( lString.find( "--multicastaddress" ) != std::string::npos )
        {
            sscanf( aArgs[i], "--multicastaddress=%s", lMulticastAddress );
        }
        else if ( lString.find( "--deviceaddress" ) != std::string::npos )
        {
            sscanf( aArgs[i], "--deviceaddress=%s", lDeviceAddress );
        }
        else if ( lString.find( "--unicast" ) != std::string::npos )
        {
            memset( lMulticastAddress, 0, 1024 );
        }
        else if ( lString.find( "--connectdevice" ) != std::string::npos )
        {
            lPassive = false;
        }
        else if ( lString.find( "--channel" ) != std::string::npos )
        {
            sscanf( aArgs[i], "--channel=%d", &lChannel );
        }
        else if ( lString.find( "--help" ) != std::string::npos )
        {
            PrintHelp();
            return 0;
        }
        else
        {
            printf( "Did not recognize argument %s\n", aArgs[i] );
            PrintHelp();
            return 1;
        }
    }

    if ( strlen( lDeviceAddress ) == 0 )
    {
        // No device address specified. Prompt with the device finder.
        PvDeviceFinderWnd lWnd;
        if ( !lWnd.ShowModal().IsOK() )
        {
            printf( "No GEV device selected.\n" );
            return 1;
        }
        PvDeviceInfo* lInfo = lWnd.GetSelected();
        sprintf( lDeviceAddress, "%s", lInfo->GetIPAddress().GetAscii() );
    }

    PvStream lStream;

    if ( strlen( lMulticastAddress ) == 0 )
    {
        lResult = lStream.Open( lDeviceAddress, lHostPort, lChannel, lLocalAddress );
        printf( "Receiving from device %s on interface %s:%d\n", 
            lDeviceAddress, lStream.GetLocalIPAddress().GetAscii(), lStream.GetLocalPort() );
    }
    else
    {
        lResult = lStream.Open( lDeviceAddress, lMulticastAddress, lHostPort, lChannel, lLocalAddress );
        printf( "Receiving from multicast address %s:%d (device %s) on interface %s:%d\n",
            lMulticastAddress, lHostPort, lDeviceAddress, lStream.GetLocalIPAddress().GetAscii(), lStream.GetLocalPort() );
    }

    if ( !lResult.IsOK() )
    {
        printf( "Failed opening the incoming stream: %s\n", lResult.GetDescription().GetAscii() );
        return 1;
    }

    PvPipeline lPipeline( &lStream );

    PvDevice lDevice;
    PvGenParameterArray *lDeviceParams = NULL;
    if ( !lPassive )
    {
        lResult = lDevice.Connect( lDeviceAddress );
        if ( !lResult.IsOK() )
        {
            printf( "Failed connecting to the device to set its destination and initiate an AcquisitionStart: %s\n", 
                lResult.GetDescription().GetAscii() );
            printf( "If the eBUS Transmitter to receive from doesn't have full device capabilities, add the --passive command line option and initiate streaming manually.\n" );
            return 1;
        }
        lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort(), lChannel );
            
        // Get device parameters need to control streaming
        lDeviceParams = lDevice.GetGenParameters();

        // Reading payload size from device. Otherwise, the pipeline may miss the first several images.
        PvInt64 lReceivePayloadSize = 0;
	    lDeviceParams->GetIntegerValue( "PayloadSize", lReceivePayloadSize );

        // Set the Buffer size and the Buffer count
        lPipeline.SetBufferSize( static_cast<PvUInt32>( lReceivePayloadSize ) );
    }

    lPipeline.SetBufferCount( 16 ); // Increase for high frame rate without missing block IDs
    lPipeline.Start();

    if ( !lPassive )
    {
        // TLParamsLocked is optional but when present, it MUST be set to 1
        // before sending the AcquisitionStart command
	    lDeviceParams->SetIntegerValue( "TLParamsLocked", 1 );

	    lDeviceParams->ExecuteCommand( "GevTimestampControlReset" );

        // The pipeline is already "armed", we just have to tell the device
        // to start sending us images
	    lDeviceParams->ExecuteCommand( "AcquisitionStart" );
    }
    
    // Get stream parameters/stats
    PvGenParameterArray *lStreamParams = lStream.GetParameters();

    printf( "Press any key to stop receiving.  \n" );

    char lDoodle[] = "|\\-|-/";
    int lDoodleIndex = 0;
    PvInt64 lImageCountVal = 0;
    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;

    while ( !PvKbHit() )
    {
        PvBuffer *lBuffer = NULL;
        PvResult  lOperationResult;
        PvResult lResult = lPipeline.RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );
        
        if ( lResult.IsOK() )
        {
            if ( lOperationResult.IsOK() )
            {
                //
                // We now have a valid buffer. This is where you would typically process the buffer.
                // -----------------------------------------------------------------------------------------
                // ...

                lStreamParams->GetIntegerValue( "ImagesCount", lImageCountVal );
                lStreamParams->GetFloatValue( "AcquisitionRateAverage", lFrameRateVal );
                lStreamParams->GetFloatValue( "BandwidthAverage", lBandwidthVal );

                printf( "%c BlockID: %016llX %.01f FPS %.01f Mb/s\r", 
                    lDoodle[ lDoodleIndex ],
                    lBuffer->GetBlockID(),
                    lFrameRateVal,
                    lBandwidthVal / 1000000.0 ); 
            }
            // We have an image - do some processing (...) and VERY IMPORTANT,
            // release the buffer back to the pipeline
            lPipeline.ReleaseBuffer( lBuffer );
        }
        else
        {
            printf( "%c Timeout\r", lDoodle[ lDoodleIndex ] );
        }

        ++lDoodleIndex %= 6;
    }
}

void PrintHelp()
{
    printf( "This utility receives raw data from a transmitter.\n" );
    printf( "Use this in conjunction with PvTransmitRawSample to illustrate transmitting and receiving raw data using the GigEVision Streaming Protocol (GVSP).\n\n\n" );
    printf( "Optional command line arguments:\n\n" );
    printf( "--hostport=<port to which the device should send> \n(default 1042)\n\n" );
    printf( "--localaddress=<address of the interface that is to receive the data> \n(default empty - the first interface that can reach the device)\n\n" );
    printf( "--multicastaddress=<address to which the device should send and to which the receiver should subscribe> \n(default 239.192.1.1)\n\n" );
    printf( "--deviceaddress=<address of the device from which to receive> \n(Default behaviour opens the device finder window to allow the user to select a device.)\n\n" );
    printf( "--unicast \nConnect as a unicast receiver. In this case the multicast address would be ignored.\n\n" );
    printf( "--connectdevice \nInitiate a GVCP connection with the device and issue an acquisition start command.\nBy default, this does not occur and the receiver expects the device to be controled by another entity.\n\n" );
    printf( "--channel=<streaming channel to receive from> \n(default 0)\n\n" );
    printf( "--help \nPrint this help message.\n" );

}

