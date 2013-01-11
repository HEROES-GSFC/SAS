// *****************************************************************************
//
//     Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "pvdualsourcesample.h"
#include "pvdualsourcesampledefines.h"

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

#include <PvDeviceFinderWnd.h>
#include <PvDisplayWnd.h>
#include <PvGenBrowserWnd.h>

#include <assert.h>

#include "source.h"
#include "sourcewidget.h"


///
/// \brief Constructor
///

PvDualSourceSample::PvDualSourceSample( QWidget *parent, Qt::WFlags flags )
    : QMainWindow( parent, flags )
	, mSource1( NULL )
	, mSource2( NULL )
	, mCommunicationBrowser( NULL )
	, mDeviceBrowser( NULL )
    , Stream1Browser( NULL )
    , Stream2Browser( NULL )
{
    CreateLayout();

    Stream2Browser = new PvGenBrowserWnd;
    Stream1Browser = new PvGenBrowserWnd;
    mDeviceBrowser = new PvGenBrowserWnd;
    mCommunicationBrowser = new PvGenBrowserWnd;
}


///
/// \brief Destructor
///

PvDualSourceSample::~PvDualSourceSample()
{
	Disconnect();

    SAFE_DELETE( Stream2Browser );
    SAFE_DELETE( Stream1Browser );
    SAFE_DELETE( mDeviceBrowser );
    SAFE_DELETE( mCommunicationBrowser );

    SAFE_DELETE( mSource2 );
    SAFE_DELETE( mSource1 );

	// UI controls are supposed to be released by the UI
}


///
/// \brief Create dialog layout
///

void PvDualSourceSample::CreateLayout()
{
	CreateMenu();

	mSource1 = new Source( 0 );
	mSource1Widget = new SourceWidget( "Source 1", mSource1 );
	mSource1->SetWidget( mSource1Widget );

	mSource2 = new Source( 1 );
	mSource2Widget = new SourceWidget( "Source 2", mSource2 );
	mSource2->SetWidget( mSource2Widget );

	QHBoxLayout *lLayout = new QHBoxLayout;
	lLayout->addWidget( mSource1Widget );
	lLayout->addWidget( mSource2Widget );

    QFrame *lMainBox = new QFrame;
    lMainBox->setLayout( lLayout );

	setCentralWidget( lMainBox );
    setWindowTitle( tr( "PvDualSourceSample" ) );
    setWindowIcon( QIcon( QPixmap( ":PvDualSourceSample/res/PvDualSourceSample.ico" ) ) );

    EnableInterface();
}


///
/// \brief Create the application menu
///

void PvDualSourceSample::CreateMenu()
{
	//
	// File
	//

    mFileMenu = menuBar()->addMenu( tr( "&File" ) );
    mFileExitAction = mFileMenu->addAction( tr( "E&xit" ) );

    QObject::connect(
    	mFileMenu, SIGNAL( aboutToShow() ),
    	this, SLOT( OnFileMenuAboutToShow() ) );
    QObject::connect(
    	mFileExitAction, SIGNAL( triggered() ),
    	this, SLOT( OnFileExitAction() ) );

    //
    // Connection
    //

    mConnectionMenu = menuBar()->addMenu( tr( "&Connection" ) );
    mConnectionConnectAction = mConnectionMenu->addAction( tr( "Select/&Connect" ) );
    mConnectionDisconnectAction = mConnectionMenu->addAction( tr( "&Disconnect" ) );

    QObject::connect(
    	mConnectionMenu, SIGNAL( aboutToShow() ),
    	this, SLOT( OnConnectionMenuAboutToShow() ) );
    QObject::connect(
    	mConnectionConnectAction, SIGNAL( triggered() ),
    	this, SLOT( OnConnectionConnectAction() ) );
    QObject::connect(
    	mConnectionDisconnectAction, SIGNAL( triggered() ),
    	this, SLOT( OnConnectionDisconnectAction() ) );

    //
    // Parameters
    //

    mParametersMenu = menuBar()->addMenu( tr( "&Parameters" ) );
    mParametersCommunicationAction = mParametersMenu->addAction( tr( "&Communication" ) );
    mParametersDeviceAction = mParametersMenu->addAction( tr( "&Device" ) );
    mParametersStream1Action = mParametersMenu->addAction( tr( "Source &1 Stream" ) );
    mParametersStream2Action = mParametersMenu->addAction( tr( "Source &2 Stream" ) );

    QObject::connect(
    	mParametersMenu, SIGNAL( aboutToShow() ),
    	this, SLOT( OnParametersMenuAboutToShow() ) );
    QObject::connect(
    	mParametersCommunicationAction, SIGNAL( triggered() ),
    	this, SLOT( OnParametersCommunicationAction() ) );
    QObject::connect(
    	mParametersDeviceAction, SIGNAL( triggered() ),
    	this, SLOT( OnParametersDeviceAction() ) );
    QObject::connect(
    	mParametersStream1Action, SIGNAL( triggered() ),
    	this, SLOT( OnParametersStream1Action() ) );
    QObject::connect(
    	mParametersStream2Action, SIGNAL( triggered() ),
    	this, SLOT( OnParametersStream2Action() ) );
}


///
/// \brief File menu is about to show
///

void PvDualSourceSample::OnFileMenuAboutToShow()
{
}


///
/// \brief File exit action handler.
///

void PvDualSourceSample::OnFileExitAction()
{
    close();
}


///
/// \brief Connection menu is about to show
///

void PvDualSourceSample::OnConnectionMenuAboutToShow()
{
	mConnectionConnectAction->setEnabled( !mDevice.IsConnected() );
	mConnectionDisconnectAction->setEnabled( mDevice.IsConnected() );
}


///
/// \brief Select/Connect action handler
///

void PvDualSourceSample::OnConnectionConnectAction()
{
    // create a device finder wnd and open the select device dialog
    PvDeviceFinderWnd lWnd;
    lWnd.SetTitle( "GEV Device Selection" );

    // Show device finder
    if ( ( !lWnd.ShowModal().IsOK() ) ||
         ( lWnd.GetSelected() == NULL ) )
    {
        return;
    }

    // Show hourglass
	QCursor lOldCursor = cursor();
    setCursor( Qt::WaitCursor );
    QCoreApplication::processEvents();

    // Connect to device
    Connect( lWnd.GetSelected() );

    // Restore cursor
    setCursor( lOldCursor );
}


///
/// \brief Disconnect action handler
///

void PvDualSourceSample::OnConnectionDisconnectAction()
{
    // Show hourglass
	QCursor lOldCursor = cursor();
    setCursor( Qt::WaitCursor );
    QCoreApplication::processEvents();

    // Disconnect from device
    Disconnect();

    // Restore cursor
    setCursor( lOldCursor );
}


///
/// \brief Parameters menu is about to show
///

void PvDualSourceSample::OnParametersMenuAboutToShow()
{
    // Device and stream: enabled if connected
	mParametersDeviceAction->setEnabled( mDevice.IsConnected() );
	mParametersStream1Action->setEnabled( mSource1->IsOpened() );
	mParametersStream2Action->setEnabled( mSource2->IsOpened() );

    // Communication button is always enabled
	mParametersCommunicationAction->setEnabled( true );
}


///
/// \brief Communication parameters action handler
///

void PvDualSourceSample::OnParametersCommunicationAction()
{
    ShowGenWindow( mCommunicationBrowser, mDevice.GetGenLink(), "Communication Control" );
}


///
/// \brief Device parameters action handler
///

void PvDualSourceSample::OnParametersDeviceAction()
{
    ShowGenWindow( mDeviceBrowser, mDevice.GetGenParameters(), "Device Control" );
}


///
/// \brief Stream 1 parameters action handler
///

void PvDualSourceSample::OnParametersStream1Action()
{
    ShowGenWindow( Stream1Browser, mSource1->GetParameters(), "Stream 1 Control" );
}


///
/// \brief Stream 2 parameters action handler
///

void PvDualSourceSample::OnParametersStream2Action()
{
    ShowGenWindow( Stream2Browser, mSource2->GetParameters(), "Stream 2 Control" );
}


///
/// \brief Connects to a device
///

void PvDualSourceSample::Connect( PvDeviceInfo *aDeviceInfo )
{
    // Just in case we came here still connected...
    Disconnect();

	// Connect to device using device info
	PvResult lResult = mDevice.Connect( aDeviceInfo );
	if ( !lResult.IsOK() )
	{
		PvMessageBox( this, lResult );
		return;
	}

	Source *lSources[ 2 ] = { mSource1, mSource2 };

	// Open channels
	PvGenEnum *lSourceSelector = mDevice.GetGenParameters()->GetEnum( "SourceSelector" );
	PvGenInteger *lSourceStreamChannel = mDevice.GetGenParameters()->GetInteger( "SourceStreamChannel" );
	if ( lSourceSelector != NULL )
	{
		for ( PvUInt16 i = 0; i < 2; i++ )
		{
			QString lValue;
			lValue.sprintf( "Source%d", i + 1 );

			lSourceSelector->SetValue( lValue.toAscii().data() );

			PvInt64 lChannel = 0;
			lSourceStreamChannel->GetValue( lChannel );

			lResult = lSources[i]->Open( &mDevice, aDeviceInfo->GetIPAddress(), static_cast<PvUInt16>( lChannel ) );
			if ( !lResult.IsOK() )
			{
				PvMessageBox( this, lResult );
				return;
			}
		}
	}
	else
	{
		// No source selector, if transmitter assume 1 on channel 0
		lResult = mSource1->Open( &mDevice, aDeviceInfo->GetIPAddress(), 0 );
		if ( !lResult.IsOK() )
		{
			PvMessageBox( this, lResult );
			return;
		}
	}

    // Sync the UI with our new status
	EnableInterface();
}


///
/// \brief Disconnects from a connected device
///

void PvDualSourceSample::Disconnect()
{
    // Close all configuration child windows
    CloseGenWindow( mDeviceBrowser );
    CloseGenWindow( Stream1Browser );
    CloseGenWindow( Stream2Browser );

    if ( mSource1->IsOpened() )
    {
		PvResult lResult = mSource1->Close();
		if ( !lResult.IsOK() )
		{
			PvMessageBox( this, lResult );
		}
    }

    if ( mSource2->IsOpened() )
    {
    	PvResult lResult = mSource2->Close();
		if ( !lResult.IsOK() )
		{
			PvMessageBox( this, lResult );
		}
    }

    if ( mDevice.IsConnected() )
    {
    	PvResult lResult = mDevice.Disconnect();
		if ( !lResult.IsOK() )
		{
			PvMessageBox( this, lResult );
		}
    }

    mSource1Widget->GetDisplay()->Clear();
    mSource2Widget->GetDisplay()->Clear();

    // Sync the UI with our new status
    EnableInterface();
}


///
/// \brief Shows one of the GenICam browsers
///

void PvDualSourceSample::ShowGenWindow( PvGenBrowserWnd *aWnd, PvGenParameterArray *aArray, const QString &aTitle )
{
    if ( aWnd->GetQWidget()->isVisible() )
    {
        // If already open, just toggle to closed...
        CloseGenWindow( aWnd );
        return;
    }

    // Create, assign parameters, set title and show modeless
    aWnd->SetTitle( aTitle.toAscii().data() );

    aWnd->ShowModeless( this );
    aWnd->SetGenParameterArray( aArray );
}


///
/// \brief Closes a GenICam tree browser.
///

void PvDualSourceSample::CloseGenWindow( PvGenBrowserWnd *aWnd )
{
    if ( aWnd->GetQWidget()->isVisible() )
    {
        aWnd->Close();
    }
}


///
/// \brief Syncs the whole UI
///

void PvDualSourceSample::EnableInterface()
{
	mSource1Widget->EnableInterface();
	mSource2Widget->EnableInterface();
}


