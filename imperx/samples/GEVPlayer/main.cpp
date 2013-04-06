// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "gevplayer.h"
#include "splashscreen.h"

#include <QtGui/QApplication>
#include <QtCore/QTime>


int main(int argc, char *argv[])
{
	// Bug 3013: now forcing locale to English, US for consistency
	QLocale::setDefault( QLocale( QLocale::English, QLocale::UnitedStates ) );

	QApplication a(argc, argv);

    SplashScreen *lSplash = new SplashScreen;
    lSplash->show();

    QTime lTimer;
    lTimer.start();

    while ( lTimer.elapsed() < 500 )
    {
    	a.processEvents();
    }

    QCoreApplication::setOrganizationName( "Pleora Technologies Inc" );
    QCoreApplication::setOrganizationDomain( "pleora.com" );
    QCoreApplication::setApplicationName( "GEVPlayer" );
    
    GEVPlayer w;

    while ( lTimer.elapsed() < 2500 )
    {
    	a.processEvents();
    }

    w.show();

    while ( lTimer.elapsed() < 3000 )
    {
    	a.processEvents();
    }

    lSplash->close();
    delete lSplash;
    lSplash = NULL;

    return a.exec();
}

