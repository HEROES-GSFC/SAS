/*
 * splashscreen.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: fgobeil
 */

#include "splashscreen.h"

#include <PvVersion.h>

#include <QtGui/QVBoxLayout>
#include <QtGui/QPainter>


SplashScreen::SplashScreen()
{
	CreateLayout();
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::CreateLayout()
{
	QFont lBoldFont;
	lBoldFont.setBold( true );

    mBitmapLabel = new QLabel;
    mBackground = new QPixmap( ":GEVPlayer/res/GEVPlayer_splash.bmp" );
    mBitmapLabel->setPixmap( *mBackground );

    mGEVPlayerLabel = new QLabel("GEVPlayer");
	mGEVPlayerLabel->setFont(lBoldFont);

	QString lPureGEVString;
	lPureGEVString.sprintf( "%s version %d.%d.%d (build %d)", PRODUCT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_SUB, VERSION_BUILD );

	mPureGEVLabel = new QLabel( lPureGEVString );
	mCopyrightLabel = new QLabel( VERSION_COPYRIGHT );
	mPleoraLabel = new QLabel( VERSION_COMPANY_NAME );

	QVBoxLayout *lLayout = new QVBoxLayout;
	lLayout->addWidget( mBitmapLabel );
	lLayout->addStretch( 25 );
	lLayout->addWidget( mGEVPlayerLabel );
	lLayout->addWidget( mPureGEVLabel );
	lLayout->addWidget( mCopyrightLabel );
	lLayout->addWidget( mPleoraLabel );
	lLayout->addStretch();

    setLayout( lLayout );

    setFixedSize( 600, 420 );

    Qt::WindowFlags lFlags = Qt::SplashScreen;
    lFlags |= Qt::WindowStaysOnTopHint;
    setWindowFlags( lFlags );
/*
    QPalette lPalette = palette();
    lPalette.setColor( QPalette::Window, Qt::black );
    setPalette( lPalette );
*/
}


void SplashScreen::paintEvent( QPaintEvent * event )
{
	QPainter lP( this );

	lP.drawRect( rect().adjusted( 0, 0, -1, -1 ) );
}


