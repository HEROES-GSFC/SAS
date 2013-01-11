// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvDeviceFinderWnd.h>
#include <PvDevice.h>
#include <PvConfigurationWriter.h>
#include <PvConfigurationReader.h>

PV_INIT_SIGNAL_HANDLER();

// insert your desired file name here
#define FILE_NAME "PersistenceTest.pvxml"

//
// Choose and Connect a device
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
	if ( !aDevice.Connect( lDeviceInfo ).IsOK() )
	{
		printf( "Unable to connect to %s\n", 
			lDeviceInfo->GetMACAddress().GetAscii() );
		return false;
	}

	aIP = lDeviceInfo->GetIPAddress();

	printf( "Successfully connected to %s\n", 
		lDeviceInfo->GetMACAddress().GetAscii() );

	return true;
}

//
//	Store device and stream configuration.
//	Also store a string information
//

bool StoreConfiguration()
{
	PvDevice lDevice;
	PvString lIP;
	if ( !Connect( lDevice, lIP ) )
	{
		printf( "Unable to connect the device" );
		return false;
	}

	PvConfigurationWriter lWriter;

	// store with a PvDevice
	printf( "Store device configuration\n" );
	PvString lName = "DeviceConfiguration";
	lWriter.Store( &lDevice, lName );

	// store with a PvStream
	printf( "Store stream configuration\n" );
	PvStream lStream;
	lStream.Open( lIP );
	lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort() );
	lName = "StreamConfiguration";
	lWriter.Store( &lStream, lName );
	lDevice.ResetStreamDestination();

	// Store with a sample string
	printf( "Store string information\n" );
	PvString lString= "AnyKindOfInformation";
	lName = "StringInformation";
	lWriter.Store( lString, lName );

	// Insert the path of your file here
	printf( "Store string information\n" );
	PvString lFilenameToSave( FILE_NAME );

	lWriter.Save( lFilenameToSave );

	lStream.Close();
	lDevice.Disconnect();

	return true;
}

//
// Restore device configuration and verify that the device is connected 
//

bool RestoreDeviceConfigurationName()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	printf( "Load information and configuration\n" );
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the device information
	printf( "Restore configuration for a device with the configuration name\n" );
	PvDevice lDevice;
	lReader.Restore( "DeviceConfiguration", &lDevice );

	printf( "Verify the restoration\n" );
	if ( !lDevice.IsConnected() )
	{
		return false;
	}

	lDevice.Disconnect();

	return true;
}

//
// Restore device configuration and verify that the device is connected 
//

bool RestoreDeviceConfigurationIndex()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	printf( "Load information and configuration\n" );
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the device information
	printf( "Restore configuration for a device with the configuration index\n" );
	PvDevice lDevice;
	lReader.Restore( PvUInt32( 0 ), &lDevice );

	printf( "Verify the restoration\n" );
	if ( !lDevice.IsConnected() )
	{
		return false;
	}

	lDevice.Disconnect();

	return true;
}

//
// Restore stream configuration and verify that the stream is open
//

bool RestoreStreamConfigurationName()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	printf( "Load information and configuration\n" );
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the stream information
	printf( "Restore configuration for a stream with the configuration name\n" );
	PvStream lStream;
	PvString lStreamConfigStr( "StreamConfiguration" );
	lReader.Restore( lStreamConfigStr, lStream );

	printf( "Verify the restoration\n" );
	if ( !lStream.Close() )
	{
		return false;
	}

	return true;
}

//
// Restore stream configuration and verify that the stream is open
//

bool RestoreStreamConfigurationIndex()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	printf( "Load information and configuration\n" );
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the stream information
	printf( "Restore configuration for a stream with the configuration index\n" );
	PvStream lStream;
	lReader.Restore( PvUInt32( 0 ), lStream );

	printf( "Verify the restoration\n" );
	if ( !lStream.Close() )
	{
		return false;
	}

	return true;
}

//
// Restore string information and verify that the string is the same
//

bool RestoreStringInformationName()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	printf( "Load information and configuration\n" );
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the stream information
	printf( "Restore information for a string with the information name\n" );
	PvString lCheck;
	PvString lStringInfoStr( "StringInformation" );
	lReader.Restore( lStringInfoStr, lCheck );

	printf( "Verify the restoration\n" );
	if ( lCheck != "AnyKindOfInformation" )
	{
		return false;
	}

	return true;
}

//
// Restore string information and verify that the string is the same
//

bool RestoreStringInformationIndex()
{
	PvConfigurationReader lReader;
	
	// Load all the information into a reader
	printf( "Load information and configuration\n" );
	PvString lFilenameToSave( FILE_NAME );

	lReader.Load( lFilenameToSave );

	// Restore the stream information
	printf( "Restore information for a string with the information index\n" );
	PvString lCheck;
	lReader.Restore( PvUInt32( 0 ), lCheck );

	printf( "Verify the restoration\n" );
	if ( lCheck != "AnyKindOfInformation" )
	{
		return false;
	}

	return true;
}

//
// Main function
//

int main()
{
	// Create the Buffer and fill it
	printf( "\n1. Store the configuration\n\n" );
	if ( !StoreConfiguration() )
	{
		printf( "Cannot store the configuration correctly" );
		return 0;
	}

	PvSleepMs( 5000 );

	printf( "\n2. Restore the device configuration with a name\n\n" );
	if ( !RestoreDeviceConfigurationName() )
	{
		printf( "Cannot restore the configuration correctly" );
		return 0;
	}

	PvSleepMs( 5000 );

	printf( "\n3. Restore the device configuration with an index\n\n" );
	if ( !RestoreDeviceConfigurationIndex() )
	{
		printf( "Cannot restore the configuration correctly" );
		return 0;
	}

	PvSleepMs( 5000 );

	printf( "\n4. Restore the stream configuration with a name\n\n" );
	if ( !RestoreStreamConfigurationName() )
	{
		printf( "Cannot restore the configuration correctly" );
		return 0;
	}

	PvSleepMs( 5000 );

	printf( "\n5. Restore the stream configuration with an index\n\n" );
	if ( !RestoreStreamConfigurationIndex() )
	{
		printf( "Cannot restore the configuration correctly" );
		return 0;
	}

	printf( "\n6. Restore the string information with a name\n\n" );
	if ( !RestoreStringInformationName() )
	{
		printf( "Cannot restore the configuration correctly" );
		return 0;
	}

	printf( "\n7. Restore the string information with an index\n\n" );
	if ( !RestoreStringInformationIndex() )
	{
		printf( "Cannot restore the configuration correctly" );
		return 0;
	}

	printf( "\n<press a key to exit>\n" );
	PvWaitForKeyPress();

	return 0;
}

