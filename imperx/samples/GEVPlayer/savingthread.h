#pragma once

#include "progressdlg.h"
#include "setupdlg.h"
#include <PvConfigurationWriter.h>
#include <PvDevice.h>
#include <PvStream.h>
#include "thread.h"


// =============================================================================
// Thread used to save a persisted state
//
class SavingThread : public Thread
{
public:

    SavingThread( SetupDlg *aSetupDlg, PvConfigurationWriter *aWriter, PvDevice *aDevice, PvStream *aStream, PvStringList *aErrorList, QWidget* aParent = NULL );
    virtual ~SavingThread();

    PvResult Save();
    virtual void stop();

protected:

    void SetPrefix( const QString &aPrefix );

    void run();

private:

    SetupDlg *mSetupDlg;
    PvConfigurationWriter *mWriter;
    PvDevice *mDevice;
    PvStream *mStream;
    ProgressDlg *mDlg;
    PvResult mResult;
    bool mStopping;
    PvStringList *mErrorList;

};
