///	\file ComStream.h
///		C++ header file declaring the class EcComStream.

#ifndef __COMSTREAM_H__
#define __COMSTREAM_H__

#include <Emb.h>
#include <time.h>
#include <stdint.h>

// Fundamental data types used by HERO

// Enumerate some types.
enum {
	CSTYPE_SERIAL = 1,
	CSTYPE_SOCKET,
	CSTYPE_SEAIO,
	CSTYPE_FILE,
	CSTYPE_PARALLEL,
    CSTYPE_HDLC,
    CSTYPE_LOGFILE,
};

///		This class is designed as a virtual base class for all communications classes.
//
///		This provides a common interface to all stream-based communications, but allows for
///		customized creation and start up for such specialized devices as files, sockets, and
///		serial ports.
///	NOTE:  There is no C++ source file, because all functions are declared blank
///		in this file.
class EcComStream
{
// Construction/destruction.
public:
	EcComStream() {m_Type = 0;};
	virtual ~EcComStream() {};

// Protected data members.
protected:
	int		m_Type;			///< Type of communications device (currently not used).

// Public member functions.
public:
	virtual int Send( const char *buffer, int *length, int timeout=-1 ) {return 0;};
	virtual int Receive( char *buffer, int *length, int timeout=-1 ) {return 0;};
	virtual void Close() {};
	virtual int Reset() {return 0;};
	virtual int GetType() {return m_Type;};
};

#endif // #ifndef __COMSTREAM_H__
