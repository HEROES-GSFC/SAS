// *****************************************************************************
//
//     Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <QtCore/QThread>

#include <PvBuffer.h>
#include <PvBufferConverter.h>
#include <PvFilterDeInterlace.h>

#include <PvFPSStabilizer.h>


class PvDualSourceSample;
class PvDisplayWnd;
class PvPipeline;


class ThreadDisplay : public QThread
{
    Q_OBJECT

public:

	ThreadDisplay( PvDisplayWnd *aDisplayWnd, PvPipeline *aPipeline );
	~ThreadDisplay();

    void stop();

    int GetFrameRate() { return mStabilizer.GetAverage(); }

    void ResetStats();

protected:

    void ReleaseLatestBuffers();
    void ProcessBuffer( PvBuffer *aBuffer );
    bool AreBlockIDsConsecutive( PvBuffer *aFirst, PvBuffer *aSecond );
    void Display( PvBuffer *aBuffer );
    bool SetLatest( PvBuffer *aBuffer );

	void run();

private:

	// We do not own these objects
	PvDualSourceSample *mMainDlg;
	PvDisplayWnd *mDisplayWnd;
    PvPipeline * mPipeline;

    PvFilterDeinterlace mFilterDeinterlace;

    PvBuffer *mLatestBuffer;
    PvBuffer *mLatestEvenBuffer;
    PvBuffer *mLatestOddBuffer;

    PvBuffer *mFinalBuffer;
    PvBuffer mDeinterlacedBuffer;

    volatile bool mStopping;

    PvFPSStabilizer mStabilizer;

	PvUInt16 mLastBlockID;
};


