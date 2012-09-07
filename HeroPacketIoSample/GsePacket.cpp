///	\file GsePacket.cpp
///		C++ source file defining the functions and behaviors of the class EcGsePacket,
//		a class used for data sent to a GSE, combining data with header, ID, and CRC
//		checksum.

#include "StdAfx.h"
#include "GsePacket.h"
#include <sys/types.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <string.h>
#include "ErrorCodes.h"

#define MAX 

////////////////////////////////////////////////////////////////////////////////
//	EcGsePacket class.
OffsetTimeCorrection* EcGsePacket::offTime = new OffsetTimeCorrection;
LinearTimeCorrection* EcGsePacket::linTime = new LinearTimeCorrection;

OffsetTimeCorrection* EcGsePacket::getOffsetTimeCorrection()
{
  return EcGsePacket::offTime;
}

LinearTimeCorrection* EcGsePacket::getLinearTimeCorrection()
{
  return EcGsePacket::linTime;
}

//	EcGsePacket:
///		Dumb default constructor.
//
EcGsePacket::EcGsePacket() : EcPacket()
{
	// Data hasn't been altered.
	m_Altered = true;

	// Create the data space.
	m_Packet = new char[sizeof( EcGsePacket::Header ) + GSEPKT_MAX_DATA_SIZE];
	m_Header = (EcGsePacket::Header *)m_Packet;
	m_Data = &m_Packet[sizeof( EcGsePacket::Header )];

	// Reset the packet.
	Reset();
}

//	~EcGsePacket:
///		Class destructor.
EcGsePacket::~EcGsePacket()
{
	delete m_Packet;
	m_Packet = NULL;
}

//	Create:
///		Initializes the class, if uninitialized.
//
///		This routine first calls VerifyType() to see if the requested packet type is defined,
///		If it is not, the routine immediately returns an error.  If the packet type is defined, the
///		routine sets the local length to the requested size, sets the local message to the correct
///		message.  In the packet structure, the sync value and the type are set, and the data length
///		is set to 0.
///		\return		0 if type is valid and packet is created, nonzero error code otherwise.
int EcGsePacket::Create( uint8_t type )
{
	// If the packet isn't reset, return FALSE.
	if ( !m_Reset )
		return HGSEPKTERR_NOTRESET;

	int rval;
	// If the type isn't valid, return FALSE.
	if ( (rval = VerifyType( type, &m_DataLength, &m_Message, &m_VariableLength )) )
		return rval;

	// Otherwise, set the sync value, the type, the data and packet length.
	m_Header->SyncVal = SYNCVAL;
	m_Header->Type = type;
	m_Header->DataLength = 0;
	m_Header->SrcID = CPU_NUM;
	m_PacketLength = sizeof( EcGsePacket::Header );

	m_Altered = true;
	m_Reset = false;

	// Return successfully.
	return 0;
}

//	VerifyType:
///		Verifies the type and returns the packet length and notification message, if any.
//
///		This is designed as a virtual function, so that the overriding classes can test for
///		valid types in this function, leaving the Create() routine defined in this base class.
///		The idea is that any overridden version will return 0 if the type is valid, and fill in the
///		two return parameters with valid information, otherwise, the function will return
///		an error.  But this version always sets the data length and the message to zero and returns
///		an error.
///		\param[in]		type			Type of GSE packet to create.
///		\param[out]		pDataLength		Pointer to the data length of the packet.
///		\param[out]		pMessage		Pointer to the notification message.
///		\param[out]		pVariableLength	Flag indicating if packet can vary in length less than max length.
///		\return			This version always returns HGSEPKTERR_TYPE.
int EcGsePacket::VerifyType( uint8_t type, int *pDataLength, uint32_t *pMessage, bool *pVariableLength )
{
	*pDataLength = 0;
	*pMessage = 0;
	*pVariableLength = false;

	return HGSEPKTERR_TYPE;
}

//	Reset:
///		Re-initializes the class so it can be created again.
void EcGsePacket::Reset()
{
	// Zero the packet length.
	m_PacketLength = 0;
	m_DataLength = 0;
	m_Header->Type = 0;
	m_Header->DataLength = 0;
	m_Header->SrcID = 0;

	// Turn off the altered data flag.
	m_Altered = false;

	// Set the reset flag.
	m_Reset = true;
}

//	SetWordData:
///		Puts a 16-bit data value into the data array.
int EcGsePacket::SetWordData( int index, uint16_t data )
{
	// Make sure we have something to work with.
	if ( m_Packet == NULL )
		return HGSEPKTERR_NOPKT;

	// Make sure the packet is not reset.
	if ( m_Reset )
		return HGSEPKTERR_RESET;

	// Protect array boundaries.
  index = index < (int)(m_DataLength/sizeof(uint16_t)) ? index :
    (int)(m_DataLength/sizeof(uint16_t));
  index = index > 0 ? index : 0;
  uint16_t *Data = (uint16_t *)m_Data;

	// Set the data value.
	Data[index] = data;
	m_Header->DataLength = index * sizeof( uint16_t ) + 1;

	// Set data altered flag to true.
	m_Altered = true;

	// Return without error.
	return 0;
}

//	SetArray:
///		Puts an array of 8-bit data values into the data array.
//
///		Rather than check that there is sufficient space for the array transfer, this routine
///		will truncate the start point to the maximum data length allowed for the given packet type,
///		and then will truncate the length transferred based on the start position and the maximum
///		length.  The passed variable length will be altered to reflect the actual amount of data
///		transferred to the packet.
///		\param[in]		pData			Array of characters to add to packet.
///		\param[in,out]	length			[IN] Requested number of bytes to add to packet. [OUT] Actual number added.
///		\param[in]		start			Optional parameter for starting byte position in packet (default=0).
///		\return			Returns 0 if the packet exists and is not reset; otherwise, a non-zero error message.	
int EcGsePacket::SetArray( char *pData, int *length, int start )
{
	// Make sure we have something to work with.
	if ( m_Packet == NULL )
		return HGSEPKTERR_NOPKT;

	// Make sure the packet is not reset.
	if ( m_Reset )
		return HGSEPKTERR_RESET;

	// Protect array boundaries.
  start = start > 0 ? start : 0;
  start = start < (int)m_DataLength ? start : (int)m_DataLength;
  *length = *length > 0 ? *length : 0;
  *length = *length < ((int)m_DataLength - start) ? *length : ((int)m_DataLength - start);

	// Copy the data.
	memcpy( &m_Data[start], pData, *length );
	m_Header->DataLength = start + *length;

	// Set data altered flag to true.
	m_Altered = true;

	// Return without error.
	return 0;
}

//	SetDataLength:
///		Sets the data length of the array in the packet header, if allowed.
int EcGsePacket::SetDataLength( uint16_t length )
{
	// If setting length is allowable, do so.
	if ( m_VariableLength )
	{
		// Set the length within bounds.
		length = m_DataLength < length ? m_DataLength : length;
		m_Header->DataLength = length;
		m_Altered = true;
		return 0;
	}

	// Otherwise return an error.
	return HGSEPKTERR_OPTYPE;
}

//	SetPacketTime:
///		Get the current system time and sets the time words in the packet.
int EcGsePacket::SetPacketTime()
{
	// Make sure we have something to work with.
	if ( m_Packet == NULL )
		return HGSEPKTERR_NOPKT;

	// Make sure the packet is not reset.
	if ( m_Reset )
		return HGSEPKTERR_RESET;

	// Get the system time.
	timespec time, otime;
	clock_gettime( CLOCK_REALTIME, &time );
  EcGsePacket::linTime->correctTime( time, otime );
	m_Header->Time[0] = otime.tv_nsec;
	m_Header->Time[1] = otime.tv_sec;

	// Set the altered flag.
	m_Altered = true;

	// Return without error.
	return 0;
}

//	SetPacketTime:
///		Sets the time data in the packet.
int EcGsePacket::SetPacketTime( const timespec* pTimestamp )
{
	// Make sure we have something to work with.
	if ( m_Packet == NULL )
		return HGSEPKTERR_NOPKT;

	// Make sure the packet is not reset.
	if ( m_Reset )
		return HGSEPKTERR_RESET;

	// Get the system time.
  timespec otime;
  EcGsePacket::linTime->correctTime( (struct timespec&)*pTimestamp, otime );
	m_Header->Time[0] = otime.tv_nsec;
	m_Header->Time[1] = otime.tv_sec;

	// Set the altered flag.
	m_Altered = true;

	// Return without error.
	return 0;
}

//	SetPacketTime:
///		Get the current system time and sets the time words in the packet.
int EcGsePacket::SetPacketTime( time_t tSeconds, uint32_t tMicrosec )
{
	// Make sure we have something to work with.
	if ( m_Packet == NULL )
		return HGSEPKTERR_NOPKT;

	// Make sure the packet is not reset.
	if ( m_Reset )
		return HGSEPKTERR_RESET;

	// Get the system time.
  timespec time, otime;
  time.tv_sec = tSeconds;
  time.tv_nsec = tMicrosec * 1000;
  EcGsePacket::linTime->correctTime( time, otime );
	m_Header->Time[0] = otime.tv_nsec;
	m_Header->Time[1] = otime.tv_sec;

	// Set the altered flag.
	m_Altered = true;

	// Return without error.
	return 0;
}

//	SetSrcID:
///		Sets the source ID in the header.
//
///		This routine does not check the input byte for validity, although this can be overridden
///		by derived classes.  It installs the ID byte into the header SrcID, and sets the ALTERED
///		flag which will force a recalculation of the checksum on send.
///		\param[in]		SrcID		Identifier of the system sending the packet.
///		\return		0 if packet exists, is not reset, nonzero error message otherwise. 
int EcGsePacket::SetSrcID( uint8_t SrcID )
{
	// Make sure we have something to work with.
	if ( m_Packet == NULL )
		return HGSEPKTERR_NOPKT;

	// Make sure the packet is not reset.
	if ( m_Reset )
		return HGSEPKTERR_RESET;

	m_Header->SrcID = SrcID;
	m_Altered = true;

	// Return without error.
	return 0;
}

//	GetSrcID:
///		Gets the source ID in the header.
//
///		This copies the ID byte from the header SrcID, returning a status value indicating if the
///		packet actually contains header data.
///		\param[out]		pSrcID		Pointer to identifier of the system sending the packet.
///		\return		0 if packet exists, is not reset, nonzero error message otherwise. 
int EcGsePacket::GetSrcID( uint8_t *pSrcID )
{
	// Make sure we have something to work with.
	if ( m_Packet == NULL )
		return HGSEPKTERR_NOPKT;

	// Make sure the packet is not reset.
	if ( m_PacketLength < (int)sizeof( EcGsePacket::Header ) )
		return HGSEPKTERR_INCOMPLETE;

	// Get the value.
	*pSrcID = m_Header->SrcID;

	// Return without error.
	return 0;
}

//	GetType:
///		Returns the type word.
uint16_t EcGsePacket::GetType()
{
	// Return type word.
	return m_Header->Type;
}

//	GetPacketLength:
///		Returns the length of the packet in bytes.
int EcGsePacket::GetPacketLength()
{
	// Return the data length.
	return m_PacketLength;
}

//	GetDataByteLength:
///		Returns the length of the data segment of the packet in bytes.
int EcGsePacket::GetDataByteLength()
{
	// Return the actual length word, as some packets set this to an odd value
	//	(e.g. GPS packets).
	return m_Header->DataLength;
}

//	GetDataWordLength:
///		Returns the length of the data segment of the packet in words.
int EcGsePacket::GetDataWordLength()
{
	// Return the actual length word, as some packets set this to an odd value
	//	(e.g. GPS packets).
	return (m_Header->DataLength+1)/sizeof(uint16_t);
}

//	GetPacketTime:
///		Returns the time from the packet as a LONGLONG object.
bool EcGsePacket::GetPacketTime( timespec *pFileTime )
{
	// Get the time from the packet.
	pFileTime->tv_nsec = m_Header->Time[0];
    pFileTime->tv_sec = m_Header->Time[1];

	// Return true.
	return true;
}

//	GetPacketTime:
///		Returns the time from the packet as two ULONG objects.
int EcGsePacket::GetPacketTime( time_t *const pTSec, uint32_t *const pTNanoSec )
{
	// Get the time from the packet.
	*pTSec = m_Header->Time[1];
	*pTNanoSec = m_Header->Time[0];

	// Return true.
	return 0;
}

//	GetPacketSeconds:
///		Returns a double value filled only with the integral seconds.
double EcGsePacket::GetPacketSeconds()
{
	// Convert the packet time to seconds.
	double dval = (double)m_Header->Time[1] + (double)m_Header->Time[0] / 1000000000.0;

	// Return the time.
	return dval;
}

//	GetData:
///		Gets a data value from the data array.
unsigned short EcGsePacket::GetWordData( int index )
{
	// Constrain the bounds of the index.
  index = index < (int)(m_Header->DataLength/sizeof(uint16_t) - 1) ? index :
    (int)(m_Header->DataLength/sizeof(uint16_t) - 1);
  index = index > 0 ? index : 0;

	// Return the value.
	uint16_t *Data = (uint16_t *)m_Data;
	return Data[index];
}

//	GetPointerToData:
///		Returns a pointer to the data segment of the packet.
void *EcGsePacket::GetPointerToData()
{
	return (void *)&m_Data[0];
}

//	GetPointerToPacket:
///		Returns a pointer to the packet.
void *EcGsePacket::GetPointerToPacket()
{
	return (void *)&m_Packet[0];
}

//	GetCRCVal:
///		Returns the CRC value.
uint16_t EcGsePacket::GetCRCVal()
{
	// Return CRC word.
	return m_Header->CheckSum;
}

//	SetChecksum:
///		Calculates the 16-bit CRC checksum for the data and places the checksum in the packet header.
void EcGsePacket::SetChecksum()
{
	// Make sure we have something to work with.
	if ( m_Packet == NULL )
		return;

	// Return if not altered since last CRC16.
	if ( !m_Altered )
		return;

	// Call the base class calculator.
	m_Header->CheckSum = 0;
	m_Header->CheckSum = EcPacket::CalcCRC16();

	// Turn off alteration flag.
	m_Altered = false;
}

//	CheckChecksum:
///		Compares the packet checksum with the calculated checksum.
//
///		Calculates the 16-bit CRC checksum for the data and compares it to the checksum
///		in the packet header.  Returns FALSE if the two do not match.
bool EcGsePacket::CheckChecksum()
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

//	Send:
///		Checks and sends a packet on a communications stream.
//
///		This version assumes the packet has been completed by the user, and does not
///		set the time, the packet length, or the checksum.  It does, however, check that
///		the length and checksum are valid before sending.
int EcGsePacket::Send( EcComStream *pStream )
{
	// Do nothing if the packet isn't valid.
	if ( m_Packet == NULL )
		return HGSEPKTERR_NOPKT;

	// Make sure the packet length reflects the current length.
	if ( m_PacketLength != m_Header->DataLength + (int)sizeof( EcGsePacket::Header ) )
		return HGSEPKTERR_INCOMPLETE;

	// Make sure actual packet length is acceptable for this type.
	if ( !m_VariableLength && m_Header->DataLength != m_DataLength )
		return HGSEPKTERR_LENGTH;

	// Make sure the checksum is valid.
	if ( !CheckChecksum() )
		return HGSEPKTERR_CRC;

	// Call the base class function.
	return EcPacket::SendPacket( pStream );
}

//	SendPacket:
///		Prepares and sends the packet on a communications stream.
//
///		Sets current time as packet time, calculates the CRC16 value and sends the
///		formatted (i.e. with header, packet type, data, and CRC value) data packet on
///		the given stream.
int EcGsePacket::SendPacket( EcComStream *pStream, int timeout )
{
	// Do nothing if the packet isn't valid.
	if ( m_Packet == NULL )
		return -1;

	// Make sure the packet length reflects the current length.
	m_PacketLength = m_Header->DataLength + sizeof( EcGsePacket::Header );

	// Set the time in the packet.
	SetPacketTime();

	// Calculate the CRC checksum and set it into the packet.
	SetChecksum();

	// Call the base class function.
	return EcPacket::SendPacket( pStream, timeout );

}

//	SendPacket:
///		Prepares and sends the packet on a communications stream.
//
///		Sets packet time, calculates the CRC16 value and sends the formatted (i.e. with
///		header, packet type, data, and CRC value) data packet on the given stream.
int EcGsePacket::SendPacket( const timespec* pTimestamp, EcComStream *pStream )
{
	// Do nothing if the packet isn't valid.
	if ( m_Packet == NULL )
		return -1;

	// Make sure the packet length reflects the current length.
	m_PacketLength = m_Header->DataLength + sizeof( EcGsePacket::Header );

	// Set the time in the packet.
	SetPacketTime( pTimestamp );

	// Calculate the CRC checksum.
	SetChecksum();

	// Call the base class function.
	return EcPacket::SendPacket( pStream );
}

//	SendPacket:
///		Prepares and sends the packet on a communications stream.
//
///		Sets packet time, calculates the CRC16 value and sends the formatted (i.e. with
///		header, packet type, data, and CRC value) data packet on the given port.
int EcGsePacket::SendPacket( time_t tSeconds, uint32_t tMicrosec, EcComStream *pStream )
{
	// Do nothing if the packet isn't valid.
	if ( m_Packet == NULL )
		return -1;

	// Make sure the packet length reflects the current length.
	m_PacketLength = m_Header->DataLength + sizeof( EcGsePacket::Header );

	// Set the time in the packet.
	SetPacketTime( tSeconds, tMicrosec );

	// Calculate the CRC checksum.
	SetChecksum();

	// Call the base class function.
	return EcPacket::SendPacket( pStream );
}

//	ProcessByte:
///		Processes incoming bytes to build a packet.
//
///		For a packet being built from an incoming byte stream, this routine processes
///		each byte as it comes, first testing for the sync word, then testing for a valid
///		type.
bool EcGsePacket::ProcessByte( char byte, int *const pErrCount )
{
	// Default return value.
	bool bval = false;
	uint16_t Check1, Check2;

	// Cast the packet to a char pointer.
	char *pPacket = (char *)m_Packet;

	// Process based on the byte count.
	switch ( m_Count )
	{
	// Check first byte (low-order byte of SYNC word).
	case 0:
		// If it is the header value, increment the count.
		if ( byte == (char)SYNCVAL1 )
		{
			m_Count++;
		}
		break;

	// Check second byte (high-order byte of SYNC word).
	case 1:
		// If this byte matches upper byte of sync word, increment count.
		if ( byte == (char)SYNCVAL2 )
		{
			m_Count++;
		}
		// Otherwise, zero the count.
		else
		{
			*pErrCount = *pErrCount + 1;
			m_Count = 0;
		}
		break;

	// Check third byte (packet TYPE).
	case 2:
		// Attempt to create the packet based on the type word.  If successful, increment count.
		if ( Create( byte ) )
		{
			m_Count++;
		}
		// Otherwise, handle the error.
		else
		{
			// Set error count and reset packet.
			*pErrCount = *pErrCount + 1;
			bval = HandleError( HGSEPKTERR_TYPE, byte );
		}
		break;

	// Check the sixth byte (high-order byte of LENGTH word).
	case 5:
		// Store the value at the current position.
		pPacket[m_Count] = byte;

		// Check that the length is within bounds.
		if ( m_Header->DataLength <= (m_DataLength * sizeof(uint16_t)))
		{
			// Set the data length and packet length.
			m_DataLength = m_Header->DataLength;
			m_PacketLength = m_DataLength + sizeof( EcGsePacket::Header );

			// Increment the count.
			m_Count++;
		}
		// Otherwise, re-initialize the packet.
		else
		{
			// Set the error.
			*pErrCount = *pErrCount + 1;

			bval = HandleError( HGSEPKTERR_LENGTH, m_Header->Type, m_Header->DataLength,
				m_DataLength * sizeof(uint16_t) );
		}
		break;

	// Store all other values in their place in the array.
	default:
		// Make sure we're within the packet length.
		if ( m_Count < (int)(m_PacketLength * sizeof(uint16_t)) )
		{
			// Cast the packet to a char pointer.
			char *pPacket = (char *)m_Packet;

			// Store the value at the current position.
			pPacket[m_Count] = byte;

			// Increment the count.
			m_Count++;

			// Test if we're at the end of the packet.
			if ( m_Count == (int)(m_PacketLength * sizeof(uint16_t)) ) {
				// If CRC value matches the one sent, return TRUE.
				if ( CheckChecksum() )
				{
					bval = true;
				}
				else
				{
					// Set the error.
					*pErrCount = *pErrCount + 1;

					// Save value from packet.
					Check1 = m_Header->CheckSum;

					// Calculate the new value.
					m_Header->CheckSum = 0;
					Check2 = EcPacket::CalcCRC16();

					bval = HandleError( HGSEPKTERR_CRC, m_Header->Type, Check1, Check2  );
				}
			}
		}
	}

	// Return the status.
	return bval;
}

//	ProcessInput:
///		Processes input data to form complete packets.
//
///		For a packet being built from an incoming byte stream, this routine processes
///		the input buffer to look for valid packets.  It checks for a valid synch word,
///		a valid type, a valid length, and a checksum that matches the CRC-16 value
///		calculated for the data.
bool EcGsePacket::ProcessInput( char *input, int *start, int length, int *const pErrCount )
{
	// Default return value.
	bool bval = false;
	uint16_t Check1, Check2;
	int index = *start, len = length, amt;

	// Loop while there is data for start of data packet.
	while ( m_PacketLength < 3 && index < len )
	{
		// Process based on the byte count.
		switch ( m_PacketLength )
		{
		// Check first byte (low-order byte of SYNC word).
		case 0:
			// If it is the header value, increment the count.
			if ( input[index] == (char)SYNCVAL1 )
			{
				m_Packet[m_PacketLength++] = input[index];
			}
			break;

		// Check second byte (high-order byte of SYNC word).
		case 1:
			// If this byte matches upper byte of sync word, increment count.
			if ( input[index] == (char)SYNCVAL2 )
			{
				m_Packet[m_PacketLength++] = input[index];
			}
			// Otherwise, zero the count.
			else {
				*pErrCount = *pErrCount + 1;
				m_PacketLength = 0;
			}
			break;

		// Check third byte (packet type).
		case 2:
			// Verify the type word.  If failed, zero the count.
			if ( !VerifyType( input[index], &m_DataLength, &m_Message, &m_VariableLength ) )
			{
				m_Packet[m_PacketLength++] = input[index];
			}
			// If successful, increment the count.
			else
			{
				// Set error count and reset packet.
				*pErrCount = *pErrCount + 1;
				bval = HandleError( HGSEPKTERR_TYPE, input[index] );
			}
			break;
		}

		// Increment the count.
		index++;
	}

	// If there's data left, read it into the header, if possible.
	if ( index < len && m_PacketLength < (int)sizeof( EcGsePacket::Header ) )
	{
		// Copy either the rest of the header, or the rest of the data into the buffer.
		amt = (len - index) < ((int)sizeof( EcGsePacket::Header ) - m_PacketLength ) ?
      (len - index) : ((int)sizeof( EcGsePacket::Header ) - m_PacketLength );
		memcpy( &m_Packet[m_PacketLength], &input[index], amt );

		// Increment the count and index by the correct amount.
		m_PacketLength += amt;
		index += amt;

		// If we got the complete header, check the length.
		if ( m_PacketLength == sizeof( EcGsePacket::Header ) )
		{
			// If the length is wrong, handle the error.
			if ( m_Header->DataLength > m_DataLength || 
				(!m_VariableLength && m_Header->DataLength < m_DataLength) )
			{
				// Set error count and reset packet.
				*pErrCount = *pErrCount + 1;
				bval = HandleError( HGSEPKTERR_LENGTH, m_Header->Type, m_Header->DataLength, m_DataLength );
			}
			// Otherwise, set the number of bytes to read in.
			else
			{
				m_Count = m_Header->DataLength;

				// Handle zero data length error.
				if ( m_Count == 0 )
				{
					bval = HandleError( HGSEPKTERR_ZEROLENGTH, m_Header->Type );
				}
			}
		}
	}

	// Otherwise, see if we need to read data into the data segment.
	if ( index < len && m_Count > 0 )
	{
		// Copy either the rest of the header, or the rest of the data into the buffer.
		amt = (len - index) < m_Count ? (len - index) : m_Count;
		memcpy( &m_Packet[m_PacketLength], &input[index], amt );

		// Increment the count and index by the correct amount.
		m_PacketLength += amt;
		m_Count -= amt;
		index += amt;

		// If we got the complete header, check the length.
		if ( m_PacketLength == m_Header->DataLength + (int)sizeof( EcGsePacket::Header ) )
		{
			if ( CheckChecksum() )
			{
				bval = true;
			}
			else
			{
				// Set error count and reset packet.
				*pErrCount = *pErrCount + 1;

				// Save value from packet.
				Check1 = m_Header->CheckSum;

				// Calculate the new value.
				m_Header->CheckSum = 0;
				Check2 = EcPacket::CalcCRC16();

				bval = HandleError( HGSEPKTERR_CRC, m_Header->Type, Check1, Check2  );
			}
		}
	}

	// Set the start value.
	*start = index;

	// Return the status.
	return bval;
}

//	VerifyBuffer:
///		Verifies that a buffer contains one single valid packet.
//
///		For a packets coming in on a reliable stream, this routine processes
///		the input buffer to look for valid packets.  It checks for a valid synch word,
///		a valid type, a valid length, and a checksum that matches the CRC-16 value
///		calculated for the data.
///		\param[in]		input		The input buffer of data.
///		\param[in]		length		The length (in bytes) of the input buffer.
///		\return			Returns a non-zero error value if the buffer is too short to hold
///						a GSE packet header, or if the sync value is not valid, or if the
///						type is not valid, or if the length of the buffer doesn't match the
///						calculated packet length, or if the checksum isn't valid.
///						Otherwise, returns zero to indicate success.
int EcGsePacket::VerifyBuffer( char *input, int length )
{
	// Default return value.
	int rval = 0;
	uint16_t Check1, Check2;

	// If there isn't enough room for a header, return with an error.
	if ( length < (int)sizeof( EcGsePacket::Header ) )
		return HGSEPKTERR_SHORT;

	// Cast the pointer as a packet header.
	EcGsePacket::Header *pHdr = (EcGsePacket::Header *)input;

	// If it is the header value, increment the count.
	if ( pHdr->SyncVal != SYNCVAL )
		return HGSEPKTERR_SYNCHERR;

	// Check the type, return any errors.
	int datLength;
	uint32_t msg;
	bool varLength;
	if ( (rval = VerifyType( pHdr->Type, &datLength, &msg, &varLength )) )
		return rval;

	// If the length is not appropriate for this type, return an error.
	if ( pHdr->DataLength > datLength ||
		(!varLength && pHdr->DataLength < datLength) )
		return HGSEPKTERR_LENGTH;

	// If the length doesn't match the user-supplied length, return an error.
	if ( (pHdr->DataLength + (int)sizeof( EcGsePacket::Header )) != length )
		return HGSEPKTERR_BUFFLENGTH;

	// Now, swap this buffer in for the packet buffer and length.
	Check1 = pHdr->CheckSum;
	pHdr->CheckSum = 0;
	m_PacketLength = length;
	char *pPacket = m_Packet;
	m_Packet = input;

	// Calculate the packet buffer CRC16 checksum.
	Check2 = EcPacket::CalcCRC16();

	// Reset the packet data.
	pHdr->CheckSum = Check1;
	m_Packet = pPacket;

	// Now, if the checksums don't match, return an error.
	if ( Check1 != Check2 )
		return HGSEPKTERR_CRC;

	// If we get to this point, the buffer is a valid packet.  Return without error.
	return 0;
}
