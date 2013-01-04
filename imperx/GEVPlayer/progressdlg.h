// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************


#pragma once

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QTimer>

#include <PvDevice.h>

#include "thread.h"

class ProgressDlg : public QDialog
{
    Q_OBJECT

public:

    ProgressDlg( Thread* aThread, QWidget* aParent );

    virtual ~ProgressDlg();

    int exec();

	virtual void paintEvent(QPaintEvent * event);
    
    void SetStatus( QString aStatus );

protected slots:

	void OnTimer();

protected:

    void CreateLayout();

	void OnOK();
	void OnCancel();

    void Update();

	QLabel *mStatusLabel;

    QPixmap *mWheel;

	QTimer *mTimer;
    unsigned long mWheelIndex;

private:

    Thread* mThread;
	PvResult mResult;
    QString mStatus;
    QMutex mMutex;

};

