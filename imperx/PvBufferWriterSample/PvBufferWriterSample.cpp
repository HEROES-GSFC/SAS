// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include <PvSampleUtils.h>
#include <PvBuffer.h>
#include <PvPixelType.h>
#include <PvBufferWriter.h>
#include <PvString.h>

#include <string>
#include <vector>

using namespace std;

PV_INIT_SIGNAL_HANDLER();

//
// Create, fill and save a buffer with dummy data to a bitmap file
//

void CreateFillAndSaveBuffer()
{
	// Create the buffer
	PvBuffer * lBuffer = new PvBuffer();

	// Create the Data (buffers are created as images, GetImage() is valid on a fresh buffer)
	lBuffer->GetImage()->Alloc( 640, 480, PvPixelMono8 );
	PvUInt8 *lDataBuffer = lBuffer->GetDataPointer();

	// Fill the buffer
	PvUInt8 lPattern = 0;
	PvUInt8 lCounter = 0;
	for ( unsigned int i = 0 ; i < 640 ; i++ )
	{
		for ( unsigned int j = 0 ; j < 480 ; j++ )
		{
			lPattern = ( lPattern + 1 ) % 255;
			lDataBuffer[(i*480)+j] = lPattern;
		}
	}
	
	// Construct the file name
	PvString lFilename( "Image.bmp" );
	PvString lCompleteFileName( lFilename );
	string lCheck = lCompleteFileName.GetAscii();

	// Create the PvBufferWriter
	PvBufferWriter lBufferWriter;
	PvResult lResult = lBufferWriter.Store( lBuffer, lCompleteFileName, PvBufferFormatBMP );

	delete lBuffer;
	lBuffer = NULL;

}

//
// Main function
//

int main()
{
	// Create the Buffer and fill it
	printf( "Creating the buffer, filling it with data and saving it as a bitmap\n" );
	CreateFillAndSaveBuffer();
	printf( "Complete\n" );

	printf( "\n<press a key to exit>\n" );
    PvWaitForKeyPress();

	return 0;
}

