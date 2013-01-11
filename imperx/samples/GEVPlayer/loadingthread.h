#pragma once

#include "progressdlg.h"
#include "setupdlg.h"
#include <PvConfigurationReader.h>
#include <PvDevice.h>
#include <PvStream.h>
#include "thread.h"

// =============================================================================
// Thread used to bring back a persisted state
//
class LoadingThread : public Thread
{
public:

    LoadingThread( SetupDlg *aSetupDlg, PvConfigurationReader *aReader, PvDevice *aDevice, PvStream *aStream, PvStringList *aErrorList, QWidget* aParent = NULL );
    virtual ~LoadingThread();
    
    PvResult Load();
    virtual void stop();

protected:

    void SetPrefix( const QString &aPrefix );

    void run();

private:

    SetupDlg *mSetupDlg;
    PvConfigurationReader *mReader;
    PvDevice *mDevice;
    PvStream *mStream;
    ProgressDlg *mDlg;
    PvResult mResult;
    bool mStopping;
    PvStringList *mErrorList;

};

