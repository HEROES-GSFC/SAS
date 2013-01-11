// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QTime>

#include <PvBuffer.h>
#include <PvBufferConverter.h>
#include <PvFilterDeInterlace.h>
#include <PvFPSStabilizer.h>

#include <PvDisplayWnd.h>

#include "logbuffer.h"


class GEVPlayer;
class ImageSaveDlg;
class FilteringDlg;


typedef enum
{
	DeinterlacingDisabled = 0,
	DeinterlacingWeaving = 1


} DeinterlacingType;


class ThreadDisplay : public QThread
{
    Q_OBJECT

public:

	ThreadDisplay(
        GEVPlayer *aDlg, PvDisplayWnd *aDisplayWnd,
        PvPipeline *aPipeline,
        FilteringDlg *aFilteringDlg, ImageSaveDlg* aImageSaveDlg,
		LogBuffer *aLogBuffer
    );
	~ThreadDisplay();

    void stop();

    PvBuffer* RetrieveLatestBuffer();
    void ReleaseLatestBuffer();

    void WhiteBalance( PvFilterRGB *aFilterRGB );

    //unsigned long long GetTimestamp() { return mImageTimestamp; }
    int GetFrameRate() { return ( mMaxFPS > 0 ) ? mStabilizer.GetAverage() : 0; }

    void ResetStats();

    bool GetVSyncEnabled() const;
    void SetVSyncEnabled( bool aEnabled );

    PvInt32 GetMaxFPS() const { return mMaxFPS; }
    void SetMaxFPS( PvInt32 aMaxFPS ) { mMaxFPS = aMaxFPS; mStabilizer.Reset(); }

    PvRendererMode GetRendererMode() const { return mDisplayWnd->GetRendererMode(); }
    void SetRendererMode( PvRendererMode aMode ) const { mDisplayWnd->SetRendererMode( aMode ); }

    bool GetKeepPartialImages() { return mKeepPartialImages; }
    void SetKeepPartialImages( bool aKeepPartialImages ) { mKeepPartialImages = aKeepPartialImages; }

    DeinterlacingType GetDeinterlacing() const { return mDeinterlacing; }
    void SetDeinterlacing( DeinterlacingType aDeinterlacing ) { mDeinterlacing = aDeinterlacing; }

protected:

    void ReleaseLatestBuffers();
    void ProcessBuffer( PvBuffer *aBuffer );
    bool AreBlockIDsConsecutive( PvBuffer *aFirst, PvBuffer *aSecond );
    void Display( PvBuffer *aBuffer );
    bool SetLatest( PvBuffer *aBuffer );
	void Log( PvBuffer *aBuffer );

	void run();

private:

	// We do not own these objects
	GEVPlayer *mMainDlg;
	PvDisplayWnd *mDisplayWnd;
    PvPipeline * mPipeline;

    PvFilterDeinterlace mFilterDeinterlace;

    QMutex mBufferMutex;
    PvBuffer *mLatestBuffer;
    PvBuffer *mLatestEvenBuffer;
    PvBuffer *mLatestOddBuffer;

    PvBuffer *mFinalBuffer;
    PvBuffer mDeinterlacedBuffer;

    FilteringDlg *mFilteringDlg;
    ImageSaveDlg *mImageSaveDlg;
	LogBuffer *mLogBuffer;

	bool mVSyncEnabled;
    PvInt32 mMaxFPS;
    bool mKeepPartialImages;

    volatile bool mStopping;

    PvFPSStabilizer mStabilizer;

    QTime mLastUpdate;
	PvUInt16 mLastBlockID;

	DeinterlacingType mDeinterlacing;
};

