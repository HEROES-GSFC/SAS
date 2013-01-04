// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "connectionthread.h"
#include "gevplayer.h"

#include <PvStreamBase.h>
#include <PvInterface.h>
#include <assert.h>


ConnectionThread::ConnectionThread( SetupDlg *aSetupDlg, PvDeviceInfo *aDeviceInfo, PvDevice *aDevice, PvStream *aStream, QWidget* aParent, const QString &aLocalIPAddress, const QString &aDeviceIPAddress )
	: mSetupDlg( aSetupDlg )
	, mDeviceInfo( aDeviceInfo )
	, mDevice( aDevice )
	, mStream( aStream )
	, mStopping( false )
	, mLocalIPAddress( aLocalIPAddress.toAscii().data() )
	, mDeviceIPAddress( aDeviceIPAddress.toAscii().data() )
	, mTitle( "Connection Progress" )
{
    mDlg = new ProgressDlg( this, aParent );
}

ConnectionThread::~ConnectionThread()
{
    if ( mDlg != NULL )
    {
        delete mDlg;
        mDlg = NULL;
    }
}

PvResult ConnectionThread::Connect()
{
    assert( mDlg != NULL );
    mDlg->setWindowTitle( mTitle );
    mDlg->exec();
    return mResult;
}

void ConnectionThread::SetStreamDestination( const QString &aIPAddress, PvUInt16 aPort, PvUInt16 aChannel )
{
	assert( mDevice != NULL );
	assert( mStream != NULL );

    // Bug 2132: only set destination if different than 0.0.0.0:0
    if ( ( aIPAddress == "0.0.0.0" ) &&
         ( aPort == 0 ) )
    {
		return;
	}

	// Bug 3202: don't set destination on receiver, peripheral or transceiver (for now)
    if ( mDeviceInfo != NULL )
    {
	    if ( ( mDeviceInfo->GetClass() == PvDeviceClassReceiver ) ||
		     ( mDeviceInfo->GetClass() == PvDeviceClassPeripheral ) ||
		     ( mDeviceInfo->GetClass() == PvDeviceClassTransceiver ) )
	    {
		    return;
	    }
    }

	mDevice->SetStreamDestination( aIPAddress.toAscii().data(), aPort, aChannel );
}

///
/// \brief From QThread, signals the thread to stop
///

void ConnectionThread::stop()
{
    mStopping = true;
}

void ConnectionThread::run()
{
    PvResult lResult = PvResult::Code::OK;
    PvUInt16 lChannel = mSetupDlg->GetDefaultChannel();

    if ( mDeviceInfo != NULL )
    {
        mDeviceIPAddress = mDeviceInfo->GetIPAddress().GetAscii();
        mLocalIPAddress = mDeviceInfo->GetInterface()->GetIPAddress().GetAscii();
    }

    try
    {
        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleMonitor ) )
        {
            if ( !mDevice->IsConnected() && mDeviceInfo != NULL )
            {
                // Connect device
                mDlg->SetStatus( "Building GenICam interface..." );
                lResult = mDevice->Connect( mDeviceInfo,
                    ( mSetupDlg->GetRole() != SetupDlg::RoleMonitor ) ?
                        PvAccessControl :
                        PvAccessReadOnly );
                if ( !lResult.IsOK() )
                {
                    mResult = lResult;
                    return;
                }
            }

            if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
                 ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl ) )
            {
                // Determine whether this device is multisource
                if ( GEVPlayer::IsMultiSourceTransmitter( mDevice ) )
                {
                    // If multisource feature exists, get the channel corresponding to the currently selected source
                    // Keep in mind there's also the SourceComponentSelector feature that can be configured using the device parameter browser
                    PvInt64 lValue = 0;
                    PvResult lResult = mDevice->GetGenParameters()->GetIntegerValue( "SourceStreamChannel", lValue );
                    if ( lResult.IsOK() )
                    {
                        lChannel = static_cast<PvUInt16>( lValue );
                    }
                    else
                    {
                    	mDlg->SetStatus( "SourceSelector feature present, but SourceStreamChannel feature missing. Defaulting to channel 0." );
                    	QThread::msleep( 3000 );
                    }
                }

                // Negotiate packet size once for all channels
                if ( ( mDeviceInfo != NULL ) && ( mDeviceInfo->GetClass() == PvDeviceClassTransmitter ) )
                {
                    NegotiatePacketSize();
                }
            }
        }

        // Open stream - and retry if it fails
        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleData ) )
        {
            QString lString;
            lString.sprintf( "Opening eBUS stream on channel %d...", lChannel );
            mDlg->SetStatus( lString );

			// Open stream
			if ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastAuto )
			{
				lResult = mStream->Open( mDeviceIPAddress, 0, lChannel, mLocalIPAddress );
			}
			else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastSpecific )
			{
				lResult = mStream->Open( mDeviceIPAddress, mSetupDlg->GetUnicastSpecificPort(), lChannel, mLocalIPAddress );
			}
			else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationMulticast )
			{
				lResult = mStream->Open( mDeviceIPAddress, mSetupDlg->GetMulticastIP().toAscii().data(),
					mSetupDlg->GetMulticastPort(), lChannel, mLocalIPAddress );
			}
			else
			{
				assert( 0 );
			}

			if ( !lResult.IsOK() )
			{
				mResult = lResult;
				return;
			}
        }

        // Now that the stream is opened, set the destination on the device
        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl ) )
        {
            if ( ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastAuto ) ||
                 ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastSpecific ) )
            {
				SetStreamDestination( mStream->GetLocalIPAddress().GetAscii(), mStream->GetLocalPort(), lChannel );
            }
            else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastOther )
            {
                SetStreamDestination( mSetupDlg->GetUnicastIP(), mSetupDlg->GetUnicastPort(), lChannel );
            }
            else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationMulticast )
            {
                SetStreamDestination( mSetupDlg->GetMulticastIP().toAscii().data(), mSetupDlg->GetMulticastPort(), lChannel );
            }
        }
    }
    catch ( ... )
    {
        lResult = PvResult( PvResult::Code::ABORTED, "Unexpected error" );
    }

    mResult = lResult;
}

void ConnectionThread::NegotiatePacketSize()
{
    bool lEnabledValue = true;
    mDevice->GetGenLink()->GetBooleanValue( "AutoNegotiation", lEnabledValue ) ;

    PvInt64 lUserPacketSizeValue = 1476;
    mDevice->GetGenLink()->GetIntegerValue( "DefaultPacketSize", lUserPacketSizeValue );

    if ( lEnabledValue )
    {
        PvInt64 lPacketSizeValue = 0;
        mDevice->GetGenParameters()->GetIntegerValue( "GevSCPSPacketSize", lPacketSizeValue );

        // Perform automatic packet size negotiation
        mDlg->SetStatus( "Optimizing streaming packet size..." );
        PvResult lResult = mDevice->NegotiatePacketSize( 0, static_cast<PvUInt32>( lPacketSizeValue ) );
        if ( !lResult.IsOK() )
        {
            QString lString;
            lString.sprintf( "WARNING: Streaming packet size negotiation failure, using original value of %d bytes!", lPacketSizeValue );
            mDlg->SetStatus( lString );
            QThread::msleep( 3000 );
        }
    }
    else
    {
        PvResult lResult = mDevice->SetPacketSize( static_cast<PvUInt32>( lUserPacketSizeValue ) );
        if ( !lResult.IsOK() )
        {
            // Last resort...
            mDevice->SetPacketSize( 576 );
        }

        QString lNewStr;
        if ( lResult.IsOK() )
        {
            lNewStr.sprintf(
                "A packet of size of %i bytes was configured for streaming. You may experience issues "
                "if your system configuration cannot support this packet size.",
                lUserPacketSizeValue );
        }
        else
        {
            lNewStr.sprintf( "WARNING: could not set streaming packet size to %i bytes, using %i bytes!",
                lUserPacketSizeValue, 576 );
        }
        mDlg->SetStatus( lNewStr );
        QThread::msleep( 3000 );
    }

    //
    // If more than one channel, propagate packet in effect size to other channels
    //

    PvInt64 lChannelCount = 0;
    PvResult lResult = mDevice->GetGenParameters()->GetIntegerValue( "GevStreamChannelCount", lChannelCount );
    if ( !lResult.IsOK() || ( lChannelCount < 2 ) )
    {
        // If we can't read the GevStreamChannelCount or of its less than 2, we are done here
        return;
    }

    PvInt64 lPacketSizeValue = 0;
    lResult = mDevice->GetGenParameters()->GetIntegerValue( "GevSCPSPacketSize", lPacketSizeValue );
    if ( !lResult.IsOK() )
    {
        mDlg->SetStatus(
            "WARNING: Failed to propagate GevSCPSPacketSize on all streaming channels. "
            "Could not read back streaming channel 0 packet size. " );
        QThread::msleep( 3000 );
        return;
    }

    for ( PvInt64 i = 1; i < lChannelCount; i++ )
    {
        lResult = mDevice->SetPacketSize( static_cast<PvUInt32>( lPacketSizeValue ), static_cast<PvUInt32>( i ) );
        if ( !lResult.IsOK() )
        {
            mDlg->SetStatus( "WARNING: Failed to propagate GevSCPSPacketSize on all streaming channels. " );
            QThread::msleep( 3000 );
            return;
        }
    }
}
