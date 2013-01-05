// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// This sample shows you how to control features programatically.
//

#include <PvSampleUtils.h>
#include <PvDeviceFinderWnd.h>
#include <PvDevice.h>
#include <PvGenParameterArray.h>
#include <PvGenParameter.h>
#include <PvStream.h>

PV_INIT_SIGNAL_HANDLER();

//
// Have the user select a device and connect the PvDevice object
// to the user's selection.
//

bool Connect( PvDevice &aDevice, PvString &aIP )
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
	printf( "Connecting to %s\n",
		lDeviceInfo->GetMACAddress().GetAscii() );
	if ( !aDevice.Connect( lDeviceInfo, PvAccessReadOnly ).IsOK() )
	{
		printf( "Unable to connect to %s\n", 
			lDeviceInfo->GetMACAddress().GetAscii() );
		return false;
	}
	printf( "Successfully connected to %s\n", 
		lDeviceInfo->GetMACAddress().GetAscii() );

	// Get the IP address to open stream later
	aIP = lDeviceInfo->GetIPAddress();

	return true;
}

//
// Have the user select a device and connect the PvDevice object
// to the user's selection.
//

bool Connect( PvDevice &aDevice )
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
	printf( "Connecting to %s\n",
		lDeviceInfo->GetMACAddress().GetAscii() );
	if ( !aDevice.Connect( lDeviceInfo, PvAccessControl ).IsOK() )
	{
		printf( "Unable to connect to %s\n", 
			lDeviceInfo->GetMACAddress().GetAscii() );
		return false;
	}
	printf( "Successfully connected to %s\n", 
		lDeviceInfo->GetMACAddress().GetAscii() );

	return true;
}


//
// Dumps the full content of a PvGenParameterArray
//

bool DumpGenParameterArray( PvGenParameterArray *aArray )
{
	// Getting array size
	PvUInt32 lParameterArrayCount = aArray->GetCount();
	printf( "\nArray has %d parameters\n", lParameterArrayCount );

	// Traverse through Array and print out parameters available
	for( PvUInt32 x = 0; x < lParameterArrayCount; x++ )
	{
		// Get a parameter
		PvGenParameter *lGenParameter = aArray->Get( x );

        // Don't show invisible parameters - display everything up to Guru
        if ( !lGenParameter->IsVisible( PvGenVisibilityGuru ) )
        {
            continue;
        }

		// Get and print parameter's name
		PvString lGenParameterName, lCategory;
		lGenParameter->GetCategory( lCategory );
		lGenParameter->GetName( lGenParameterName );
		printf( "%s:%s, ", lCategory.GetAscii(), lGenParameterName.GetAscii() );

        // Parameter available?
        if ( !lGenParameter->IsAvailable() )
        {
            printf( "{Not Available}\n" );
			continue;
        }

		// Parameter readable?
		if ( !lGenParameter->IsReadable() )
		{
            printf( "{Not readable}\n" );
			continue;
		}
		
		// Get the parameter type
		PvGenType lType;
		lGenParameter->GetType( lType );
		switch ( lType )
		{
			// If the parameter is of type PvGenTypeInteger
			case PvGenTypeInteger:
				{
					PvInt64 lValue;				
					static_cast<PvGenInteger *>( lGenParameter )->GetValue( lValue );
					printf( "Integer: %d", lValue );
				}
				break;
			// If the parameter is of type PvGenTypeEnum
			case PvGenTypeEnum:
				{
					PvString lValue;				
					static_cast<PvGenEnum *>( lGenParameter )->GetValue( lValue );
					printf( "Enum: %s", lValue.GetAscii() );
				}
				break;
			// If the parameter is of type PvGenTypeBoolean
			case PvGenTypeBoolean:
				{
					bool lValue;				
					static_cast<PvGenBoolean *>( lGenParameter )->GetValue( lValue );
					if( lValue ) 
					{
						printf( "Boolean: TRUE" );
					}
					else 
					{
						printf( "Boolean: FALSE" );
					}
				}
				break;
			// If the parameter is of type PvGenTypeString
			case PvGenTypeString:
				{
					PvString lValue;
					static_cast<PvGenString *>( lGenParameter )->GetValue( lValue );
					printf( "String: %s", lValue.GetAscii() );
				}
				break;
			// If the parameter is of type PvGenTypeCommand
			case PvGenTypeCommand:
				printf( "Command" );
				break;
			// If the parameter is of type PvGenTypeFloat
			case PvGenTypeFloat:
				{
					double lValue;				
					static_cast<PvGenFloat *>( lGenParameter )->GetValue( lValue );
					printf( "Float: %f", lValue );
				}
				break;
		}
		printf("\n");
	}

	return true;
}


//
// Get PC's communication-related settings
//

bool GetPCCommunicationRelatedSettings()
{
	// Communication link can be configured before we connect to the device.
	// No need to connect to the device
	printf( "Using un-connected PvDevice\n" );
	PvDevice lDevice;

	// Get the communication link parameters array
	printf( "Retrieving communication link parameters array\n" );
	PvGenParameterArray* lGenLink = lDevice.GetGenLink();

	// Dumping communication link parameters array content
	printf( "Dumping communication link parameters array content\n" );
	DumpGenParameterArray( lGenLink );

	lDevice.Disconnect();

	return true;
}


//
// Get the GEV Device's settings
//

bool GetIPEngineSettings()
{
	// Connect to the GEV Device.
	PvDevice lDevice;
	if ( !Connect( lDevice ) )
	{
		return false;
	}

	// Get the device's parameters array. It is built from the 
	// GenICam XML file provided by the device itself
	printf( "Retrieving device's parameters array\n" );
	PvGenParameterArray* lParameters = lDevice.GetGenParameters();

	// Dumping device's parameters array content
	printf( "Dumping device's parameters array content\n" );
	DumpGenParameterArray( lParameters );

	// Get width parameter - mandatory GigE Vision parameter, it should be there
	PvGenParameter *lParameter = lParameters->Get( "Width" );

	// Converter generic parameter to width using dynamic cast. If the
	// type is right, the conversion will work otherwise lWidth will be NULL
	PvGenInteger *lWidthParameter = dynamic_cast<PvGenInteger *>( lParameter );

	// Read current width value
	PvInt64 lOriginalWidth = 0;
	if ( !(lWidthParameter->GetValue( lOriginalWidth ).IsOK()) )
	{
		printf( "Error retrieving width from device\n" );	
		return false;
	}

    // Read max
	PvInt64 lMaxWidth = 0;
	if ( !(lWidthParameter->GetMax( lMaxWidth ).IsOK()) )
	{
		printf( "Error retrieving width max from device\n" );	
		return false;
	}

	// Change width value
	if ( !lWidthParameter->SetValue( lMaxWidth ).IsOK() )
	{
		printf( "Error changing width on device - the device is on Read Only Mode, please change to Exclusive to change value\n" );	
		return false;
	} 

	// Reset width to original value
	if ( !lWidthParameter->SetValue( lOriginalWidth ).IsOK() )
	{
		printf( "Error changing width on device\n" );	
		return false;
	}

	return true;
}


//
// Get Image stream controller settings
//

bool GetImageStreamControllerSettings()
{
	// Connect to the GEV Device.
	PvDevice lDevice;
	PvString lIP;
	if ( !Connect( lDevice, lIP ) )
	{
		return false;
	}

	// Open a Stream
	printf( "Opening stream.\n" );
	PvStream lStream;
	if ( !lStream.Open( lIP ).IsOK() )
	{
		printf( "Error opening stream\n" );
		return false;
	}

	// Get the stream parameters. These are used to configure/control
	// some stream releated parameters and timings and provides
	// access to statistics from this stream
	printf( "Retrieving streams's parameters array\n" );
	PvGenParameterArray* lParameters = lStream.GetParameters();

	// Dumping device's parameters array content
	printf( "Dumping stream's parameters array content\n" );
	DumpGenParameterArray( lParameters );

	lStream.Close();

	return true;
}

//
// Main function
//

int main()
{
	// Communication link parameters display
	printf("1. Communication link parameters display\n\n");
	GetPCCommunicationRelatedSettings();

	// GEV Device parameters display
	printf("\n2. GEV Device parameters display\n\n");
	GetIPEngineSettings();

	// Image stream parameters display
	printf("\n3. Image stream parameters display\n\n");
	GetImageStreamControllerSettings();

	printf( "\n<press a key to exit>\n" );
    PvWaitForKeyPress();

	return 0;
}


