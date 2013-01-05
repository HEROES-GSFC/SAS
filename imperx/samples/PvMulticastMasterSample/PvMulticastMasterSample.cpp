// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// Multicasting master
//

#include <PvSampleUtils.h>
#include <PvDeviceFinderWnd.h>
#include <PvStream.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>

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
	if ( !aDevice.Connect( lDeviceInfo ).IsOK() )
	{
		printf( "Unable to connect to %s\n", 
			lDeviceInfo->GetMACAddress().GetAscii() );
		return false;
	}
	printf( "Successfully connected to %s\n", 
		lDeviceInfo->GetMACAddress().GetAscii() );

	aIP = lDeviceInfo->GetIPAddress();

	return true;
}


//
// Shows how to control a multicast master without receiving data
//

bool StartMaster()
{
	// Connect to the GEV Device.
	PvString lDeviceIP;
	PvDevice lDevice;
	if ( !Connect( lDevice, lDeviceIP ) )
	{
		return false;
	}

	printf( "\n" );

	bool lCheck = lDevice.IsConnected();

	PvResult lResult = lDevice.SetStreamDestination( "239.192.1.1", 1042 );

	// Get device parameters need to control streaming
	PvGenParameterArray *lDeviceParams = lDevice.GetGenParameters();
	PvGenInteger *lTLLocked = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "TLParamsLocked" ) );
	PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
	PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );
	PvGenInteger *lGevSCPSPacketSize = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "GevSCPSPacketSize" ) );

	// Auto packet size negotiation does not happen with multicasting, here
	// we set it to small packets in order to be on the safe side
	lGevSCPSPacketSize->SetValue( 1440 );

	// TLParamsLocked is optional but when present, it MUST be set to 1
	// before sending the AcquisitionStart command
	if ( lTLLocked != NULL )
	{
		printf( "Setting TLParamsLocked to 1\n" );
		lTLLocked->SetValue( 1 );
	}

	// Tell the device to start sending images to the multicast group
	printf( "Sending StartAcquisition command to device\n" );
	lStart->Execute();

	// Acquire images until the user instructs us to stop
	printf( "\n<press a key to stop streaming>\n" );
    PvWaitForKeyPress();

	PvGetChar(); // Flush key buffer for next stop
	printf( "\n" );

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
	// Connect to device and start streaming as a multicast master
	printf( "1. Connect to device and start streaming as a multicast master\n\n" );
	StartMaster();

	printf( "\n<press a key to exit>\n" );
    PvWaitForKeyPress();

	return 0;
}

