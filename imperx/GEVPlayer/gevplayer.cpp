// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "gevplayer.h"

#include "loadingthread.h"
#include "connectionthread.h"
#include "savingthread.h"
#include "pvmessagebox.h"
#include "errorsdlg.h"
#include "warningdlg.h"
#include "bufferoptionsdlg.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QDockWidget>
#include <QtGui/QMenuBar>
#include <QtGui/QGridLayout>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#include <QtGui/QShortcut>
#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>

#include <PvConfigurationWriter.h>
#include <PvConfigurationReader.h>
#include <PvRegisterInterfaceWnd.h>
#include <PvDeviceFinderWnd.h>
#include <PvGenStateStack.h>
#include <PvStreamInfo.h>

#include <assert.h>


#ifdef WIN32
#include <shlobj.h>
#include <io.h>
#define PtAccess _access
#else
#define PtAccess access
#include <pwd.h>
#include <sys/stat.h>
#endif //WIN32


#define TAG_COMMUNICATIONVISIBILITY ( "communicationvisibility" )
#define TAG_DEVICEVISIBILITY ( "devicevisibility" )
#define TAG_STREAMVISIBILITY ( "streamvisibility" )
#define TAG_COMMUNICATIONBROWSER ( "communicationbrowser" )
#define TAG_DEVICEBROWSER ( "devicebrowser" )
#define TAG_STREAMBROWSER ( "streambrowser" )
#define TAG_DEINTERLACING ( "deinterlacing" )
#define TAG_DEINTERLACINGDISABLED ( "off" )
#define TAG_DEINTERLACINGWEAVING ( "weaving" )
#define TAG_ACTIVESOURCE ( "activesource" )


// Simple event taking two integer parameters like messages on Win32
class QWindowsEvent : public QEvent
{
public:

	QWindowsEvent( QEvent::Type aEventID, PvUInt64 aWParam = 0, PvInt64 aLParam = 0 )
		: QEvent( aEventID )
		, mWParam( aWParam )
		, mLParam( aLParam )
	{
	}

	PvUInt64 GetWParam() const { return mWParam; }
	PvInt64 GetLParam() const { return mLParam; }

private:

	PvUInt64 mWParam;
	PvInt64 mLParam;
};


///
/// \brief Constructor
///

GEVPlayer::GEVPlayer( QWidget *parent, Qt::WFlags flags )
    : QMainWindow( parent, flags )
    , mNewIP( false )
    , mDeviceConnected( false )
    , mSerial0IPEngineSupported( false )
    , mStatusColor( SCDefault )
    , mThreadDisplay( NULL )
    , mEventMonitorDlg( NULL )
    , mFilteringDlg( NULL )
    , mSetupDlg( NULL )
	, mLogBuffer( NULL )
	, mAcquisitionStateManager( NULL )
	, mDeinterlacing( DeinterlacingWeaving )
	, mUpdatingSource( false )
	, mUpdatingSources( false )
	, mUpdatingAcquisitionMode( false )
	, mShowAcquisitionWarning( true )
	, mBuffersReallocated( false )
{
    mStream = new PvStream;
    mPipeline = new PvPipeline( mStream );
    mPipeline->RegisterEventSink( this );

    mDeviceWnd = new PvGenBrowserWnd();
    mCommunicationWnd = new PvGenBrowserWnd();
    mStreamWnd = new PvGenBrowserWnd();
    mTerminalIPEngineWnd = new PvTerminalIPEngineWnd();

    mFilteringDlg = new FilteringDlg( this );
    mImageSaveDlg = new ImageSaveDlg( this, mFilteringDlg );
    mDisplayOptionsDlg = new DisplayOptionsDlg( this );
    mSetupDlg = new SetupDlg( this );

	mLogBuffer = new LogBuffer;

    CreateLayout();
}


///
/// \brief Destructor
///

GEVPlayer::~GEVPlayer()
{
    Disconnect();

	if ( mLogBuffer != NULL )
	{
		delete mLogBuffer;
		mLogBuffer = NULL;
	}

    if ( mTimer != NULL )
    {
        delete mTimer;
        mTimer = NULL;
    }

    if ( mDeviceWnd != NULL )
    {
        delete mDeviceWnd;
        mDeviceWnd = NULL;
    }

    if ( mCommunicationWnd != NULL )
    {
        delete mCommunicationWnd;
        mCommunicationWnd = NULL;
    }

    if ( mStreamWnd != NULL )
    {
        delete mStreamWnd;
        mStreamWnd = NULL;
    }

    if ( mDisplayOptionsDlg != NULL )
    {
        delete mDisplayOptionsDlg;
        mDisplayOptionsDlg = NULL;
    }

    if ( mImageSaveDlg != NULL )
    {
        delete mImageSaveDlg;
        mImageSaveDlg = NULL;
    }

    if ( mPipeline != NULL )
    {
        assert( !mPipeline->IsStarted() );

        mPipeline->UnregisterEventSink( this );

        delete mPipeline;
        mPipeline = NULL;
    }

    if ( mStream != NULL )
    {
        assert( !mStream->IsOpen() );

        delete mStream;
        mStream = NULL;
    }

    if ( mFilteringDlg != NULL )
    {
        delete mFilteringDlg;
        mFilteringDlg = NULL;
    }

    if ( mSetupDlg != NULL )
    {
        delete mSetupDlg;
        mSetupDlg = NULL;
    }

    if ( mTerminalIPEngineWnd != NULL )
    {
        delete mTerminalIPEngineWnd;
        mTerminalIPEngineWnd = NULL;
    }

    if ( mAcquisitionStateManager != NULL )
    {
        delete mAcquisitionStateManager;
        mAcquisitionStateManager = NULL;
    }
}


///
/// \brief Create dialog layout
///

void GEVPlayer::CreateLayout()
{
    QGroupBox *lConnectionBox = CreateConnectGroup();
    QGroupBox *lAcqBox = CreateAcquisition();
    QGroupBox *lParametersBox = CreateParameters();
    QGroupBox *lDisplayBox = CreateDisplay();

    QVBoxLayout *lLayoutLeft = new QVBoxLayout();
    lLayoutLeft->addWidget( lConnectionBox, Qt::AlignTop );
    lLayoutLeft->addStretch();
    lLayoutLeft->addWidget( lAcqBox, Qt::AlignTop );
    lLayoutLeft->addStretch();
    lLayoutLeft->addWidget( lParametersBox, Qt::AlignTop );

    QHBoxLayout *lMainLayout = new QHBoxLayout;
    lMainLayout->addLayout( lLayoutLeft, Qt::AlignLeft );
    lMainLayout->addWidget( lDisplayBox );

    QFrame *lMainBox = new QFrame;
    lMainBox->setLayout( lMainLayout );

    setCentralWidget( lMainBox );
    setWindowTitle( tr( "GEVPlayer" ) );
    setWindowIcon( QIcon( QPixmap( ":GEVPlayer/res/GEVPlayer.ico" ) ) );

    CreateMenu();

    LoadMRUFromRegistry();
    UpdateMRUMenu();

    EnableInterface();

    // create a defaults persistence file if it doesn't already exist
    QString lDefaultPath = GetDefaultPath();
    SaveConfig( lDefaultPath, false );

    if ( !mFileName.isEmpty() )
    {
        OpenConfig( mFileName );
    }
    else
    {
        // check for the existence of the sticky configuration file. If it exists, load it.
        QString lStickyPath = GetStickyPath();
        if( PtAccess( lStickyPath.toAscii().data(), 0 ) == 0 )
        {
            OpenConfig( lStickyPath );
        }
    }

    mTimer = new QTimer( this );
    mTimer->setInterval( 250 );
    connect( mTimer, SIGNAL( timeout() ), this, SLOT( OnTimer() ) );
    mTimer->start();
}


///
/// \brief Create connect group box
///

QGroupBox *GEVPlayer::CreateConnectGroup()
{
    mConnectButton = new QPushButton( tr( "Select/&Connect" ) );
    mConnectButton->setMinimumHeight( 27 );
    QObject::connect( mConnectButton, SIGNAL( clicked() ), this, SLOT( OnConnectButton() ) );

    mDisconnectButton = new QPushButton( tr( "&Disconnect" ) );
    mDisconnectButton->setMinimumHeight( 27 );
    QObject::connect( mDisconnectButton, SIGNAL( clicked() ), this, SLOT( OnDisconnectButton() ) );

    QHBoxLayout *lButtonsLayout = new QHBoxLayout;
    lButtonsLayout->addWidget( mConnectButton );
    lButtonsLayout->addWidget( mDisconnectButton );

    QLabel *lIpAddressLabel = new QLabel( tr( "IP Address" ) );
    mIpAddressLine = new QLineEdit;
    mIpAddressLine->setReadOnly( true );
    mIpAddressLine->setEnabled( false );

    QLabel *lMacAddressLabel = new QLabel( tr( "MAC Address" ) );
    mMacAddressLine = new QLineEdit;
    mMacAddressLine->setReadOnly( true );
    mMacAddressLine->setEnabled( false );

    QLabel *lManufacturerLabel = new QLabel( tr( "Manufacturer" ) );
    mManufacturerLine = new QLineEdit;
    mManufacturerLine->setReadOnly( true );
    mManufacturerLine->setEnabled( false );

    QLabel *lModelLabel = new QLabel( tr( "Model" ) );
    mModelLine = new QLineEdit;
    mModelLine->setReadOnly( true );
    mModelLine->setEnabled( false );

    QLabel *lNameLabel = new QLabel( tr( "Name" ) );
    mNameLine = new QLineEdit;
    mNameLine->setReadOnly( true );
    mNameLine->setEnabled( false );

    QGridLayout *lGridLayout = new QGridLayout;
    lGridLayout->addWidget( lIpAddressLabel, 0, 0 );
    lGridLayout->addWidget( mIpAddressLine, 0, 1, 1, 2 );
    lGridLayout->addWidget( lMacAddressLabel, 1, 0 );
    lGridLayout->addWidget( mMacAddressLine, 1, 1, 1, 2 );
    lGridLayout->addWidget( lManufacturerLabel, 2, 0 );
    lGridLayout->addWidget( mManufacturerLine, 2, 1, 1, 2 );
    lGridLayout->addWidget( lModelLabel, 3, 0 );
    lGridLayout->addWidget( mModelLine, 3, 1, 1, 2 );
    lGridLayout->addWidget( lNameLabel, 4, 0 );
    lGridLayout->addWidget( mNameLine, 4, 1, 1, 2 );

    QVBoxLayout *lBoxLayout = new QVBoxLayout;
    lBoxLayout->addLayout( lButtonsLayout );
    lBoxLayout->addLayout( lGridLayout );
    lBoxLayout->addStretch();

    QGroupBox *lConnectionBox = new QGroupBox( tr( "Connection" ) );
    lConnectionBox->setLayout( lBoxLayout );
    lConnectionBox->setMinimumWidth( 300 );
    lConnectionBox->setMaximumWidth( 300 );
    lConnectionBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );

    return lConnectionBox;
}


///
/// \brief Create acquisition group box
///

QGroupBox *GEVPlayer::CreateAcquisition()
{
    QLabel *lSourceLabel = new QLabel( tr( "Source" ) );
    mSourceCombo = new QComboBox;
    mSourceCombo->setEnabled( false );

    QObject::connect( mSourceCombo, SIGNAL( activated( int ) ), this, SLOT( OnCbnSelchangeSource( int ) ) );

    QLabel *lModeLabel = new QLabel( tr( "Mode" ) );
    mModeCombo = new QComboBox;
    mModeCombo->setEnabled( false );

    QObject::connect( mModeCombo, SIGNAL( activated( int ) ), this, SLOT( OnCbnSelchangeMode( int ) ) );

    QGridLayout *lAcqGridLayout = new QGridLayout;
    lAcqGridLayout->addWidget( lSourceLabel, 0, 0 );
    lAcqGridLayout->addWidget( mSourceCombo, 0, 1, 1, 2 );
    lAcqGridLayout->addWidget( lModeLabel, 1, 0 );
    lAcqGridLayout->addWidget( mModeCombo, 1, 1, 1, 2 );

    mStartButton = new QPushButton( tr( "Play" ) );
    mStartButton->setMinimumHeight( 45 );
    mStartButton->setEnabled( false );

    QPixmap lPlayPixmap(":GEVPlayer/res/play.bmp");
    lPlayPixmap.setMask(lPlayPixmap.createMaskFromColor(QColor(0xFF, 0xFF, 0xFF)));
    mStartButton->setIcon(QIcon(lPlayPixmap));

    mStopButton = new QPushButton( tr( "Stop" ) );
    mStopButton->setMinimumHeight( 45 );
    mStopButton->setEnabled( false );

    QPixmap lStopPixmap(":GEVPlayer/res/stop.bmp");
    lStopPixmap.setMask(lStopPixmap.createMaskFromColor(QColor(0xFF, 0xFF, 0xFF)));
    mStopButton->setIcon(QIcon(lStopPixmap));

    QObject::connect( mStartButton, SIGNAL( clicked() ), this, SLOT( OnStart() ) );
    QObject::connect( mStopButton, SIGNAL( clicked() ), this, SLOT( OnStop() ) );

    QHBoxLayout *lButtonsLayout = new QHBoxLayout;
    lButtonsLayout->addWidget( mStartButton );
    lButtonsLayout->addWidget( mStopButton );

    QVBoxLayout *lAcqLayout = new QVBoxLayout;
    lAcqLayout->addLayout( lAcqGridLayout );
    lAcqLayout->addLayout( lButtonsLayout );
    lAcqLayout->addStretch();

    QGroupBox *lAcqBox = new QGroupBox( tr( "Acquisition Control" ) );
    lAcqBox->setLayout( lAcqLayout );
    lAcqBox->setMinimumWidth( 300 );
    lAcqBox->setMaximumWidth( 300 );
    lAcqBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );

    return lAcqBox;
}


///
/// \brief Create parameters group box
///

QGroupBox *GEVPlayer::CreateParameters()
{
    mCommunicationButton = new QPushButton( tr( "Communication control" ) );
    mCommunicationButton->setMinimumHeight( 27 );

    mDeviceButton = new QPushButton( tr( "Device control" ) );
    mDeviceButton->setMinimumHeight( 27 );

    mStreamButton = new QPushButton( tr( "Image stream control" ) );
    mStreamButton->setMinimumHeight( 27 );

    QObject::connect( mCommunicationButton, SIGNAL( clicked() ), this, SLOT( OnShowCommParameters() ) );
    QObject::connect( mDeviceButton, SIGNAL( clicked() ), this, SLOT( OnShowDeviceParameters() ) );
    QObject::connect( mStreamButton, SIGNAL( clicked() ), this, SLOT( OnShowStreamParameters() ) );

    QVBoxLayout *layoutParameters = new QVBoxLayout;
    layoutParameters->addWidget( mCommunicationButton );
    layoutParameters->addWidget( mDeviceButton );
    layoutParameters->addWidget( mStreamButton );
    layoutParameters->addStretch();

    QGroupBox *parametersBox = new QGroupBox( tr( "Parameters and Control" ) );
    parametersBox->setLayout( layoutParameters );
    parametersBox->setMinimumWidth( 300 );
    parametersBox->setMaximumWidth( 300 );

    return parametersBox;
}


///
/// \brief Create display
///

QGroupBox *GEVPlayer::CreateDisplay()
{
    mDisplay = new PvDisplayWnd;

    mStatusLine = new QTextEdit;
    mStatusLine->setReadOnly( true );
    mStatusLine->setEnabled( false );
	mStatusLine->setReadOnly( true );
	mStatusLine->setBackgroundRole( QPalette::Base );
	mStatusLine->setWordWrapMode( QTextOption::NoWrap );
	mStatusLine->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	mStatusLine->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

	QFontMetrics lFM( mStatusLine->font() );
	mStatusLine->setMaximumHeight( lFM.height() * 3 + lFM.lineSpacing() );

    QVBoxLayout *displayLayout = new QVBoxLayout;
    displayLayout->addWidget( mDisplay->GetQWidget() );
    displayLayout->addWidget( mStatusLine, Qt::AlignBottom );

    QGroupBox *lDisplayBox = new QGroupBox( tr( "Display" ) );
    lDisplayBox->setLayout( displayLayout );
    lDisplayBox->setMinimumWidth( 600 );

    return lDisplayBox;
}


///
/// \brief Create menu
///

void GEVPlayer::CreateMenu()
{
    //
    // File
    //

    mFileMenu = menuBar()->addMenu( tr( "&File" ) );
    QObject::connect( mFileMenu, SIGNAL( aboutToShow() ), this, SLOT( OnFileMenuAboutToShow() ) );

    mFileOpenAction = mFileMenu->addAction( tr( "&Open..." ) );
    mFileOpenRecentMenu = mFileMenu->addMenu( tr( "Open Recent" ) );
    mFileSaveAction = mFileMenu->addAction( tr( "&Save" ) );
    mFileSaveAsAction = mFileMenu->addAction( tr( "Save &As..." ) );
    mFileMenu->addSeparator();
    mFileExitAction = mFileMenu->addAction( tr( "E&xit" ) );

    QObject::connect( mFileOpenAction, SIGNAL( triggered() ), this, SLOT( OnFileOpenAction() ) );
    QObject::connect( mFileSaveAction, SIGNAL( triggered() ), this, SLOT( OnFileSaveAction() ) );
    QObject::connect( mFileSaveAsAction, SIGNAL( triggered() ), this, SLOT( OnFileSaveAsAction() ) );
    QObject::connect( mFileExitAction, SIGNAL( triggered() ), this, SLOT( OnFileExitAction() ) );

    QMenu *lDeinterlacingMenu = new QMenu( tr( "Deinterlacing" ) );

    QActionGroup *lDeinterlaceGroup = new QActionGroup( lDeinterlacingMenu );
    mToolsDeinterlacingDisabled = lDeinterlaceGroup->addAction( tr( "Off" ) );
    mToolsDeinterlacingDisabled->setCheckable( true );
    mToolsDeinterlacingWeaving = lDeinterlaceGroup->addAction( tr( "Weave" ) );
    mToolsDeinterlacingWeaving->setCheckable( true );
    lDeinterlacingMenu->addActions( lDeinterlaceGroup->actions() );

    //
    // Tools
    //

    QMenu *mToolsMenu = menuBar()->addMenu( tr( "&Tools" ) );
    QObject::connect( mToolsMenu, SIGNAL( aboutToShow() ), this, SLOT( OnToolsMenuAboutToShow() ) );

    mToolsSetupAction = mToolsMenu->addAction( tr( "&Setup..." ) );
    mToolsMenu->addSeparator();
    mToolsEventMonitorAction = mToolsMenu->addAction( tr( "&Event Monitor..." ) );
    mToolsSerialAction = mToolsMenu->addAction( tr( "IP Engine Serial &Communication..." ) );
    mToolsMenu->addSeparator();
    mToolsFilteringAction = mToolsMenu->addAction( tr( "Image &Filtering..." ) );
    mToolsMenu->addMenu( lDeinterlacingMenu );
    mToolsMenu->addSeparator();
    mToolsDisplayOptionsAction = mToolsMenu->addAction( tr( "Display Options..." ) );
    mToolsBufferOptionsAction = mToolsMenu->addAction( tr( "Buffer Options..." ) );
    mToolsMenu->addSeparator();
    mToolsSaveImagesAction = mToolsMenu->addAction( tr( "Save Images..." ) );
    mToolsSaveCurrentImageAction = mToolsMenu->addAction( tr( "Save Current Image" ) );
    mToolsMenu->addSeparator();
    mToolsSavePreferencesAction = mToolsMenu->addAction( tr( "Save Preferences" ) );
    mToolsRestoreDefaultAction = mToolsMenu->addAction( tr( "Restore Default Preferences" ) );
    mToolsMenu->addSeparator();
    mToolsSaveXMLAction = mToolsMenu->addAction( tr( "Save XML" ) );

    QObject::connect( mToolsSetupAction, SIGNAL( triggered() ), this, SLOT( OnToolsSetupAction() ) );
    QObject::connect( mToolsEventMonitorAction, SIGNAL( triggered() ), this, SLOT( OnToolsEventMonitorAction() ) );
    QObject::connect( mToolsSerialAction, SIGNAL( triggered() ), this, SLOT( OnToolsSerialAction() ) );
    QObject::connect( mToolsFilteringAction, SIGNAL( triggered() ), this, SLOT( OnToolsFilteringAction() ) );
    QObject::connect( mToolsDisplayOptionsAction, SIGNAL( triggered() ), this, SLOT( OnToolsDisplayOptionsAction() ) );
    QObject::connect( mToolsBufferOptionsAction, SIGNAL( triggered() ), this, SLOT( OnToolsBufferOptionsAction() ) );
    QObject::connect( mToolsSaveImagesAction, SIGNAL( triggered() ), this, SLOT( OnToolsSaveImagesAction() ) );
    QObject::connect( mToolsSaveCurrentImageAction, SIGNAL( triggered() ), this, SLOT( OnToolsSaveCurrentImageAction() ) );
    QObject::connect( mToolsSavePreferencesAction, SIGNAL( triggered() ), this, SLOT( OnToolsSavepreferences() ) );
    QObject::connect( mToolsRestoreDefaultAction, SIGNAL( triggered() ), this, SLOT( OnToolsRestoredefaultpreferences() ) );
    QObject::connect( mToolsSaveXMLAction, SIGNAL( triggered() ), this, SLOT( OnToolsSaveXML() ) );
    QObject::connect( mToolsDeinterlacingDisabled, SIGNAL( triggered() ), this, SLOT( OnToolsDeinterlacingDisabled() ) );
    QObject::connect( mToolsDeinterlacingWeaving, SIGNAL( triggered() ), this, SLOT( OnToolsDeinterlacingWeaving() ) );

    //
    // Help
    //

    QMenu *mHelpMenu = menuBar()->addMenu( tr( "&Help" ) );
    QObject::connect( mHelpMenu, SIGNAL( aboutToShow() ), this, SLOT( OnHelpMenuAboutToShow() ) );

    mHelpAboutAction = mHelpMenu->addAction( tr( "About GEVPlayer..." ) );
    QObject::connect( mHelpAboutAction, SIGNAL( triggered() ), this, SLOT( OnHelpAboutAction() ) );

    // 
    // Register interface (only accessible by shortcut)
    //
    QShortcut* lRegisterInterfaceAction = new QShortcut( QKeySequence(Qt::ControlModifier + Qt::AltModifier + Qt::Key_R), this );
    QObject::connect( lRegisterInterfaceAction, SIGNAL( activated() ), this, SLOT( OnRegisterInterface() ) );
}


///
/// \brief Register interface menu handler
///

void GEVPlayer::OnRegisterInterface()
{
    PvRegisterInterfaceWnd lInterface;
    lInterface.SetDevice( &mDevice );
    lInterface.ShowModal( this );
}


///
/// \brief Connect button click handler
///

void GEVPlayer::OnConnectButton()
{
    // create a device finder wnd and open the select device dialog
    PvDeviceFinderWnd lFinder;
    PvResult lResult = lFinder.ShowModal(this);
    if ( ( !lResult.IsOK() ) || ( lFinder.GetSelected() == NULL ) )
    {
        return;
    }

    Connect( lFinder.GetSelected(), NULL );
}


///
/// \brief Disconnect button click handler
///

void GEVPlayer::OnDisconnectButton()
{
    Disconnect();
}


///
/// \brief Start button click handler
///

void GEVPlayer::OnStart()
{
    if ( !mDeviceConnected )
    {
        return;
    }

    StartAcquisition();

    EnableInterface();
}


///
/// \brief Stop button click handler
///

void GEVPlayer::OnStop()
{
    if ( !mDeviceConnected )
    {
        return;
    }

    SetStatusColor( SCDefault );
    mStatusLine->setText( tr( "" ) );

    PvGenCommand *lStop = mDevice.GetGenParameters()->GetCommand( "AcquisitionStop" );
    if ( lStop->IsAvailable() )
    {
        StopAcquisition();
    }

    EnableInterface();
}


///
/// \brief Syncs the UI enabled state
///

void GEVPlayer::EnableInterface()
{
    mConnectButton->setEnabled( !mDeviceConnected && !mStream->IsOpen() );
    mDisconnectButton->setEnabled( mDeviceConnected || mStream->IsOpen() );

	EnableGenBrowsers( true );
	EnableControls( mDeviceConnected );
}


///
/// \brief Enables Start, Stop controls
///

void GEVPlayer::EnableControls( bool aEnabled )
{
	bool lModeEnabled = false;
	bool lStartEnabled = false;
	bool lStopEnabled = false;
	bool lSourceEnabled = false;

	PvAcquisitionState lAcquisitionState = PvAcquisitionStateUnknown;
	if ( mAcquisitionStateManager != NULL )
	{
		lAcquisitionState = mAcquisitionStateManager->GetState();
	}

	PvGenCommand *lStart = mDevice.GetGenParameters()->GetCommand( "AcquisitionStart" );
	PvGenCommand *lStop = mDevice.GetGenParameters()->GetCommand( "AcquisitionStop" );
	PvGenEnum *lMode = mDevice.GetGenParameters()->GetEnum( "AcquisitionMode" );
	PvGenEnum *lSource = mDevice.GetGenParameters()->GetEnum( "SourceSelector" );

	// Used to temporarely select the source in use
	PvGenStateStack lStack( mDevice.GetGenParameters() );
    PushSource( &lStack );

	if ( IsMultiSourceTransmitter( &mDevice ) )
	{
		lSourceEnabled = aEnabled &&
			lSource != NULL &&
			lSource->IsWritable() &&
			lAcquisitionState != PvAcquisitionStateLocked;
	}

	if ( lMode != NULL )
	{
		lModeEnabled = aEnabled &&
			lMode->IsWritable() &&
			lAcquisitionState != PvAcquisitionStateLocked;
	}

	if ( lStart != NULL )
	{
		lStartEnabled = aEnabled
			&& lAcquisitionState != PvAcquisitionStateLocked;
	}

	if ( lStop != NULL )
	{
		lStopEnabled = aEnabled
			&& lAcquisitionState != PvAcquisitionStateUnlocked;
	}

	mStartButton->setEnabled( lStartEnabled );
	mStopButton->setEnabled( lStopEnabled );
	mModeCombo->setEnabled( lModeEnabled );
	mSourceCombo->setEnabled( lSourceEnabled );
}


///
/// \brief Enables the buttons used to open the browsers
///

void GEVPlayer::EnableGenBrowsers( bool aEnabled )
{
    mDeviceButton->setEnabled( aEnabled && mDeviceConnected );
    mStreamButton->setEnabled( aEnabled && mStream->IsOpen() );
    mCommunicationButton->setEnabled( aEnabled && ( mSetupDlg->GetRole() != SetupDlg::RoleData ) );
}


///
/// \brief Shows one of the GenICam browsers
///

void GEVPlayer::ShowGenWindow( PvGenBrowserWnd *aWnd, PvGenParameterArray *aArray, const QString &aTitle )
{
    if ( aWnd->GetQWidget()->isVisible() )
    {
        // If already open, just toggle to closed...
        CloseGenWindow( aWnd );
        return;
    }

    // Create, assigne parameters, set title and show modeless
    aWnd->SetTitle( aTitle.toAscii().data() );
    
    aWnd->ShowModeless(this);
    aWnd->SetGenParameterArray( aArray );
}


///
/// \brief Closes a GenICam tree browser.
///

void GEVPlayer::CloseGenWindow( PvGenBrowserWnd *aWnd )
{
    if ( aWnd->GetQWidget()->isVisible() )
    {
        aWnd->Close();
    }
}


///
/// \brief Connects the device/stream
///

void GEVPlayer::Connect( PvDeviceInfo *aDI, PvConfigurationReader *aCR )
{
    assert( aDI != NULL || aCR != NULL );
    if ( aDI == NULL && aCR == NULL )
    {
        return;
    }

    if ( aDI != NULL )
    {
        PvDeviceClass lClass = aDI->GetClass();
        SetupDlg::Role lRole = mSetupDlg->GetRole();

		if ( ( lClass == PvDeviceClassReceiver ) ||
			 ( lClass == PvDeviceClassPeripheral ) ||
			 ( lClass == PvDeviceClassTransceiver ) )
        {
			// For receiver and peripheral, we make sure  role is controller only
            if ( lRole == SetupDlg::RoleCtrlData || lRole == SetupDlg::RoleData )
            {
                QString lMessage;

				if ( ( lClass == PvDeviceClassReceiver ) ||
					 ( lClass == PvDeviceClassPeripheral ) )
				{
					// Receiver and peripheral message
					lMessage = tr( "You have chosen to connect to a " );
					lMessage += ( lClass == PvDeviceClassReceiver ) ? tr( "receiver" ) : tr( "peripheral" );
					lMessage += tr( " however GEVPlayer is currently set up to connect as a " );
					lMessage += ( lRole == SetupDlg::RoleCtrlData ) ? tr( "controller and data receiver" ) : tr( "data receiver" );
					lMessage += tr( ". \r\n\r\nRe-attempt connection as a controller?" );
				}
				else if ( lClass == PvDeviceClassTransceiver )
				{
					// Transceiver message
					lMessage = tr( "You have chosen to connect to a transceiver. GEVPlayer does not fully\r\n" );
					lMessage += tr( "support transceivers in this version. Re-attempt connection as a \r\n" );
					lMessage += tr( "controller?" );
				}
				else
				{
					assert( 0 );
				}

                QMessageBox::StandardButton lRetVal = QMessageBox::question( this, "Warning", lMessage, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
                if ( lRetVal == QMessageBox::No )
                {
                    return;
                }
                else
                {
                    mSetupDlg->SetRole( SetupDlg::RoleCtrl );
					mSetupDlg->SetDestination( SetupDlg::DestinationUnicastOther );
					mSetupDlg->SetUnicastIP( tr( "0.0.0.0" ) );
					mSetupDlg->SetUnicastPort( 0 );
                }
            }
        }
    }

    // Just in case we came here still connected...
    Disconnect();

    PvInt32 lPreferredSource = -1;

    // Device connection, packet size negociation and stream opening
    // is performed in a separate thread while we display a progress
    // dialog to the user
    PvResult lResult;
    if ( aDI != NULL )
    {
        ConnectionThread lConnectionThread( mSetupDlg, aDI, &mDevice, mStream, this );
        lResult = lConnectionThread.Connect();
    }
    else if ( aCR != NULL )
    {
        PvStringList lErrorList;
        LoadingThread lLoadingThread( mSetupDlg, aCR, &mDevice, mStream, &lErrorList, this );
        lResult = lLoadingThread.Load();

        if ( lErrorList.GetSize() > 0 )
        {
            ErrorsDlg lDlg( this, &lErrorList );
            lDlg.exec();
        }

        if ( IsMultiSourceTransmitter( &mDevice ) )
        {
            PvString lSourceStr;
            if ( aCR->Restore( TAG_ACTIVESOURCE, lSourceStr ).IsOK() )
            {
                sscanf( lSourceStr.GetAscii(), "%d", &lPreferredSource );
            }
        }
    }

    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
        Disconnect();

        return;
    }

    mDeviceConnected = mDevice.IsConnected();

    if ( aDI != NULL )
    {
        mManufacturerLine->setText( aDI->GetVendor().GetAscii() );
        mModelLine->setText( aDI->GetModel().GetAscii() );
        mIpAddressLine->setText( aDI->GetIPAddress().GetAscii() );
        mMacAddressLine->setText( aDI->GetMACAddress().GetAscii() );
        mNameLine->setText( aDI->GetUserDefinedName().GetAscii() );
    }

    if ( mAcquisitionStateManager != NULL )
    {
        delete mAcquisitionStateManager;
        mAcquisitionStateManager = NULL;
    }

    if ( mDeviceConnected )
    {
        // Create the event monitor *before* registering the device event sink
        assert( mEventMonitorDlg == NULL );
        mEventMonitorDlg = new EventMonitorDlg( this, mLogBuffer, mDevice.GetGenParameters() );

        // Register device event sink
        mDevice.RegisterEventSink( this );

        // Register to all events of the parameters in the device's node map
        PvGenParameterArray *lGenDevice = mDevice.GetGenParameters();
        for ( PvUInt32 i = 0; i < lGenDevice->GetCount(); i++ )
        {
            lGenDevice->Get( i )->RegisterEventSink( this );
        }

        if ( aDI == NULL )
        {
            PvGenInteger *lIPAddressParam = lGenDevice->GetInteger( "GevCurrentIPAddress" );
            PvGenInteger *lMACAddressParam = lGenDevice->GetInteger( "GevMACAddress" );
            PvGenString *lManufacturer = lGenDevice->GetString( "DeviceVendorName" );
            PvGenString *lModelName = lGenDevice->GetString( "DeviceModelName" );
            PvGenString *lDeviceVersion = lGenDevice->GetString( "DeviceVersion" );
            PvGenString *lNameParam = lGenDevice->GetString( "GevDeviceUserID " );

            // Verify all params - except name which is optional
            assert( lIPAddressParam != NULL );
            assert( lMACAddressParam != NULL );
            assert( lManufacturer != NULL );
            assert( lModelName != NULL );
            assert( lDeviceVersion != NULL );

            PvString lManufacturerStr, lModelNameStr, lDeviceVersionStr;
            lManufacturer->GetValue( lManufacturerStr );
            lModelName->GetValue( lModelNameStr );
            lDeviceVersion->GetValue( lDeviceVersionStr );

            // IP
            QString lIPStr;
            PvInt64 lIPAddress = 0;
            lIPAddressParam->GetValue( lIPAddress );
            unsigned char *lIPPtr = reinterpret_cast<unsigned char *>( &lIPAddress );
			lIPStr.sprintf( "%i.%i.%i.%i",
				lIPPtr[3], lIPPtr[2], lIPPtr[1], lIPPtr[0] );

            // MAC address
            QString lMACStr;
            PvInt64 lMACAddress;
            lMACAddressParam->GetValue( lMACAddress );
            unsigned char *lMACPtr = reinterpret_cast<unsigned char *>( &lMACAddress );
			lMACStr.sprintf( "%02X:%02X:%02X:%02X:%02X:%02X",
				lMACPtr[5], lMACPtr[4], lMACPtr[3], lMACPtr[2], lMACPtr[1], lMACPtr[0] );

            // Device name (User ID)
            QString lNameStr( "N/A" );
            if ( lNameParam != NULL )
            {
                PvString lStr;
                lNameParam->GetValue( lStr );
                lNameStr = lStr.GetAscii();
            }

            mManufacturerLine->setText( lManufacturerStr.GetAscii() );
            mModelLine->setText( lModelNameStr.GetAscii() );
            mIpAddressLine->setText( lIPStr );
            mMacAddressLine->setText( lMACStr );
            mNameLine->setText( lNameStr );
        }

        // Fill acquisition mode combo box
        mModeCombo->clear();
        PvGenEnum *lMode = mDevice.GetGenParameters()->GetEnum( "AcquisitionMode" );
        if ( lMode != NULL )
        {
            assert( lMode != NULL ); // Mandatory parameter
            PvInt64 lEntriesCount = 0;
            lMode->GetEntriesCount( lEntriesCount );
            for ( PvUInt32 i = 0; i < lEntriesCount; i++ )
            {
                const PvGenEnumEntry *lEntry = NULL;
                lMode->GetEntryByIndex( i, &lEntry );

                bool lAvailable = false;
                lEntry->IsAvailable( lAvailable );
                if ( lAvailable )
                {
                    PvString lEEName;
                    lEntry->GetName( lEEName );

                    PvInt64 lEEValue;
                    lEntry->GetValue( lEEValue );

                    mModeCombo->addItem( lEEName.GetAscii(), lEEValue );
                }
            }
        }

        PvGenCommand *lStart = mDevice.GetGenParameters()->GetCommand( "AcquisitionStart" );
        PvGenCommand *lStop = mDevice.GetGenParameters()->GetCommand( "AcquisitionStop" );

        // Check whether IP Engine serial communication is supported or not
        mSerial0IPEngineSupported = PvSerialPortIPEngine::IsSupported( &mDevice, PvIPEngineSerial0 );
        mSerial0IPEngineSupported |= PvSerialPortIPEngine::IsSupported( &mDevice, PvIPEngineSerialBulk0 );

        PvInt64 lSource = 0;
        mDevice.GetGenParameters()->GetIntegerValue( "SourceSelector", lSource );

        // Regardless of whether a corresponding stream is open or not, we can still pass it to the acquisition state manager
        assert( mAcquisitionStateManager == NULL );
        mAcquisitionStateManager = new PvAcquisitionStateManager( &mDevice, mStream, static_cast<PvUInt32>( lSource ) );
        mAcquisitionStateManager->RegisterEventSink( this );

        QCoreApplication::sendEvent( this, new QWindowsEvent( static_cast<QEvent::Type>( WM_UPDATESOURCES ), 0, lPreferredSource ) );
        QCoreApplication::sendEvent( this, new QEvent( static_cast<QEvent::Type>( WM_UPDATEACQUISITIONMODE ) ) );
    }

    if ( mStream->IsOpen() )
    {
        // Ready image reception
        StartStreaming();
    }

    // Sync the UI with our new status
    EnableInterface();
}


///
/// \brief Disconnects the device/stream
///

void GEVPlayer::Disconnect()
{
    // Close all configuration child windows
    CloseGenWindow( mDeviceWnd );
    CloseGenWindow( mStreamWnd );
    mFilteringDlg->hide();
    if ( mTerminalIPEngineWnd->GetQWidget()->isVisible() )
    {
        mTerminalIPEngineWnd->Close();
    }

    if ( mDevice.IsConnected() || mDeviceConnected )
    {
        // Unregister device event sink
        mDevice.UnregisterEventSink( this );

        // Close event monitor
        if ( mEventMonitorDlg != NULL )
        {
            if ( mEventMonitorDlg->isVisible() != 0 )
            {
                mEventMonitorDlg->close();
            }

            delete mEventMonitorDlg;
            mEventMonitorDlg = NULL;
        }

        mDevice.Disconnect();
    }

    // If streaming, stop streaming
    if ( mStream->IsOpen() )
    {
        StopStreaming();
        mStream->Close();
    }

    mDisplay->Clear();

    // Reset device ID
    mManufacturerLine->setText( "" );
    mModelLine->setText( "" );
    mIpAddressLine->setText( "" );
    mMacAddressLine->setText( "" );
    mNameLine->setText( "" );

    mStatusLine->setText( "" );
    SetStatusColor( SCDefault );

    mDeviceConnected = false;

    // Sync the UI with our new status
    EnableInterface();
    setWindowTitle( "GEVPlayer" );

    mSourceCombo->clear();
    mModeCombo->clear();
}


///
/// \brief Start display thread, arm pipeline
///

void GEVPlayer::StartStreaming()
{
    QMutexLocker lLock( &mStartStreamingMutex );

    if ( mThreadDisplay == NULL )
    {
        // Create display thread
        mThreadDisplay = new ThreadDisplay( this, mDisplay, mPipeline,
            mFilteringDlg, mImageSaveDlg, mLogBuffer );

        // apply the options from the previously saved configuration
        mDisplayOptionsDlg->Apply( mThreadDisplay );
		mThreadDisplay->SetDeinterlacing( mDeinterlacing );

        // Start threads
        mThreadDisplay->start();
        mThreadDisplay->setPriority( QThread::HighPriority );
    }

    if ( !mPipeline->IsStarted() )
    {
        mPipeline->Start();
    }
}


///
/// \brief Stops display thread, pipeline
///

void GEVPlayer::StopStreaming()
{
    QMutexLocker lLock( &mStartStreamingMutex );

    // Stop display thread
    if ( mThreadDisplay != NULL )
    {
        mThreadDisplay->stop();
        mThreadDisplay->wait();

        if ( mPipeline->IsStarted() )
        {
        	mPipeline->Stop();
        }

        // Destroy thread. Blocks until the thread is completed.
        delete mThreadDisplay;
        mThreadDisplay = NULL;
    }
}


///
/// \brief Starts the acquisition
///

void GEVPlayer::StartAcquisition()
{
    PvGenCommand *lResetStats = mStream->GetParameters()->GetCommand( "Reset" );

    if ( mStream->IsOpen() )
    {
        PvUInt32 lPayloadSize = GetPayloadSize();
        if ( lPayloadSize > 0 )
        {
            mPipeline->SetBufferSize( lPayloadSize );
        }

        mPipeline->Reset();
    }

    lResetStats->Execute();
    mBuffersReallocated = false;

    if ( mThreadDisplay != NULL )
    {
        mThreadDisplay->ResetStats();
    }

    if ( mImageSaveDlg != NULL )
    {
        mImageSaveDlg->ResetStats();
    }

    assert( mAcquisitionStateManager != NULL );
    PvResult lResult = mAcquisitionStateManager->Start();
    if ( !lResult.IsOK() )
    {
        if ( mShowAcquisitionWarning )
        {
            WarningDlg lAcquisitionWarning( this, lResult.GetDescription().GetAscii(), "Don't show this warning again in this session." );
            lAcquisitionWarning.exec();
            if ( lAcquisitionWarning.IsDontShowAgain() )
            {
                mShowAcquisitionWarning = false;
            }
        }
    }
    else if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
    }
}


///
/// \brief Stops the acquisition
///

void GEVPlayer::StopAcquisition()
{
    assert( mAcquisitionStateManager != NULL );
    PvResult lResult = mAcquisitionStateManager->Stop();
    if ( !lResult.IsOK() )
    {
        if ( mShowAcquisitionWarning )
        {
            WarningDlg lAcquisitionWarning( this, lResult.GetDescription().GetAscii(), "Don't show this warning again in this session." );
            lAcquisitionWarning.exec();
            if ( lAcquisitionWarning.IsDontShowAgain() )
            {
                mShowAcquisitionWarning = false;
            }
        }
    }
    else if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
    }
}


///
/// \brief Acquisition mode combo box change handler
///

void GEVPlayer::OnCbnSelchangeMode( int aIndex )
{
    if ( !mDeviceConnected || ( mAcquisitionStateManager == NULL ) )
    {
        return;
    }

    // No selection?
    if ( mModeCombo->currentIndex() < 0 )
    {
        return;
    }

    // Set source selector if needed
    PvGenStateStack lStack( mDevice.GetGenParameters() );
    PushSource( &lStack );

    // Get acquisition mode parameter
    PvGenEnum *lMode = mDevice.GetGenParameters()->GetEnum( "AcquisitionMode" );
    if ( lMode == NULL )
    {
        return;
    }

    // Retrieve item data value from combo box item, assign it to acquisition mode parameter
    QVariant lData = mModeCombo->itemData( aIndex );
    if ( lData.canConvert<PvInt64>() )
    {
        PvInt64 lValue = lData.value<PvInt64>();
        PvResult lResult = lMode->SetValue( lValue );
        if ( !lResult.IsOK() )
        {
			PvMessageBox( this, lResult );
        }
    }
}


///
/// \brief Image displayed async handler.
///
/// Typically called from the display thread.
///

void GEVPlayer::OnImageDisplayedEvent()
{
    // Stream opened, image save dlg exists, thread display is up
    if ( ( mStream == NULL ) ||
		 ( !mStream->IsOpen() ) ||
         ( mImageSaveDlg == NULL ) ||
         ( mThreadDisplay == NULL ) )
    {
        return;
    }

    QLocale lLocale;

    PvInt64 lCount = 0;
    QString lTimestampWarning = "";
    QString lImagesCount = "N/A";
    QString lAvgFPS = "N/A";
    QString lBandwidth = "N/A";
    QString lStr;
    QString lDisplayFPS = "N/A";

    if ( mImageSaveDlg->GetSaveEnabled() )
    {
        SetStatusColor( SCRed );

        if ( mImageSaveDlg->GetFrames() > 0 )
        {
			lAvgFPS = lLocale.toString( mImageSaveDlg->GetFPS(), 'f', 1 );
        }

        QString lMbps = lLocale.toString( mImageSaveDlg->GetMbps(), 'f', 1 );

        lStr.sprintf( "RECORDING %d images    %lld MB    %s FPS    %s Mbps", mImageSaveDlg->GetFrames(),
            mImageSaveDlg->GetTotalSize(), lAvgFPS.toAscii().data(), lMbps.toAscii().data() );
        mStatusLine->setText( lStr );
    }
    else
    {
    	PvStreamInfo lInfo( mStream );
    	QString lStatistics = lInfo.GetStatistics( mThreadDisplay->GetFrameRate() ).GetAscii();
    	QString lErrors = lInfo.GetErrors().GetAscii();
    	QString lWarnings = lInfo.GetWarnings( mBuffersReallocated ).GetAscii();

        SetStatusColor( SCDefault );
        mStatusLine->setText( lStatistics + "\r\n" + lErrors + "\r\n" + lWarnings );
    }

    return;
}


///
/// \brief OnLinkDisconnected callback
///

void GEVPlayer::OnLinkDisconnected( PvDevice *aDevice )
{
    mDisconnectTime.restart();

    QCoreApplication::postEvent( this, new QEvent( static_cast<QEvent::Type>( WM_STOPSTREAMING ) ) );
    QCoreApplication::postEvent( this, new QEvent( static_cast<QEvent::Type>( WM_DISCONNECT ) ) );
}


///
/// \brief OnLinkReconnected PvDevice callback
///

void GEVPlayer::OnLinkReconnected( PvDevice *aDevice )
{
    QCoreApplication::postEvent( this, new QEvent( static_cast<QEvent::Type>( WM_LINKRECONNECTED ) ) );
}


///
/// \brief OnParameter update PvParameter callback
///

void GEVPlayer::OnParameterUpdate( PvGenParameter *aParameter )
{
    if ( mAcquisitionStateManager == NULL )
    {
        return;
    }

    PvString lName = aParameter->GetName();
    if ( lName == "AcquisitionMode" )
    {
        // Prevent re-entry
        if ( !mUpdatingAcquisitionMode )
        {
            if ( IsMultiSourceTransmitter( &mDevice ) )
            {
                PvInt64 lSource = 0;
                mDevice.GetGenParameters()->GetEnumValue( "SourceSelector", lSource );
                if ( lSource == mAcquisitionStateManager->GetSource() )
                {
                    // Only refresh if the source selector is that same as the source used by GEVPlayer
                    mUpdatingAcquisitionMode = true;
                    QCoreApplication::postEvent( this, new QEvent( static_cast<QEvent::Type>( WM_UPDATEACQUISITIONMODE ) ) );
                }
            }
            else
            {
                // No source selector, always refresh
                mUpdatingAcquisitionMode = true;
                QCoreApplication::postEvent( this, new QEvent( static_cast<QEvent::Type>( WM_UPDATEACQUISITIONMODE ) ) );
            }
        }
    }

    if ( ( lName == "SourceSelector" ) && ( IsMultiSourceTransmitter( &mDevice ) ) )
    {
        if ( !mUpdatingSources && IsMultiSourceTransmitter( &mDevice ) )
        {
            PvGenEnum *lSourceSelector = dynamic_cast<PvGenEnum *>( aParameter );
            if ( lSourceSelector == NULL )
            {
                return;
            }

            // Retrieve enum entry count from source selector
            PvInt64 lSourceCount = 0;
            lSourceSelector->GetEntriesCount( lSourceCount );

            // Build list of available sources
            std::vector<PvInt64> lSources;
            for ( PvInt64 i = 0; i < lSourceCount; i++ )
            {
                const PvGenEnumEntry *lEE = NULL;
                lSourceSelector->GetEntryByIndex( i, &lEE );
                if ( lEE->IsAvailable() )
                {
                    PvInt64 lValue = 0;
                    lEE->GetValue( lValue );

                    lSources.push_back( lValue );
                }
            }

            // Compare available sources vs. cached combo box content
            bool lIsTheSame = false;
            if ( lSources.size() == mAvailableSources.size() )
            {
                lIsTheSame = true;
                std::vector<PvInt64>::const_iterator lIt1 = lSources.begin();
                std::vector<PvInt64>::const_iterator lIt2 = mAvailableSources.begin();
                while ( ( lIt1 != lSources.end() ) && ( lIt2 != mAvailableSources.end() ) )
                {
                    if ( *lIt1 != *lIt2 )
                    {
                        lIsTheSame = false;
                        break;
                    }

                    lIt1++;
                    lIt2++;
                }
            }

            if ( !lIsTheSame )
            {
                mUpdatingSources = true;
                QCoreApplication::postEvent( this, new QWindowsEvent( static_cast<QEvent::Type>( WM_UPDATESOURCES ) ) );
            }
        }
    }
}


///
/// \brief Changes the status color (text and/or background)
///

void GEVPlayer::SetStatusColor( StatusColor aColor )
{
    switch( aColor )
    {
    case SCRed:
        mStatusLine->setPalette( QPalette(  Qt::red ) );
        break;
    case SCYellow:
        mStatusLine->setPalette( QPalette( Qt::yellow ) );
        break;
    default:
        mStatusLine->setPalette( palette() );
    }
    
}


///
/// \brief Generic event handler
///

bool GEVPlayer::event( QEvent *aEvent )
{
    switch ( aEvent->type() )
    {
    case WM_IMAGEDISPLAYED:
        OnImageDisplayedEvent();
        break;

    case WM_STOPSTREAMING:
        OnStopStreamingEvent();
        break;

    case WM_STREAM:
    {
    	QWindowsEvent *lWE = dynamic_cast<QWindowsEvent *>( aEvent );
    	assert( lWE != NULL );

    	if ( lWE != NULL )
    	{
        	OnStreamEvent( lWE->GetWParam() != 0 );
    	}
    }
    	break;

    case WM_DISCONNECT:
        OnDisconnectEvent();
        break;

    case WM_LINKRECONNECTED:
        OnLinkReconnectedEvent();
        break;

    case WM_ACQUISITIONSTATECHANGED:
    	OnAcquisitionStateChangedEvent();
    	break;

    case WM_UPDATESOURCE:
    	OnUpdateSourceEvent();
    	break;

    case WM_UPDATESOURCES:
    {
    	QWindowsEvent *lWE = dynamic_cast<QWindowsEvent *>( aEvent );
    	assert( lWE != NULL );

    	if ( lWE != NULL )
    	{
    		OnUpdateSourcesEvent( lWE->GetLParam() );
    	}
    }
    	break;

    case WM_UPDATEACQUISITIONMODE:
    	OnUpdateAcquisitionModeEvent();
    	break;
    }

    return QWidget::event( aEvent );
}


///
/// \brief File menu about to show. Update what needs to be updated.
///

void GEVPlayer::OnFileMenuAboutToShow()
{
    mFileSaveAction->setEnabled( mDeviceConnected || mStream->IsOpen() );
    mFileSaveAsAction->setEnabled( mDeviceConnected || mStream->IsOpen() );
    mFileOpenAction->setEnabled( ( !mCommunicationWnd->GetQWidget()->isVisible() ) &&
            ( !mDeviceWnd->GetQWidget()->isVisible() ) &&
            ( !mStreamWnd->GetQWidget()->isVisible() ) );
}


///
/// \brief Tools menu about to show. Update what needs to be updated.
///

void GEVPlayer::OnToolsMenuAboutToShow()
{
    mToolsEventMonitorAction->setEnabled( mEventMonitorDlg != NULL );
    mToolsEventMonitorAction->setChecked( ( ( mEventMonitorDlg != NULL ) && mEventMonitorDlg->isVisible() ) );

    mToolsDisplayOptionsAction->setEnabled( mThreadDisplay != NULL );
    mToolsBufferOptionsAction->setEnabled( mStream->IsOpen() );

    mToolsSaveImagesAction->setEnabled( mThreadDisplay != NULL );
    mToolsSaveCurrentImageAction->setEnabled( mThreadDisplay != NULL );

    mToolsFilteringAction->setEnabled( mStream->IsOpen() );
    mToolsFilteringAction->setChecked( mFilteringDlg->isVisible() );

    mToolsSerialAction->setEnabled( mDevice.IsConnected() && mSerial0IPEngineSupported );
    mToolsSaveXMLAction->setEnabled( mDevice.IsConnected() );

    mToolsDeinterlacingDisabled->setEnabled( mThreadDisplay != NULL );
	mToolsDeinterlacingDisabled->setChecked( false );
    mToolsDeinterlacingWeaving->setEnabled( mThreadDisplay != NULL );
	mToolsDeinterlacingWeaving->setChecked( false );

    switch ( mDeinterlacing )
    {
    case DeinterlacingDisabled:
    	mToolsDeinterlacingDisabled->setChecked( true );
    	break;

    case DeinterlacingWeaving:
    	mToolsDeinterlacingWeaving->setChecked( true );
    	break;

    default:
    	assert( 0 );
    }
}


///
/// \brief Help menu about to show. Update what needs to be updated.
///

void GEVPlayer::OnHelpMenuAboutToShow()
{
}


///
/// \brief Menu action handler
///

void GEVPlayer::OnFileOpenAction()
{
     mFileName = QFileDialog::getOpenFileName(this, tr("Open eBUS SDK Project"),
                                                     NULL,
                                                     tr("PureGEV Configuration Files (*.pvcfg)"));
     if ( !mFileName.isNull() )
     {
         OpenConfig( mFileName );
     }
}


///
/// \brief Open a configuration file and restore previous state from it
///

void GEVPlayer::OpenConfig( QString aFileName )
{
    assert( mDisplayOptionsDlg != NULL );
    assert( mSetupDlg != NULL );
    assert( mFilteringDlg != NULL );
    assert( mImageSaveDlg != NULL );
    assert( mCommunicationWnd != NULL );
    assert( mDeviceWnd != NULL );
    assert( mStreamWnd != NULL );
    assert( mPipeline != NULL );

    if ( mCommunicationWnd != NULL && mCommunicationWnd->GetQWidget()->isVisible() )
    {
        QMessageBox::warning( this, "Error", "Please close the communication control window before loading a new configuration." );
        return;
    }
    if ( mStreamWnd != NULL && mStreamWnd->GetQWidget()->isVisible() )
    {
        QMessageBox::warning( this, "Error", "Please close the image stream control window before loading a new configuration." );
        return;
    }
    if ( mDeviceWnd != NULL && mDeviceWnd->GetQWidget()->isVisible() )
    {
        QMessageBox::warning( this, "Error", "Please close the Device control window before loading a new configuration." );
        return;
    }

    // Create reader, load file
    PvConfigurationReader lReader;
    if ( !lReader.Load( aFileName.toAscii().data() ).IsOK() )
    {
        QMessageBox::warning( this, "Error", "The configuration you have attempted to load is invalid.");
        return;
    }

    // we're loading a new persistence file. Report it now so that it's visible in the title bar while we're loading.
    if ( !mFileName.isEmpty() && aFileName == mFileName )
    {
        ReportMRU( mFileName );
    }

    // Check if roles are the same if not, we must disconnect first
    if ( ( mDeviceConnected || mStream->IsOpen() ) &&
    	 !mSetupDlg->IsTheSame( lReader ) )
    {
        QMessageBox::StandardButton lRetVal = QMessageBox::question( this, "Warning"
                                , "Setup of this configuration file does not match the current "
                                "connection.\r\n\r\nIn order to proceed with the operation, "
                                "the GigE Vision Device will be disconnected.\r\n\r\nContinue "
                                "restoring configuration?"
                                , QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
        if ( lRetVal == QMessageBox::No )
        {
            return;
        }

        Disconnect();
    }

    mSetupDlg->Load( lReader );
    mFilteringDlg->Load( lReader );

    // Load data in log buffer. If event monitor is up, notify it to update
    // its content based on the log buffer data that likely just changed
    mLogBuffer->Load( lReader );
	if ( mEventMonitorDlg != NULL )
	{
		mEventMonitorDlg->LoadData();
	}
    
    if ( lReader.GetGenParameterArrayCount() > 0 )
    {
        lReader.Restore( "communication", *mDevice.GetGenLink() );
    }

    if ( lReader.GetDeviceCount() > 0 || lReader.GetStreamCount() > 0 )
    {
        // If we're connected, just apply the settings. Otherwise connect from the settings in the persistence file.
        if ( mDeviceConnected )
        {
                QMessageBox::StandardButton lRetVal = QMessageBox::question( this, "Warning"
                    , "Opening a file now will apply the settings to the currently connected "
                     "GigE Vision Device. \r\n\r\nIf you wish to connect to a different GigEVision Device associated "
                     "with the file, disconnect first, then open the file. \r\n\r\nContinue opening file?"
                    , QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
            if ( lRetVal == QMessageBox::No )
            {
                return;
            }

            PvStringList lErrorList;
            LoadingThread lLoadingThread( mSetupDlg, &lReader, &mDevice, mStream, &lErrorList, this );
            lLoadingThread.Load();

            if ( lErrorList.GetSize() > 0 )
            {
                ErrorsDlg lDlg( this, &lErrorList );
                lDlg.exec();
            }

            if ( IsMultiSourceTransmitter( &mDevice ) )
            {
                PvString lSourceStr;
                if ( lReader.Restore( TAG_ACTIVESOURCE, lSourceStr ).IsOK() )
                {
                    PvInt32 lValue = 0;
                    sscanf( lSourceStr.GetAscii(), "%d", &lValue );

                    QCoreApplication::postEvent( this, new QWindowsEvent( static_cast<QEvent::Type>( WM_UPDATESOURCES ), 0, lValue ) );
                }
            }
        }
        else
        {
            Connect( NULL, &lReader );
        }

        mDeviceConnected = mDevice.IsConnected();
    }

    mImageSaveDlg->Load( lReader );

    mDisplayOptionsDlg->Load( lReader );
    if ( mThreadDisplay != NULL )
    {
        mDisplayOptionsDlg->Apply( mThreadDisplay );
    }

    // Whether you are connected or not you should be able to save the current buffer options
    bool lRestartStreaming = false;
    if ( mThreadDisplay != NULL )
    {
    	StopStreaming();
    	lRestartStreaming = true;
    }
    BufferOptionsDlg::Load( lReader, mPipeline );
    if ( lRestartStreaming )
    {
    	StartStreaming();
    }

    // note that these items weren't saved in versions of GEVPlayer prior to 1.2 so we can't count on them being there
    PvUInt32 lValue;
    PvString lCommunicationVisibility;
    if ( lReader.Restore( TAG_COMMUNICATIONVISIBILITY, lCommunicationVisibility ).IsOK() )
    {
    	// Keep for pre 2.1 compatibility
    	sscanf( lCommunicationVisibility.GetAscii(), "%d", &lValue );
        mCommunicationWnd->SetVisibility( (PvGenVisibility) lValue );
    }
    PvString lDeviceVisibility;
    if ( lReader.Restore( TAG_DEVICEVISIBILITY, lDeviceVisibility ).IsOK() )
    {
    	// Keep for pre 2.1 compatibility
        sscanf( lDeviceVisibility.GetAscii(), "%d", &lValue );
        mDeviceWnd->SetVisibility( (PvGenVisibility) lValue );
    }
    PvString lStreamVisibility;
    if ( lReader.Restore( TAG_STREAMVISIBILITY, lStreamVisibility ).IsOK() )
    {
    	// Keep for pre 2.1 compatibility
        sscanf( lStreamVisibility.GetAscii(), "%d", &lValue );
        mStreamWnd->SetVisibility( (PvGenVisibility) lValue );
    }
    PvPropertyList lCommunicationBrowserOptions;
    if ( lReader.Restore( TAG_COMMUNICATIONBROWSER, lCommunicationBrowserOptions ).IsOK() )
    {
        mCommunicationWnd->Load( lCommunicationBrowserOptions );
    }
    PvPropertyList lDeviceBrowserOptions;
    if ( lReader.Restore( TAG_DEVICEBROWSER, lDeviceBrowserOptions ).IsOK() )
    {
    	mDeviceWnd->Load( lDeviceBrowserOptions );
    }
    PvPropertyList lStreamBrowserOptions;
    if ( lReader.Restore( TAG_STREAMBROWSER, lStreamBrowserOptions ).IsOK() )
    {
    	mStreamWnd->Load( lStreamBrowserOptions );
    }

    PvString lDeinterlacing;
    if ( lReader.Restore( TAG_DEINTERLACING, lDeinterlacing ).IsOK() )
    {
    	if ( lDeinterlacing == TAG_DEINTERLACINGDISABLED )
    	{
    		mDeinterlacing = DeinterlacingDisabled;
    	}
    	else if ( lDeinterlacing == TAG_DEINTERLACINGWEAVING )
    	{
    		mDeinterlacing = DeinterlacingWeaving;
    	}
    	if ( mThreadDisplay != NULL )
    	{
    		mThreadDisplay->SetDeinterlacing( mDeinterlacing );
    	}
    }

    EnableInterface();
}


// =============================================================================
void GEVPlayer::ReportMRU(QString aFileName)
{
    int curPos= 0;
    QString lName;
#ifdef WIN32
    QString lSlash = "\\";
#else
    QString lSlash = "/";
#endif
    lName = aFileName.section(lSlash, -1);


    setWindowTitle( lName + " - GEVPlayer" );

    // Try to find the item in our current list

    std::list<QString>::iterator lIt;
    std::list<QString>::iterator lToRemove;

    lIt = mRecentList.begin();
    bool lRemove = false;
    while (lIt != mRecentList.end())
    {
        if (*lIt == aFileName)
        {
            // If match, remove it, will be added as top latter
            lToRemove = lIt;
            lRemove = true;
        }
        lIt++;
    }

    if ( lRemove )
    {
        mRecentList.erase(lToRemove);
    }

    // Whether we removed already existing or not, insert at top
    mRecentList.push_front(aFileName);

    // trim the end (we don't want more than 10)
    if ( mRecentList.size() > 10 )
    {
        lIt = mRecentList.begin();
        std::list<QString>::iterator lLast;
        while (lIt != mRecentList.end())
        {
            lLast = lIt;
            lIt++;
        }
        // erase the back, not the front
        mRecentList.erase( lLast );
    }

    // Save current MRU to registry
    SaveMRUToRegistry();

    UpdateMRUMenu();
}

// =============================================================================
void GEVPlayer::SaveMRUToRegistry()
{
    QSettings lSettings;
    std::list<QString>::iterator lIt;
    lIt = mRecentList.begin();
    PvUInt32 lCount = 0;
    while( lIt != mRecentList.end() )
    {
        QString lKey = "MRU/" + QString::number(lCount);
        lSettings.setValue( lKey, *lIt);
        lIt++;
        lCount++;
    }
}

// =============================================================================
void GEVPlayer::LoadMRUFromRegistry()
{
    QSettings lSettings;
    PvUInt32 lCount = 0;
    QString lValue;
    do
    {
        QString lKey = "MRU/" + QString::number(lCount);
        lValue = lSettings.value(lKey).toString();
        if (!lValue.isEmpty())
        {
             mRecentList.push_back( lValue );
        }
        lCount++;
    } while ( !lValue.isEmpty() && lCount <= 10 );
}


// =============================================================================
void GEVPlayer::UpdateMRUMenu()
{
    mFileOpenRecentMenu->clear();
    std::list<QString>::iterator lIt;
    lIt = mRecentList.begin();

    unsigned int i = 0;
    while( lIt != mRecentList.end() )
    {
        mOpenRecent[i].mFileOpenRecentAction = mFileOpenRecentMenu->addAction( *lIt );
        mOpenRecent[i].mFileName = *lIt;
        mOpenRecent[i].mGEVPlayer = this;
        QObject::connect( mOpenRecent[i].mFileOpenRecentAction, SIGNAL( triggered() ), &mOpenRecent[i], SLOT( OnFileOpenRecentAction() ) );
        lIt++;
        i++;
    }
}



///
/// \brief Menu action handler
///

void OpenRecent::OnFileOpenRecentAction()
{
    mGEVPlayer->SetFileName( mFileName );
    mGEVPlayer->OpenConfig( mFileName );
}


///
/// \brief Menu action handler
///

void GEVPlayer::OnFileSaveAction()
{
    if ( mFileName.isNull() )
    {
        OnFileSaveAsAction();
        return;
    }

    SaveConfig( mFileName, true );

    ReportMRU( mFileName );
}


///
/// \brief Menu action handler
///

void GEVPlayer::OnFileSaveAsAction()
{
    mFileName = QFileDialog::getSaveFileName(this, tr("Save eBUS SDK Project"),
                                                         NULL,
                                                         tr("PureGEV Configuration Files (*.pvcfg)"));

    if ( !mFileName.isNull() )
    {
        if ( !mFileName.endsWith(".pvcfg") )
        {
            mFileName.append(".pvcfg");
        }
        OnFileSaveAction();
    }
}


///
/// \brief Menu action handler
///

void GEVPlayer::OnFileExitAction()
{
    close();
}


///
/// \brief Menu action handler
///

void GEVPlayer::OnToolsSetupAction()
{
    mSetupDlg->SetEnabled( !mDeviceConnected && !mStream->IsOpen() );
    mSetupDlg->exec();
    EnableInterface();
}


///
/// \brief Menu action handler
///

void GEVPlayer::OnToolsEventMonitorAction()
{
    if ( !mDeviceConnected )
    {
        return;
    }

    assert( mEventMonitorDlg != NULL );
    if ( mEventMonitorDlg == NULL )
    {
        return;
    }

    if ( mEventMonitorDlg->isVisible() )
    {
        // If already open, just toggle to closed...
        mEventMonitorDlg->close();
        return;
    }

    mEventMonitorDlg->show();
}


///
/// \brief Menu action handler
///

void GEVPlayer::OnToolsSerialAction()
{
    if ( mTerminalIPEngineWnd->GetQWidget()->isVisible() )
    {
        // If already open, just toggle to closed...     
        mTerminalIPEngineWnd->Close();
        return;
    }

    CloseGenWindow( mCommunicationWnd );
    CloseGenWindow( mDeviceWnd );
    CloseGenWindow( mStreamWnd );

    mTerminalIPEngineWnd->SetDevice( &mDevice );
    mTerminalIPEngineWnd->ShowModal( this );
}


///
/// \brief Menu action handler
///

void GEVPlayer::OnToolsFilteringAction()
{
    if ( mFilteringDlg->isVisible() )
    {
        // If already open, just toggle to closed...
        mFilteringDlg->close();
        return;
    }

    mFilteringDlg->show();
}


///
/// \brief Display options menu handler
///

void GEVPlayer::OnToolsDisplayOptionsAction()
{
    if ( mThreadDisplay == NULL )
    {
        return;
    }

    mDisplayOptionsDlg->Init( mThreadDisplay );
    if ( mDisplayOptionsDlg->exec() == QDialog::Accepted )
    {
        mDisplayOptionsDlg->Apply( mThreadDisplay );
    }
}


///
/// \brief Display buffer options
///

void GEVPlayer::OnToolsBufferOptionsAction()
{
    if ( !mStream->IsOpen() )
    {
        return;
    }

    BufferOptionsDlg lDlg( mPipeline, this );
    lDlg.exec();
}


///
/// \brief Menu action handler
///

void GEVPlayer::OnToolsSaveImagesAction()
{
    mImageSaveDlg->exec();
}


///
/// \brief Menu action handler
///

void GEVPlayer::OnToolsSaveCurrentImageAction()
{
    assert( mThreadDisplay != NULL );
    if ( mThreadDisplay == NULL )
    {
        return;
    }
    mImageSaveDlg->SaveThisImage();
}


///
/// \brief Menu action handler
///

void GEVPlayer::OnHelpAboutAction()
{
    AboutBox lHelpDlg(this);
    lHelpDlg.exec();
}


///
/// \brief Displays communication parameters
///

void GEVPlayer::OnShowCommParameters()
{
    ShowGenWindow(
        mCommunicationWnd,
        mDevice.GetGenLink(),
        "Communication Control" );
}


///
/// \brief Displays device parameters
///

void GEVPlayer::OnShowDeviceParameters()
{
    if ( !mDeviceConnected )
    {
        return;
    }

    ShowGenWindow(
        mDeviceWnd,
        mDevice.GetGenParameters(),
        "Device Control" );
}


///
/// \brief Display stream parameters
///

void GEVPlayer::OnShowStreamParameters()
{
    if ( !mStream->IsOpen() )
    {
        return;
    }

    ShowGenWindow(
        mStreamWnd,
        mStream->GetParameters(),
        "Image Stream Control" );
}


///
/// \brief Event handler to complete async disconnect in UI thread
///

void GEVPlayer::OnDisconnectEvent()
{
    // Is reconnecting enabled?
    PvGenBoolean *lLinkRecoveryEnabled = mDevice.GetGenLink()->GetBoolean( "LinkRecoveryEnabled"  );
    assert( lLinkRecoveryEnabled != NULL );

    bool lEnabled = false;
    lLinkRecoveryEnabled->GetValue( lEnabled );

    if ( lEnabled )
    {
        SetStatusColor( SCYellow );
        mStatusLine->setText( tr( "Connection to device lost..." ) );

		CloseGenWindow( mDeviceWnd );
		CloseGenWindow( mCommunicationWnd );
		CloseGenWindow( mStreamWnd );

		EnableGenBrowsers( false );
		EnableControls( false );
    }
    else
    {
        SetStatusColor( SCDefault );
        mStatusLine->setText( "" );

        Disconnect();

        QMessageBox::warning( this, "GEVPlayer", "Connection to device lost." );
    }
}


///
/// \brief Event handler to complete async start streaming in UI thread
///

void GEVPlayer::OnStreamEvent( bool aRestartAcquisition )
{
    if ( !mDevice.IsConnected() )
    {
        return;
    }

    PvGenStateStack lStack( mDevice.GetGenParameters() );
    PushSource( &lStack );

    PvInt64 lChannel = 0;
    if ( IsMultiSourceTransmitter( &mDevice ) )
    {
        mDevice.GetGenParameters()->GetIntegerValue( "SourceStreamChannel", lChannel );
    }

    if ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData )
    {
        //
        // Only re-open stream if control and data.
        //     - If control only, no need for stream
        //     - If data only, there is no recovery required
        //     - If monitor, well there is no stream!
        //

        PvGenInteger *lInteger = mDevice.GetGenParameters()->GetInteger( "GevCurrentIPAddress" );
        assert( lInteger != NULL );

        PvInt64 lValue;
        lInteger->GetValue( lValue );

        QString lIP;
        lIP.sprintf( "%i.%i.%i.%i",
            (int)( ( lValue & 0xFF000000 ) >> 24 ),
            (int)( ( lValue & 0x00FF0000 ) >> 16 ),
            (int)( ( lValue & 0x0000FF00 ) >> 8 ),
            (int)( ( lValue & 0x000000FF ) ) );

        PvResult lResult = PvResult::Code::GENERIC_ERROR;
        switch ( mSetupDlg->GetDestination() )
        {
            case SetupDlg::DestinationUnicastAuto:
                lResult = mStream->Open( lIP.toAscii().data() );
                break;

            case SetupDlg::DestinationUnicastSpecific:
                lResult = mStream->Open( lIP.toAscii().data(),
                    mSetupDlg->GetUnicastSpecificPort(), (PvUInt16)lChannel );
                break;

            case SetupDlg::DestinationMulticast:
                lResult = mStream->Open( lIP.toAscii().data(),
                    mSetupDlg->GetMulticastIP().toAscii().data(), mSetupDlg->GetMulticastPort(), (PvUInt16)lChannel );
                break;

            default:
                assert( 0 );
        }

        if ( lResult.IsOK() )
        {
            mDevice.SetStreamDestination( mStream->GetLocalIPAddress(), mStream->GetLocalPort(), (PvUInt16)lChannel );
            StartStreaming();

            if ( aRestartAcquisition )
            {
                StartAcquisition();
            }
        }
    }
    else if ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl )
    {
        //
        // Just controlling: set the destination as configured, start acquisition if needed
        //

        PvResult lResult = PvResult::Code::GENERIC_ERROR;
        switch ( mSetupDlg->GetDestination() )
        {
            case SetupDlg::DestinationUnicastOther:
                lResult = mDevice.SetStreamDestination( mSetupDlg->GetUnicastIP().toAscii().data(),
                    mSetupDlg->GetUnicastPort(), (PvUInt16)lChannel );
                break;

            case SetupDlg::DestinationMulticast:
                lResult = mDevice.SetStreamDestination( mSetupDlg->GetMulticastIP().toAscii().data(),
                    mSetupDlg->GetMulticastPort(), (PvUInt16)lChannel );
                break;

            default:
                assert( 0 );
        }

        if ( lResult.IsOK() )
        {
            assert( mAcquisitionStateManager != NULL );
            if ( mAcquisitionStateManager->GetState() == PvAcquisitionStateLocked )
            {
                StartAcquisition();
            }
        }
    }

    SetStatusColor( SCDefault );
    mStatusLine->setText( tr( "" ) );

	EnableInterface();
}

///
/// \brief Event handler to complete async stop streaming in UI thread
///

void GEVPlayer::OnStopStreamingEvent()
{
    assert( mStream != NULL );

    if ( mStream->IsOpen() )
    {
        StopStreaming();
        mStream->Close();
    }
}


///
/// \brief Event handler to complete link reconnect in UI thread
///

void GEVPlayer::OnLinkReconnectedEvent()
{
    PvGenInteger *lInteger = mDevice.GetGenParameters()->GetInteger( "GevCurrentIPAddress" );
    if ( lInteger != NULL )
    {
        PvInt64 lValue;
        lInteger->GetValue( lValue );

        QString lIP;
        lIP.sprintf( "%i.%i.%i.%i",
            (int)( ( lValue & 0xFF000000 ) >> 24 ),
            (int)( ( lValue & 0x00FF0000 ) >> 16 ),
            (int)( ( lValue & 0x0000FF00 ) >> 8 ),
            (int)( ( lValue & 0x000000FF ) ) );

        mIpAddressLine->setText( lIP );
    }

    bool lStreaming = false;
    if ( mAcquisitionStateManager != NULL )
    {
        lStreaming = mAcquisitionStateManager->GetState() == PvAcquisitionStateLocked;
    }

    // Important - this will, amongst other, make sure TLParamsLocked is set to 0!
    StopAcquisition();

    QCoreApplication::postEvent( this, new QWindowsEvent( static_cast<QEvent::Type>( WM_STREAM ), lStreaming ? 1 : 0 ) );

    SetStatusColor( SCDefault );
    mStatusLine->setText("" );
}


///
/// \brief Timer event. Refresh stream status bar
///

void GEVPlayer::OnTimer()
{
    QCoreApplication::sendEvent( this, new QEvent( static_cast<QEvent::Type>( WM_IMAGEDISPLAYED ) ) );
}


///
/// \brief Computes payload size
///

PvUInt32 GEVPlayer::GetPayloadSize()
{
    if ( !mDevice.IsConnected() )
    {
        return 0;
    }

    // Used to temporarely select the source in use
    PvGenStateStack lStack( mDevice.GetGenParameters() );
    PushSource( &lStack );

    PvGenInteger *lPayloadSize = mDevice.GetGenParameters()->GetInteger( "PayloadSize" );
    PvGenInteger *lWidth = mDevice.GetGenParameters()->GetInteger( "Width" );
    PvGenInteger *lHeight = mDevice.GetGenParameters()->GetInteger( "Height" );
    PvGenEnum *lPixelFormat = mDevice.GetGenParameters()->GetEnum( "PixelFormat" );

    // Get payload size
    PvInt64 lPayloadSizeValue = 0;
    if ( lPayloadSize != NULL )
    {
        lPayloadSize->GetValue( lPayloadSizeValue );

        // Round up to make it mod 32 (works around an issue with some devices)
        if ((lPayloadSizeValue % 32) != 0)
        {
            lPayloadSizeValue = ((lPayloadSizeValue / 32) + 1) * 32;
        }
    }

    // Compute poor man's payload size - for devices not maintaining PayloadSize properly
    PvInt64 lPoorMansPayloadSize = 0;
    if ( ( lWidth != NULL ) && ( lHeight != NULL ) && ( lPixelFormat != NULL ) )
    {
        PvInt64 lWidthValue, lHeightValue;
        lWidth->GetValue( lWidthValue );
        lHeight->GetValue( lHeightValue );

        PvInt64 lPixelFormatValue;
        lPixelFormat->GetValue( lPixelFormatValue );
        PvInt64 lPixelSizeInBits = PvGetPixelBitCount( static_cast<PvPixelType>( lPixelFormatValue ) );

        lPoorMansPayloadSize = ( lWidthValue * lHeightValue * lPixelSizeInBits ) / 8;
    }

    // Take max, let pipeline know what the payload size is
    PvInt64 lBestPayloadSize = lPayloadSizeValue > lPoorMansPayloadSize ? lPayloadSizeValue : lPoorMansPayloadSize;
    if ( ( lBestPayloadSize > 0 ) && ( lBestPayloadSize < UINT_MAX ) )
    {
        return static_cast<PvUInt32>( lBestPayloadSize );
    }

    // Could not compute/retrieve payload size...
    return 0;
}


QString GEVPlayer::GetStickyPath()
{
    return GetAppDataPath() + "GEVPlayer.pvcfg";
}

QString GEVPlayer::GetAppDataPath()
{

    QString lLocation;
#ifdef WIN32
    TCHAR lAppData[MAX_PATH];
    if ( SHGetSpecialFolderPath(      
        GetDesktopWindow(),
        lAppData,
        CSIDL_APPDATA,
        TRUE ) )
    {
        lLocation += lAppData;
    }
    lLocation += "\\Pleora\\";
    // create the folder if it doesn't exist
    if ( PtAccess( lLocation.toAscii().data(), 0 ) != 0 )
    {
        // default security attributes should suffice
        CreateDirectory( lLocation.toAscii().data(), NULL );
    }
#endif // WIN32
    
#ifdef _UNIX_
    passwd *lPW = getpwuid( getuid() );
    if ( lPW )
    {
        lLocation += lPW->pw_dir;
    }
    lLocation += "/.config";
    
    // create the folder if it doesn't exist
    if ( PtAccess( lLocation.toAscii().data(), 0 ) != 0 )
    {
        mkdir( lLocation.toAscii().data(), S_IRWXU | S_IRGRP | S_IXGRP );
    }
    
    
    lLocation += "/Pleora/";

    // create the folder if it doesn't exist
    if ( PtAccess( lLocation.toAscii().data(), 0 ) != 0 )
    {
        mkdir( lLocation.toAscii().data(), S_IRWXU | S_IRGRP | S_IXGRP );
    }
#endif

    return lLocation;
}

QString GEVPlayer::GetDefaultPath()
{
    return GetAppDataPath() + "GEVPlayerDefaults.pvcfg";
}

void GEVPlayer::OnToolsSavepreferences()
{
    SaveConfig( GetStickyPath(), false );
}

void GEVPlayer::SaveConfig( QString aLocation, bool aSaveConnectedDevice )
{
    PvConfigurationWriter lWriter;
    lWriter.Store( "1", "gevplayerconfig" ); 

    assert( mDisplayOptionsDlg != NULL );
    assert( mSetupDlg != NULL );
    assert( mFilteringDlg != NULL );
    assert( mImageSaveDlg != NULL );
    assert( mPipeline != NULL );
    assert( mLogBuffer != NULL );

    mDisplayOptionsDlg->Save( lWriter );
    mImageSaveDlg->Save( lWriter );
    mSetupDlg->Save( lWriter );
    mFilteringDlg->Save( lWriter );
	mLogBuffer->Save( lWriter );

    BufferOptionsDlg::Save( lWriter, mPipeline );

    // Save browser options for each PvGenBrowserWnd
    PvPropertyList lPropertyList;
    mCommunicationWnd->Save( lPropertyList );
    lWriter.Store( &lPropertyList, TAG_COMMUNICATIONBROWSER );
    mDeviceWnd->Save( lPropertyList );
    lWriter.Store( &lPropertyList, TAG_DEVICEBROWSER );
    mStreamWnd->Save( lPropertyList );
    lWriter.Store( &lPropertyList, TAG_STREAMBROWSER );

    switch ( mDeinterlacing )
    {
    case DeinterlacingDisabled:
        lWriter.Store( TAG_DEINTERLACINGDISABLED, TAG_DEINTERLACING );
    	break;

    case DeinterlacingWeaving:
        lWriter.Store( TAG_DEINTERLACINGWEAVING, TAG_DEINTERLACING );
    	break;

    default:
    	assert( 0 );
    	break;
    }

    if( aSaveConnectedDevice )
    {
        if ( mSourceCombo->currentIndex() >= 0 )
        {
            QVariant lData = mSourceCombo->itemData( mSourceCombo->currentIndex() );
            if ( lData.canConvert<PvInt64>() )
            {
            	PvInt64 lValue = lData.value<PvInt64>();

				QString lSourceStr;
				lSourceStr.sprintf( "%d", (int)lValue );

				lWriter.Store( lSourceStr.toAscii().data(), TAG_ACTIVESOURCE );
            }
        }

    	if ( mEventMonitorDlg != NULL )
    	{
    		mEventMonitorDlg->EnableGenICamMonitoring( false );
    	}

        PvStringList lErrorList;
        SavingThread lSavingThread( mSetupDlg, &lWriter, &mDevice, mStream, &lErrorList, this );
        lSavingThread.Save();

        if ( mEventMonitorDlg != NULL )
        {
        	mEventMonitorDlg->EnableGenICamMonitoring( true );
        }

        if ( lErrorList.GetSize() > 0 )
        {
            ErrorsDlg lDlg( this, &lErrorList );
            lDlg.exec();
        }
    }
    else
    {
        lWriter.Store( mDevice.GetGenLink(), "communication" );
    }

    lWriter.Save( aLocation.toAscii().data() );
}

void GEVPlayer::OnToolsRestoredefaultpreferences()
{
    // load the default file
    OpenConfig( GetDefaultPath() );

    // save the default settings so they are persisted next time
    SaveConfig( GetStickyPath(), false );
}

void GEVPlayer::OnToolsSaveXML()
{
	if ( !mDeviceConnected )
	{
		return;
	}
	
	PvString lFilename;
	if ( !mDevice.GetDefaultGenICamXMLFilename( lFilename ).IsOK() )
	{
		lFilename = "Default.xml";
	}

    QString lFileName = QFileDialog::getSaveFileName( this, tr( "Save XML" ), lFilename.GetAscii(), tr( "GenICam XML Files (*.xml)" ) );
    if ( !lFileName.isNull() )
    {
        if ( !lFileName.endsWith(".xml") )
        {
            lFileName.append( ".xml" );
        }

        mDevice.DumpGenICamXML( lFileName.toAscii().data() );
    }
}

void GEVPlayer::OnToolsDeinterlacingDisabled()
{
	mDeinterlacing = DeinterlacingDisabled;
	if ( mThreadDisplay != NULL )
	{
		mThreadDisplay->SetDeinterlacing( mDeinterlacing );
	}
}

void GEVPlayer::OnToolsDeinterlacingWeaving()
{
	mDeinterlacing = DeinterlacingWeaving;
	if ( mThreadDisplay != NULL )
	{
		mThreadDisplay->SetDeinterlacing( mDeinterlacing );
	}
}

void GEVPlayer::OnCbnSelchangeSource( int aIndex )
{
    QCoreApplication::postEvent( this, new QEvent( static_cast<QEvent::Type>( WM_UPDATESOURCE ) ) );
}

void GEVPlayer::OnAcquisitionStateChanged( PvDevice* aDevice, PvStreamBase* aStream, PvUInt32 aSource, PvAcquisitionState aState )
{
    QCoreApplication::postEvent( this, new QEvent( static_cast<QEvent::Type>( WM_ACQUISITIONSTATECHANGED ) ) );
}

void GEVPlayer::OnAcquisitionStateChangedEvent()
{
	// Events may be processed after the acquisition manager was released
	if ( mAcquisitionStateManager != NULL )
	{
		EnableControls( mDeviceConnected );
	}
}

void GEVPlayer::OnUpdateSourceEvent()
{
    // Since we get here through the event queue, things may have happened since the message was posted
    if ( !mDevice.IsConnected() )
    {
        return;
    }

    if ( mAcquisitionStateManager == NULL )
    {
    	return;
    }

    PvGenEnum *lSourceSelector = mDevice.GetGenParameters()->GetEnum( "SourceSelector" );
    assert( lSourceSelector != NULL );

    PvInt64 lOldSource = mAcquisitionStateManager->GetSource();

    PvInt64 lNewSource = 0;
    QVariant lData = mSourceCombo->itemData( mSourceCombo->currentIndex() );
    if ( lData.canConvert<PvInt64>() )
    {
    	lNewSource = lData.value<PvInt64>();
    }

    // If no source change, return
    if ( lOldSource == lNewSource )
    {
        return;
    }

    // Reset current destination
    if ( mDevice.IsConnected() )
    {
        PvInt64 lOldChannel = 0;
        mDevice.ResetStreamDestination( (PvUInt32)lOldChannel );
    }

    // Obtain the local IP address and device IP address
    PvString lLocalIPAddress = mStream->GetLocalIPAddress();
    PvGenInteger *lGenDeviceIPAddress = mStream->GetParameters()->GetInteger( "DeviceIPAddress" );
    assert( lGenDeviceIPAddress != NULL );

    PvInt64 lValue = 0;
    lGenDeviceIPAddress->GetValue( lValue );

    QString lDeviceIPAddress;
    lDeviceIPAddress.sprintf( "%i.%i.%i.%i",
        (int)( ( lValue & 0xFF000000 ) >> 24 ),
        (int)( ( lValue & 0x00FF0000 ) >> 16 ),
        (int)( ( lValue & 0x0000FF00 ) >> 8 ),
        (int)( ( lValue & 0x000000FF ) ) );

    // Stop streaming
    StopAcquisition();
    if ( mStream->IsOpen() )
    {
        StopStreaming();
        mStream->Close();
    }

    // Reset acquisition state manager
    delete mAcquisitionStateManager;
    mAcquisitionStateManager = NULL;

    // Clear the display
    mDisplay->Clear();

    // Select the new input
    PvResult lResult = lSourceSelector->SetValue( lNewSource );
    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
    }

    // Spawn connection thread to open new stream, set streaming destination
    ConnectionThread lConnectionThread( mSetupDlg, NULL, &mDevice, mStream, this, lLocalIPAddress.GetAscii(), lDeviceIPAddress );
    lConnectionThread.SetTitle( "Changing Source" );
    lResult = lConnectionThread.Connect();
    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
        Disconnect();
        return;
    }

    // Regardless of whether a corresponding stream is open or not, we can still pass it to the acquisition state manager
    assert( mAcquisitionStateManager == NULL );
    mAcquisitionStateManager = new PvAcquisitionStateManager( &mDevice, mStream, lNewSource );
    mAcquisitionStateManager->RegisterEventSink( this );

    if ( mStream->IsOpen() )
    {
        // Ready image reception
        StartStreaming();
    }

    EnableControls( true );

    // Update acquisition mode combo box, which may have changed on a new source
    QCoreApplication::sendEvent( this, new QEvent( static_cast<QEvent::Type>( WM_UPDATEACQUISITIONMODE ) ) );
}

void GEVPlayer::OnUpdateSourcesEvent( PvInt64 aPreferedSource )
{
    if ( mAcquisitionStateManager == NULL )
    {
        // Not ready yet...
        mUpdatingSources = false;
        return;
    }

    PvGenEnum *lSourceSelector = mDevice.GetGenParameters()->GetEnum( "SourceSelector" );
    if ( lSourceSelector == NULL )
    {
        mAvailableSources.clear();
        mSourceCombo->clear();
        mUpdatingSources = false;
        return;
    }

    PvInt64 lSourceCount = 0;
    lSourceSelector->GetEntriesCount( lSourceCount );

    PvUInt32 lActiveSource = mAcquisitionStateManager->GetSource();

    // Fill source selector combo box
    mAvailableSources.clear();
    mSourceCombo->clear();
    for ( PvInt64 i = 0; i < lSourceCount; i++ )
    {
        const PvGenEnumEntry *lEE = NULL;
        lSourceSelector->GetEntryByIndex( i, &lEE );

        if ( lEE->IsAvailable() )
        {
            PvString lDisplayName;
            lEE->GetDisplayName( lDisplayName );

            PvInt64 lValue = 0;
            lEE->GetValue( lValue );

            mSourceCombo->addItem( lDisplayName.GetAscii(), lValue );
            mAvailableSources.push_back( lValue );

            // Same as active source, select
            if ( lActiveSource == static_cast<PvUInt32>( lValue ) )
            {
                mSourceCombo->setCurrentIndex( mSourceCombo->count() - 1 );
            }
        }
    }

    // If we have a preferred selection, attempt to find, select it
    if ( ( aPreferedSource >= 0 ) && ( mSourceCombo->count() > 0 ) )
    {
        for ( int i = 0; i < mSourceCombo->count(); i++ )
        {
            QVariant lData = mSourceCombo->itemData( i );
            if ( lData.canConvert<PvInt64>() )
            {
            	PvInt64 lValue = lData.value<PvInt64>();
            	if ( lValue == aPreferedSource )
                {
                    mSourceCombo->setCurrentIndex( i );
                    QCoreApplication::postEvent( this, new QEvent( static_cast<QEvent::Type>( WM_UPDATESOURCE ) ) );
                }
            }
        }
    }

    // If no selelection, select first available
    if ( ( mSourceCombo->count() > 0 ) && ( mSourceCombo->currentIndex() < 0 ) )
    {
        mSourceCombo->setCurrentIndex( 0 );
        QCoreApplication::postEvent( this, new QEvent( static_cast<QEvent::Type>( WM_UPDATESOURCE ) ) );
    }

    mUpdatingSources = false;

    return;
}

void GEVPlayer::OnUpdateAcquisitionModeEvent()
{
    if ( mAcquisitionStateManager == NULL )
    {
        // Not ready yet...
        mUpdatingAcquisitionMode = false;
        return;
    }

    PvGenEnum *lEnum = mDevice.GetGenParameters()->GetEnum( "AcquisitionMode" );
    if ( lEnum != NULL )
    {
        PvGenStateStack lStack( mDevice.GetGenParameters() );
        PushSource( &lStack );

        // Set mode combo box to value currently used by the device
        PvInt64 lEnumValue = 0;
        lEnum->GetValue( lEnumValue );
        for ( int i = 0; i < mModeCombo->count(); i++ )
        {
            QVariant lData = mModeCombo->itemData( i );
            if ( lData.canConvert<PvInt64>() )
            {
            	PvInt64 lComboValue = lData.value<PvInt64>();
                if ( lEnumValue == lComboValue )
                {
                    mModeCombo->setCurrentIndex( i );
                    break;
                }
            }
        }

        PvAcquisitionState lAcquisitionState = mAcquisitionStateManager->GetState();

        mModeCombo->setEnabled(
            lEnum->IsWritable() &&
            ( lAcquisitionState != PvAcquisitionStateLocked ) );
    }

    mUpdatingAcquisitionMode = false;

    return;
}


bool GEVPlayer::IsMultiSourceTransmitter( PvDevice *aDevice )
{
	if ( ( aDevice != NULL ) && aDevice->IsConnected() )
	{
		PvGenEnum *lEnum = aDevice->GetGenParameters()->GetEnum( "SourceSelector" );
		PvGenEnum *lDeviceClass = aDevice->GetGenParameters()->GetEnum( "GevDeviceClass" );

		if ( ( lEnum != NULL ) && ( lDeviceClass != NULL ) )
		{
			PvString lClass;
			if ( lDeviceClass->GetValue( lClass ).IsOK() )
			{
				return ( lClass == "Transmitter" );
			}
		}
	}

	return false;
}

void GEVPlayer::PushSource( PvGenStateStack *aStack )
{
	// If multi source, push the active source on the stack
    if ( IsMultiSourceTransmitter( &mDevice ) )
    {
        if ( mAcquisitionStateManager != NULL )
        {
        	// Select source (will be reset when lStack goes out of scope)
            aStack->SetEnumValue( "SourceSelector", mAcquisitionStateManager->GetSource() );
        }
    }
}


///
/// \brief PvPipeline callback used to notify that buffers are too small.
///
/// By setting aReallocAll to true, we ask to PvPipeline to immediately
/// realloc all buffers.
///
/// By setting aResetStats to true, we ask the PvPipeline to reset the
/// stream statistics after reallocating the buffers.
///

void GEVPlayer::OnBufferTooSmall( PvPipeline *aPipeline, bool *aReallocAll, bool *aResetStats )
{
	*aReallocAll = true;
	*aResetStats = true;

	mBuffersReallocated = true;
}

