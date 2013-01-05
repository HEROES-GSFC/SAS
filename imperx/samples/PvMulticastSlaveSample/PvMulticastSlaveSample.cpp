// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// To receive images using PvPipeline
//

#include <PvSampleUtils.h>
#include <PvDeviceFinderWnd.h>
#include <PvStream.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>

PV_INIT_SIGNAL_HANDLER();

//
// Select the device we will be receiving data from. 
//
// We only return the IP address (which we need) and WILL NOT 
// connect to the device. We will just passively open a multicast
// stream as a receiver from this device.
//

bool SelectDevice( PvString &aIP )
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
	
	aIP = lDeviceInfo->GetIPAddress();

	return true;
}

//
// PvPipeline event handler class. Used to trap buffer too small events
// and let the pipeline know we want all the buffers to be resized immediately.
//

class PipelineEventSink : public PvPipelineEventSink
{
protected:

    void OnBufferTooSmall( PvPipeline *aPipeline, bool *aReallocAll, bool *aResetStats )
    {
        *aReallocAll = true;
        *aResetStats = true;

        printf( "Buffers reallocated by the pipeline\n" );
    }

};

//
// Starts a multicast slave
//

bool StartSlave()
{
	// Let the user select the device to receive from
	PvString lDeviceIP;
	if ( !SelectDevice( lDeviceIP ) )
	{
		return false;
	}
	
	// Create the PvStream object
	PvStream lStream;

	// Create the PvPipeline object
	PvPipeline lPipeline( &lStream );

    // Create a PvPipeline event sink (used to trap buffer too small events)
    PipelineEventSink lPipelineEventSink;
    lPipeline.RegisterEventSink( &lPipelineEventSink );

	// Open stream
	printf( "Opening stream\n" );
	lStream.Open( lDeviceIP, "239.192.1.1", 1042 );

	// IMPORTANT: the pipeline needs to be "armed", or started before 
	// we instruct the device to send us images
	printf( "Starting pipeline\n" );
    lPipeline.SetBufferCount( 16 );
	lPipeline.Start();

	// Get stream parameters/stats
	PvGenParameterArray *lStreamParams = lStream.GetParameters();
	PvGenInteger *lCount = dynamic_cast<PvGenInteger *>( lStreamParams->Get( "ImagesCount" ) );
	PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRateAverage" ) );
	PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "BandwidthAverage" ) );
	PvGenBoolean *lIgnoreMissingPackets = dynamic_cast<PvGenBoolean *>( lStreamParams->Get( "IgnoreMissingPackets" ) );

	// Disabling resend packets
	lIgnoreMissingPackets->SetValue( true );

	char lDoodle[] = "|\\-|-/";
	int lDoodleIndex = 0;
	PvInt64 lImageCountVal = 0;
	double lFrameRateVal = 0.0;
	double lBandwidthVal = 0.0;

	// Acquire images until the user instructs us to stop
	printf( "\n<press a key to stop receiving>\n" );
	while ( !PvKbHit() )
	{
		// Retrieve next buffer		
		PvBuffer *lBuffer = NULL;
        PvResult  lOperationResult;
		PvResult lResult = lPipeline.RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );
		
        if ( lResult.IsOK() )
		{
            if (lOperationResult.IsOK())
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
			// release the buffer back to the pipeline
			lPipeline.ReleaseBuffer( lBuffer );
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

	// We stop the pipeline - letting the object lapse out of 
	// scope would have had the destructor do the same, but we do it anyway
	printf( "Stop pipeline\n" );
	lPipeline.Stop();

	// Now close the stream. Also optionnal but nice to have
	printf( "Closing stream\n" );
	lStream.Close();

    // Unregister pipeline event sink. Optional but nice to have.
    lPipeline.UnregisterEventSink( &lPipelineEventSink );

	return true;
}


//
// Main function
//

int main()
{
	// Starts the multicast slave
	printf( "1. Starts the multicast slave\n\n" );
	StartSlave();

	printf( "\n<press a key to exit>\n" );
    PvWaitForKeyPress();

	return 0;
}

