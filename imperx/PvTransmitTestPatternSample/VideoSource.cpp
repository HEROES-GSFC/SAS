// *****************************************************************************
//
// Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvBuffer.h>

#include "VideoSource.h"


VideoSource::VideoSource()
    : mSeed( 0 )
{
}

VideoSource::~VideoSource()
{
}

void VideoSource::CopyPattern( PvBuffer *aBuffer )
{
    PvUInt32 lWidth = aBuffer->GetImage()->GetWidth();
    PvUInt32 lHeight = aBuffer->GetImage()->GetHeight();

    unsigned char *lPtr = aBuffer->GetDataPointer();
    for ( PvUInt32 y = 0; y < lHeight; y++ )
    {
        unsigned char lValue = mSeed + y;
        for ( PvUInt32 x = 0; x < lWidth; x++ )
        {
            *( lPtr++ ) = lValue++;
        }
    }

    // Make sure we get a different pattern next time by
    mSeed++;
}




