// *****************************************************************************
//
//     Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __SOURCE_H__
#define __SOURCE_H__

#include <PvAcquisitionStateManager.h>
#include <PvDevice.h>
#include <PvStream.h>
#include <PvPipeline.h>

#include <vector>


class PvDisplayWnd;
class SourceWidget;
class ThreadDisplay;


class Source : protected PvAcquisitionStateEventSink, protected PvPipelineEventSink, protected PvGenEventSink
{
public:

    Source( PvUInt16 aSourceIndex );
    virtual ~Source();

    PvResult Open( PvDevice *aDevice, const PvString &aIPAddress, PvUInt16 aChannel );
    PvResult Close();

    PvResult StartStreaming();
    PvResult StopStreaming();

    PvResult StartAcquisition();
    PvResult StopAcquisition();

	bool IsOpened() { return mStream->IsOpen(); }
	PvGenParameterArray *GetParameters() { return mStream->GetParameters(); }
	PvAcquisitionState GetAcquisitionState();

	void GetAcquisitionMode( bool aSelectSourceIfNeeded, QString &aAcquisitionMode );
	void GetAcquisitionModes( std::vector<QString> &aVector );
	PvResult SetAcquisitionMode( const QString &aAcquisitionMode );

	void SetWidget( SourceWidget *aWidget );

	void GetStreamInfo( QString &aInfo );

protected:

    PvUInt32 GetPayloadSize();

     // PvPipelineEventSink events
    void OnBufferTooSmall( PvPipeline *aPipeline, bool *aReallocAll, bool *aResetStats );

    // PvAcquisitionStateEventSink events
    void OnAcquisitionStateChanged( PvDevice* aDevice, PvStreamBase* aStream, PvUInt32 aSource, PvAcquisitionState aState );

    // PvGenParameterEventSink events
	void OnParameterUpdate( PvGenParameter *aParameter );

private:

    // Owned by the source object
    PvStream *mStream;
    PvPipeline *mPipeline;
    PvAcquisitionStateManager *mAcquisitionManager;

    // Referenced from outside
    PvDevice *mDevice;
    PvDisplayWnd *mDisplayWnd;
    SourceWidget *mWidget;

    ThreadDisplay *mThreadDisplay;

    PvUInt16 mSourceIndex;

    bool mMultiSource;
    bool mBuffersReallocated;
};


#endif // __SOURCE_H__


