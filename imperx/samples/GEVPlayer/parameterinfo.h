// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <PvGenParameterArray.h>

#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QTime>

#include "logbuffer.h"


class ParameterInfo : public PvGenEventSink
{
public:

	ParameterInfo();
	~ParameterInfo();

	void Initialize( PvGenParameter *aParameter, LogBuffer *aLogBuffer );
	void Reset();

	// Used for temporarely disabling node map access
	bool IsEnabled() const { return mEnabled; }
	void Enable( bool aEnabled ) { mEnabled = aEnabled; }

private:

	void OnParameterUpdate( PvGenParameter *aParameter );

	void SetLastValue();
	void LogToBuffer();

	PvGenParameter *mParameter;
	QString mLastValue;

	LogBuffer *mLogBuffer;

	bool mEnabled;
};

