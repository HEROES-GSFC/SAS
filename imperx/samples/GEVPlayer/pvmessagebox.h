// *****************************************************************************
//
//     Copyright (c) 2010, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __PVMESSAGEBOX_H__
#define __PVMESSAGEBOX_H__

#include <QtGui/QMessageBox>


void PvMessageBox( QWidget *aWidget, PvResult &aResult )
{
	QString lError = aResult.GetCodeString().GetAscii();
	QString lDescription = aResult.GetDescription().GetAscii();
	QMessageBox::critical( aWidget, "Error", lError + "\r\n\r\n" + lDescription + "\r\n" );
}


#endif // __PVMESSAGEOX_H__



