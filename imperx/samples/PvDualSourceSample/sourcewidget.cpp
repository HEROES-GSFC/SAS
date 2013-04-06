// *****************************************************************************
//
//     Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "sourcewidget.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QDockWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QMessageBox>
#include <QtGui/QBitmap>
#include <QtCore/QEvent>

#include <PvDisplayWnd.h>

#include <assert.h>

#include "source.h"
#include "pvdualsourcesampledefines.h"


///
/// \brief Constructor
///

SourceWidget::SourceWidget( const QString &aTitle, Source *aSource, QWidget *parent, Qt::WFlags flags )
    : QWidget( parent, flags )
	, mTitle( aTitle )
	, mSource( aSource )
	, mTimer( NULL )
{
    CreateLayout();
}


///
/// \brief Destructor
///

SourceWidget::~SourceWidget()
{
	SAFE_DELETE( mTimer );
}


///
/// \brief Create dialog layout
///

void SourceWidget::CreateLayout()
{
	QVBoxLayout *lVLayout = new QVBoxLayout;
	lVLayout->addLayout( CreateDisplayLayout() );
	lVLayout->addLayout( CreateControlsLayout() );

	QGroupBox *lGroupBox = new QGroupBox( mTitle );
	lGroupBox->setLayout( lVLayout );
	lGroupBox->setMinimumWidth( 480 );
	lGroupBox->setMinimumHeight( 420 );

	QHBoxLayout *lLayout = new QHBoxLayout;
	lLayout->addWidget( lGroupBox );

	setLayout( lLayout );

    mTimer = new QTimer( this );
    mTimer->setInterval( 250 );
    connect(
    	mTimer, SIGNAL( timeout() ),
    	this, SLOT( OnTimer() ) );
    mTimer->start();
}


///
/// \brief Creates the display (PvDisplayWnd and status control) layout
///

QLayout *SourceWidget::CreateDisplayLayout()
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

	QVBoxLayout *lDisplayLayout = new QVBoxLayout;
	lDisplayLayout->addWidget( mDisplay->GetQWidget() );
	lDisplayLayout->addWidget( mStatusLine, Qt::AlignBottom );

	return lDisplayLayout;
}


///
/// \brief Create acquisition controls layout
///

QLayout *SourceWidget::CreateControlsLayout()
{
	mModeComboBox = new QComboBox;
	mModeComboBox->setMinimumWidth( 150 );

    QObject::connect(
    	mModeComboBox, SIGNAL( activated( int ) ),
    	this, SLOT( OnCbnSelchangeMode( int ) ) );

	QVBoxLayout *lModeLayout = new QVBoxLayout;
	lModeLayout->addWidget( new QLabel( "Acquisition mode" ) );
	lModeLayout->addWidget( mModeComboBox );

    mPlayButton = new QPushButton( tr( "Play" ) );
    mPlayButton->setMinimumHeight( 60 );
    mPlayButton->setMinimumWidth( 100 );
    mPlayButton->setEnabled( false );

    QPixmap lPlayPixmap(":PvDualSourceSample/res/play.bmp");
    lPlayPixmap.setMask( lPlayPixmap.createMaskFromColor( QColor( 0xFF, 0xFF, 0xFF ) ) );
    mPlayButton->setIcon(QIcon(lPlayPixmap));

    mStopButton = new QPushButton( tr( "Stop" ) );
    mStopButton->setMinimumHeight( 60 );
    mStopButton->setMinimumWidth( 100 );
    mStopButton->setEnabled( false );

    QPixmap lStopPixmap( ":PvDualSourceSample/res/stop.bmp" );
    lStopPixmap.setMask( lStopPixmap.createMaskFromColor( QColor( 0xFF, 0xFF, 0xFF ) ) );
    mStopButton->setIcon( QIcon( lStopPixmap ) );

    QObject::connect(
    	mPlayButton, SIGNAL( clicked() ),
    	this, SLOT( OnStart() ) );
    QObject::connect(
    	mStopButton, SIGNAL( clicked() ),
    	this, SLOT( OnStop() ) );

	QHBoxLayout *lLayout = new QHBoxLayout;
	lLayout->addLayout( lModeLayout );
	lLayout->addStretch();
	lLayout->addWidget( mPlayButton );
	lLayout->addWidget( mStopButton );

	return lLayout;
}


///
/// \brief Syncs the UI state
///

void SourceWidget::EnableInterface()
{
	bool lModeEnabled = false;
	bool lPlayEnabled = false;
	bool lStopEnabled = false;

    if ( ( mSource->GetAcquisitionState() == PvAcquisitionStateUnknown ) )
    {
        // Not connected: acquisition state manager not available, disable all
    }
    else
    {
        // Read current acquisition state
        bool lLocked = mSource->GetAcquisitionState() == PvAcquisitionStateLocked;

        // Mode and play are enabled if not locked
        lModeEnabled = !lLocked;
        lPlayEnabled = !lLocked;

        // Stop is enabled only if locked
        lStopEnabled = lLocked;
    }

    mModeComboBox->setEnabled( lModeEnabled );
    mPlayButton->setEnabled( lPlayEnabled );
    mStopButton->setEnabled( lStopEnabled );
}


///
/// \brief Play button handler
///

void SourceWidget::OnStart()
{
	PvResult lResult = mSource->StartAcquisition();
    if ( !lResult.IsOK() )
    {
		PvMessageBox( this, lResult );
    }
}


///
/// \brief Stop button handler
///

void SourceWidget::OnStop()
{
	PvResult lResult = mSource->StopAcquisition();
    if ( !lResult.IsOK() )
    {
		PvMessageBox( this, lResult );
    }
}


///
/// \brief Timer event handler
///

void SourceWidget::OnTimer()
{
	QString lInfo;
	mSource->GetStreamInfo( lInfo );

	mStatusLine->setText( lInfo );
}


///
/// \brief Mode combobox changed handler
///

void SourceWidget::OnCbnSelchangeMode( int aIndex )
{
	if ( aIndex < 0 )
	{
		return;
	}

	QString lMode = mModeComboBox->itemText( aIndex );
	PvResult lResult = mSource->SetAcquisitionMode( lMode );
    if ( !lResult.IsOK() )
    {
		PvMessageBox( this, lResult );
    }
}


///
/// \brief Generic event handler
///

bool SourceWidget::event( QEvent *aEvent )
{
    switch ( aEvent->type() )
    {
    	case WM_UPDATEACQCONTROLS:
    		EnableInterface();
    		break;

    	case WM_UPDATEACQMODE:
    		UpdateAcquisitionMode();
    		break;

    	case WM_UPDATEACQMODES:
    		UpdateAcquisitionModes();
    		break;
    }

    return QWidget::event( aEvent );
}


///
/// \brief Updates the content of the acquisition mode combo box
///

void SourceWidget::UpdateAcquisitionModes()
{
	// Get current mode
	QString lCurrentMode;
	mSource->GetAcquisitionMode( true, lCurrentMode );

	// Get all possible modes
	std::vector<QString> lModes;
	mSource->GetAcquisitionModes( lModes );

	// Fill combo box
	mModeComboBox->clear();
	std::vector<QString>::iterator lIt = lModes.begin();
	while ( lIt != lModes.end() )
	{
		mModeComboBox->addItem( *lIt );
		if ( *lIt == lCurrentMode )
		{
			mModeComboBox->setCurrentIndex( mModeComboBox->count() - 1 );
		}

		lIt++;
	}
}


///
/// \brief Updates the acquisition mode from the device value
///

void SourceWidget::UpdateAcquisitionMode()
{
	QString lMode;
	mSource->GetAcquisitionMode( false, lMode );
	if ( lMode.length() > 0 )
	{
		for ( int i = 0; i < mModeComboBox->count(); i++ )
		{
			if ( lMode == mModeComboBox->itemText( i ) )
			{
				mModeComboBox->setCurrentIndex( i );
				return;
			}
		}
	}
}


