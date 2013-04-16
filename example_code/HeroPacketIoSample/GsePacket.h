///	\file GsePacket.h
///		C++ header file declaring the class EcGsePacket.

#ifndef __GSEPACKET_H__
#define __GSEPACKET_H__

#ifdef _WIN32
#include "Packet.h"
#include "TimeCorrection.h"
#else
#include <packets/Packet.h>
#include <TimeCorrection.h>
#endif

// Valid data types for EcGsePacket creation. (Data length in parentheses)
enum
{
	// Invalid type.
	GSEPTYPE_NONE = 0,
};

///	Maximum packet size (in bytes).
#define GSEPKT_MAX_SIZE	(1024)

/// Maximum packet payload size
#define GSEPKT_MAX_DATA_SIZE (GSEPKT_MAX_SIZE - sizeof(EcGsePacket::Header))

/////////////////////////////////////////////////////////////////////////////////////
//	class EcGsePacket
///		Generic GSE data packet class.
//
///		This is a data class used for wrapping
///		data sent to the GSE.  This class combines raw data with a header, an
///		identifier, a time stamp, and a CRC checksum.
class EcGsePacket : public EcPacket
{
// Constructors and destructors.
public:
	EcGsePacket();
	virtual ~EcGsePacket();

	// The universal sync word.
	enum {
		SYNCVAL = 0xc39a,
		SYNCVAL1 = 0x9a,
		SYNCVAL2 = 0xc3,
	};

  //	EcGsePacket::Header:
  ///		Structure to hold GSE packet header values.
  #pragma pack( push, ambient )
  #pragma pack( 1 )
  struct Header
  {
	  uint16_t	SyncVal;			///< Synchronization value.
	  uint8_t		Type;				///< Packet type.
	  uint8_t		SrcID;				///< ID of sending CPU.
	  uint16_t	DataLength;			///< Length of data segment in bytes.
	  uint16_t	CheckSum;			///< CRC16 checksum.
	  uint32_t	Time[2];			///< Time stamp (Time[0] = nanoseconds, Time[1] = time_t (UNIX time)).
  };
  #pragma pack( pop, ambient )

// Protected data members.
protected:
	Header*	m_Header;		///< Pointer to the header part of the packet.
	char*	m_Data;					///< Pointer to the payload data part of the packet.
	bool	m_Altered;				///< Flag set if data altered since last CRC calculation.
	uint32_t	m_Message;				///< Message corresponding to this type.
	int		m_DataLength;			///< Data length or max data length based on type.
	int		m_Count;				///< Byte count for packet assembled by ProcessByte.
	bool	m_Reset;				///< Flag to control creation and routine calls.
	bool	m_VariableLength;		///< Flag indicating if packet is of variable length.
  static  OffsetTimeCorrection*  offTime;
  static  LinearTimeCorrection*  linTime;

// Public operations.
public:
	// Creation/reset.
	virtual int Create( uint8_t type );
	void Reset();

  // Time correction access functions.
  static OffsetTimeCorrection*  getOffsetTimeCorrection();
  static LinearTimeCorrection*  getLinearTimeCorrection();

	// Packet header set value functions.
	int SetPacketTime();
	int SetPacketTime( const timespec* pTimestamp );
	int SetPacketTime( time_t tSeconds, uint32_t tNanosec );
	void SetChecksum();
	int SetSrcID( uint8_t SrcID );

	// Packet header get functions.
	virtual bool GetPacketTime( timespec *pFileTime );
	virtual int GetPacketTime( time_t *const pTSec, uint32_t *const pTNanoSec );
	virtual double GetPacketSeconds();
	virtual uint16_t GetCRCVal();
	int GetSrcID( uint8_t *pSrcID );
	virtual uint16_t GetType();
	virtual int GetPacketLength();
	virtual int GetDataWordLength();
	virtual int GetDataByteLength();

	int SetWordData( int index, uint16_t data );
	int SetArray( char *pData, int *length, int start=0 );
	int	SetDataLength( uint16_t length );

	// Get header and data values.
	virtual uint16_t GetWordData( int index );
	virtual void *GetPointerToData();
	virtual void *GetPointerToPacket();

	// Packet input functions.
	bool ProcessByte( char byte, int *const pErrCount );
	bool ProcessInput( char *input, int *start, int length, int *const pErrCount );
	int VerifyBuffer( char *input, int length );

	// Helper functions for input.
	bool CheckChecksum();
  uint32_t GetMessage() {return m_Message;};
	virtual bool HandleError( uint16_t ErrVal1, uint16_t ErrVal2=0, uint16_t ErrVal3=0, uint16_t ErrVal4=0 ) {return false;};

	// Packet output functions.
	virtual int Send( EcComStream *pStream );
	virtual int SendPacket( EcComStream *pStream, int timeout=-1 );
	virtual int SendPacket( const timespec* pTimestamp, EcComStream *pStream );
	virtual int SendPacket( time_t tSeconds, uint32_t tMicrosec, EcComStream *pStream );

protected:
	virtual int VerifyType( uint8_t type, int *pDataLength, uint32_t *pMessage, bool *pVariableLength );
};

#endif	// ifndef __GSEDATA_H__
