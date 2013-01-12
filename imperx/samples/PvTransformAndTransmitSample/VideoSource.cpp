// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvDeviceFinderWnd.h>
#include <PvTransmitterRaw.h>
#include <PvVersion.h>

#include <opencv2/opencv.hpp>
#include <sstream>

#include "VideoSource.h"


#define TEXT_COLOR ( CV_RGB( 0, 180, 0 ) )


VideoSource::VideoSource( const char *aDeviceAddress )
    : mDeviceAddress( aDeviceAddress )
{
    mPipeline = new PvPipeline( &mStream );

    mFont = new CvFont;
    ::cvInitFont( mFont, CV_FONT_HERSHEY_DUPLEX, 1, 1, 0, 2, 8 );
}

VideoSource::~VideoSource()
{
    // Pipeline was dynamically allcated, release it
    if ( mPipeline != NULL )
    {
        delete mPipeline;
        mPipeline = NULL;
    }

    // Release font
    if ( mFont != NULL )
    {
        delete mFont;
        mFont = NULL;
    }

    // Free OpenCV images
    IplImageMap::iterator lIt = mMap.begin();
    while ( lIt != mMap.end() )
    {
        IplImage *lImage = ( lIt++ )->second;
        ::cvReleaseImage( &lImage );
    }
}

void VideoSource::Connect()
{
    if ( mDeviceAddress.size() <= 0 )
    {
        // No device address, prompts user
        PvDeviceFinderWnd lWnd;
        if ( lWnd.ShowModal().IsOK() )
        {
            mDeviceAddress = lWnd.GetSelected()->GetIPAddress().GetAscii();
        }
    }

    // Connect device
    PvResult lResult = mDevice.Connect( mDeviceAddress.c_str() );
    if ( !lResult )
    {
        cout << "Unable to connect to device." << endl;
        exit( 1 );
    }

    // Open stream
    lResult = mStream.Open( mDeviceAddress.c_str() );
    if ( !lResult )
    {
        cout << "Unable to open stream." << endl;
        exit( 1 );
    }
}

void VideoSource::StartAcquisition()
{
    // Start pipeline
    mPipeline->Start();

    // Set streaming destination
    mDevice.SetStreamDestination( mStream.GetLocalIPAddress(), mStream.GetLocalPort() );

    // Set TLParamsLocked to 1
    mDevice.GetGenParameters()->SetIntegerValue( "TLParamsLocked", 1 );

    // Start acquisition
    mDevice.GetGenParameters()->ExecuteCommand( "AcquisitionStart" );
}

void VideoSource::StopAcquisition()
{
    // Stop acquisition
    mDevice.GetGenParameters()->ExecuteCommand( "AcquisitionStop" );

    // Set TLParamsLocked to 0
    mDevice.GetGenParameters()->SetIntegerValue( "TLParamsLocked", 0 );

    // Stop pipeline
    mPipeline->Stop();
}

void VideoSource::Disconnect()
{
    if ( mStream.IsOpen() )
    {
        // Close stream
        mStream.Close();
    }

    if ( mDevice.IsConnected() )
    {
        // Disconect device
        mDevice.Disconnect();
    }
}

bool VideoSource::FillBuffer( PvBuffer *aBuffer, PvTransmitterRaw *aTransmitter )
{
    // Get input buffer from pipeline (stream in)
    PvBuffer *lInBuffer = NULL;
    if ( !mPipeline->RetrieveNextBuffer( &lInBuffer, 100 ).IsOK() )
    {
        return false;
    }

    // Get image by ID (one for every buffer, attached for the buffer)
    IplImage *lImage = GetImageForBuffer( aBuffer );

    PvUInt32 lWidth = GetWidth();
    PvUInt32 lHeight = GetHeight();
    PvUInt32 lPaddingX = lImage->widthStep - ( lWidth * 3 ); // width is in pixels, padding in bytes!

    // Attach image to output buffer with conversion-happy pixel type
    aBuffer->GetImage()->Attach( lImage->imageData, 
        lWidth, lHeight, PvPixelWinBGR24, lPaddingX );

    // Copy in buffer into image attached to out buffer using eBUS SDK pixel converters
    mConverter.Convert( lInBuffer, aBuffer, false );

    // Draw (transform part of the sample) on image attached to out buffer
    Draw( aTransmitter, lImage );

    // Release buffer back to pipeline
    mPipeline->ReleaseBuffer( lInBuffer );

    // Reattach output buffer to a GEV compliant pixel type
    aBuffer->GetImage()->Attach( lImage->imageData, 
        lWidth, lHeight, ::PvPixelRGB8Packed, lPaddingX );

    return true;
}

PvUInt32 VideoSource::GetWidth()
{
    PvInt64 lWidth;
    mDevice.GetGenParameters()->GetIntegerValue( "Width", lWidth );

    return static_cast<PvUInt32>( lWidth );
}

PvUInt32 VideoSource::GetHeight()
{
    PvInt64 lHeight;
    mDevice.GetGenParameters()->GetIntegerValue( "Height", lHeight );

    return static_cast<PvUInt32>( lHeight );
}

PvPixelType VideoSource::GetPixelFormat()
{
    PvInt64 lFormat;
    mDevice.GetGenParameters()->GetEnumValue( "PixelFormat", lFormat );

    return static_cast<PvPixelType>( lFormat );
}

IplImage *VideoSource::GetImageForBuffer( PvBuffer *aBuffer )
{
    PvUInt32 lID = static_cast<PvUInt32>( aBuffer->GetID() );
    IplImage *lImage = NULL;

    PvUInt32 lWidth = GetWidth();
    PvUInt32 lHeight = GetHeight();

    if ( mMap.find( lID ) == mMap.end() )
    {
        // No buffer found at map position
    }
    else
    {
        lImage = mMap[ lID ];
        if ( ( lImage->width != lWidth ) ||
             ( lImage->height != lHeight ) )
        {
            // Detach from PvBuffer
            aBuffer->Detach();

            // Image found, but not the right size
            ::cvReleaseImage( &lImage );
            lImage = NULL;
        }
    }

    if ( lImage == NULL )
    {
        // Create a new image of the right size
        lImage = ::cvCreateImage( ::cvSize( lWidth, lHeight ), IPL_DEPTH_8U, 3 );
        mMap[ lID ] = lImage;
    }

    return lImage;
}

void VideoSource::Draw( PvTransmitterRaw *aTransmitter, IplImage *aImage )
{
    CvPoint lOrigin;
    lOrigin.x = 50;
    lOrigin.y = 50;

    ::cvPutText( aImage, "PvTransformAndTransmitSample", lOrigin, mFont, CV_RGB( 0, 180, 0 ) );
    lOrigin.y += 50;

    stringstream lSS;
    lSS.str( "" );
    lSS << PRODUCT_NAME << " " << NVERSION_STRING;
    ::cvPutText( aImage, lSS.str().c_str(), lOrigin, mFont, TEXT_COLOR );
    lOrigin.y += 100;

    lSS.str( "" );
    lSS << fixed << setprecision( 1 );
    lSS << "Transmitted " << aTransmitter->GetBlocksTransmitted() << " blocks";
    ::cvPutText( aImage, lSS.str().c_str(), lOrigin, mFont, TEXT_COLOR );
    lOrigin.y += 50;

    lSS.str( "" );
    lSS << fixed << setprecision( 1 );
    lSS << aTransmitter->GetAverageTransmissionRate() <<
        " (" << aTransmitter->GetInstantaneousTransmissionRate() << ") FPS";
    ::cvPutText( aImage, lSS.str().c_str(), lOrigin, mFont, TEXT_COLOR );
    lOrigin.y += 50;

    lSS.str( "" );
    lSS << fixed << setprecision( 1 );
    lSS << aTransmitter->GetAveragePayloadThroughput() / 1000000.0f << 
        " (" << aTransmitter->GetInstantaneousPayloadThroughput() / 1000000.0f << ") Mb/s";
    ::cvPutText( aImage, lSS.str().c_str(), lOrigin, mFont, TEXT_COLOR );
    lOrigin.y += 50;
}


