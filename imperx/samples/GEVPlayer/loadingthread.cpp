

#include "loadingthread.h"
#include <assert.h>

#include <QtGui/QMessageBox>

LoadingThread::LoadingThread( SetupDlg *aSetupDlg, PvConfigurationReader *aReader, PvDevice *aDevice, PvStream *aStream, PvStringList *aErrorList, QWidget* aParent )
    : mSetupDlg( aSetupDlg )
    , mReader( aReader )
    , mDevice( aDevice )
    , mStream( aStream )
    , mStopping( false )
	, mErrorList( aErrorList )
{
    mDlg = new ProgressDlg( this, aParent );
}

LoadingThread::~LoadingThread()
{
    if ( mDlg != NULL )
    {
        delete mDlg;
        mDlg = NULL;
    }
}

///
/// \brief From QThread, signals the thread to stop
///

void LoadingThread::stop()
{
    mStopping = true;
}

PvResult LoadingThread::Load()
{
    assert( mDlg != NULL );
    mDlg->exec();
    return mResult;
}

void LoadingThread::run()
{
    PvResult lResult = PvResult::Code::NOT_CONNECTED;
    PvUInt16 lChannel = mSetupDlg->GetDefaultChannel();

    try
    {
        assert( mDevice != NULL );
        assert( mStream != NULL );

        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleMonitor ) )
        {
            if ( mDevice->IsConnected() )
            {
                mDlg->SetStatus( tr( "Restoring device parameters from file..." ) );
            }
            else
            {
                mDlg->SetStatus( tr( "Connecting to device and restoring it from file..." ) );
            }

            SetPrefix( tr( "Device restore error. " ) );
            lResult = mReader->Restore( 0, mDevice );
            if ( !lResult.IsOK() )
            {
                mResult = lResult;
                return;
            }
        }

        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleData ) )
        {
            if ( mStream->IsOpen() )
            {
                mDlg->SetStatus( tr( "Restoring stream parameters from file..." ) );
            }
            else
            {
                mDlg->SetStatus( tr( "Opening a stream and restoring it from file..." ) );
            }

            SetPrefix( tr( "Stream restore error. " ) );
            lResult = mReader->Restore( 0, *mStream );

            if ( lResult.IsOK() )
            {
                // obtain the proper channel from the stream we restored
                PvInt64 lValue;
                PvGenInteger* lChannelID = mStream->GetParameters()->GetInteger( "ChannelID" );
                lChannelID->GetValue( lValue );
                lChannel = (PvUInt16) lValue;
            }
        }

        if ( !lResult.IsOK() )
        {
            mResult = lResult;
            return;
        }

        // Now that the stream is opened, set the destination on the device
        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl ) )
        {
            if ( ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastAuto ) ||
                 ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastSpecific ) )
            {
                mDevice->SetStreamDestination( mStream->GetLocalIPAddress(), mStream->GetLocalPort(), lChannel );
            }
            else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastOther )
            {
                mDevice->SetStreamDestination( mSetupDlg->GetUnicastIP().toAscii().data(), mSetupDlg->GetUnicastPort(), lChannel );
            }
            else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationMulticast )
            {
                mDevice->SetStreamDestination( mSetupDlg->GetMulticastIP().toAscii().data(), mSetupDlg->GetMulticastPort(), lChannel );
            }
        }
    }
    catch ( ... )
    {
        lResult = PvResult( PvResult::Code::ABORTED, "Unexpected error loading persistence file" );
    }

    mResult = lResult;
}


///
/// \brief Sets the error list contextual prefix
///

void LoadingThread::SetPrefix( const QString &aPrefix )
{
    PvString lPrefix( aPrefix.toAscii().data() );
    mReader->SetErrorList( mErrorList, lPrefix );
}


