// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************


#include "threaddisplay.h"
#include "gevplayer.h"
#include "utilities.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QTime>

#include <assert.h>

#include <PvFilterRGB.h>


///
/// \brief Constructor
///

ThreadDisplay::ThreadDisplay(
    GEVPlayer *aDlg, PvDisplayWnd *aDisplayWnd, 
    PvPipeline *aPipeline,
    FilteringDlg *aFilteringDlg, 
    ImageSaveDlg* aImageSaveDlg,
    LogBuffer *aLogBuffer )
    : mMainDlg( aDlg )
    , mDisplayWnd( aDisplayWnd )
    , mPipeline( aPipeline )
    , mLatestBuffer( NULL )
    , mLatestOddBuffer( NULL )
    , mLatestEvenBuffer( NULL )
    , mFinalBuffer( NULL )
    , mImageSaveDlg( aImageSaveDlg )
    , mFilteringDlg( aFilteringDlg )
    , mStopping( false )
    , mVSyncEnabled( false )
    , mMaxFPS( 30 )
    , mLogBuffer( aLogBuffer )
    , mLastBlockID( 0 )
    , mKeepPartialImages( false )
{
    mFilteringDlg->SetThreadDisplay( this );
    mImageSaveDlg->SetThreadDisplay( this );

    ResetStats();

    mLastUpdate.start();
}


///
/// \brief Destructor
///

ThreadDisplay::~ThreadDisplay()
{
    mFilteringDlg->SetThreadDisplay( NULL );
    mImageSaveDlg->SetThreadDisplay( NULL );

    // Wait for the thread to stop
    wait();
}


///
/// \brief Display thread run method
///

void ThreadDisplay::run()
{
    assert( mDisplayWnd != NULL );
    assert( mMainDlg != NULL );

    for ( ;; )
    {
        // Check if we were signaled to terminate
        if ( mStopping )
        {
            ReleaseLatestBuffers();
            return;
        }

        PvBuffer *lBuffer = NULL;

        // Try retrieving a buffer, using default timeout
        PvResult lResult = mPipeline->RetrieveNextBuffer( &lBuffer );
		
		// Perfrom logging on the just retrieved buffer
		Log( lBuffer );

        if ( lResult.IsOK() )
        {
            if ( lBuffer->GetOperationResult().IsOK() || mKeepPartialImages )
            {
                ProcessBuffer( lBuffer );
            }
            else
            {
                mPipeline->ReleaseBuffer( lBuffer );
            }
        }
    }
}


///
/// \brief Locks (and get) the latest buffer
///
/// \note Remember to release it!
///

PvBuffer* ThreadDisplay::RetrieveLatestBuffer()
{
    mBufferMutex.lock();
    return mLatestBuffer;
}


///
/// \brief Releases the latest buffer
///

void ThreadDisplay::ReleaseLatestBuffer()
{
    mBufferMutex.unlock();
}


///
/// \brief Performs white balance on latest full buffer
///

void ThreadDisplay::WhiteBalance( PvFilterRGB *aFilterRGB )
{
    if ( mFinalBuffer == NULL )
    {
        return;
    }

    mBufferMutex.lock();
    
	PvImage *lFinalImage = mFinalBuffer->GetImage();
	assert( lFinalImage != NULL );

    PvBuffer lBufferDisplay;
	PvImage *lImageDisplay = lBufferDisplay.GetImage();
	assert( lImageDisplay != NULL );
    lImageDisplay->Alloc( lFinalImage->GetWidth(), lFinalImage->GetHeight(), PvPixelWinRGB32 );

    // Convert last good buffer to RGB, one-time use converter
    PvBufferConverter lConverter;
    lConverter.Convert( mFinalBuffer, &lBufferDisplay );

    aFilterRGB->WhiteBalance( &lBufferDisplay );

    Display( mFinalBuffer );

    mBufferMutex.unlock();
}


///
/// \brief From QThread, signals the thread to stop
///

void ThreadDisplay::stop()
{
    mStopping = true;
}


///
/// \brief Reset display thread stats
///

void ThreadDisplay::ResetStats()
{
    mStabilizer.Reset();
	mLastBlockID = 0;
}


///
/// \brief Returns display VSync
///

bool ThreadDisplay::GetVSyncEnabled() const
{
	return mVSyncEnabled;
}


///
/// \brief Controls display VSync
///

void ThreadDisplay::SetVSyncEnabled( bool aEnabled )
{
	mVSyncEnabled = aEnabled;
}


///
/// \brief Releases buffers in circulation to the pipeline
///

void ThreadDisplay::ReleaseLatestBuffers()
{
    if ( mLatestBuffer != NULL )
    {
        mPipeline->ReleaseBuffer( mLatestBuffer );
        mLatestBuffer = NULL;
    }

    if ( mLatestOddBuffer != NULL )
    {
        mPipeline->ReleaseBuffer( mLatestOddBuffer );
        mLatestOddBuffer = NULL;
    }

    if ( mLatestEvenBuffer != NULL )
    {
        mPipeline->ReleaseBuffer( mLatestEvenBuffer );
        mLatestEvenBuffer = NULL;
    }
}


///
/// \brief Checks that two buffers are consecutive, based on BlockID
///

bool ThreadDisplay::AreBlockIDsConsecutive( PvBuffer *aFirst, PvBuffer *aSecond )
{
    PvUInt16 lFirst = aFirst->GetBlockID();
    PvUInt16 lSecond = aSecond->GetBlockID();

    PvUInt16 lExpected = lFirst + 1;
    if ( lExpected == 0 )
    {
        lExpected = 1;
    }

    return ( lSecond == lExpected );
}


///
/// \brief Processes the latest buffer retrieve from the pipeline
///

void ThreadDisplay::ProcessBuffer( PvBuffer *aBuffer )
{
    // Based on regular or interlaced status, set latest buffer
    if ( !SetLatest( aBuffer ) )
    {
        return;
    }

    // Display and main dialog refresh throttled by FPS stabilizer
    bool lDisplay = mStabilizer.IsTimeToDisplay( ( mMaxFPS > 0 ) ? mMaxFPS : 10 );
    if ( lDisplay ) 
    {
        // Only display if the display option is enabled!
        if ( mMaxFPS > 0 )
        {
            mBufferMutex.lock();
            Display( mFinalBuffer );
            mBufferMutex.unlock();
        }
    }
/*
    if ( mLastUpdate.elapsed() > 100 )
    {
	    QCoreApplication::postEvent( mMainDlg, new QEvent( static_cast<QEvent::Type>( WM_IMAGEDISPLAYED ) ) );
	    mLastUpdate.restart();
    }
*/
    // Give the buffer to the image saving dialog - will save if necessary
    mImageSaveDlg->SaveIfNecessary( mFinalBuffer );

}


///
/// \brief Sends one buffer to the display
///

void ThreadDisplay::Display( PvBuffer *aBuffer )
{
    // Display buffer
    mFilteringDlg->ConfigureConverter( mDisplayWnd->GetConverter() );
    mDisplayWnd->Display( *aBuffer, mVSyncEnabled );
}


///
/// \brief Takes incoming buffer, try to come up with a displayable buffer
///

// ==========================================================================
bool ThreadDisplay::SetLatest( PvBuffer *aBuffer )
{
    mBufferMutex.lock();

	PvImage *lImage = aBuffer->GetImage();

    // since we've added the "keep partial images" option, this is a safeguard to make sure
    // we don't try to display a buffer with 0 width or height or not an image
	if ( ( lImage == NULL ) ||
         ( lImage->GetWidth() == 0 ) ||
		 ( lImage->GetHeight() == 0 ) ||
		 ( ( aBuffer->GetAcquiredSize() - lImage->GetPaddingY() ) > aBuffer->GetSize() ) ||
		 ( ( aBuffer->GetRequiredSize() - lImage->GetPaddingY() ) > aBuffer->GetSize() ) )
    {
        mPipeline->ReleaseBuffer( aBuffer );
        mBufferMutex.unlock();
        return false;
    }

    if ( ( lImage->IsInterlacedOdd() ) && ( mDeinterlacing != DeinterlacingDisabled ) )
    {
        // Save/replace Odd temp buffer, wait for matching Even
        ReleaseLatestBuffers();
        mLatestOddBuffer = aBuffer;
        mBufferMutex.unlock();
        return false;
    }
    else if ( ( lImage->IsInterlacedEven() ) && ( mDeinterlacing != DeinterlacingDisabled ) )
    {
        // Do we have an Odd buffer? Is the Odd buffer matching this one?
        if ( ( mLatestOddBuffer != NULL ) && ( AreBlockIDsConsecutive( mLatestOddBuffer, aBuffer ) ) )
        {
            mLatestEvenBuffer = aBuffer;
        }
        else
        {
            mPipeline->ReleaseBuffer( aBuffer );
            mBufferMutex.unlock();
            return false;
        }
    }
    else
    {
        // No interlacing, just keep this buffer
        ReleaseLatestBuffers();
        mLatestBuffer = aBuffer;
    }

    if ( mLatestBuffer != NULL )
    {
        mFinalBuffer = mLatestBuffer;
    }
    else if ( ( mLatestOddBuffer != NULL ) && ( mLatestEvenBuffer != NULL ) )
    {
        // Odd and Even buffers available: new iPORT ANL-Pro de-interlacing
        mFilterDeinterlace.Apply( mLatestOddBuffer, mLatestEvenBuffer, &mDeinterlacedBuffer );
        mFinalBuffer = &mDeinterlacedBuffer;

        // Final pointing on de-interlaced buffer, we can release
        ReleaseLatestBuffers();
    }

    mBufferMutex.unlock();

    return true;
}

///
/// \brief Performs logging on the buffer
///

void ThreadDisplay::Log( PvBuffer *aBuffer )
{
	if ( aBuffer == NULL )
	{
		return;
	}

	PvUInt32 lBlockID = aBuffer->GetBlockID();
	PvResult lResult = aBuffer->GetOperationResult();

	if ( mLogBuffer->IsBufferAllEnabled() || mLogBuffer->IsBufferErrorEnabled() )
	{
		PvUInt32 lBlockIDsMissing = 0;
		if ( ( mLastBlockID != 0 ) && ( lBlockID != 0 ) )
		{
			if  ( lBlockID > mLastBlockID )
			{
				// Easy
				lBlockIDsMissing = lBlockID - mLastBlockID - 1;
			}
			else
			{
				// Handle wrap-around
				lBlockIDsMissing =
					( USHRT_MAX - mLastBlockID ) +
					( lBlockID - 1 );
			}

			if ( lBlockIDsMissing > 0 )
			{
				QString lStr;
				lStr.sprintf( "Block IDs between %04X and %04X missing",
					mLastBlockID, lBlockID );

				mLogBuffer->Log( lStr );
			}
		}

		if ( !lResult.IsOK() || mLogBuffer->IsBufferAllEnabled() )
		{
			QString lStr;
			lStr.sprintf( "Result: %s BlockID: %04X Timestamp: %016llX", 
				lResult.GetCodeString().GetAscii(), lBlockID, aBuffer->GetTimestamp() );

			mLogBuffer->Log( lStr );
		}
	}

	// Save for next run...
	mLastBlockID = lBlockID;
}




