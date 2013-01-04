// *****************************************************************************
//
//     Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "threaddisplay.h"

#include <PvPipeline.h>
#include <PvDisplayWnd.h>

#include <assert.h>


#define VSYNC_ENABLED ( false )
#define MAX_FPS ( 60 )
#define KEEP_PARTIAL_IMAGES ( false )


///
/// \brief Constructor
///

ThreadDisplay::ThreadDisplay( PvDisplayWnd *aDisplayWnd, PvPipeline *aPipeline )
    : mDisplayWnd( aDisplayWnd )
    , mPipeline( aPipeline )
    , mLatestBuffer( NULL )
    , mLatestOddBuffer( NULL )
    , mLatestEvenBuffer( NULL )
    , mFinalBuffer( NULL )
    , mStopping( false )
    , mLastBlockID( 0 )
{
    ResetStats();
}


///
/// \brief Destructor
///

ThreadDisplay::~ThreadDisplay()
{
    // Wait for the thread to stop
    wait();
}


///
/// \brief Display thread run method
///

void ThreadDisplay::run()
{
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

        if ( lResult.IsOK() )
        {
            if ( lBuffer->GetOperationResult().IsOK() )
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
    bool lDisplay = mStabilizer.IsTimeToDisplay( MAX_FPS );
    if ( lDisplay )
    {
        Display( mFinalBuffer );
    }
}


///
/// \brief Sends one buffer to the display
///

void ThreadDisplay::Display( PvBuffer *aBuffer )
{
    // Display buffer
    mDisplayWnd->Display( *aBuffer, VSYNC_ENABLED );
}


///
/// \brief Takes incoming buffer, try to come up with a displayable buffer
///

bool ThreadDisplay::SetLatest( PvBuffer *aBuffer )
{
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
        return false;
    }

    if ( lImage->IsInterlacedOdd() )
    {
        // Save/replace Odd temp buffer, wait for matching Even
        ReleaseLatestBuffers();
        mLatestOddBuffer = aBuffer;
        return false;
    }
    else if ( lImage->IsInterlacedEven() )
    {
        // Do we have an Odd buffer? Is the Odd buffer matching this one?
        if ( ( mLatestOddBuffer != NULL ) && ( AreBlockIDsConsecutive( mLatestOddBuffer, aBuffer ) ) )
        {
            mLatestEvenBuffer = aBuffer;
        }
        else
        {
            mPipeline->ReleaseBuffer( aBuffer );
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

    return true;
}


