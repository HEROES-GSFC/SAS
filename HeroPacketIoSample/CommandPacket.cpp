///	\file CommandPacket.cpp
///		C++ source file defining the class EcCommandPacket.
//		This is a data class which takes raw
//		words, sorts them into a valid packet, checks the checksum argument, and notifies
//		a calling process of completed packet assembly.

#include "StdAfx.h"
#include "CommandPacket.h"
#include "ErrorCodes.h"
#include <stdio.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////
//	Class EcCommandPacket

///	Default constructor.
EcCommandPacket::EcCommandPacket()
{
	// Create the data space.
	m_Packet = new char[sizeof( EcCommandPacket::Header ) + CMDPKT_MAX_SIZE];
  m_Header = (EcCommandPacket::Header *)m_Packet;
	m_Data = (uint16_t *) &m_Packet[sizeof( EcCommandPacket::Header )];

	// Initialize the sequence number.
	m_SequenceNumber = 0;

	// Initialize the object.
	Reset();
}

/// Destructor.
EcCommandPacket::~EcCommandPacket()
{
	// Delete the data segment and the packet structure.
	delete m_Packet;

	// Set pointer to NULL.
	m_Packet = NULL;
	m_Header = NULL;
	m_Data = NULL;
}

//	Reset:
///		Resets the packet for reuse.
//
///		Re-initializes the object to its initial condition, which is with no data, and no
///		header (i.e. a packet length of zero).  The read and write counts are set to zero and
///		the completion flag is reset.
void EcCommandPacket::Reset()
{
	// Set the initial conditions to no data, no count.
	m_Header->Length = 0;
	m_PacketLength = 0;
	m_ReadCount = 0;
	m_WriteCount = 0;
	m_Completed = false;
}

//	ResetOutputPacket:
///		Resets a packet for use as an output packet.
//
///		This routine is not quite as complete a reset as Reset().  The packet length is set to the
///		length of the header, not to zero.  The sync value is preserved, the data length is set
///		to zero (as are the read and write counters, and the completed flag), and the sequence number
///		is incremented.
void EcCommandPacket::ResetOutputPacket()
{
	m_Header->Length = 0;
	m_Header->SyncVal = SYNCVAL;
	m_Header->PktID = m_SequenceNumber++;
	m_PacketLength = sizeof( EcCommandPacket::Header );
	m_Completed = false;
	m_WriteCount = 0;
	m_ReadCount = 0;
}

//	VerifyPacketForWrite:
///		Verifies that the packet exists and has enough space for the requested write length.
//
///		This routine first checks that the packet pointer is not null, then checks the maximum
///		packet length against the current write index to see if there is room for the requested
///		operation.
///		\param[in]		length			Number of bytes to write to packet.
///		return		If packet exists and has at least length number of bytes free for writing,
///					this routine returns 0.  Otherwise, it returns a non-zero error code.
int EcCommandPacket::VerifyPacketForWrite( int length )
{
	// Check that packet exists.
	if ( m_Packet == NULL )
		return CMDPKTERR_NOBUFF;

	// Check that packet is not full.
	if ( (int)m_Header->Length + length >= CMDPKT_MAX_SIZE )
		return 	CMDPKTERR_FULL;

	// Return without error.
	return 0;
}

//	VerifyPacketForRead:
///		Verifies that the packet exists and has enough space for the requested write length.
//
///		This routine first checks that the packet pointer is not null, then checks that the packet
///		was completed, and then checks the maximum packet length against the current write index to
///		see if there is room for the requested
///		operation.
///		\param[in]		length			Number of bytes to write to packet.
///		return		If packet exists and has at least length number of bytes free for reading,
///					this routine returns 0.  Otherwise, it returns a non-zero error code.
int EcCommandPacket::VerifyPacketForRead( int length )
{
	// Check that packet exists.
	if ( m_Packet == NULL )
		return CMDPKTERR_NOBUFF;

	// Check that packet is completed.
	if ( !m_Completed )
		return CMDPKTERR_INCOMPLETE;

	// Check that packet is not full.
	if ( m_ReadCount * sizeof( uint16_t ) + length > m_Header->Length )
		return 	CMDPKTERR_EOP;

	// Return without error.
	return 0;
}

//	ReadFloat:
///		Reads a float from a packet without verifying anything or updating anything but the read pointer.
//
///		This routine is protected so that only trusted subclasses will use it, after first verifying
///		for packet and space.  This is so that specific add and get functions can test for total space
///		once, then use these quick routines to actually serialize the command data.
///		return			Returns a floating point value read from the packet.
float EcCommandPacket::ReadFloat()
{
	float rval;
	uint16_t *pval = (uint16_t *)&rval;

	// Read the data by uint16_t lengths.
	pval[0] = m_Data[m_ReadCount++];
	pval[1] = m_Data[m_ReadCount++];

	return rval;
}

//	WriteFloat:
///		Writes a float to a packet without verifying anything or updating anything but the write pointer.
//
///		This routine is protected so that only trusted subclasses will use it, after first verifying
///		for packet and space.  This is so that specific add and get functions can test for total space
///		once, then use these quick routines to actually serialize the command data.
///		\param[in]		fVal			Floating point value to write to the packet.
void EcCommandPacket::WriteFloat( float fVal )
{
	uint16_t *pval = (uint16_t *)&fVal;

	// Write the data by uint16_t lengths.
	m_Data[m_WriteCount++] = pval[0];
	m_Data[m_WriteCount++] = pval[1];
}

//	ReadDouble:
///		Reads a double from a packet without verifying anything or updating anything but the read pointer.
//
///		This routine is protected so that only trusted subclasses will use it, after first verifying
///		for packet and space.  This is so that specific add and get functions can test for total space
///		once, then use these quick routines to actually serialize the command data.
///		return			Returns a double-precision value read from the packet.
double EcCommandPacket::ReadDouble()
{
	double rval;
	uint16_t *pval = (uint16_t *)&rval;

	// Read the data by uint16_t lengths.
	pval[0] = m_Data[m_ReadCount++];
	pval[1] = m_Data[m_ReadCount++];
	pval[2] = m_Data[m_ReadCount++];
	pval[3] = m_Data[m_ReadCount++];

	return rval;
}

//	WriteDouble:
///		Writes a double to a packet without verifying anything or updating anything but the write pointer.
//
///		This routine is protected so that only trusted subclasses will use it, after first verifying
///		for packet and space.  This is so that specific add and get functions can test for total space
///		once, then use these quick routines to actually serialize the command data.
///		\param[in]		dVal			Double-precision value to write to the packet.
void EcCommandPacket::WriteDouble( double dVal )
{
	uint16_t *pval = (uint16_t *)&dVal;

	// Write the data by uint16_t lengths.
	m_Data[m_WriteCount++] = pval[0];
	m_Data[m_WriteCount++] = pval[1];
	m_Data[m_WriteCount++] = pval[2];
	m_Data[m_WriteCount++] = pval[3];
}

//	AddWord:
///		Adds a word to the current packet.
//
///		First checks that the packet exists, and that the packet length is at least the length
///		of the header.  Then the word is added to the packet.
///		\param[in]		Val			16-bit value to add to tail of packet.
///		\return		If packet exists and is not full, the function returns 0, otherwise it returns an error code.
int EcCommandPacket::AddWord(uint16_t val)
{
	int rval;
	if ( !(rval = VerifyPacketForWrite( sizeof( uint16_t ) )) )
	{
		m_Data[m_WriteCount++] = val;
		m_Header->Length += sizeof( uint16_t );
		m_PacketLength += sizeof( uint16_t );
	}

	// Return status.
	return rval;
}

//	GetWord:
///		Returns the next word in the data.
//
///		This function first calls VerifyPacketForRead() to check for an existing, completed packet
///		with enough data left to satisfy this read operation.  If no error is returned, it passes back
///		the next word read from the packet, and increments the read index.
///		\param[out]		next		Next availabe 16-bit uint16_t from the packet.
///		return		If packet exists and has at least length number of bytes free for reading,
///					this routine returns 0.  Otherwise, it returns a non-zero error code.
int EcCommandPacket::GetWord( uint16_t *next )
{
	int rval;
	if ( !(rval = VerifyPacketForRead( sizeof( uint16_t ) )) )
	{
		// Get the next available word.
		*next = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	PeekWord:
///		Returns the next word in the data, but doesn't advance read pointer.
//
///		This function first calls VerifyPacketForRead() to check for an existing, completed packet
///		with enough data left to satisfy this read operation.  If no error is returned, it passes back
///		the next word read from the packet.
///		\param[out]		next		Next availabe 16-bit uint16_t from the packet.
///		return		If packet exists and has at least length number of bytes free for reading,
///					this routine returns 0.  Otherwise, it returns a non-zero error code.
int EcCommandPacket::PeekWord( uint16_t *next )
{
	int rval;
	if ( !(rval = VerifyPacketForRead( sizeof( uint16_t ) )) )
	{
		// Get the next available word.
		*next = m_Data[m_ReadCount];
	}

	// Return status.
	return rval;
}

//	CheckBuffer:
//		Checks an input buffer to see if it is a valid array and if the data in it
//		forms a valid EcCipPacket.  Sets the packet data pointer to the buffer pointer
//		if it is.
int EcCommandPacket::CheckBuffer( uint16_t *const Buffer, int Length )
{
	// Check that the buffer pointer is not NULL.
	if ( Buffer == NULL )
		return CMDPKTERR_NOBUFF;

	// Convert uint16_t size into char length.
	int size = Length * sizeof( uint16_t );

	// Check for negative or insufficient size.
	if ( size <= (int)sizeof( EcCommandPacket::Header ) )
		return CMDPKTERR_NOLENGTH;

	// Cast the buffer to a header pointer to check the values.
	EcCommandPacket::Header *Head = (EcCommandPacket::Header *)Buffer;

	// Check for the sync word.
	if ( Head->SyncVal != SYNCVAL )
		return CMDPKTERR_NOSYNC;

	// Check for a proper length.
	if ( Head->Length > CMDPKT_MAX_SIZE )
		return CMDPKTERR_LENGTHERR;

	// Check that there is room in the buffer for this length packet.
	if ( (int)(Head->Length + sizeof( EcCommandPacket::Header )) > size )
		return CMDPKTERR_BUFFSHORT;

	// Save the packet length and then set it to new packet length.
	int PLength = m_PacketLength;
	m_PacketLength = Head->Length + sizeof( EcCommandPacket::Header );

	// Save the packet pointer and then set it to the passed buffer.
	char *pTemp = m_Packet;
	m_Packet = (char *)Buffer;
	m_Header = Head;

	// Check the checksum.
	if ( !CheckChecksum() )
	{
		// If failed, restore the packet length and packet pointer.
		m_PacketLength = PLength;
		m_Packet = pTemp;

		// Return the checksum error.
		return CMDPKTERR_CRCERR;
	}

	// Set the pointer back to the storage buffer.
	m_Packet = pTemp;
	m_Header = (EcCommandPacket::Header *)m_Packet;

	// Copy the buffer to local packet area.
	memcpy( m_Packet, Buffer, m_PacketLength );

	// Set the packet state as completed.
	m_ReadCount = 0;
	m_Completed = true;

	// Return okay.
	return 0;
}

//	ProcessBuffer:
///		Builds command packets from input data.
//
///		For a packet being built from an incoming byte stream, this routine processes
///		the input buffer to look for valid packets.
int EcCommandPacket::ProcessBuffer( char *input, int *start, int length )
{
	// Default return value.
	int index = *start, len = length, amt;

	// Check that the buffer pointer is not NULL.
	if ( input == NULL )
		return CMDPKTERR_NOBUFF;

	// Loop while there is data for start of data packet.
	while ( m_PacketLength < 4 && index < len )
	{
		// Process based on the byte count.
		switch ( m_PacketLength )
		{
		// Check first byte (low-order byte of SYNC word).
		case 0:
			// If it is the header value, increment the count.
			if ( input[index] == (char)SYNCVAL1 )
				m_Packet[m_PacketLength++] = input[index];
			break;

		// Check second byte (high-order byte of SYNC word).
		case 1:
			// If this byte matches upper byte of sync word, increment count.
			if ( input[index] == (char)SYNCVAL2 )
			{
				m_Packet[m_PacketLength++] = input[index];
			}
			// Otherwise, zero the count.
			else
			{
				m_PacketLength = 0;
				*start = index;
				return CMDPKTERR_NOSYNC;
			}
			break;

		// Check third byte (target computer).
		case 2:
			// Put byte into temporary storage.
			m_Packet[m_PacketLength++] = input[index];

			break;

		// Check fourth byte (length byte).
		case 3:
			// Attempt to create the packet based on the type word.  If failed, zero the count.
			if ( (uint8_t)input[index] < CMDPKT_MAX_SIZE || input[index] == 0 )
			{
				m_Packet[m_PacketLength++] = input[index];
			}
			// If successful, increment the count.
			else
			{
				// Set error count and reset packet.
				Reset();
				*start = index;
				return CMDPKTERR_LENGTHERR;
			}
			break;
		}

		// Increment the count.
		index++;
	}

	// If there's data left, read it into the header, if possible.
	if ( index < len && m_PacketLength < (int)sizeof( EcCommandPacket::Header ) )
	{
		// Copy either the rest of the header, or the rest of the data into the buffer.
		amt = (len - index) < ((int)sizeof( EcCommandPacket::Header ) - m_PacketLength ) ?
      (len - index) : ((int)sizeof( EcCommandPacket::Header ) - m_PacketLength );
		memcpy( &m_Packet[m_PacketLength], &input[index], amt );

		// Increment the count and index by the correct amount.
		m_PacketLength += amt;
		index += amt;
	}

	// Otherwise, see if we need to read data into the data segment.
	if ( index < len && m_PacketLength < (int)m_Header->Length + (int)sizeof( EcCommandPacket::Header ) )
	{
		// Copy either the rest of the header, or the rest of the data into the buffer.
		amt = ( len - index ) < ((int)m_Header->Length + (int)sizeof( EcCommandPacket::Header )
      - (int)m_PacketLength ) ? ( len - index ) : ((int)m_Header->Length +
      (int)sizeof( EcCommandPacket::Header ) - (int)m_PacketLength );
		memcpy( &m_Packet[m_PacketLength], &input[index], amt );

		// Increment the count and index by the correct amount.
		m_PacketLength += amt;
		index += amt;

		// If we got the complete header, check the length.
		if ( m_PacketLength == (int)m_Header->Length + (int)sizeof( EcCommandPacket::Header ) )
		{
			if ( CheckChecksum() )
			{
				*start = index;
				m_Completed = true;
				return 0;
			}
			else 
			{
				// Set error count and reset packet.
				Reset();
				*start = index;
				return CMDPKTERR_CRCERR;
			}
		}
	}

	// Return the status.
	*start = index;
	return CMDPKTERR_INCOMPLETE;
}

//	ProcessInput:
///		Builds command packets from input data.
//
///		For a packet being built from an incoming byte stream, this routine processes
///		the input buffer to look for valid packets.
bool EcCommandPacket::ProcessInput( char *input, int *start, int length, int *const pErrCount )
{
	// Default return value.
	bool bval = false;
	int index = *start, len = length, amt;

	// Loop while there is data for start of data packet.
	while ( m_PacketLength < 4 && index < len )
	{
		// Process based on the byte count.
		switch ( m_PacketLength )
		{
		// Check first byte (low-order byte of SYNC word).
		case 0:
			// If it is the header value, increment the count.
			if ( input[index] == (char)SYNCVAL1 )
				m_Packet[m_PacketLength++] = input[index];
			break;

		// Check second byte (high-order byte of SYNC word).
		case 1:
			// If this byte matches upper byte of sync word, increment count.
			if ( input[index] == (char)SYNCVAL2 )
			{
				m_Packet[m_PacketLength++] = input[index];
			}
			// Otherwise, zero the count.
			else
			{
				*pErrCount = *pErrCount + 1;
				m_PacketLength = 0;
			}
			break;

		// Check third byte (target computer).
		case 2:
			// Put byte into temporary storage.
			m_Packet[m_PacketLength++] = input[index];

			break;

		// Check fourth byte (length byte).
		case 3:
			// Attempt to create the packet based on the type word.  If failed, zero the count.
			if ( (uint8_t)input[index] < CMDPKT_MAX_SIZE )
			{
				m_Packet[m_PacketLength++] = input[index];
			}
			// If successful, increment the count.
			else
			{
				// Set error count and reset packet.
				*pErrCount = *pErrCount + 1;
				Reset();
			}
			break;
		}

		// Increment the count.
		index++;
	}

	// If there's data left, read it into the header, if possible.
	if ( index < len && m_PacketLength < (int)sizeof( EcCommandPacket::Header ) )
	{
		// Copy either the rest of the header, or the rest of the data into the buffer.
		amt = (len - index) < ((int)sizeof( EcCommandPacket::Header ) - m_PacketLength) ?
      (len - index) : ((int)sizeof( EcCommandPacket::Header ) - m_PacketLength);
		memcpy( &m_Packet[m_PacketLength], &input[index], amt );

		// Increment the count and index by the correct amount.
		m_PacketLength += amt;
		index += amt;
	}

	// Otherwise, see if we need to read data into the data segment.
	if ( index < len && m_PacketLength < (int)m_Header->Length + (int)sizeof( EcCommandPacket::Header ) )
	{
		// Copy either the rest of the header, or the rest of the data into the buffer.
		amt = (len - index) < ((int)m_Header->Length + (int)sizeof( EcCommandPacket::Header )
     - (int)m_PacketLength) ? (len - index) : ((int)m_Header->Length +
     (int)sizeof( EcCommandPacket::Header ) - (int)m_PacketLength);
		memcpy( &m_Packet[m_PacketLength], &input[index], amt );

		// Increment the count and index by the correct amount.
		m_PacketLength += amt;
		index += amt;

		// If we got the complete header, check the length.
		if ( m_PacketLength == (int)m_Header->Length + (int)sizeof( EcCommandPacket::Header ) )
		{
			if ( CheckChecksum() )
			{
				bval = true;
				m_Completed = true;
			}
			else 
			{
				// Set error count and reset packet.
				*pErrCount = *pErrCount + 1;
				Reset();
			}
		}
	}

	// Return the status.
	return bval;
}

//	SetChecksum:
///		Calculates the 16-bit CRC checksum for the data and places the checksum in the packet header.
void EcCommandPacket::SetChecksum()
{
	// Make sure we have something to work with.
	if ( m_Packet == NULL )
		return;

	// Call the base class calculator.
	m_Header->CheckSum = 0;
	m_Header->CheckSum = EcPacket::CalcCRC16();
}

//	CheckChecksum:
///		Checks the checksum in packet with that calculated from the data.
//
///		Calculates the 16-bit CRC checksum for the data and compares it to the checksum
///		in the packet header.  Returns FALSE if they do not match.
bool EcCommandPacket::CheckChecksum()
{
	// Make sure we have something to work with.
	if ( m_Packet == NULL )
		return false;

	// Save value from packet.
	uint16_t Check1 = m_Header->CheckSum;

	// Calculate the new value.
	m_Header->CheckSum = 0;
	uint16_t Check2 = EcPacket::CalcCRC16();

	// Replace the original checksum.
	m_Header->CheckSum = Check1;

	// Set the return value TRUE if the two are equal, false otherwise.
	return (Check1 == Check2);
}

//	GetChecksum:
///		Returns the checksum value stored in the packet.
uint16_t EcCommandPacket::GetChecksum()
{
	// If there is no packet, return 0.
	if ( m_Packet == NULL )
		return 0;

	// Otherwise, return the check sum.
	return m_Header->CheckSum;
}

//	GetDataByteLength:
///		Gets the length of the data segment (in bytes). 
//
///		Returns 0 if the packet isn't valid.
int EcCommandPacket::GetDataByteLength()
{
	if ( !m_Completed )
		return 0;
	else
		return m_Header->Length;
}

//	GetDataWordLength:
///		Gets the length of the data segment (in uint16_ts).
//
///		Returns 0 if the packet isn't valid.
int EcCommandPacket::GetDataWordLength()
{
	if ( !m_Completed )
		return 0;
	else
		return m_Header->Length/sizeof(uint16_t);
}

//	GetSequenceNumber:
///		Returns the first word of the data array, which should be the sequence number.
uint16_t EcCommandPacket::GetSequenceNumber()
{
	// Make sure there's data.
	if ( m_PacketLength < (int)sizeof( EcCommandPacket::Header ) )
		return 0;

	// Return the first data value.
	return m_Header->PktID;
}

//	GetCpuId:
///		Returns the CPU ID portion of the CIP packet ID/Length word.
int EcCommandPacket::GetCpuId()
{
	// Define the return value default.
	int rval = 0;

	// Only process the CPU ID for completed packets.
	if ( m_Completed )
	{
		rval = (int)(m_Header->Target);
	}
	
	// Return the value.
	return rval;
}

//	Send:
///		Sends the packet on an EcSocketServer stream.
int EcCommandPacket::Send( EcComStream *pStream )
{
	// Only send completed packets.
	if ( m_Completed )
	{
		// Return with the stream return value.
		int length = m_PacketLength;
		return pStream->Send( (char *)m_Packet, &length );
	}

	// Otherwise, return incomplete.
	return CMDPKTERR_INCOMPLETE;
}

//	SendOutputPacket:
///		Prepares and sends a packet for output on a stream.
//
///		Sets the packet length and packet ID, calculates the CRC value and sets it, then
///		sends the packet on the output socket.
void EcCommandPacket::SendOutputPacket( EcComStream *pStream )
{
	m_PacketLength = m_Header->Length + sizeof( EcCommandPacket::Header );
	SetChecksum();

	// Send the packet.
	int length = m_PacketLength;
	if ( m_Header->Length > 0 )
		pStream->Send( (char *)m_Packet, &length );
}

//  FinalizeOutputPacket:
///   Prepares a packet for output on a stream, but does not send the packet.
//
///   Sets the packet length and packet ID, calculates the CRC value and sets it.
void EcCommandPacket::FinalizeOutputPacket()
{
	m_PacketLength = m_Header->Length + sizeof( EcCommandPacket::Header );
	SetChecksum();
}

//	SetTargetID:
///		Sets the CPU ID passed by the user into local storage for use in transmitting.
void EcCommandPacket::SetTargetID( uint8_t TargetId )
{
	m_Header->Target = TargetId;
}
