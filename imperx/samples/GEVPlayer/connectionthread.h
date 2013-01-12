#pragma once

#include "progressdlg.h"
#include "setupdlg.h"
#include <PvDeviceInfo.h>
#include <PvDevice.h>
#include <PvStream.h>
#include "thread.h"

// =============================================================================
// Thread used to setup the system on a connect operation
//
class ConnectionThread : public Thread
{
public:

    ConnectionThread( SetupDlg *aSetupDlg, PvDeviceInfo *aDeviceInfo, PvDevice *aDevice, PvStream *aStream, QWidget* aParent, const QString &aLocalIPAddress = "", const QString &aDeviceIPAddress = "" );
    virtual ~ConnectionThread();
    
    PvResult Connect();
    virtual void stop();

    void SetTitle( const QString &aTitle ) { mTitle = aTitle; }

protected:

   void SetStreamDestination( const QString &aIPAddress, PvUInt16 aPort, PvUInt16 aChannel = 0 );
   void NegotiatePacketSize();

   void run();

private:

    SetupDlg *mSetupDlg;
    PvDeviceInfo *mDeviceInfo;
    PvDevice *mDevice;
    PvStream *mStream;
    ProgressDlg *mDlg;
    PvResult mResult;
    bool mStopping;
    PvString mLocalIPAddress;
    PvString mDeviceIPAddress;

    QString mTitle;
};


