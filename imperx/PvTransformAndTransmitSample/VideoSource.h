// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __VIDEO_SOURCE_H__
#define __VIDEO_SOURCE_H__

#include <PvSampleUtils.h>
#include <PvDevice.h>
#include <PvStream.h>
#include <PvPipeline.h>
#include <PvBufferConverter.h>

#include <map>


struct CvFont;
struct _IplImage;
typedef _IplImage IplImage;
typedef map<PvUInt32, IplImage *> IplImageMap;
class PvTransmitterRaw;


// Video source
class VideoSource
{
public:

    VideoSource( const char *aDeviceAddress );
    ~VideoSource();

    PvUInt32 GetWidth();
    PvUInt32 GetHeight();
    PvPixelType GetPixelFormat();

    void Connect();
    void StartAcquisition();
    void StopAcquisition();
    void Disconnect();

    bool FillBuffer( PvBuffer *aBuffer, PvTransmitterRaw *aTransmitter );

private:

    IplImage *GetImageForBuffer( PvBuffer *aBuffer );
    void Draw( PvTransmitterRaw *aTransmitter, IplImage *aImage );

    std::string mDeviceAddress;

    PvDevice mDevice;
    PvStream mStream;
    PvPipeline *mPipeline;

    PvBufferConverter mConverter;

    CvFont *mFont;

    IplImageMap mMap;
};


#endif // __VIDEO_SOURCE_H__

