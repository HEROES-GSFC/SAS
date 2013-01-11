// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvFilterRGB.h>
#include <PvBuffer.h>
#include <PvDeviceFinderWnd.h>
#include <PvDevice.h>
#include <PvStream.h>
#include <PvBufferWriter.h>
#include <PvBufferConverter.h>

PV_INIT_SIGNAL_HANDLER();

//
// Main function
//

int main()
{
	int					Width;
	int					Height;
	PvStream			pvStream;
	PvDevice			pvDevice;

	PvGenEnum* lPvGenEnum;
	PvGenInteger *lPvGenInteger ;

	// ********************************* //
	// Step 1: Select the device         //
	// ********************************* //
    printf( "\n1. Select a device to connect" );
	PvDeviceFinderWnd pvDeviceFinderWnd;
	if ( !pvDeviceFinderWnd.ShowModal().IsOK() )
	{ 
		printf( "\n  No device is selected" );
		return 0;
	}
	PvDeviceInfo* pvDeviceInfo = pvDeviceFinderWnd.GetSelected();
	if( pvDeviceInfo == NULL )
	{
		return 0;
	}

	// ******************************** //
	// Step 2: Connect to the device    //
	// ******************************** //
    printf( "\n2. Connecting to the selected device......" );
	PvResult lResult = pvDevice.Connect( pvDeviceInfo, PvAccessControl );
    if(lResult != 0)
	{ 
		printf( "\n  Failed to connect to device" );
		return 0;
	}

    // Perform automatic packet size negociation
    lResult = pvDevice.NegotiatePacketSize( );
    if ( !lResult.IsOK() )
    {
        printf( "\n Failed to negotiate a packet size setting GevSCPSPacketSize to original value" );
        PvSleepMs( 2500 );
    }

	// ************************************************* //
	// Step 3: Open and set stream destination    //
	// ************************************************* //
    printf( "\n3. Open stream......" );
	lResult = pvStream.Open( pvDeviceInfo->GetIPAddress() );
	if ( !lResult.IsOK() )
	{	
		printf( "\n  Failed to open stream" );
		return 0;
	}
	pvDevice.SetStreamDestination( pvStream.GetLocalIPAddress(), pvStream.GetLocalPort() );

	// Note: Set long Heartbeat Timeout for the debugging purpose
	// If the program is stopped at the breakpoint too long then the
	// hertbeat may expired

#ifdef _DEBUG
	lPvGenInteger = dynamic_cast<PvGenInteger *>( pvDevice.GetGenParameters()->Get( "GevHeartbeatTimeout" ) );
	lPvGenInteger->SetValue(60000);
#endif

    // ****************************************************** //
	// Step 4: Get image size information and prepare buffer  //
	// ****************************************************** //
	PvInt64 value;
	lPvGenInteger = dynamic_cast<PvGenInteger *>(pvDevice.GetGenParameters()->Get( "Width" ) );
	lPvGenInteger->GetValue(value);
	 Width = (int)value;
  
	
	lPvGenInteger = dynamic_cast<PvGenInteger *>(pvDevice.GetGenParameters()->Get( "Height" ) );
	lPvGenInteger->GetValue(value);
	Height = (int)value;

    PvInt64  i64PixelFormat;
	lPvGenEnum = dynamic_cast<PvGenEnum *>(pvDevice.GetGenParameters()->Get("PixelFormat"));
	lPvGenEnum->GetValue(i64PixelFormat);
	PvString pvString;
	lPvGenEnum->GetValue(pvString);
	const char* name = pvString.GetAscii();
	printf( "\n4. Image size: %d x %d, Pixel Type: %s", Width, Height, name );

	const char* charTemp = strstr(name, "Mono");
	if ( charTemp != NULL )
	{
		printf( "\n\nThe pixel type is not color, you could not see the effect of white balance!\n" );
	}

	// Create the buffer
	PvBuffer * lBuffer = new PvBuffer();

	// Alloc buffer (buffers are created as images, GetImage() is valid on a fresh buffer)
	lBuffer->GetImage()->Alloc( 640, 480, PvPixelMono8 );

	PvBuffer * lPtr; 

	// ************************************************* //
	// Step 5: Grab image                                //
	// ************************************************* //
	printf( "\n5. Grab one image\n" );
	pvStream.QueueBuffer( lBuffer );
	PvGenParameterArray *pvDeviceParams = pvDevice.GetGenParameters();
	PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( pvDeviceParams->Get( "AcquisitionStart" ) );
	PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( pvDeviceParams->Get( "AcquisitionStop" ) );
	
	lStart->Execute();
	PvResult lStreamResult;
	lResult = pvStream.RetrieveBuffer( &lPtr, &lStreamResult, 10000 );
	lStop->Execute();
	if (lResult.IsOK())
	{
		if ( lStreamResult.IsOK() )
		{
			// ************************************************* //
			// Step 6: Using RGB Filter                          //
			// ************************************************* //
			printf( "\n6. Using RGB Filter");

			// Alloc buffer (buffers are created as images, GetImage() is valid on a fresh buffer)
			PvBuffer * lBufferRGB32 = new PvBuffer();
			lBufferRGB32->GetImage()->Alloc( Width, Height, PvPixelWinRGB32 );

			// Alloc buffer (buffers are created as images, GetImage() is valid on a fresh buffer)
			PvBuffer * lBufferDst = new PvBuffer();
			lBufferDst->GetImage()->Alloc( Width, Height, PvPixelWinRGB32 );

			PvBufferConverter lBufferConverter;
			lResult = lBufferConverter.Convert( lPtr, lBufferRGB32, true );

			// Create RGB filter, has default gains: 1 and offsets: 0
			PvFilterRGB lFilter;
			PvBufferWriter lBufferWriter;

			//Save original image
			printf( "\n  a. Save the original image into ImageOriginal.bmp" );
			lBufferWriter.Store( lBufferRGB32, "ImageOriginal.bmp", PvBufferFormatBMP );

			// Enhance the Red
			printf( "\n  b. Enhance the Red color and save the image into ImageRed.bmp" );
			lFilter.SetGainR( 2 );
			lFilter.SetOffsetR( 100 );
			memcpy(lBufferDst->GetDataPointer(), lBufferRGB32->GetDataPointer(), lBufferRGB32->GetSize());
			lFilter.Apply( lBufferDst );
			lBufferWriter.Store( lBufferDst, "ImageRed.bmp", PvBufferFormatBMP );
			lFilter.SetGainR(1);
			lFilter.SetOffsetR( 0 );
	
			// Enhance the Blue
			printf( "\n  c. Enhance the Blue color and save the image into ImageBlue.bmp" );
			lFilter.SetGainB( 2 );
			lFilter.SetOffsetB( 100 );
			memcpy(lBufferDst->GetDataPointer(), lBufferRGB32->GetDataPointer(), lBufferRGB32->GetSize() );
			lFilter.Apply( lBufferDst );
			lBufferWriter.Store( lBufferDst, "ImageBlue.bmp", PvBufferFormatBMP );
			lFilter.SetGainB( 1);
			lFilter.SetOffsetB( 0 );

			// Enhance the Green
			printf( "\n  d. Enhance the Green color and save the imageinto ImageGreen.bmp" );
			lFilter.SetGainG( 2 );
			lFilter.SetOffsetG( 100 );
			memcpy(lBufferDst->GetDataPointer(), lBufferRGB32->GetDataPointer(), lBufferRGB32->GetSize() );
			lFilter.Apply( lBufferDst );
			lBufferWriter.Store( lBufferDst, "ImageGreen.bmp", PvBufferFormatBMP );
			//lFilter.SetGainG( 1 );
			//lFilter.SetOffsetG( 0 );

			// Do White Balance
			printf( "\n  e. Do white balance and save the image into ImageBalanced.bmp\n" );
			lFilter.WhiteBalance( lBufferRGB32 );
			lFilter.Apply( lBufferRGB32 );
			lBufferWriter.Store( lBufferRGB32, "ImageBalanced.bmp", PvBufferFormatBMP );
			lBufferRGB32->Free();
			lBufferDst->Free();
		}
	}
	
	// 
  	lBuffer->Free();
	pvDevice.ResetStreamDestination();
	pvStream.Close();
	pvDevice.Disconnect();
		
	printf( "\n<press a key to exit>\n" );
    PvWaitForKeyPress();

	return 0;
}

