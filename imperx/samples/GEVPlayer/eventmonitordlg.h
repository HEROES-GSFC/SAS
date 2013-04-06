// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
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
#include <QtCore/QTimer>

#include "parameterinfo.h"
#include "logbuffer.h"


class EventMonitorDlg : public QDialog
{
    Q_OBJECT

public:

	EventMonitorDlg( QWidget *aParent, LogBuffer *aLogBuffer, PvGenParameterArray *aParameters );
	virtual ~EventMonitorDlg();

	void LoadData();

        // Controls GenICam monitoring. Different than logging control, for performance purpose
        void EnableGenICamMonitoring( bool aEnabled );

protected slots:

	void OnBnClickedSaveLogCheckBox();
	void OnBnClickedSaveLogButton();
	void OnBnClickedClearButton();
	void OnBnClickedGenICamCheckBox();
	void OnBnClickedBufferAllCheckBox();
	void OnBnClickedBufferErrorCheckBox();
	void OnTimer();

protected:

	void CreateLayout();
	void EnableInterface();

private:

	PvGenParameterArray *mParameters;
	std::map<QString, ParameterInfo> mInfoMap;

	LogBuffer *mLogBuffer;

	QCheckBox* mSaveLogCheckBox;
	QLineEdit* mSaveLogLineEdit;
	QPushButton* mSaveLogButton;
	QTextEdit* mLogEdit;
	QPushButton* mClearLogButton;
	QCheckBox* mGenICamCheckBox;
	QCheckBox* mBufferAllCheckBox;
	QCheckBox* mBufferErrorCheckBox;

	QTimer *mTimer;

	int mLogEditTextLength;
	int mLastUpdateTime;

	bool mNeedInit;

};

