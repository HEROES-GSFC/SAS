// *****************************************************************************
//
//     Copyright (c) 2010, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// To receive images using PvStream directly
//

#include <PvSampleUtils.h>
#include <PvDeviceFinderWnd.h>
#include <PvDevice.h>
#include <PvBuffer.h>
#include <PvStream.h>
#include <PvStreamRaw.h>

PV_INIT_SIGNAL_HANDLER();

#define BUFFER_COUNT ( 16 )


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
        printf( "No device selected.\n" );
        return false;
    }

    // Connect to the GEV Device
    PvDevice lDevice;
    printf( "Connecting to %s\n", lDeviceInfo->GetMACAddress().GetAscii() );
    if ( !lDevice.Connect( lDeviceInfo ).IsOK() )
    {
        printf( "Unable to connect to %s\n", lDeviceInfo->GetMACAddress().GetAscii() );
        return false;
    }
    printf( "Successfully connected to %s\n", lDeviceInfo->GetMACAddress().GetAscii() );
    printf( "\n" );

    // Get device parameters need to control streaming
    PvGenParameterArray *lDeviceParams = lDevice.GetGenParameters();
    PvGenInteger *lTLLocked = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "TLParamsLocked" ) );
    PvGenInteger *lPayloadSize = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "PayloadSize" ) );
    PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
    PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );

    // Negotiate streaming packet size
    lDevice.NegotiatePacketSize();

    // Create the PvStream object
    PvStream lStream;

    // Open stream - have the PvDevice do it for us
    printf( "Opening stream to device\n" );
    lStream.Open( lDeviceInfo->GetIPAddress() );

    // Reading payload size from device
    PvInt64 lSize = 0;
    lPayloadSize->GetValue( lSize );

    // Use min of BUFFER_COUNT and how many buffers can be queued in PvStream
    PvUInt32 lBufferCount = ( lStream.GetQueuedBufferMaximum() < BUFFER_COUNT ) ? 
        lStream.GetQueuedBufferMaximum() : 
        BUFFER_COUNT;

    // Create, alloc buffers
    PvBuffer *lBuffers = new PvBuffer[ lBufferCount ];
    for ( PvUInt32 i = 0; i < lBufferCount; i++ )
    {
        lBuffers[ i ].Alloc( static_cast<PvUInt32>( lSize ) );
    }

    // Have to set the Device IP destination to the Stream
    lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort() );

    // Get stream parameters/stats
    PvGenParameterArray *lStreamParams = lStream.GetParameters();
    PvGenInteger *lCount = dynamic_cast<PvGenInteger *>( lStreamParams->Get( "ImagesCount" ) );
    PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRateAverage" ) );
    PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "BandwidthAverage" ) );

    // Queue all buffers in the stream
    for ( PvUInt32 i = 0; i < lBufferCount; i++ )
    {
        lStream.QueueBuffer( lBuffers + i );
    }

    // TLParamsLocked is optional but when present, it MUST be set to 1
    // before sending the AcquisitionStart command
    if ( lTLLocked != NULL )
    {
        printf( "Setting TLParamsLocked to 1\n" );
        lTLLocked->SetValue( 1 );
    }

    printf( "Resetting timestamp counter...\n" );
    PvGenCommand *lResetTimestamp = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "GevTimestampControlReset" ) );
    lResetTimestamp->Execute();

    // The buffers are queued in the stream, we just have to tell the device
    // to start sending us images
    printf( "Sending StartAcquisition command to device\n" );
    PvResult lResult = lStart->Execute();

    char lDoodle[] = "|\\-|-/";
    int lDoodleIndex = 0;
    PvInt64 lImageCountVal = 0;
    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;

    // Acquire images until the user instructs us to stop
    printf( "\n<press a key to stop streaming>\n" );
    while ( !PvKbHit() )
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;

        // Retrieve next buffer		
        PvResult lResult = lStream.RetrieveBuffer( &lBuffer, &lOperationResult, 1000 );
        if ( lResult.IsOK() )
        {
        	if(lOperationResult.IsOK())
        	{
                //
                // We now have a valid buffer. This is where you would typically process the buffer.
                // -----------------------------------------------------------------------------------------
                // ...

				lCount->GetValue( lImageCountVal );
				lFrameRate->GetValue( lFrameRateVal );
				lBandwidth->GetValue( lBandwidthVal );

				// If the buffer contains an image, display width and height
				PvUInt32 lWidth = 0, lHeight = 0;
				if ( lBuffer->GetPayloadType() == PvPayloadTypeImage )
				{
					// Get image specific buffer interface
					PvImage *lImage = lBuffer->GetImage();

					// Read width, height
					lWidth = lBuffer->GetImage()->GetWidth();
					lHeight = lBuffer->GetImage()->GetHeight();
				}

				printf( "%c BlockID: %016llX W: %i H: %i %.01f FPS %.01f Mb/s\r",
					lDoodle[ lDoodleIndex ],
					lBuffer->GetBlockID(),
					lWidth,
					lHeight,
					lFrameRateVal,
					lBandwidthVal / 1000000.0 );
        	}
			// We have an image - do some processing (...) and VERY IMPORTANT,
			// re-queue the buffer in the stream object
			lStream.QueueBuffer( lBuffer );

        }
        else
        {
            // Timeout
            printf( "%c Timeout\r", lDoodle[ lDoodleIndex ] );
        }

        ++lDoodleIndex %= 6;
    }

    PvGetChar(); // Flush key buffer for next stop
    printf( "\n\n" );

    // Tell the device to stop sending images
    printf( "Sending AcquisitionStop command to the device\n" );
    lStop->Execute();

    // If present reset TLParamsLocked to 0. Must be done AFTER the 
    // streaming has been stopped
    if ( lTLLocked != NULL )
    {
        printf( "Resetting TLParamsLocked to 0\n" );
        lTLLocked->SetValue( 0 );
    }

    // Abort all buffers from the stream, unqueue
    printf( "Aborting buffers still in stream\n" );
    lStream.AbortQueuedBuffers();
    while ( lStream.GetQueuedBufferCount() > 0 )
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;

        lStream.RetrieveBuffer( &lBuffer, &lOperationResult );

        printf( "  Post-abort retrieved buffer: %s\n", lOperationResult.GetCodeString().GetAscii() );
    }

    // Release buffers
    printf( "Releasing buffers\n" );
    delete []lBuffers;

    // Now close the stream. Also optionnal but nice to have
    printf( "Closing stream\n" );
    lStream.Close();

    // Finally disconnect the device. Optional, still nice to have
    printf( "Disconnecting device\n" );
    lDevice.Disconnect();

    return true;
}


//
// Main function
//

int main()
{
    // PvPipeline used to acquire images from a device
    printf( "1. PvStream sample - image acquisition from a device\n\n" );
    AcquireImages();

    printf( "\n<press a key to exit>\n" );
    PvWaitForKeyPress();

    return 0;
}

