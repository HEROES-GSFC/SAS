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


class WarningDlg : public QDialog
{
    Q_OBJECT

public:

	WarningDlg( QWidget *aParent, const QString &aMessage, const QString &aCheckBoxLabel = "" );
	virtual ~WarningDlg();

	bool IsDontShowAgain() const { return mDontShowAgain; }

protected slots:

protected:

	void CreateLayout();

protected slots:

	void accept();

private:

	QCheckBox *mCheckBox;
	QPushButton *mOKButton;

	bool mDontShowAgain;

	QString mMessage;
	QString mCheckBoxLabel;
};


