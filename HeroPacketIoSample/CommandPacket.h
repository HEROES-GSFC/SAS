///	\file CommandPacket.h
///		C++ header file declaring the class EcCommandPacket.

#ifndef __COMMANDPACKET_H__
#define __COMMANDPACKET_H__

#ifdef _WIN32
#include "Packet.h"
#include <ComStream.h>
#else
#include <packets/Packet.h>
#include <streams/ComStream.h>
#endif


// Maximum size of the packet.
#define CMDPKT_MAX_SIZE  254
#define CMDPKT_MAX_WORDCOUNT 127

//	EcCommandPacket
///		Generic command packet class.
//
///		This data class handles the basic functions required of command packets:  It
///		can build and send output packets from user data, and it can build and check
///		the integrity of incoming packets from a data stream.
class EcCommandPacket : public EcPacket
{
// Construction/destruction.
public:
  //	Header:
  ///		Structure to hold command packet header data.
  #pragma pack( push, ambient )
  #pragma pack( 1 )
  struct Header	
  {
	  uint16_t	SyncVal;					///< Synchronization value.
	  uint8_t	  Target;						///< Identification number of target.
	  uint8_t	  Length;						///< Length of data segment of packet (in bytes).
	  uint16_t	PktID;						///< Sequence number identifying command packet.
	  uint16_t	CheckSum;					///< CRC16 checksum.
  };
  #pragma pack( pop, ambient )

	EcCommandPacket();
	virtual ~EcCommandPacket();

	/// The universal sync word.
	enum
  {
		SYNCVAL = 0xc39a,
		SYNCVAL1 = 0x9a,
		SYNCVAL2 = 0xc3
	};

// Data members.
protected:
	bool			    m_Completed;			///< Flag indicating complete packet has been received.
	uint16_t			m_ReadCount;			///< Current input data word (for command processing).
	uint16_t			m_WriteCount;			///< Current output data word (for command building).
	uint16_t			m_SequenceNumber;	///< Sequence number of packet.
	uint16_t			m_ID;					    ///< ID of CPU generating packet.
	Header*     	m_Header;				  ///< Becomes pointer to command packet header.
	uint16_t*			m_Data;					  ///< Becomes pointer to command packet data.

// Public member functions.
public:
	void	Reset();
	int		AddWord(uint16_t val);
	int		GetWord( uint16_t *next );
	int		PeekWord( uint16_t *next );
	int		CheckBuffer( uint16_t *const buffer, int size );
	int		ProcessBuffer( char *input, int *start, int length );
	bool	ProcessInput( char *input, int *start, int length, int *const pErrCount );
	void	SetChecksum();
	bool	CheckChecksum();
	uint16_t	GetChecksum();
	int		GetDataWordLength();
	int		GetDataByteLength();
	uint16_t	GetSequenceNumber();
	int		GetCpuId();
	int		Send( EcComStream *pSocket );
	void	ResetOutputPacket();
	void	SendOutputPacket( EcComStream *pSocket );
  void  FinalizeOutputPacket();
	void	SetTargetID( uint8_t TargetId );

// Protected member functions.
protected:
	int		VerifyPacketForWrite( int length );
	int		VerifyPacketForRead( int length );
	float	ReadFloat();
	void	WriteFloat( float fVal );
	double	ReadDouble();
	void	WriteDouble( double dVal );
};

#endif // #ifndef __COMMANDPACKET_H__
