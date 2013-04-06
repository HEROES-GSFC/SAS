// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************


#include "progressdlg.h"
#include "gevplayer.h"

#include <QtGui/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QBitmap>
#include <QtGui/QBoxLayout>

#include <PvDevice.h>
#include <PvInterface.h>

#include <assert.h>



// =============================================================================
ProgressDlg::ProgressDlg( Thread* aThread, QWidget* aParent )	
    : QDialog( aParent )
    , mThread( aThread )
{
    CreateLayout();
    mTimer = new QTimer( this );
    mTimer->setInterval( 100 );
    connect( mTimer, SIGNAL( timeout() ), this, SLOT( OnTimer() ) );
    mTimer->start();
}


// =============================================================================
ProgressDlg::~ProgressDlg()
{
    if ( mTimer != NULL )
    {
        delete mTimer;
        mTimer = NULL;
    }
}


// =============================================================================
void ProgressDlg::CreateLayout()
{
    mStatusLabel = new QLabel( tr( "" ) );
    mStatusLabel->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    mStatusLabel->setWordWrap( true );

    QVBoxLayout *lLayout = new QVBoxLayout;
    QBoxLayout *lBoxLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    lBoxLayout->addSpacing(30);
    lBoxLayout->addWidget( mStatusLabel);
    lLayout->addLayout(lBoxLayout);

    setLayout( lLayout );
    setFixedSize( 400, 120 );

    Qt::WindowFlags lFlags = windowFlags();
    lFlags |= Qt::CustomizeWindowHint;
    lFlags &= ~Qt::WindowContextHelpButtonHint;
    lFlags &= ~Qt::WindowSystemMenuHint;
    lFlags &= ~Qt::WindowCloseButtonHint;
    setWindowFlags( lFlags );

    mWheel = new QPixmap (":GEVPlayer/res/wheel.bmp");
    mWheel->setMask(mWheel->createMaskFromColor(QColor(0xFF, 0xFF, 0xFF)));

    setWindowTitle( tr( "Connection Progress" ) );
}


// =============================================================================
void ProgressDlg::OnOK()
{
}


// =============================================================================
void ProgressDlg::OnCancel()
{
}

void ProgressDlg::paintEvent(QPaintEvent * event)
{
	QPainter lPainter(this);

	QRectF target(12, 15, 16, 16);
	QRectF source(mWheelIndex*16, 0, 16, 16);
	lPainter.drawPixmap(target, *mWheel, source);

	QWidget::paintEvent(event);
}

// ==============================================================================
void ProgressDlg::OnTimer()
{
    // Advance to next image in sprite
    ( ++mWheelIndex ) %= 8;

    // Tell Qt to repaint
    update();

    // Update dialog
    Update();

	// If the device thread is done, complete and start stream thread
	if ( mThread->isFinished() )
	{
		accept();
	}
}


///
/// \brief Start thread, call base class
///

int ProgressDlg::exec()
{
    mThread->start();
    return QDialog::exec();
}

// ==========================================================================
void ProgressDlg::Update()
{
    QString lOldStr;
    lOldStr = mStatusLabel->text();

    /////////////////////////////////////////////////////////////////
    mMutex.lock();

    	if ( lOldStr != mStatus )
        {
            mStatusLabel->setText( mStatus );
        }

    mMutex.unlock();
    /////////////////////////////////////////////////////////////////
}

// ==========================================================================
void ProgressDlg::SetStatus( QString aStatus )
{
    /////////////////////////////////////////////////////////////////
    mMutex.lock();

    	mStatus = aStatus;

    mMutex.unlock();
    /////////////////////////////////////////////////////////////////
}

