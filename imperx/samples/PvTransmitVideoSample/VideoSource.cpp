// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvBuffer.h>

#include <opencv2/opencv.hpp>

#include "VideoSource.h"


VideoSource::VideoSource( PvInt32 aCaptureDevice, const std::string &aVideoFile )
    : mCvCapture( NULL )
    , mCaptureDevice( aCaptureDevice )
    , mVideoFile( aVideoFile )
{
}

VideoSource::~VideoSource()
{
    if ( mCvCapture != NULL )
    {
        ::cvReleaseCapture( &mCvCapture );
        mCvCapture = NULL;
    }
}

void VideoSource::OpenCaptureDevice()
{
    mCvCapture = NULL;
    if ( mCaptureDevice != -1 )
    {
        mCvCapture = ::cvCaptureFromCAM( mCaptureDevice );
    }
    else
    {
        mCvCapture = ::cvCaptureFromFile( mVideoFile.c_str() );
    }

    if ( mCvCapture == NULL )
    {
        cout << "Failed initializing video capture object." << endl;
        exit( 1 );
    }
}

bool VideoSource::CopyImage( PvBuffer *aBuffer )
{
    if ( !::cvGrabFrame( mCvCapture ) )
    {
        if ( mCaptureDevice == -1 )
        {
            // Loop the video file if we have reached the end
            ::cvReleaseCapture( &mCvCapture );
            mCvCapture = ::cvCaptureFromFile( mVideoFile.c_str() );
            ::cvGrabFrame( mCvCapture );
        }
    }

    IplImage *lImage = ::cvRetrieveFrame( mCvCapture );
    if ( lImage != NULL )
    {
        memcpy( aBuffer->GetDataPointer(), lImage->imageData, aBuffer->GetSize() );
        return true;
    }

    return false;
}

PvUInt32 VideoSource::GetWidth()
{
    return (PvUInt32) ::cvGetCaptureProperty( mCvCapture, CV_CAP_PROP_FRAME_WIDTH );
}

PvUInt32 VideoSource::GetHeight() 
{
    return (PvUInt32) ::cvGetCaptureProperty( mCvCapture, CV_CAP_PROP_FRAME_HEIGHT );
}
