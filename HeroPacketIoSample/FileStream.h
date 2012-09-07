// EcFileStream.h: interface for the EcFileStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECFILESTREAM_H__67D35AA8_29A4_49F8_89FA_D272C56BFBE6__INCLUDED_)
#define AFX_ECFILESTREAM_H__67D35AA8_29A4_49F8_89FA_D272C56BFBE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
#include "ComStream.h"
#else
#include <streams/ComStream.h>
#endif

#include <stdio.h>

class EcFileStream : public EcComStream  
{
private:
	/// pointer to the current file
	FILE*	m_file;

	/// boolean flag that true if file is for input
	bool	m_bInput;

	/// stores the file name (used to reset the stream)
	char	m_fileName[256];

public:
	EcFileStream();
	virtual ~EcFileStream();

	/// \brief Opens the file stream
	///
	/// \params fname The name of the file being opened
	/// \params bRead TRUE means that the file is opened for read only, otherwise write only
	/// \params bAppend TRUE means append new data at end of file, otherwise create empty file
	/// \return 0 on success
	///
	int Open(const char* fname, bool bRead = true, bool bAppend = true);

	/// Save data to file
	virtual int Send(const char *buffer, int *length, int timeout=-1);

	/// Read data from file
	virtual int Receive(char *buffer, int *length, int timeout=-1 );

	/// Close file
	virtual void Close();

	/// Reset file stream
	///
	/// If Read-Mode then reset file pointer to beginning of file.
	/// If Write-Mode then destroy contents of file.
	///
	virtual int Reset();


};

#endif // !defined(AFX_ECFILESTREAM_H__67D35AA8_29A4_49F8_89FA_D272C56BFBE6__INCLUDED_)
