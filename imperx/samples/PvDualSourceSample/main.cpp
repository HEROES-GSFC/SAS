// *****************************************************************************
//
//     Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "pvdualsourcesample.h"

#include <QtGui/QApplication>
#include <QtCore/QTime>


int main(int argc, char *argv[])
{
	// Forcing locale to English, US for consistency
	QLocale::setDefault( QLocale( QLocale::English, QLocale::UnitedStates ) );

	QApplication a(argc, argv);

    QCoreApplication::setOrganizationName( "Pleora Technologies Inc" );
    QCoreApplication::setOrganizationDomain( "pleora.com" );
    QCoreApplication::setApplicationName( "GEVPlayer" );
    
    PvDualSourceSample w;
    w.show();

    return a.exec();
}



