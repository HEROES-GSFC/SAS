
#include "savingthread.h"
#include <assert.h>


SavingThread::SavingThread( SetupDlg *aSetupDlg, PvConfigurationWriter *aWriter, PvDevice *aDevice, PvStream *aStream, PvStringList *aErrorList, QWidget* aParent )
    : mSetupDlg( aSetupDlg )
    , mWriter( aWriter )
    , mDevice( aDevice )
    , mStream( aStream )
    , mStopping( false )
	, mErrorList( aErrorList )
{
    mDlg = new ProgressDlg( this, aParent );
}

SavingThread::~SavingThread()
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

void SavingThread::stop()
{
    mStopping = true;
}

PvResult SavingThread::Save()
{
    mDlg->exec();
    return mResult;
}

void SavingThread::run()
{
    PvResult lResult = PvResult::Code::NOT_CONNECTED;

    try
    {
        assert( mDevice != NULL );
        assert( mStream != NULL );
        assert( mDlg != NULL );

        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleMonitor ) )
        {
            mDlg->SetStatus( "Saving the device parameters..." );

            SetPrefix( tr( "Device state save error. " ) );
            lResult = mWriter->Store( mDevice );
            if ( !lResult.IsOK() )
            {
                mResult = lResult.GetCode();
                return;
            }
        }

        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
            ( mSetupDlg->GetRole() == SetupDlg::RoleData ) )
        {
            mDlg->SetStatus( "Saving the stream parameters..." );

            lResult = mWriter->Store( mStream );
            if ( !lResult.IsOK() )
            {
                mResult = lResult.GetCode();
                return;
            }
        }
    }
    catch ( ... )
    {
        lResult = PvResult::Code::ABORTED;
    }

    mResult = lResult.GetCode();
    return;
}


///
/// \brief Sets the error list contextual prefix
///

void SavingThread::SetPrefix( const QString &aPrefix )
{
    PvString lPrefix( aPrefix.toAscii().data() );
    mWriter->SetErrorList( mErrorList, lPrefix );
}

