#include <ImperxStream.hpp>

ImperxStream::ImperxStream()
: lStream()
, lPipeline( &lStream )
{
    lDeviceInfo = 0;
}

int ImperxStream::Connect()
{
    PvResult lResult;	

    // Create an GEV system and an interface.
    PvSystem lSystem;

    // Find all GEV Devices on the network.
    lSystem.SetDetectionTimeout( 2000 );
    lResult = lSystem.Find();
    if( !lResult.IsOK() )
    {
	printf( "PvSystem::Find Error: %s", lResult.GetCodeString().GetAscii() );
	return -1;
    }

    // Get the number of GEV Interfaces that were found using GetInterfaceCount.
    PvUInt32 lInterfaceCount = lSystem.GetInterfaceCount();

    // Display information about all found interface
    // For each interface, display information about all devices.
    for( PvUInt32 x = 0; x < lInterfaceCount; x++ )
    {
	// get pointer to each of interface
	PvInterface * lInterface = lSystem.GetInterface( x );

	printf( "Interface %i\nMAC Address: %s\nIP Address: %s\nSubnet Mask: %s\n\n",
		x,
		lInterface->GetMACAddress().GetAscii(),
		lInterface->GetIPAddress().GetAscii(),
		lInterface->GetSubnetMask().GetAscii() );

	// Get the number of GEV devices that were found using GetDeviceCount.
	PvUInt32 lDeviceCount = lInterface->GetDeviceCount();

	for( PvUInt32 y = 0; y < lDeviceCount ; y++ )
	{
	    lDeviceInfo = lInterface->GetDeviceInfo( y );
	    printf( "Device %i\nMAC Address: %s\nIP Address: %s\nSerial number: %s\n\n",
		    y,
		    lDeviceInfo->GetMACAddress().GetAscii(),
		    lDeviceInfo->GetIPAddress().GetAscii(),
		    lDeviceInfo->GetSerialNumber().GetAscii() );
	}
    }

    // Connect to the last GEV Device found.
    if( lDeviceInfo != NULL )
    {
	printf( "Connecting to %s\n",
		lDeviceInfo->GetMACAddress().GetAscii() );

	lResult = lDevice.Connect( lDeviceInfo );
	if ( !lResult.IsOK() )
	{
	    printf( "Unable to connect to %s\n", 
		    lDeviceInfo->GetMACAddress().GetAscii() );
	}
	else
	{
	    printf( "Successfully connected to %s\n", 
		    lDeviceInfo->GetMACAddress().GetAscii() );
	}
    }
    else
    {
	printf( "No device found\n" );
    }

    // Get device parameters need to control streaming
    lDeviceParams = lDevice.GetGenParameters();

    return 0;
}

int ImperxStream::Connect(const std::string &IP)
{
    PvResult lResult;
    
    // Create an GEV system and an interface.
    PvSystem lSystem;

    // Find all GEV Devices on the network.
    lSystem.SetDetectionTimeout( 2000 );
    lResult = lSystem.Find();
    if( !lResult.IsOK() )
    {
	//Failed to find PvAnything
	printf( "PvSystem::Find Error: %s", lResult.GetCodeString().GetAscii() );
	return -1;
    }

    // Get the number of GEV Interfaces that were found using GetInterfaceCount.
    PvUInt32 lInterfaceCount = lSystem.GetInterfaceCount();

    // Search through interfaces for any devices
    // Check devices for correct target IP
    for( PvUInt32 x = 0; x < lInterfaceCount; x++ )
    {
	// get pointer to each of interface
	PvInterface * lInterface = lSystem.GetInterface( x );

	// Get the number of GEV devices that were found using GetDeviceCount.
	PvUInt32 lDeviceCount = lInterface->GetDeviceCount();

	for( PvUInt32 y = 0; y < lDeviceCount ; y++ )
	{
	    PvDeviceInfo *tDeviceInfo = lInterface->GetDeviceInfo( y );
	    std::string laddress(tDeviceInfo->GetIPAddress().GetAscii());
	    if (!laddress.compare(IP))
	    {
		lDeviceInfo = tDeviceInfo;
		printf( "Interface %i\nMAC Address: %s\nIP Address: %s\nSubnet Mask: %s\n\n",
			x,
			lInterface->GetMACAddress().GetAscii(),
			lInterface->GetIPAddress().GetAscii(),
			lInterface->GetSubnetMask().GetAscii() );
		printf( "Device %i\nMAC Address: %s\nIP Address: %s\nSerial number: %s\n\n",
			y,
			tDeviceInfo->GetMACAddress().GetAscii(),
			tDeviceInfo->GetIPAddress().GetAscii(),
			tDeviceInfo->GetSerialNumber().GetAscii() );
	    }
	}
    }

    // Connect to the last GEV Device found.
    if( lDeviceInfo != NULL )
    {
	printf( "Connecting to %s\n",
		lDeviceInfo->GetMACAddress().GetAscii() );

	lResult = lDevice.Connect( lDeviceInfo );
	if ( !lResult.IsOK() )
	{
	    printf( "Unable to connect to %s\n", 
		    lDeviceInfo->GetMACAddress().GetAscii() );
	}
	else
	{
	    printf( "Successfully connected to %s\n", 
		    lDeviceInfo->GetMACAddress().GetAscii() );
	}
    }
    else
    {
	printf( "No device found\n" );
    }
    // Get device parameters need to control streaming
    PvGenParameterArray *lDeviceParams = lDevice.GetGenParameters();

    return 0;
}

void ImperxStream::Initialize()
{
    // Negotiate streaming packet size
    lDevice.NegotiatePacketSize();

    // Open stream - have the PvDevice do it for us
    printf( "Opening stream to device\n" );
    lStream.Open( lDeviceInfo->GetIPAddress() );

    // Reading payload size from device
    PvInt64 lSize = 0;
    lDeviceParams->GetIntegerValue( "PayloadSize", lSize );

    // Set the Buffer size and the Buffer count
    lPipeline.SetBufferSize( static_cast<PvUInt32>( lSize ) );
    lPipeline.SetBufferCount( 16 ); // Increase for high frame rate without missing block IDs

    // Have to set the Device IP destination to the Stream
    lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort() ); 
}
    
void ImperxStream::Start(char &frame, Semaphore &frame_semaphore, Flag &stream_flag)
{
    // IMPORTANT: the pipeline needs to be "armed", or started before 
    // we instruct the device to send us images
    printf( "Starting pipeline\n" );
    lPipeline.Start();

    // Get stream parameters/stats
    PvGenParameterArray *lStreamParams = lStream.GetParameters();

    // TLParamsLocked is optional but when present, it MUST be set to 1
    // before sending the AcquisitionStart command
    lDeviceParams->SetIntegerValue( "TLParamsLocked", 1 );

    printf( "Resetting timestamp counter...\n" );
    lDeviceParams->ExecuteCommand( "GevTimestampControlReset" );

    // The pipeline is already "armed", we just have to tell the device
    // to start sending us images
    printf( "Sending StartAcquisition command to device\n" );
    lDeviceParams->ExecuteCommand( "AcquisitionStart" );

    char lDoodle[] = "|\\-|-/";
    int lDoodleIndex = 0;
    PvInt64 lImageCountVal = 0;
    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;

    // Acquire images until the user instructs us to stop
    printf( "\n<press a key to stop streaming>\n" );
    while ( stream_flag.check() )
    {
        // Retrieve next buffer		
        PvBuffer *lBuffer = NULL;
        PvResult  lOperationResult;
        PvResult lResult = lPipeline.RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );
        
        if ( lResult.IsOK() )
        {
            if ( lOperationResult.IsOK() )
            {
                // Process Buffer
		lStreamParams->GetIntegerValue( "ImagesCount", lImageCountVal );
		lStreamParams->GetFloatValue( "AcquisitionRateAverage", lFrameRateVal );
		lStreamParams->GetFloatValue( "BandwidthAverage", lBandwidthVal );
            
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

	    //semaphore thing
	    //get all in there.
	    //a semaphore thing
	    
            lPipeline.ReleaseBuffer( lBuffer );
        }
        else
        {
            // Timeout
            printf( "%c Timeout\r", lDoodle[ lDoodleIndex ] );
        }

        ++lDoodleIndex %= 6;
    
    }
}

void ImperxStream::Stop()
{
    // Tell the device to stop sending images
    printf( "Sending AcquisitionStop command to the device\n" );
    lDeviceParams->ExecuteCommand( "AcquisitionStop" );

    // If present reset TLParamsLocked to 0. Must be done AFTER the 
    // streaming has been stopped
    lDeviceParams->SetIntegerValue( "TLParamsLocked", 0 );

    // We stop the pipeline - letting the object lapse out of 
    // scope would have had the destructor do the same, but we do it anyway
    printf( "Stop pipeline\n" );
    lPipeline.Stop();

    // Now close the stream. Also optionnal but nice to have
    printf( "Closing stream\n" );
    lStream.Close();
}

void ImperxStream::Disconnect()
{
    printf( "Disconnecting device\n" );
    lDevice.Disconnect();
}

int main(void)
{
    ImperxStream SweetThang;
    char lazy;
    Semaphore whatever;
    Flag givup;
    SweetThang.Connect();
    SweetThang.Initialize();
    SweetThang.Start(lazy, whatever, givup);
    fine_wait(15,0,0,0);
    SweetThang.Stop();
    SweetThang.Disconnect();
    return 0;
}
