// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

// 
// This sample shows how to use PvDeviceFinder to find GEV Devices on a network 
// using two different methods.
// 1. Finding Pleora GigE Vision devices programatically 
// 2. Finding Pleora GigE Vision devices using a GUI
// 

#include <PvSampleUtils.h>
#include <PvSystem.h>
#include <PvInterface.h>
#include <PvDeviceFinderWnd.h>
#include <PvDevice.h>

PV_INIT_SIGNAL_HANDLER();

//
// To find GEV Devices on a network
//

int GEVDeviceFinding()
{
	PvResult lResult;	
	PvDeviceInfo *lDeviceInfo = 0;

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

		PvDevice lDevice;
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

	return 0;
}


//
// To find GEV Devices on a network (using a GUI)
//

int GUIGEVDeviceFinding()
{
	// Create a GUI GEV Device finder. Use PvDeviceFinderWnd::PvDeviceFinderWnd.
	PvDeviceFinderWnd lDeviceFinderWnd;

	// Prompt the user to select a GEV Device. Use PvDeviceFinderWnd::ShowModal.
	if ( !lDeviceFinderWnd.ShowModal().IsOK() )
	{
		// Error can include dialog cancel as well
		return -1;
	}

	// When dismissed with OK, the device finder dialog keeps a reference
	// on the device that was selected by the user. Retrieve this reference.
	PvDeviceInfo* lDeviceInfo = lDeviceFinderWnd.GetSelected();

	// Connect to the selected GEV Device.
	if( lDeviceInfo != NULL )
	{
		printf( "Connecting to %s\n",
			lDeviceInfo->GetMACAddress().GetAscii() );

		PvDevice lDevice;
		PvResult lResult = lDevice.Connect( lDeviceInfo );
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
		printf( "No device selected\n" );
	}

	return 0;
}


//
// Main function
//

int main()
{
	// Find devices programmatically
	printf( "1. Find devices programmatically\n\n" );
	GEVDeviceFinding();

	// Prompt the user to select a device using the GUI
	printf( "\n2. Prompt the user to select a device using the GUI\n\n" );
	GUIGEVDeviceFinding();

	printf( "\n<press a key to exit>\n" );
    PvWaitForKeyPress();

	return 0;
}

