// EcFileStream.cpp: implementation of the EcFileStream class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FileStream.h"
#include "ErrorCodes.h"
#include <string.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EcFileStream::EcFileStream() : m_file(NULL), m_bInput(false)
{
}

EcFileStream::~EcFileStream()
{
	Close();
}

/// Opens the file stream
int EcFileStream::Open(const char* fname, bool bRead, bool bAppend)
{
	// store the read flag
	m_bInput = bRead;

	// store the file name
	strncpy(m_fileName, fname, sizeof(m_fileName));

	/// if applicable, open the file for writing
	if(!bRead)
	{
		if(bAppend)
			m_file = fopen(fname, "ab");
		else
			m_file = fopen(fname, "wb");
	}
	else
		m_file = fopen(fname, "rb"); // otherwise open file for reading


	if(!m_file)
		return EFSERR_OPENFAIL;

	return 0;
}

/// Save data to file
int EcFileStream::Send(const char *buffer, int* length, int timeout)
{
	// make sure file is opened
	if(!m_file)
		return EFSERR_FILECLOSED;

	// make sure file is opened for writing
	if(m_bInput)
		return EFSERR_READONLY;

	int bytesWritten = fwrite(buffer, sizeof(char), *length, m_file);

	*length = bytesWritten;

	if(ferror(m_file))
		return EFSERR_WRITEFAIL;


	return 0;
}

/// Read data from file
int EcFileStream::Receive(char *buffer, int* length, int timeout)
{
	// make sure file is opened
	if(!m_file)
		return EFSERR_FILECLOSED;

	// make sure file is opened for reading
	if(!m_bInput)
		return EFSERR_WRITEONLY;

	int bytesRead = fread((void*)buffer, sizeof(char), *length, m_file);

	*length = bytesRead;

	if(ferror(m_file))
		return EFSERR_READFAIL;

	return 0;
}

/// Close file
void EcFileStream::Close()
{
	if(m_file)
		fclose(m_file);

	m_file = NULL;
}


/// Reset file stream
int EcFileStream::Reset()
{
	// make sure file is opened
	if(!m_file)
		return EFSERR_FILECLOSED;

	// reset depends on data direction
	if(m_bInput)
	{
		// If Read-Mode then reset file pointer to beginning of file.
		fseek(m_file, 0, SEEK_SET);

		if(ferror(m_file))
			return EFSERR_RESETERR;
	}
	else
	{
		// If Write-Mode then destroy contents of file.
		fclose(m_file);
		m_file = fopen(m_fileName, "wb");

		if(!m_file)
			return EFSERR_RESETERR;
	}

	return 0;
}

