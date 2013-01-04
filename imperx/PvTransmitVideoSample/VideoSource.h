// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __VIDEO_SOURCE_H__
#define __VIDEO_SOURCE_H__

#include <PvSampleUtils.h>


struct CvCapture;
class PvBuffer;


// Video source
class VideoSource
{
public:

    VideoSource( PvInt32 aCaptureDevice, const std::string &aVideoFile );
    ~VideoSource();

    void OpenCaptureDevice();

    bool CopyImage( PvBuffer *aBuffer );
    PvUInt32 GetWidth();
    PvUInt32 GetHeight();

private:

    CvCapture* mCvCapture;

    PvInt32 mCaptureDevice;
    std::string mVideoFile;
};


#endif // __VIDEO_SOURCE_H__

