// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "gevplayer.h"
#include "eventmonitordlg.h"
#include <assert.h>

#include <QtGui/QFileDialog>


#define MAX_LOG_SIZE ( 256 * 1024 )


// ==========================================================================
EventMonitorDlg::EventMonitorDlg( QWidget *aParent, LogBuffer *aLogBuffer, PvGenParameterArray *aParameters )
	: QDialog( aParent )
	, mParameters( aParameters )
	, mNeedInit( TRUE )
	, mLogEditTextLength( 0 )
	, mLastUpdateTime( 0 )
	, mTimer( 0 )
	, mLogBuffer( aLogBuffer )
{
	assert( mLogBuffer != NULL );

	CreateLayout();

	mLogBuffer->Reset();
	mNeedInit = FALSE;
	mInfoMap.clear();
	for ( PvUInt32 i = 0; i < mParameters->GetCount(); i++ )
	{
		PvGenParameter *lParameter = mParameters->Get( i );

		// We only display invisible parameter events when in debug mode
#ifndef _DEBUG
		PvGenVisibility lVisibility = PvGenVisibilityUndefined;
		assert( lParameter->GetVisibility( lVisibility ) );
		if ( lVisibility < PvGenVisibilityInvisible )
#endif // _DEBUG
		{
			PvString lName;
			assert( lParameter->GetName( lName ).IsOK() );

			mInfoMap[ lName.GetAscii() ].Initialize( lParameter, mLogBuffer );
		}
	}

	mTimer->start( 250 );
}


// ==============================================================================
void
EventMonitorDlg::CreateLayout()
{
	mClearLogButton = new QPushButton( tr( "Clear log" ) );
    mLogEdit = new QTextEdit( this );
    mLogEdit->setReadOnly( true );
    mLogEdit->setUndoRedoEnabled( false );

	mGenICamCheckBox = new QCheckBox( tr( "GenICam parameters updates" ) );
	mBufferErrorCheckBox = new QCheckBox( tr( "PvStream buffers (errors)" ) );
	mBufferAllCheckBox = new QCheckBox( tr( "PvStream buffers (all)" ) );

	mSaveLogCheckBox = new QCheckBox( tr( "Save to file" ) );
	mSaveLogLineEdit = new QLineEdit( tr( "" ) );
	mSaveLogButton = new QPushButton( tr( "..." ) );
	mSaveLogButton->setMaximumWidth( 32 );

    QHBoxLayout *lSaveLogLayout = new QHBoxLayout;
	lSaveLogLayout->addWidget( mSaveLogCheckBox );
	lSaveLogLayout->addWidget( mSaveLogLineEdit );
	lSaveLogLayout->addWidget( mSaveLogButton );

    QVBoxLayout *lLeft = new QVBoxLayout;
    lLeft->addWidget( mLogEdit );
	lLeft->addWidget( mClearLogButton );
	lLeft->addLayout( lSaveLogLayout );

    QVBoxLayout *lRight = new QVBoxLayout;
    lRight->addWidget( mGenICamCheckBox );
    lRight->addWidget( mBufferErrorCheckBox );
    lRight->addWidget( mBufferAllCheckBox );
	lRight->addStretch();

	QHBoxLayout *lLayout = new QHBoxLayout;
	lLayout->addLayout( lLeft );
	lLayout->addLayout( lRight );

	setLayout( lLayout );
    setMinimumHeight( 400 );
    setMinimumWidth( 800 );

    Qt::WindowFlags lFlags = windowFlags();
    lFlags |= Qt::CustomizeWindowHint;
    lFlags &= ~Qt::WindowContextHelpButtonHint;
    lFlags &= ~Qt::WindowSystemMenuHint;
    setWindowFlags( lFlags );

    setWindowTitle( tr( "Event Monitor" ) );

	QObject::connect( mSaveLogCheckBox, SIGNAL( clicked() ), this, SLOT( OnBnClickedSaveLogCheckBox() ) );
	QObject::connect( mSaveLogButton, SIGNAL( clicked() ), this, SLOT( OnBnClickedSaveLogButton() ) );
	QObject::connect( mClearLogButton, SIGNAL( clicked() ), this, SLOT( OnBnClickedClearButton() ) );
	QObject::connect( mGenICamCheckBox, SIGNAL( clicked() ), this, SLOT( OnBnClickedGenICamCheckBox() ) );
	QObject::connect( mBufferAllCheckBox, SIGNAL( clicked() ), this, SLOT( OnBnClickedBufferAllCheckBox() ) );
	QObject::connect( mBufferErrorCheckBox, SIGNAL( clicked() ), this, SLOT( OnBnClickedBufferErrorCheckBox() ) );

	mTimer = new QTimer(this);
	QObject::connect( mTimer, SIGNAL( timeout() ), this, SLOT( OnTimer() ) );

	LoadData();
}


// ==========================================================================
EventMonitorDlg::~EventMonitorDlg()
{
	mTimer->stop();
}


// ==========================================================================
void EventMonitorDlg::OnBnClickedClearButton()
{
	mLogEdit->setText( "" );
	mLogEditTextLength = 0;
	mLogBuffer->Reset();
}


// ==============================================================================
void EventMonitorDlg::OnTimer()
{
	QString lEntry = mLogBuffer->Empty();
	int lLength = lEntry.length();

    if ( lLength <= 0 )
	{
		return;
	}

    mLogEdit->moveCursor( QTextCursor::End );
	mLogEditTextLength += lLength;
	mLogEdit->insertPlainText( lEntry );

    if ( mLogEditTextLength > MAX_LOG_SIZE )
    {
    	QTextCursor lCursor = mLogEdit->textCursor();
    	lCursor.setPosition( 0, QTextCursor::MoveAnchor );
    	lCursor.setPosition( lLength - 1, QTextCursor::KeepAnchor );
    	lCursor.insertText( "" );

    	mLogEditTextLength -= lLength;
    }

    mLogEdit->moveCursor( QTextCursor::End );
}


// ==============================================================================
void EventMonitorDlg::OnBnClickedGenICamCheckBox()
{
	mLogBuffer->SetGenICamEnabled( mGenICamCheckBox->checkState() == Qt::Checked );
}


// ==============================================================================
void EventMonitorDlg::OnBnClickedBufferAllCheckBox()
{
	mLogBuffer->SetBufferAllEnabled( mBufferAllCheckBox->checkState() == Qt::Checked );
}


// ==============================================================================
void EventMonitorDlg::OnBnClickedBufferErrorCheckBox()
{
	mLogBuffer->SetBufferErrorEnabled( mBufferErrorCheckBox->checkState() == Qt::Checked );
}


// ==============================================================================
void EventMonitorDlg::OnBnClickedSaveLogCheckBox()
{
	mLogBuffer->SetWriteToFileEnabled( mSaveLogCheckBox->checkState() == Qt::Checked );
	EnableInterface();
}


// ==============================================================================
void EventMonitorDlg::OnBnClickedSaveLogButton()
{
	QFileDialog lDlg;
	lDlg.setAcceptMode( QFileDialog::AcceptSave );
	lDlg.setFileMode( QFileDialog::AnyFile );
	lDlg.setNameFilter( "Text files (*.txt);;All files (*.*)" );
	lDlg.selectFile( mLogBuffer->GetFilename() );

	if ( lDlg.exec() )
	{
		QStringList lList = lDlg.selectedFiles();
		if ( lList.size() > 0 )
		{
			mLogBuffer->SetFilename( *lList.begin() );
		}
	}

	LoadData();
}


// ==============================================================================
void EventMonitorDlg::EnableInterface()
{
	bool lEnabled = ( mSaveLogCheckBox->checkState() == Qt::Checked );
	mSaveLogLineEdit->setEnabled( false );
	mSaveLogButton->setEnabled( lEnabled );
}


// ==============================================================================
void EventMonitorDlg::LoadData()
{
	mGenICamCheckBox->setCheckState( ( mLogBuffer->IsGenICamEnabled() ) ? Qt::Checked : Qt::Unchecked );
	mBufferAllCheckBox->setCheckState( ( mLogBuffer->IsBufferAllEnabled() ) ? Qt::Checked : Qt::Unchecked );
	mBufferErrorCheckBox->setCheckState( ( mLogBuffer->IsBufferErrorEnabled() ) ? Qt::Checked : Qt::Unchecked );
	mSaveLogCheckBox->setCheckState( ( mLogBuffer->IsWriteToFileEnabled() ) ? Qt::Checked : Qt::Unchecked );
	mSaveLogLineEdit->setText( mLogBuffer->GetFilename() );
	
	EnableInterface();
}

// ==============================================================================
void EventMonitorDlg::EnableGenICamMonitoring( bool aEnabled )
{
	std::map<QString, ParameterInfo>::iterator lIt = mInfoMap.begin();
	while ( lIt != mInfoMap.end() )
	{
		lIt->second.Enable( aEnabled );
		lIt++;	
	}
}


