// *****************************************************************************
//
//     Copyright (c) 2010, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "logbuffer.h"

#include <stdio.h>
#include <assert.h>

#ifdef WIN32
#include <shlobj.h>
#endif


#define LOGBUFFER_VERSION ( "1.0.0.0" )

#define TAG_VERSION ( "logbufferversion" )
#define TAG_GENICAMENABLED ( "loggenicamenabled" )
#define TAG_BUFFERALLENABLED ( "logbufferallenabled" )
#define TAG_BUFFERERRORENABLED ( "logbuffererrorenabled" )
#define TAG_LOGFILENAME ( "logfilename" )
#define TAG_WRITETOFILEENABLED ( "logwritetofileenabled" )

#define VAL_TRUE ( "true" )
#define VAL_FALSE ( "false" )


#define MAX_LOG_SIZE ( 256 * 1024 )


// =============================================================================
LogBuffer::LogBuffer()
	: mGenICamEnabled( true )
	, mBufferAllEnabled( false )
	, mBufferErrorEnabled( true )
	, mWriteToFileEnabled( false )
{
	ResetConfig();
}


// =============================================================================
LogBuffer::~LogBuffer()
{
}


// =============================================================================
void LogBuffer::ResetConfig()
{
	mGenICamEnabled = true;
	mBufferAllEnabled = false;
	mBufferErrorEnabled = true;
	mWriteToFileEnabled = false;

#ifdef WIN32
    char lDesktop[ MAX_PATH ];
	SHGetSpecialFolderPath( NULL, lDesktop, CSIDL_DESKTOP, true );

    mFilename = QString( lDesktop ) + "\\EventMonitor.txt";
#endif //WIN32

#ifdef _LINUX_
    //TODO: Find API call to get the proper default folder
    mFilename = "EventMonitor.txt";
#endif //_LINUX_
}


// =============================================================================
void LogBuffer::Reset()
{
	mMutex.lock();
	//////////////////////////////////

    mStartTimestamp.start();
	mBuffer = "";

	//////////////////////////////////
    mMutex.unlock();
}


// =============================================================================
void LogBuffer::Log( const QString &aItem )
{
	QString lLast = "0";
	if ( mStartTimestamp.elapsed() >= 0 )
	{
		int lTimeDiff = mStartTimestamp.elapsed();
		lLast.sprintf( "%04i.%03i",
			lTimeDiff / 1000, lTimeDiff % 1000 );
	}

	QString lStr;
	lStr.sprintf( "%s    %s",
		lLast.toAscii().data(), aItem.toAscii().data() );

	mMutex.lock();
	//////////////////////////////////

	FILE *lFile = NULL;
	if ( mWriteToFileEnabled && ( mFilename.length() > 0 ) )
	{
		// Open file
		lFile = fopen( mFilename.toAscii().data(), "at" );
	}

	if ( mBuffer.length() < MAX_LOG_SIZE )
	{
		mBuffer += lStr + "\r\n";
	}

	if ( lFile != NULL )
	{
		fprintf( lFile, "%s\n", lStr.toAscii().data() );
		fclose( lFile );
	}

	//////////////////////////////////
	mMutex.unlock();
}


// =============================================================================
QString LogBuffer::Empty()
{
	mMutex.lock();
	//////////////////////////////////

	QString lTemp = mBuffer;
	mBuffer = "";

	//////////////////////////////////
	mMutex.unlock();

	return lTemp;
}


// =============================================================================
void LogBuffer::SetFilename( const QString &aFilename ) 
{ 
	mMutex.lock();
	//////////////////////////////////

	mFilename = aFilename; 

	//////////////////////////////////
	mMutex.unlock();
}


// =============================================================================
void LogBuffer::SetWriteToFileEnabled( bool aEnabled ) 
{ 
	mMutex.lock();
	//////////////////////////////////

	mWriteToFileEnabled = aEnabled; 

	//////////////////////////////////
	mMutex.unlock();
}


// =============================================================================
void LogBuffer::Save( PvConfigurationWriter &aWriter )
{
    // Save a version string, just in case we need it in the future
    aWriter.Store( LOGBUFFER_VERSION, TAG_VERSION );

	// bool mGenICamEnabled;
    aWriter.Store( mGenICamEnabled ? VAL_TRUE : VAL_FALSE, TAG_GENICAMENABLED );

	// bool mBufferAllEnabled;
    aWriter.Store( mBufferAllEnabled ? VAL_TRUE : VAL_FALSE, TAG_BUFFERALLENABLED );

	// bool mBufferErrorEnabled;
    aWriter.Store( mBufferErrorEnabled ? VAL_TRUE : VAL_FALSE, TAG_BUFFERERRORENABLED );

	// QString mFilename;
	aWriter.Store( mFilename.toAscii().data(), TAG_LOGFILENAME );

	// bool mWriteToFileEnabled;
    aWriter.Store( mWriteToFileEnabled ? VAL_TRUE : VAL_FALSE, TAG_WRITETOFILEENABLED );
}


// =============================================================================
void LogBuffer::Load( PvConfigurationReader &aReader )
{
    PvResult lResult;
    PvString lPvStr;

	// Always load from a blank setup!
	ResetConfig();

	// bool mGenICamEnabled;
    lResult = aReader.Restore( TAG_GENICAMENABLED, lPvStr );
    if ( lResult.IsOK() )
    {
        mGenICamEnabled = ( lPvStr == VAL_TRUE );
    }

	// bool mBufferAllEnabled;
    lResult = aReader.Restore( TAG_BUFFERALLENABLED, lPvStr );
    if ( lResult.IsOK() )
    {
        mBufferAllEnabled = ( lPvStr == VAL_TRUE );
    }

	// bool mBufferErrorEnabled;
    lResult = aReader.Restore( TAG_BUFFERERRORENABLED, lPvStr );
    if ( lResult.IsOK() )
    {
        mBufferErrorEnabled = ( lPvStr == VAL_TRUE );
    }

	// QString mFilename;
    lResult = aReader.Restore( TAG_LOGFILENAME, lPvStr );
    if ( lResult.IsOK() )
	{
		SetFilename( lPvStr.GetAscii() );
	}

	// bool mWriteToFileEnabled;
    lResult = aReader.Restore( TAG_WRITETOFILEENABLED, lPvStr );
    if ( lResult.IsOK() )
    {
        SetWriteToFileEnabled( lPvStr == VAL_TRUE );
    }
}


