// *****************************************************************************
//
//     Copyright (c) 2011, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "gevplayer.h"
#include "errorsdlg.h"
#include <assert.h>


// ==========================================================================
ErrorsDlg::ErrorsDlg( QWidget *aParent, PvStringList *aErrorList )
	: QDialog( aParent )
	, mErrorList( aErrorList )
{
	assert( mErrorList != NULL );

	CreateLayout();
	ErrorsToUI();
}


// ==========================================================================
ErrorsDlg::~ErrorsDlg()
{
}


// ==============================================================================
void ErrorsDlg::CreateLayout()
{
	QLabel *lLabel = new QLabel( "The following error(s) occurred during the operation:" );

    mErrorsEdit = new QTextEdit( this );
    mErrorsEdit->setReadOnly( true );
    mErrorsEdit->setUndoRedoEnabled( false );

    mOKButton = new QPushButton( "OK" );

    QObject::connect( mOKButton, SIGNAL( clicked() ), this, SLOT( accept() ) );

    QHBoxLayout *lBottom = new QHBoxLayout;
    lBottom->addStretch();
    lBottom->addWidget( mOKButton );

	QVBoxLayout *lLayout = new QVBoxLayout;
	lLayout->addWidget( lLabel );
	lLayout->addWidget( mErrorsEdit );
	lLayout->addLayout( lBottom );

	setLayout( lLayout );
    setMinimumHeight( 300 );
    setMinimumWidth( 500 );

    Qt::WindowFlags lFlags = windowFlags();
    lFlags |= Qt::CustomizeWindowHint;
    lFlags &= ~Qt::WindowContextHelpButtonHint;
    lFlags &= ~Qt::WindowSystemMenuHint;
    setWindowFlags( lFlags );

    setWindowTitle( tr( "GEVPlayer" ) );
}


// ==============================================================================
void ErrorsDlg::ErrorsToUI()
{
	assert( mErrorList != NULL );

	QString lErrors;
	PvString *lError = mErrorList->GetFirst();
	while ( lError != NULL )
	{
		lErrors += lError->GetAscii();
		lErrors += "\r\n\r\n";

		lError = mErrorList->GetNext();
	}

	mErrorsEdit->setText( lErrors );
}


// ==============================================================================
void ErrorsDlg::accept()
{
    QDialog::accept();
}

