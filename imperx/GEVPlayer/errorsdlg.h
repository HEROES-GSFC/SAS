// *****************************************************************************
//
//     Copyright (c) 2011, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <map>

#include <QtGui/QDialog>
#include <QtGui/QTextEdit>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtCore/QString>

#include <PvStringList.h>


class ErrorsDlg : public QDialog
{
    Q_OBJECT

public:

	ErrorsDlg( QWidget *aParent, PvStringList *aErrorList );
	virtual ~ErrorsDlg();

protected slots:

protected:

	void ErrorsToUI();
	void CreateLayout();

protected slots:

	void accept();

private:

	QTextEdit *mErrorsEdit;
	QPushButton *mOKButton;

	PvStringList *mErrorList;
};


