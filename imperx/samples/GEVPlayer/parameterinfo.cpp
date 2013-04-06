// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************


#include "parameterinfo.h"

#include <QtCore/QLocale>

#include <assert.h>
#ifdef WIN32
#include <windows.h>
#endif


ParameterInfo::ParameterInfo()
	: mParameter( NULL )
	, mLogBuffer( NULL )
	, mEnabled( true )
{
}

ParameterInfo::~ParameterInfo()
{
	if ( mParameter != NULL )
	{
		mParameter->UnregisterEventSink( this );
	}
}

void ParameterInfo::Initialize( PvGenParameter *aParameter, LogBuffer *aLogBuffer )
{
	assert( aParameter != NULL );
	assert( aLogBuffer != NULL );
	
	// We do not support double-init (yet)
	assert( mParameter == NULL );
	assert( mLogBuffer == NULL );

	mParameter = aParameter;
	mLogBuffer = aLogBuffer;

	mParameter->RegisterEventSink( this );
}

void ParameterInfo::Reset()
{
	mLastValue = "";
}


void ParameterInfo::OnParameterUpdate( PvGenParameter *aParameter )
{
	if ( mEnabled )
	{
		SetLastValue();
		LogToBuffer();
	}
}


void ParameterInfo::SetLastValue()
{
	PvGenType lType = PvGenTypeUndefined;
	if ( !mParameter->GetType( lType ).IsOK() )
	{
		assert( 0 ); // Totally unexpected
		mLastValue = "???";
		return;
	}

	if ( !mParameter->IsReadable() || !mParameter->IsAvailable() )
	{
		return;
	}

	switch ( lType )
	{
		case PvGenTypeInteger:
			{
				PvInt64 lValue;
				static_cast<PvGenInteger *>( mParameter )->GetValue( lValue );
				mLastValue.sprintf( "%lld", lValue );
			}
			break;

		case PvGenTypeEnum:
			{
				PvString lValue;
				static_cast<PvGenEnum *>( mParameter )->GetValue( lValue );
				mLastValue = lValue.GetAscii();
			}
			break;

		case PvGenTypeBoolean:
			{
				bool lValue;
				static_cast<PvGenBoolean *>( mParameter )->GetValue( lValue );
				mLastValue = lValue ? "TRUE" : "FALSE";
			}
			break;

		case PvGenTypeString:
			{
				PvString lValue;
				static_cast<PvGenString *>( mParameter )->GetValue( lValue );
				mLastValue = lValue.GetAscii();
			}
			break;

		case PvGenTypeCommand:
			mLastValue = "N/A";
			break;

		case PvGenTypeFloat:
			{
				double lValue;
				static_cast<PvGenFloat *>( mParameter )->GetValue( lValue );

				QLocale lLocale;
				mLastValue = lLocale.toString( lValue );
			}
			break;
	}
}


void ParameterInfo::LogToBuffer()
{
	if ( !mLogBuffer->IsGenICamEnabled() )
	{
		return;
	}

	PvString lName;
	if ( !mParameter->GetName( lName ).IsOK() )
	{
		assert( 0 ); // Totally unexpected...
	}

	QString lStr;
	lStr.sprintf( "%s: %s",
		lName.GetAscii(), mLastValue.toAscii().data() );

	mLogBuffer->Log( lStr );
}

