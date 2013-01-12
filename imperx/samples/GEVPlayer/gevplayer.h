// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __GEVPLAYER_H__
#define __GEVPLAYER_H__

#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>
#include <QtCore/QMutex>
#include <QtCore/QEvent>
#include <QtCore/QTime>
#include <QtCore/QTimer>

#include <PvGenBrowserWnd.h>
#include <PvDisplayWnd.h>
#include <PvTerminalIPEngineWnd.h>

#include <PvDevice.h>
#include <PvStream.h>
#include <PvPipeline.h>

#include <PvAcquisitionStateManager.h>

#include "aboutbox.h"
#include "eventmonitordlg.h"
#include "filteringdlg.h"
#include "threaddisplay.h"
#include "setupdlg.h"
#include "imagesavedlg.h"
#include "displayoptionsdlg.h"
#include "logbuffer.h"


#define WM_STREAM ( QEvent::User + 0x4420 )
#define WM_IMAGEDISPLAYED ( QEvent::User + 0x4431 )
#define WM_DISCONNECT ( QEvent::User + 0x4432 )
#define WM_STOPSTREAMING ( QEvent::User + 0x4434 )
#define WM_LINKRECONNECTED ( QEvent::User + 0x4438 )
#define WM_ACQUISITIONSTATECHANGED ( QEvent::User + 0x4440 )
#define WM_UPDATESOURCE ( QEvent::User + 0x4441 )
#define WM_UPDATESOURCES ( QEvent::User + 0x4442 )
#define WM_UPDATEACQUISITIONMODE ( QEvent::User + 0x4443 )


// forward declare (for now)
class GEVPlayer;

class OpenRecent : public QObject
{
    Q_OBJECT

public:

	QString mFileName;
	QAction* mFileOpenRecentAction;
	GEVPlayer* mGEVPlayer;

public slots:

    void OnFileOpenRecentAction();

};


class PvGenStateStack;


class GEVPlayer : public QMainWindow,
	protected PvDeviceEventSink, protected PvGenEventSink, protected PvAcquisitionStateEventSink,
	protected PvPipelineEventSink
{
    Q_OBJECT

public:

    GEVPlayer( QWidget *parent = 0, Qt::WFlags flags = 0 );
    virtual ~GEVPlayer();
    
    void SetFileName( QString aFileName ) { mFileName = aFileName; }
    void OpenConfig( QString aLocation );

    void StartStreaming();
    void StopStreaming();

    static bool IsMultiSourceTransmitter( PvDevice *aDevice );

protected slots:

    void OnConnectButton();
    void OnDisconnectButton();
    void OnStart();
    void OnStop();
    void OnCbnSelchangeMode( int aIndex );
    void OnFileMenuAboutToShow();
    void OnToolsMenuAboutToShow();
    void OnHelpMenuAboutToShow();
    void OnFileOpenAction();
    void OnFileSaveAction();
    void OnFileSaveAsAction();
    void OnFileExitAction();
    void OnToolsSetupAction();
    void OnToolsEventMonitorAction();
    void OnToolsSerialAction();
    void OnToolsFilteringAction();
    void OnToolsDisplayOptionsAction();
    void OnToolsBufferOptionsAction();
    void OnToolsSaveImagesAction();
    void OnToolsSaveCurrentImageAction();
    void OnHelpAboutAction();
    void OnShowCommParameters();
    void OnShowDeviceParameters();
    void OnShowStreamParameters();
    void OnTimer();
    void OnRegisterInterface();
    void OnToolsRestoredefaultpreferences();
    void OnToolsSavepreferences();
    void OnToolsSaveXML();
    void OnToolsDeinterlacingDisabled();
    void OnToolsDeinterlacingWeaving();
    void OnCbnSelchangeSource( int aIndex );

protected:

    enum StatusColor { SCDefault, SCRed, SCYellow };

    // UI
    void CreateLayout();
    QGroupBox *CreateConnectGroup();
    QGroupBox *CreateAcquisition();
    QGroupBox *CreateParameters();
    QGroupBox *CreateDisplay();
    void CreateMenu();

    bool event( QEvent *aEvent );

    void SetStatusColor( StatusColor aColor );
    void ShowGenWindow( PvGenBrowserWnd *aWnd, PvGenParameterArray *aArray, const QString &aTitle );
    void CloseGenWindow( PvGenBrowserWnd *aWnd );

    void EnableInterface();
    void EnableControls( bool aEnabled );
    void EnableGenBrowsers( bool aEnabled );

    // Core
	void Connect( PvDeviceInfo *aDI, PvConfigurationReader *aCR );
    void StartAcquisition();
    void StopAcquisition();
	void Disconnect();
    PvUInt32 GetPayloadSize();

    // PvGenEventSink implementation
	void OnParameterUpdate( PvGenParameter *aParameter );

    // PvDeviceEventSink implementation
    void OnLinkDisconnected( PvDevice *aDevice );
    void OnLinkReconnected( PvDevice *aDevice );

    // PvAcquisitionStateManager event sink
    void OnAcquisitionStateChanged( PvDevice* aDevice, PvStreamBase* aStream, PvUInt32 aSource, PvAcquisitionState aState );

    // PvPipelineEventSink implementation
    void OnBufferTooSmall( PvPipeline *aPipeline, bool *aReallocAll, bool *aResetStats );

    void ReportMRU( QString aFileName );
    void SaveMRUToRegistry();
    void LoadMRUFromRegistry();
    void UpdateMRUMenu();

    // Events used to bring async code back in main thread context
    void OnImageDisplayedEvent();
    void OnStopStreamingEvent();
    void OnDisconnectEvent();
    void OnLinkReconnectedEvent();
	void OnAcquisitionStateChangedEvent();
	void OnUpdateSourceEvent();
	void OnUpdateSourcesEvent( PvInt64 aPreferedSource );
	void OnUpdateAcquisitionModeEvent();
	void OnStreamEvent( bool aRestartAcquisition );

    void SaveConfig( QString aLocation, bool aSaveConnectedDevice );

    QString GetDefaultPath();
    QString GetStickyPath();
    QString GetAppDataPath();

	QString GetErrorString();

    void PushSource( PvGenStateStack *aStack );

private:

    // UI
    QPushButton *mConnectButton;
    QPushButton *mDisconnectButton;
    QLineEdit *mIpAddressLine;
    QLineEdit *mMacAddressLine;
    QLineEdit *mManufacturerLine;
    QLineEdit *mModelLine;
    QLineEdit *mNameLine;
    QComboBox *mModeCombo;
    QComboBox *mSourceCombo;
    QPushButton *mStartButton;
    QPushButton *mStopButton;
    QPushButton *mCommunicationButton;
    QPushButton *mDeviceButton;
    QPushButton *mStreamButton;
    PvDisplayWnd *mDisplay;

    QTextEdit *mStatusLine;
	QTimer *mTimer;

    // Menu
    QMenu *mFileMenu;
    QAction *mFileOpenAction;
    QMenu *mFileOpenRecentMenu;
    OpenRecent mOpenRecent[10];
    QAction *mFileSaveAction;
    QAction *mFileSaveAsAction;
    QAction *mFileExitAction;
    QMenu *mToolsMenu;
    QAction *mToolsSetupAction;
    QAction *mToolsEventMonitorAction;
    QAction *mToolsSerialAction;
    QAction *mToolsFilteringAction;
    QAction *mToolsDisplayOptionsAction;
    QAction *mToolsBufferOptionsAction;
    QAction *mToolsSaveImagesAction;
    QAction *mToolsSaveCurrentImageAction;
    QAction *mToolsSaveXMLAction;
    QAction *mToolsDeinterlacingDisabled;
    QAction *mToolsDeinterlacingWeaving;
    QMenu *mHelpMenu;
    QAction *mHelpAboutAction;

    QAction *mToolsSavePreferencesAction;
    QAction *mToolsRestoreDefaultAction;

    SetupDlg* mSetupDlg;
    EventMonitorDlg *mEventMonitorDlg;
    FilteringDlg* mFilteringDlg;
    StatusColor mStatusColor;
    PvTerminalIPEngineWnd* mTerminalIPEngineWnd;

    // Core
	PvDevice mDevice;
    PvStream *mStream;
    PvPipeline *mPipeline;
	
	LogBuffer *mLogBuffer;
	ThreadDisplay *mThreadDisplay;

	QMutex mStartAcquisitionMutex;
	QMutex mStartStreamingMutex;

	QTime mDisconnectTime;
	bool mNewIP;
    bool mDeviceConnected;
    bool mSerial0IPEngineSupported;
    bool mBuffersReallocated;

	PvGenBrowserWnd *mDeviceWnd;
	PvGenBrowserWnd *mCommunicationWnd;
	PvGenBrowserWnd *mStreamWnd;

	QString mFileName;
	std::list<QString> mRecentList;

	ImageSaveDlg *mImageSaveDlg;
	DisplayOptionsDlg *mDisplayOptionsDlg;

	PvAcquisitionStateManager *mAcquisitionStateManager;

	DeinterlacingType mDeinterlacing;

	// Flags to prevent re-entry
	bool mUpdatingSource;
	bool mUpdatingSources;
	bool mUpdatingAcquisitionMode;

	std::vector<PvInt64> mAvailableSources;

	bool mShowAcquisitionWarning;
};


#endif // __GEVPLAYER_H__

