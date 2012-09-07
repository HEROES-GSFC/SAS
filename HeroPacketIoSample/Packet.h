///	\file Packet.h
///		C++ header filed declaring the class EcPacket.

#ifndef __PACKET_H__
#define __PACKET_H__

#ifdef _WIN32
#include <ComStream.h>
#else
#include <streams/ComStream.h>
#endif

#include <stdio.h>

//	EcPacket:
///		Generic virtual data packet class.
//
///		This class is the basis for all packets (command and GSE)
///		with sync word, type/length and checksum information included.
class EcPacket
{
// Construction/destruction.
public:
	EcPacket();
	virtual ~EcPacket();

// Protected data members.
protected:
	char*		m_Packet;				///< pointer to packet.
	int			m_PacketLength;			///< Length of packet (in bytes).

// Public member functions.
public:
	int		GetPacketByteLength();
  char* GetPacket() {return m_Packet;};
	virtual uint16_t  CalcCRC16();
	virtual int		  SendPacket( EcComStream* pStream, int timeout=-1 );
	virtual int		  SavePacket( FILE *pFile );
};

#endif // #ifndef __PACKET_H__
