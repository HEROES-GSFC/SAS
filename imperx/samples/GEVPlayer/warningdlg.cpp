// *****************************************************************************
//
//     Copyright (c) 2011, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "gevplayer.h"
#include "warningdlg.h"
#include <assert.h>


// ==========================================================================
WarningDlg::WarningDlg( QWidget *aParent, const QString &aMessage, const QString &aCheckBoxLabel )
	: QDialog( aParent )
	, mMessage( aMessage )
	, mDontShowAgain( false )
	, mCheckBoxLabel( aCheckBoxLabel )
{
	CreateLayout();
}


// ==========================================================================
WarningDlg::~WarningDlg()
{
}


// ==============================================================================
void WarningDlg::CreateLayout()
{
	QLabel *lLabel = new QLabel( mMessage );
	lLabel->setWordWrap( true );

	mCheckBox = ( mCheckBoxLabel.length() > 0 ) ? mCheckBox = new QCheckBox( mCheckBoxLabel ) : NULL;

    mOKButton = new QPushButton( "OK" );

    QObject::connect( mOKButton, SIGNAL( clicked() ), this, SLOT( accept() ) );

    QHBoxLayout *lBottom = new QHBoxLayout;
    lBottom->addStretch();
    lBottom->addWidget( mOKButton );

	QVBoxLayout *lLayout = new QVBoxLayout;
	lLayout->addWidget( lLabel );
	lLayout->addStretch();
	if ( mCheckBox != NULL )
	{
		lLayout->addWidget( mCheckBox );
	}
	lLayout->addLayout( lBottom );

	setLayout( lLayout );
    setFixedSize( 450, 150 );

    Qt::WindowFlags lFlags = windowFlags();
    lFlags |= Qt::CustomizeWindowHint;
    lFlags &= ~Qt::WindowContextHelpButtonHint;
    lFlags &= ~Qt::WindowSystemMenuHint;
    setWindowFlags( lFlags );

    setWindowTitle( tr( "GEVPlayer" ) );
}


// ==============================================================================
void WarningDlg::accept()
{
	if ( mCheckBox != NULL )
	{
		mDontShowAgain = mCheckBox->isChecked();
	}

	QDialog::accept();
}

