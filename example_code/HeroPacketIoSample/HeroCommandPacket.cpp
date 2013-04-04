//	HeroCommandPacket.cpp:
///		\file HeroCommandPacket.cpp
///		C++ source file defining the behavior of the HeroCommandPacket class.
//		This class defines the custom commands for the Ampr project, as well as special
//		command handling routines.

//#include <algorithm>
//using namespace std;

#include "StdAfx.h"
#include <stdio.h>
#include <string.h>

#include "HeroCommandPacket.h"
#include "ErrorCodes.h"

//	HeroCommandPacket:
///		Default constructor.
HeroCommandPacket::HeroCommandPacket() : EcCommandPacket()
{
}

//	~HeroCommandPacket:
///		Destructor.
HeroCommandPacket::~HeroCommandPacket()
{
}

//////////////////////////////////////////////////////////////////////////////////////
//	Commands for general use.

//	SetStowPinCmd:
///		Sets the correct command word (to send stow pin in or out) into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\param[in]		StowPinIn			Flag indicating direction to send stow pin.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetStowPinCmd( bool StowPinIn )
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( StowPinIn )
			m_Data[m_WriteCount++] = CIP_STOW_IN;
		else
			m_Data[m_WriteCount++] = CIP_STOW_OUT;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetGpsTimingCmd:
///		Sets the correct command word (to enable or disable GPS as time source) into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\param[in]		UseGps			Flag indicating whether to user GPS as timing source or not.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetGpsTimingCmd( bool UseGps )
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( UseGps )
			m_Data[m_WriteCount++] = CIP_GPSTIME_ON;
		else
			m_Data[m_WriteCount++] = CIP_GPSTIME_OFF;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetGpsResetCmd:
///		Sets the GPS reset command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetGpsResetCmd()
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_GPS_RESET;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetRelayBitsCmd:
///		Sets the relay bit set command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		RelayBits		16-bit word with relay bit pattern.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetRelayBitsCmd( uint16_t RelayBits )
{
	int length = sizeof( uint16_t ) * 2;
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_RELAY_SETBITS;
		m_Data[m_WriteCount++] = RelayBits;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetRelayBitsData:
///		Gets the data from the relay bit set command from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct data from the packet.
///		\param[out]		pRelayBits		16-bit word with relay bit pattern.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetRelayBitsData( uint16_t* pRelayBits )
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pRelayBits = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetGeoSrcCmd:
///		Sets the geographic position source command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Src			Source of data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetGeoSrcCmd( GEOPOS_SRC Src )
{
	// Set the command and data length.
	int length = sizeof( uint16_t );

	// Verify the packet and set the commands.
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		switch ( Src )
		{
		case GEOSRC_FDR_GPS:
			m_Data[m_WriteCount++] = CIP_GEOSRC_FDRGPS;
			break;

		case GEOSRC_CTL_GPS:
			m_Data[m_WriteCount++] = CIP_GEOSRC_CTLGPS;
			break;

		case GEOSRC_DGPS:
			m_Data[m_WriteCount++] = CIP_GEOSRC_DGPS;
			break;

		case GEOSRC_MANUAL:
			m_Data[m_WriteCount++] = CIP_GEOSRC_MANUAL;
			break;
		}

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetGeoPosCmd:
///		Sets the geographic position command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Src			Source of data.
///		\param[in]		Lat			Latitude from source.
///		\param[in]		Lon			Longitude from source.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetGeoPosCmd( GEOPOS_SRC Src, float Lat, float Lon )
{
	int length = sizeof( uint16_t ) + sizeof( float ) * 2;
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( Src == GEOSRC_FDR_GPS )
			m_Data[m_WriteCount++] = CIP_GEOPOS_GPS;
		else if ( Src == GEOSRC_CTL_GPS )
		  m_Data[m_WriteCount++] = CIP_GEOPOS_CTLGPS;
		else if ( Src == GEOSRC_DGPS )
			m_Data[m_WriteCount++] = CIP_GEOPOS_DGPS;
		else
			m_Data[m_WriteCount++] = CIP_GEOPOS_MANUAL;

		// Set the data.
		WriteFloat( Lat );
		WriteFloat( Lon );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetGeoPosData:
///		Gets the geographic position command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct data from the packet.
///		\param[out]		pLat			Latitude from source.
///		\param[out]		pLon			Longitude from source.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetGeoPosData( float *pLat, float *pLon )
{
	int length = sizeof( float ) * 2;
	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pLat = ReadFloat();
		*pLon = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetDefaultSaveCmd:
///		Sets the save current values as defaults command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDefaultSaveCmd()
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_SAVE_DEFAULTS;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}
	// Return status.

	return rval;
}

//	SetDefaultRestoreCmd:
///		Sets the restore current values from saved defaults command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDefaultRestoreCmd()
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_LOAD_DEFAULTS;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	CTL specific commands.

//	SetDgpsResetCmd:
///		Sets the DGPS reset command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDgpsResetCmd()
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_RESET_DGPS;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

/**
 *	Sets the magnetometer forwarding command and data into the packet.
 *
 *	This routine first checks that adequate space is available in the packet,
 *	then inserts the correct command word and data into the packet.
 *	\param[in]	fwdHeading		Flag forwarding mag heading packet to ground.
 *	\param[in]	fwdNonHeading	Flag forwarding mag non-heading packet to ground.
 *	\return			If the packet exists and has room, this routine returns 0, otherwise
 *  						it returns a non-zero error code.
 */
int HeroCommandPacket::SetMagForwarding(bool fwdHeading, bool fwdNonHeading)
{
	int length = sizeof( uint16_t ) * 2;
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_RESET_DGPS;

		// Set the data.
		uint16_t val = 0;
		val |= fwdHeading ? 1 : 0;
		val |= fwdNonHeading ? 2 : 0;
		m_Data[m_WriteCount++] = val;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

/**
 *	Gets the magnetometer forwarding data from the packet.
 *
 *	This routine first checks that adequate space is available in the packet,
 *	then inserts the correct command word and data into the packet.
 *	\param[out]	fwdHeading		Flag forwarding mag heading packet to ground.
 *	\param[out]	fwdNonHeading	Flag forwarding mag non-heading packet to ground.
 *	\return			If the packet exists and has room, this routine returns 0, otherwise
 *  						it returns a non-zero error code.
 */
int HeroCommandPacket::GetMagForwarding(bool &fwdHeading, bool &fwdNonHeading)
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Set the command.
		uint16_t val = m_Data[m_ReadCount++];
		fwdHeading = (val & 0x0001) > 0;
		fwdNonHeading = (val & 0x0002) > 0;
	}

	// Return status.
	return rval;
}

/**
 *	Sets a raw magnetometer command and data into the packet.
 *
 *	This routine first checks that adequate space is available in the packet,
 *	then inserts the correct command word and data into the packet.
 *	\param[in]	cmdBuff		Buffer holding command string.
 *	\param[in]	cmdLen		Number of bytes to write (must be <= 40).
 *	\return			If the packet exists and has room, this routine returns 0, otherwise
 *  						it returns a non-zero error code.
 */
int HeroCommandPacket::SetMagRawCommand(char *cmdBuff, int &cmdLen)
{
	int length = sizeof( uint16_t ) + MAGCMD_MAXLEN;
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_RESET_DGPS;

		// Get a pointer to the current position.
		char *ptr = (char *)m_Data[m_WriteCount];
		m_WriteCount += MAGCMD_MAXLEN;
		memset( ptr, 0, MAGCMD_MAXLEN );
		cmdLen = cmdLen < MAGCMD_MAXLEN ? cmdLen : MAGCMD_MAXLEN;
		memcpy( ptr, cmdBuff, cmdLen );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

/**
 *	Gets a raw magnetometer data string from the packet.
 *
 *	This routine first checks that adequate space is available in the packet,
 *	then read the correct  data from the packet.
 *	\param[in]	cmdBuff		Buffer holding command string.
 *	\param[in,out]	cmdLen		Number of bytes to read (must be <= 40).
 *	\return			If the packet exists and has room, this routine returns 0, otherwise
 *  						it returns a non-zero error code.
 */
int HeroCommandPacket::GetMagRawCommand(char *cmdBuff, int &cmdLen)
{
	int length = MAGCMD_MAXLEN;
	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get a pointer to the current position.
		char *ptr = (char *)m_Data[m_ReadCount];
		memset( cmdBuff, 0, MAGCMD_MAXLEN );
		cmdLen = cmdLen < MAGCMD_MAXLEN ? cmdLen : MAGCMD_MAXLEN;
		memcpy( cmdBuff, ptr, cmdLen );
		m_ReadCount += MAGCMD_MAXLEN;
	}

	// Return status.
	return rval;
}

//	SetNoPointCmd:
///		Sets the no pointing command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetNoPointCmd()
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_NO_POINT;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetSafePointCmd:
///		Sets the safe pointing command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetSafePointCmd()
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_SAFE_POINT;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetStowPointCmd:
///		Sets the stow pointing command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetStowPointCmd()
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_STOW_POINT;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetStaticPointCmd:
///		Sets the static pointing command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Azimuth			Azimuth of target (deg).
///		\param[in]		Elevation		Elevation of target (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetStaticPointCmd( float Azimuth, float Elevation )
{
	int length = sizeof( uint16_t ) * 3;
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_STATIC_POINT;
		m_Data[m_WriteCount++] = (uint16_t)(Azimuth * 65536.0f / 360.0f);
		m_Data[m_WriteCount++] = (uint16_t)(Elevation * 65536.0f / 360.0f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetStaticPointData:
///		Gets the static pointing command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pAzimuth			Azimuth of target (deg).
///		\param[out]		pElevation			Elevation of target (deg).
///		\return			If the packet exists and has data, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetStaticPointData( float *pAzimuth, float *pElevation )
{
	int length = sizeof( uint16_t ) * 2;
	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Set the command.
		*pAzimuth = (float)m_Data[m_ReadCount++] * 360.0f / 65536.0f;
		*pElevation = (float)m_Data[m_ReadCount++] * 360.0f / 65536.0f;
	}

	// Return status.
	return rval;
}

//	SetInertialPointCmd:
///		Sets the inertial pointing command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		RA				Right ascension of target (deg).
///		\param[in]		Dec				Declination of target (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetInertialPointCmd( float RA, float Dec )
{
	int length = sizeof( uint16_t ) + sizeof( float ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_INERTIAL_POINT;

		// Set the right ascension.
		WriteFloat( RA );

		// Set the declination.
		WriteFloat( Dec );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetInertialPointData:
///		Gets the inertial pointing command  data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pRA				Right ascension of target (deg).
///		\param[out]		pDec			Declination of target (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetInertialPointData( float *pRA, float *pDec )
{
	int length = sizeof( float ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the right ascension.
		*pRA = ReadFloat();

		// Get the declination.
		*pDec = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetCelestialPointCmd:
///		Sets the celestial pointing command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		UseCam0			Flag controlling use of camera 0 as image data source.
///		\param[in]		UseCam1			Flag controlling use of camera 1 as image data source.
///		\param[in]		RA				Right ascension of target (deg).
///		\param[in]		Dec				Declination of target (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetCelestialPointCmd( bool UseCam0, bool UseCam1,
  float RA, float Dec )
{
	int length = sizeof( uint16_t ) * 2 + sizeof( float ) * 2;

	int rval;
	uint16_t wval=0;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CELESTIAL_POINT;

		// Set the camera use flags.
		if ( UseCam0 )
			wval |= 1;
    if ( UseCam1 )
      wval |= 2;
		m_Data[m_WriteCount++] = wval;

		// Set the right ascension.
		WriteFloat( RA );

		// Set the declination.
		WriteFloat( Dec );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetCelestialPointData:
///		Gets the inertial/celestial pointing command  data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pUseCam0		Flag controlling use of camera 0 as image data source.
///		\param[out]		pUseCam1		Flag controlling use of camera 1 as image data source.
///		\param[out]		pRA				Right ascension of target (deg).
///		\param[out]		pDec			Declination of target (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetCelestialPointData( bool *pUseCam0, bool *pUseCam1,
  float *pRA, float *pDec )
{
	int length = sizeof(uint16_t) + sizeof( float ) * 2;

	int rval;
	uint16_t wval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the flag word.
		wval = m_Data[m_ReadCount++];
		*pUseCam0 = (wval & 0x0001) > 0;
    *pUseCam1 = (wval & 0x0002) > 0;

		// Get the right ascension.
		*pRA = ReadFloat();

		// Get the declination.
		*pDec = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetWheelVoltageCmd:
///		Sets the wheel voltage command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Volts				Azimuth torque wheel motor voltage (V).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetWheelVoltageCmd( float Volts )
{
	int length = sizeof (uint16_t ) * 2;
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_WHEEL_VOLTAGE;

		// Set the data.
		m_Data[m_WriteCount++] = (short)(Volts * 1000.0f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetShaftVoltageCmd:
///		Sets the shaft voltage command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Volts				Azimuth shaft motor voltage (V).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetShaftVoltageCmd( float Volts )
{
	int length = sizeof (uint16_t ) * 2;
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_SHAFT_VOLTAGE;

		// Set the data.
		m_Data[m_WriteCount++] = (short)(Volts * 1000.0f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetElevationVoltageCmd:
///		Sets the elevation voltage command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Volts				Elevation motor voltage (V).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetElevationVoltageCmd( float Volts )
{
	int length = sizeof (uint16_t ) * 2;
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_ELEVATION_VOLTAGE;

		// Set the data.
		m_Data[m_WriteCount++] = (short)(Volts * 1000.0f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetBearingVoltageCmd:
///		Sets the bearing voltage command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Volts				Azimuth bearing race motor voltage (V).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetBearingVoltageCmd( float Volts )
{
	int length = sizeof (uint16_t ) * 2;
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_BEARING_VOLTAGE;

		// Set the data.
		m_Data[m_WriteCount++] = (short)(Volts * 1000.0f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetCtlMotorVoltageData:
///		Gets the motor voltage command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pmVolts			Voltage set for the motor voltage command (millivolts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetCtlMotorVoltageData( float* pmVolts )
{
	int length = sizeof (uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pmVolts = (short)m_Data[m_ReadCount++] * 0.001f;
	}

	// Return status.
	return rval;
}

//	SetElSaeOffsetCmd:
///		Sets the elevation shaft angle encoder offset command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Offset				True - measured offset (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetElSaeOffsetCmd( float Offset )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_SAEOFFSET;

		// Set the offset.
		WriteFloat( Offset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetElIncOffsetCmd:
///		Sets the elevation inclinometer offset command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Offset				True - measured offset (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetElIncOffsetCmd( float Offset )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_EINCOFFSET;

		// Set the offset.
		WriteFloat( Offset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetElSaeIncOffsetCmd:
///		Sets the elevation SAE + inclinometer offset command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Offset				True - measured offset (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetElSaeIncOffsetCmd( float Offset )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_SAEINCOFFSET;

		// Set the offset.
		WriteFloat( Offset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetElStowPosCmd:
///		Sets the elevation SAE stow position command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		StowEl				SAE elevation reading of stow position.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetElStowPosCmd( float StowEl )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_STOWPOS;

		// Set the offset.
		WriteFloat( StowEl );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetElCurAsStowCmd:
///		Sets the current elevation as stow position command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetElCurAsStowCmd()
{
	int length = sizeof( uint16_t );
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_CTL_CUR2STOW;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAzDgpsOffsetCmd:
///		Sets the DGPS azimuth offset command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Offset				True - measured offset (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAzDgpsOffsetCmd( float Offset )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_DGPSOFFSET;

		// Set the offset.
		WriteFloat( Offset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAzMagOffsetCmd:
///		Sets the magnetometer azimuth offset command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Offset				True - measured offset (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAzMagOffsetCmd( float Offset )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_MAGOFFSET;

		// Set the offset.
		WriteFloat( Offset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetRollIncOffsetCmd:
///		Sets the roll inclinometer offset command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Offset				True - measured offset (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetRollIncOffsetCmd( float Offset )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_ROLLINCOFFSET;

		// Set the offset.
		WriteFloat( Offset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetElRateFggOffsetCmd:
///		Sets the elevation fine-guidance gyro rate offset command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Offset				True - measured offset (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetElRateFggOffsetCmd( float Offset )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_ELFGGOFFSET;

		// Set the offset.
		WriteFloat( Offset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAzRateFggOffsetCmd:
///		Sets the azimuth fine-guidance gyro rate offset command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Offset				True - measured offset (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAzRateFggOffsetCmd( float Offset )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_AZFGGOFFSET;

		// Set the offset.
		WriteFloat( Offset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAzRateHrgOffsetCmd:
///		Sets the azimuth high-rate gyro rate offset command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Offset				True - measured offset (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAzRateHrgOffsetCmd( float Offset )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_AZHRGOFFSET;

		// Set the offset.
		WriteFloat( Offset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetRollRateFggOffsetCmd:
///		Sets the roll fine-guidance gyro rate offset command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Offset				True - measured offset (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetRollRateFggOffsetCmd( float Offset )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_ROLLFGGOFFSET;

		// Set the offset.
		WriteFloat( Offset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetOffsetData:
///		Gets the aspect sensor offset command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pOffset				True-measure offset (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetOffsetData( float *pOffset )
{
	int length = sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the offset.
		*pOffset = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetElAutoTrimFlags:
///		Sets the elevation axis sensor auto-trim flag command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		The data is set by converting the trim enable flags into bit entries in a uint16_t value.
///		\param[in]		TrimSaeInc		Flag enabling auto-trim of the SAE + inclinometer position.
///		\param[in]		TrimFggRate		Flag enabling auto-trim of the fine-guidance gyro rate.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetElAutoTrimFlags( bool SaeUseCam, bool FggUseCam,
    bool FggUseSae )
{
	int length = sizeof(uint16_t) * 2;

	int rval;
	uint16_t flag=0;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_ELTRIM;

		// Set the data into the flag value.
		if ( SaeUseCam )
			flag |= 1;
		if ( FggUseCam )
			flag |= 2;
		if ( FggUseSae )
			flag |= 4;

		// Set the flag value into the packet.
		m_Data[m_WriteCount++] = flag;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetElAutoTrimFlags:
///		Gets the elevation axis sensor auto-trim flag command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command word and data.
///		The data is set by converting the bit entries in the uint16_t data into flag values.
///		\param[out]		pTrimSaeInc		Flag enabling auto-trim of the SAE + inclinometer position.
///		\param[out]		pTrimFggRate	Flag enabling auto-trim of the fine-guidance gyro rate.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForRead()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int HeroCommandPacket::GetElAutoTrimFlags( bool *SaeUseCam, bool *FggUseCam,
    bool *FggUseSae )
{
	int length = sizeof( uint16_t );
	int rval;
	uint16_t flag;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the mode.
		flag = (int)m_Data[m_ReadCount++];

		// Parse out the flags.
		*SaeUseCam = ((flag & 1) > 0);
		*FggUseCam = ((flag & 2) > 0);
		*FggUseSae = ((flag & 4) > 0);
	}

	// Return status.
	return rval;
}

//	SetAzAutoTrimFlags:
///		Sets the azimuth axis sensor auto-trim flag command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		The data is set by converting the trim enable flags into bit entries in a uint16_t value.
///		\param[in]		TrimDgps		Flag enabling auto-trim of the DGPS position.
///		\param[in]		TrimMag			Flag enabling auto-trim of the magnetometer position.
///		\param[in]		TrimFggRate		Flag enabling auto-trim of the fine-guidance gyro rate.
///		\param[in]		TrimHrgRate		Flag enabling auto-trim of the high-rate gyro rate.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetAzAutoTrimFlags( bool DgpsUseCam, bool MagUseCam,
    bool MagUseDgps, bool FggUseCam, bool FggUseDgps, bool FggUseMag )
{
	int length = sizeof(uint16_t) * 2;

	int rval;
	uint16_t flag=0;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_AZTRIM;

		// Set the data into the flag value.
		if ( DgpsUseCam )
			flag |= 1;
		if ( MagUseCam )
			flag |= 2;
		if ( MagUseDgps )
			flag |= 4;
		if ( FggUseCam )
			flag |= 8;
		if ( FggUseDgps )
			flag |= 0x10;
		if ( FggUseMag )
			flag |= 0x20;

		// Set the flag value into the packet.
		m_Data[m_WriteCount++] = flag;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAzAutoTrimFlags:
///		Gets the azimuth axis sensor auto-trim flag command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command word and data.
///		The data is set by converting the bit entries in the uint16_t data into flag values.
///		\param[out]		pTrimDgps		Flag enabling auto-trim of the DGPS position.
///		\param[out]		pTrimMag		Flag enabling auto-trim of the magnetometer position.
///		\param[out]		pTrimFggRate	Flag enabling auto-trim of the fine-guidance gyro rate.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForRead()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int HeroCommandPacket::GetAzAutoTrimFlags( bool *DgpsUseCam, bool *MagUseCam,
    bool *MagUseDgps, bool *FggUseCam, bool *FggUseDgps, bool *FggUseMag )
{
	int length = sizeof( uint16_t );
	int rval;
	uint16_t flag;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the mode.
		flag = (int)m_Data[m_ReadCount++];

		// Parse out the flags.
		*DgpsUseCam = ((flag & 1) > 0);
		*MagUseCam = ((flag & 2) > 0);
		*MagUseDgps = ((flag & 4) > 0);
		*FggUseCam = ((flag & 8) > 0);
		*FggUseDgps = ((flag & 0x10) > 0);
		*FggUseMag = ((flag & 0x20) > 0);
	}

	// Return status.
	return rval;
}

//	SetRollAutoTrimFlags:
///		Sets the roll axis sensor auto-trim flag command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		The data is set by converting the trim enable flags into bit entries in a uint16_t value.
///		\param[in]		TrimInc			Flag enabling auto-trim of the inclinometer position.
///		\param[in]		TrimFggRate		Flag enabling auto-trim of the fine-guidance gyro rate.
////		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetRollAutoTrimFlags( bool TrimInc, bool TrimFggRate )
{
	int length = sizeof(uint16_t) * 2;

	int rval;
	uint16_t flag=0;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_ROLLTRIM;

		// Set the data into the flag value.
		if ( TrimInc )
			flag |= 1;
		if ( TrimFggRate )
			flag |= 4;

		// Set the flag value into the packet.
		m_Data[m_WriteCount++] = flag;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetRollAutoTrimFlags:
///		Gets the roll axis sensor auto-trim flag command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command word and data.
///		The data is set by converting the bit entries in the uint16_t data into flag values.
///		\param[out]		pTrimInc			Flag enabling auto-trim of the inclinometer position.
///		\param[out]		pTrimFggRate	Flag enabling auto-trim of the fine-guidance gyro rate.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForRead()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int HeroCommandPacket::GetRollAutoTrimFlags( bool *pTrimInc,
		bool *pTrimFggRate )
{
	int length = sizeof( uint16_t );
	int rval;
	uint16_t flag;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the mode.
		flag = (int)m_Data[m_ReadCount++];

		// Parse out the flags.
		*pTrimInc = ((flag & 1) > 0);
		*pTrimFggRate = ((flag & 4) > 0);
	}

	// Return status.
	return rval;
}

//	SetDgpsValidFlag:
///		Sets the command to either validate or invalidate the DGPS data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word based on the input flag value.
///		\param[in]		TrimHrgRate		Flag enabling or disabling DGPS data in the control loop.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetDgpsValidFlag( bool DgpsValid )
{
	int length = sizeof(uint16_t);

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		if ( DgpsValid )
			m_Data[m_WriteCount++] = CIP_CTL_DGPSVALID;
		else
			m_Data[m_WriteCount++] = CIP_CTL_DGPSINVALID;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetFggValidFlag:
///		Sets the command to either validate or invalidate the fine-guidance gyro data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word based on the input flag value.
///		\param[in]		TrimHrgRate		Flag enabling or disabling the fine-guidance gyro rate
///										data in the control loop.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetFggValidFlag( bool FggValid )
{
	int length = sizeof(uint16_t);

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		if ( FggValid )
			m_Data[m_WriteCount++] = CIP_CTL_FGGVALID;
		else
			m_Data[m_WriteCount++] = CIP_CTL_FGGINVALID;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetCtlHrgCutoff:
///		Sets the high-rate gyro filter cutoff command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word based on the input flag value.
///		\param[in]		hrgCutoff		Cutoff frequency in Hz.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetCtlHrgCutoff( float HrgCutoff )
{
	int length = sizeof(uint16_t) + sizeof(float);

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_HRGCUTOFF;
		WriteFloat( HrgCutoff );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetCtlFggCutoff:
///		Sets the fine-guidance gyro filter cutoff command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word based on the input flag value.
///		\param[in]		fggCutoff		Cutoff frequency in Hz.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetCtlFggCutoff( float FggCutoff )
{
	int length = sizeof(uint16_t) + sizeof(float);

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_FGGCUTOFF;
		WriteFloat( FggCutoff );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetFilterCutoffData:
///		Gets the digital low-pass filter cutoff command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pCutoff			Digital filter cutoff frequency (Hz).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetFilterCutoffData( float *pCutoff )
{
	int length = sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the offset.
		*pCutoff = ReadFloat();
	}

	// Return status.
	return rval;
}

///		Sets the sensor parameter command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word based on the input flag value.
///		\param[in]		fggCutoff		Cutoff frequency in Hz.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetAzCamSensorParam( int numPoints, double maxAge )
{
	int length = sizeof(uint16_t) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_SETAZCAMSENPARAM;
    m_Data[m_WriteCount++] = (uint16_t)numPoints;
    m_Data[m_WriteCount++] = (uint16_t)maxAge;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

///		Sets the sensor parameter command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word based on the input flag value.
///		\param[in]		fggCutoff		Cutoff frequency in Hz.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetAzDgpsSensorParam( int numPoints, double maxAge )
{
	int length = sizeof(uint16_t) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_SETAZDGPSSENPARAM;
    m_Data[m_WriteCount++] = (uint16_t)numPoints;
    m_Data[m_WriteCount++] = (uint16_t)maxAge;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

///		Sets the sensor parameter command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word based on the input flag value.
///		\param[in]		fggCutoff		Cutoff frequency in Hz.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetAzMagSensorParam( int numPoints, double maxAge )
{
	int length = sizeof(uint16_t) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_SETAZMAGSENPARAM;
    m_Data[m_WriteCount++] = (uint16_t)numPoints;
    m_Data[m_WriteCount++] = (uint16_t)maxAge;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

///		Sets the sensor parameter command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word based on the input flag value.
///		\param[in]		fggCutoff		Cutoff frequency in Hz.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetAzFggSensorParam( int numPoints, double maxAge )
{
	int length = sizeof(uint16_t) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_SETAZFGGSENPARAM;
    m_Data[m_WriteCount++] = (uint16_t)numPoints;
    m_Data[m_WriteCount++] = (uint16_t)maxAge;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

///		Sets the sensor parameter command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word based on the input flag value.
///		\param[in]		fggCutoff		Cutoff frequency in Hz.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetElCamSensorParam( int numPoints, double maxAge )
{
	int length = sizeof(uint16_t) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_SETELCAMSENPARAM;
    m_Data[m_WriteCount++] = (uint16_t)numPoints;
    m_Data[m_WriteCount++] = (uint16_t)maxAge;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

///		Sets the sensor parameter command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word based on the input flag value.
///		\param[in]		fggCutoff		Cutoff frequency in Hz.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetElSaeSensorParam( int numPoints, double maxAge )
{
	int length = sizeof(uint16_t) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_SETELSAESENPARAM;
    m_Data[m_WriteCount++] = (uint16_t)numPoints;
    m_Data[m_WriteCount++] = (uint16_t)maxAge;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

///		Sets the sensor parameter command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word based on the input flag value.
///		\param[in]		fggCutoff		Cutoff frequency in Hz.
///		\return			This packet returns a non-zero error if the call to VerifyPacketForWrite()
///						returns a non-zero error (i.e. if space is not available in the packet).
///						Otherwise, returns zero to indicate success.
int	HeroCommandPacket::SetElFggSensorParam( int numPoints, double maxAge )
{
	int length = sizeof(uint16_t) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_SETELFGGSENPARAM;
    m_Data[m_WriteCount++] = (uint16_t)numPoints;
    m_Data[m_WriteCount++] = (uint16_t)maxAge;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetImageErrorThreshData:
///		Gets the ASP image total error threshold command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pThresh			ASP image error threshold (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetCtlSensorParam( int &numPoints, double &maxAge )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the threshold.
    numPoints = (int)m_Data[m_ReadCount++];
    maxAge = (double)m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetImageErrorThreshCmd:
///		Sets the ASP image total error threshold command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\param[in]		Thresh			ASP image error threshold (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetImageErrorThreshCmd( float Thresh )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_SETIMERRTHRESH;

		// Set the threshold.
		WriteFloat( Thresh );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetImageErrorThreshData:
///		Gets the ASP image total error threshold command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pThresh			ASP image error threshold (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetImageErrorThreshData( float *pThresh )
{
	int length = sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the threshold.
		*pThresh = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetImageAspectCmd:
///		Sets the ASP image aspect update command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word.
///		\param[in]		CamID			ID of camera that produced data.
///		\param[in]		AzOffset		Azimuth offset of image center from target.(deg).
///		\param[in]		ElOffset		Elevation offset of image center from target (deg).
///		\param[in]		Roll			ASP image roll (deg).
///		\param[in]		Error			ASP image position fit error (deg).
///		\param[in]		Time			ASP image time (sec).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetImageAspectCmd( int CamID, double AzOffset,
		double ElOffset, double Roll, double Error, double Time )
{
	int length = sizeof( uint16_t ) * 2 + sizeof( double ) * 5;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_CTL_SETIMASP;

		// Set the camera data.
		m_Data[m_WriteCount++] = (uint16_t)CamID;

		// Set the data.
		WriteDouble( AzOffset );
		WriteDouble( ElOffset );
		WriteDouble( Roll );
		WriteDouble( Error );
		WriteDouble( Time );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetImageAspectData:
///		Gets the ASP image aspect update command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pCamID			ID of camera that produced data.
///		\param[out]		pAzOffset		Azimuth offset of image center from target.(deg).
///		\param[out]		pElOffset		Elevation offset of image center from target (deg).
///		\param[out]		pRoll			ASP image roll (deg).
///		\param[out]		pError			ASP image total offset from target (deg).
///		\param[out]		pTime			ASP image time (sec).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetImageAspectData( int *pCamID, double *pAzOffset,
			double *pElOffset, double *pRoll, double *pError, double *pTime )
{
	int length = sizeof( double ) * 5 + sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pCamID = (int)m_Data[m_ReadCount++];
		*pAzOffset = ReadDouble();
		*pElOffset = ReadDouble();
		*pRoll = ReadDouble();
		*pError = ReadDouble();
		*pTime = ReadDouble();
	}

	// Return status.
	return rval;
}

//	SetGyroUpdateCmd:
///		Sets the gyro update command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		ElRate			Elevation angular rate (deg/s).
///		\param[in]		AzRate			Azimuth angular rate (deg/s).
///		\param[in]		RollRate		Roll angular rate (deg/s).
///		\param[in]		HrgElRate		Elevation angular rate from the high-rate gyro (deg/s).
///		\param[out]		ElvIncl			Elevation angle from inclinometer (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetGyroUpdateCmd( float ElRate, float AzRate,
		float RollRate )
{
	int length = sizeof (uint16_t ) + sizeof( float ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_GYRO_UPDATE;

		// Set the data.
		WriteFloat( ElRate );
		WriteFloat( AzRate );
		WriteFloat( RollRate );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetGyroUpdateData:
///		Gets the gyro update command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pElRate			Elevation angular rate (deg/s).
///		\param[out]		pAzRate			Azimuth angular rate (deg/s).
///		\param[out]		pRollRate		Roll angular rate (deg/s).
///		\param[out]		pHrgElRate		Elevation angular rate from the high-rate gyro (deg/s).
///		\param[out]		pElvIncl		Elevation angle from inclinometer (deg).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetGyroUpdateData( float* pElRate, float* pAzRate,
		float* pRollRate )
{
	int length = sizeof (float ) * 3;
	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the elevation rate.
		*pElRate = ReadFloat();
		*pAzRate = ReadFloat();
		*pRollRate = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetAspAckTrkOnCmd:
///		Sets the acknowledge that ASP started cam tracking command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspAckTrkOnCmd()
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_CTL_ASPACKTRKON;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspAckTrkOffCmd:
///		Sets the acknowledge that ASP stopped cam tracking command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspAckTrkOffCmd()
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_CTL_ASPACKTRKOFF;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspAckTrkErrCmd:
///		Sets the acknowledge that ASP encountered an error command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspAckTrkErrCmd()
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_CTL_ASPACKTRKERR;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetControlModeParamCmd:
///		Sets the control loop parameters for a specific mode command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Mode			Control mode for which the parameters will be set.
///		\param[in]		PGain			Control mode proportional gain.
///		\param[in]		IGain			Control mode integral gain.
///		\param[in]		DGain			Control mode differential gain.
///		\param[in]		MaxOutV			Maximum control voltage output (Volts).
///		\param[in]		ILimit			Fraction of maximum output that I output is allowed.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetControlModeParamCmd( uint16_t Mode, float PGain,
		float IGain, float DGain, float MaxOutV, float ILimit )
{
	int length = sizeof (uint16_t ) * 2 + sizeof( float ) * 5;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_SET_PIDPARAM;

		// Set the mode.
		m_Data[m_WriteCount++] = Mode;

		// Set the control loop gains.
		WriteFloat( PGain );
		WriteFloat( IGain );
		WriteFloat( DGain );

		// Add the max output data.
		WriteFloat( MaxOutV );

		// Set the maximum offset and ILimit.
		WriteFloat( ILimit );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetControlModeParamData:
///		Gets the control loop parameters for a specific mode data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pMode			Control mode for which the parameters will be set.
///		\param[out]		pPGain			Control mode proportional gain.
///		\param[out]		pIGain			Control mode integral gain.
///		\param[out]		pDGain			Control mode differential gain.
///		\param[out]		pMaxOut			Maximum control voltage output (millivolts).
///		\param[out]		pILimit			Fraction of maximum output that I output is allowed.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetControlModeParamData( int *pMode, float *pPGain,
		float *pIGain, float *pDGain, float *pMaxOut, float *pILimit )
{
	int length = sizeof( uint16_t ) * 1 + sizeof (float ) * 5;
	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the mode.
		*pMode = (int)m_Data[m_ReadCount++];

		// Get the gains.
		*pPGain = ReadFloat();
		*pIGain = ReadFloat();
		*pDGain = ReadFloat();

		// Get the maximum output in millivolts.
		*pMaxOut = ReadFloat() * 1000.0f;

		// Get the elevation rate.
		*pILimit = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetControlModeBoundsCmd:
///		Sets the control mode bounds for a specific mode command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Mode			Control mode for which the parameters will be set.
///		\param[in]		PBound			Maximum proportional error allowed in this mode.
///		\param[in]		DBound			Maximum differential error allowed in this mode.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetControlModeBoundsCmd( int Mode, float PBound, float DBound )
{
	int length = sizeof (uint16_t ) * 2 + sizeof( float ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_SET_MODEBOUNDS;

		// Set the mode.
		m_Data[m_WriteCount++] = Mode;

		// Set the bounds.
		WriteFloat( PBound );
		WriteFloat( DBound );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetControlModeBoundsData:
///		Gets the control mode bounds for a specific mode data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pMode			Control mode for which the parameters will be set.
///		\param[out]		pPBound			Maximum proportional error allowed in this mode.
///		\param[out]		pDBound			Maximum differential error allowed in this mode.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetControlModeBoundsData( int *pMode, float *pPBound, float *pDBound )
{
	int length = sizeof( uint16_t ) + sizeof (float ) * 2;
	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the mode.
		*pMode = (int)m_Data[m_ReadCount++];

		// Get the bounds.
		*pPBound = ReadFloat();
		*pDBound = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetControlModeSlewParamCmd:
///		Sets the slew parameters for a specific mode command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Mode			Control mode for which the parameters will be set.
///		\param[in]		MaxRate			Maximum rate of slew.
///		\param[in]		Accel			Acceleration and deceleration rate of slew profile.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetControlModeSlewParamCmd( int Mode, float MaxRate, float Accel )
{
	int length = sizeof (uint16_t ) * 2 + sizeof( float ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_SET_SLEWPARAM;

		// Set the mode.
		m_Data[m_WriteCount++] = Mode;

		// Set the slew parameters.
		WriteFloat( MaxRate );
		WriteFloat( Accel );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetControlModeSlewParamData:
///		Gets the slew parameters for a specific mode data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pMode			Control mode for which the parameters will be set.
///		\param[out]		pMaxRate		Maximum rate of slew.
///		\param[out]		pAccel			Acceleration and deceleration rate of slew profile.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetControlModeSlewParamData( int *pMode, float *pMaxRate, float *pAccel )
{
	int length = sizeof( uint16_t ) + sizeof (float ) * 2;
	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the mode.
		*pMode = (int)m_Data[m_ReadCount++];

		// Get the bounds.
		*pMaxRate = ReadFloat();
		*pAccel = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetWheelDumpParamCmd:
///		Sets the wheel dump control loop parameters command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		PGain			Proportional PID loop gain.
///		\param[in]		IGain			Integral PID loop gain.
///		\param[in]		DGain			Differential PID loop gain.
///		\param[in]		MaxOutV			Maximum shaft voltage output (V).
///		\param[in]		ILimit			Fraction of maximum output that I output is allowed.
///		\param[in]		OmegaMax		Maximum allowed wheel angular momentum (rad/s).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetWheelDumpParamCmd( float PGain, float IGain,
		float DGain, float MaxOut, float ILimit, float OmegaMax )
{
	int length = sizeof (uint16_t ) * 4 + sizeof(float) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_CTL_WHEELDUMPPARM;

		// Write the data.
		WriteFloat( PGain );
		WriteFloat( IGain );
		WriteFloat( DGain );
		m_Data[m_WriteCount++] = (uint16_t)(MaxOut * 1000.0f);
		m_Data[m_WriteCount++] = (uint16_t)(ILimit * 10000.0f);
		m_Data[m_WriteCount++] = (uint16_t)(OmegaMax * 1000.0f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetWheelDumpParamData:
///		Gets the wheel dump control loop parameters command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pPGain			Proportional PID loop gain.
///		\param[out]		pIGain			Integral PID loop gain.
///		\param[out]		pDGain			Differential PID loop gain.
///		\param[out]		pMaxOut			Maximum shaft voltage output (mV).
///		\param[out]		pILimit			Fraction of maximum output that I output is allowed.
///		\param[out]		pOmegaMax		Maximum allowed wheel angular momentum (rad/s).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetWheelDumpParamData( float *pPGain,
		float *pIGain, float *pDGain,float *pMaxOut, float *pILimit,
		float *pOmegaMax )
{
	int length = sizeof (uint16_t ) * 3 + sizeof(float) * 3;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pPGain = ReadFloat();
		*pIGain = ReadFloat();
		*pDGain = ReadFloat();
		*pMaxOut = (float)m_Data[m_ReadCount++];
		*pILimit = (float)m_Data[m_ReadCount++] * 0.0001f;
		*pOmegaMax = (float)m_Data[m_ReadCount++] * 0.001f;
	}

	// Return status.
	return rval;
}

//	SetCtlParamDacSelectCmd:
///		Sets the motor DAC selection parameters command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		ShaftSel		Shaft motor DAC selection (0 = primary, 1 = backup).
///		\param[in]		WheelSel		Wheel motor DAC selection (0 = primary, 1 = backup).
///		\param[in]		ElSel			Elevation motor DAC selection (0 = primary, 1 = backup).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetCtlParamDacSelectCmd(  int ShaftSel, int WheelSel, int ElSel  )
{
	int length = sizeof (uint16_t ) * 2;
	uint16_t dacsel = 0;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_CTL_DACSELECTPARAM;

		// Set the data value.
		if ( ShaftSel )
			dacsel |= 0x0001;
		if ( WheelSel )
			dacsel |= 0x0002;
		if ( ElSel )
			dacsel |= 0x0004;

		// Write the data.
		m_Data[m_WriteCount++] = dacsel;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetCtlParamDacSelectData:
///		Gets the motor DAC selection parameters command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pShaftSel		Shaft motor DAC selection (0 = primary, 1 = backup).
///		\param[out]		pWheelSel		Wheel motor DAC selection (0 = primary, 1 = backup).
///		\param[out]		pElSel			Elevation motor DAC selection (0 = primary, 1 = backup).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetCtlParamDacSelectData( int *pShaftSel, int *pWheelSel, int *pElSel )
{
	int length = sizeof (uint16_t );
	uint16_t dacsel;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		dacsel = m_Data[m_ReadCount++];

		// Set the return values.
		if ( (dacsel & 0x0001) )
			*pShaftSel = 1;
		else
			*pShaftSel = 0;

		if ( (dacsel & 0x0002) )
			*pWheelSel = 1;
		else
			*pWheelSel = 0;

		if ( (dacsel & 0x0004) )
			*pElSel = 1;
		else
			*pElSel = 0;
	}

	// Return status.
	return rval;
}

int HeroCommandPacket::SetCtlParamGyroTweakCmd( uint16_t cmdID,
  float slope, float offset )
{
	int length = sizeof (uint16_t ) + sizeof(float) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = cmdID;

		// Write the data.
    WriteFloat( slope );
    WriteFloat( offset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

int HeroCommandPacket::GetCtlParamGyroTweakData( float &slope, float &offset )
{
	int length = sizeof(float) * 2;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
    slope = ReadFloat();
    offset = ReadFloat();
	}

	// Return status.
	return rval;
}

//////////////////////////////////////////////////////////////////////////////////////
//	Commands for the Aspect system

//	SetAspExposureTimeCmd:
///		Sets the aspect camera exposure time command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID				Identifies which camera to send command to (0 or 1).
///		\param[in]		ExpTimeMs			Exposure time in milliseconds.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspExposureTimeCmd( int CamID, uint16_t ExpTimeMs )
{
	int length = sizeof (uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETEXP0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETEXP1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = ExpTimeMs;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspExposureTimeData:
///		Gets the aspect camera exposure time command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pExpTimeMs			Exposure time in milliseconds.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspExposureTimeData( uint16_t *pExpTimeMs )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pExpTimeMs = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetAspImageCenterCmd:
///		Sets the dynamical center of camera image command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID				Identifies which camera to send command to (0 or 1).
///		\param[in]		XCenter			Dynamical center of image x-coordinate (pixels).
///		\param[in]		YCenter			Dynamical center of image y-coordinate (pixels).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspImageCenterCmd( int CamID, uint16_t XCenter, uint16_t YCenter )
{
	int length = sizeof (uint16_t ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETIMCENTER0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETIMCENTER1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = XCenter;
		m_Data[m_WriteCount++] = YCenter;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspImageCenterData:
///		Gets the dynamical center of camera image command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pXCenter			Dynamical center of image x-coordinate (pixels).
///		\param[out]		pYCenter			Dynamical center of image y-coordinate (pixels).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspImageCenterData( uint16_t *pXCenter, uint16_t *pYCenter )
{
	int length = sizeof (uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pXCenter = m_Data[m_ReadCount++];
		*pYCenter = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetAspCamParamCmd:
///		Sets the image scale, electrons per count and image orientation command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID				Identifies which camera to send command to (0 or 1).
///		\param[in]		Scale			Image scale in deg/pixel.
///		\param[in]		EPerCount			Number of electrons per CCD count
///		\param[in]		XFlip			Flag indicating if X axis must be flipped to align with Az/Alt.
///		\param[in]		YFlip			Flag indicating if Y axis must be flipped to align with Az/Alt.
///		\param[in]		RotAngleDeg		Rotation angle of camera from vertical (in degrees).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspCamParamCmd( int CamID, float Scale, float EPerCount, bool XFlip, bool YFlip,
			float RotAngleDeg )
{
	int length = sizeof (uint16_t ) * 3 + sizeof( float ) * 2;
	uint16_t flag = 0;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCAMPARAM0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCAMPARAM1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the rotation angle.
		WriteFloat( Scale );

		// Convert and write the electrons per count.
		m_Data[m_WriteCount++] = (uint16_t)(EPerCount * 1000.0f);

		// Set the flip flags.
		if ( XFlip )
			flag |= 0x0001;
		if ( YFlip )
			flag |= 0x0100;
		m_Data[m_WriteCount++] = flag;

		// Set the rotation angle.
		WriteFloat( RotAngleDeg );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspCamParamData:
///		Gets the image scale, electrons per count and image orientation command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pScale			Image scale in deg/pixel.
///		\param[out]		pEPerCount			Number of electrons per CCD count
///		\param[in]		pXFlip				Flag indicating if X axis must be flipped to align with Az/Alt.
///		\param[in]		pYFlip				Flag indicating if Y axis must be flipped to align with Az/Alt.
///		\param[out]		pRotAngleDeg		Rotation angle of camera from vertical (in degrees).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspCamParamData( float *pScale, float *pEPerCount, bool *pXFlip,
		bool *pYFlip, float *pRotAngleDeg )
{
	int length = sizeof (uint16_t ) + sizeof( float );

	int rval;
	uint16_t val;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the scale.
		*pScale = ReadFloat();

		// Get and convert the electrons per count.
		*pEPerCount = (float)m_Data[m_ReadCount++] * 0.001f;

		// Get the orientation flags..
		val = m_Data[m_ReadCount++];
		*pXFlip = (val & 0x0001) > 0;
		*pYFlip = (val & 0x0100) > 0;

		// Get the rotation angle.
		*pRotAngleDeg = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetAspCatalogParamCmd:
///		Sets the target catalog parameters command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID				Identifies which camera to send command to (0 or 1).
///		\param[in]		Radius			Radius of catalog object field (in degrees).
///		\param[in]		VMin			Minimum visual magnitude of objects in catalog.
///		\param[in]		VMax			Maximum visual magnitude of objects in catalog.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspCatalogParamCmd( int CamID, float Radius, float VMin, float VMax )
{
	int length = sizeof (uint16_t ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCATPARAM0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCATPARAM1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t)(Radius * 1000.0f);
		m_Data[m_WriteCount++] = (uint16_t)((uint8_t)(VMin * 10.0f) | ((uint8_t)(VMax * 10.0f) << 8));

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspCatalogParamData:
///		Gets the target catalog parameters command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pRadius			Radius of catalog object field (in degrees).
///		\param[out]		pVMin			Minimum visual magnitude of objects in catalog.
///		\param[out]		pVMax			Maximum visual magnitude of objects in catalog.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspCatalogParamData( float *pRadius, float *pVMin, float *pVMax )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	uint16_t val;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pRadius = (float)m_Data[m_ReadCount++] * 0.001f;
		val = m_Data[m_ReadCount++];
		*pVMin = (float)(val & 0x00ff) * 0.1f;
		*pVMax = (float)(val >> 8) * 0.1f;
	}

	// Return status.
	return rval;
}

//	SetAspTrackThreshCmd:
///		Sets the image tracking thresholds command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID				Identifies which camera to send command to (0 or 1).
///		\param[in]		PixelThresh		Minimum significance of a valid object pixel (in sigma).
///		\param[in]		ObjThresh		Minimum significance of a valid object (in sigma).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspTrackThreshCmd( int CamID, float PixelThresh, float ObjThresh )
{
	int length = sizeof (uint16_t ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETTRKTHRESH0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETTRKTHRESH1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t)(PixelThresh * 100.0f);
		m_Data[m_WriteCount++] = (uint16_t)(ObjThresh * 100.0f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspTrackThresData:
///		Gets the image tracking parameters command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pPixelThresh	Minimum significance of a valid object pixel (in sigma).
///		\param[out]		pObjThresh		Minimum significance of a valid object (in sigma).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspTrackThreshData( float *pPixelThresh, float *pObjThresh )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pPixelThresh = (float)m_Data[m_ReadCount++] * 0.01f;
		*pObjThresh = (float)m_Data[m_ReadCount++] * 0.01f;
	}

	// Return status.
	return rval;
}

//	SetAspTrackMaxPosErrCmd:
///		Sets the image tracking maximum position error command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID				Identifies which camera to send command to (0 or 1).
///		\param[in]		MaxPosErr		Maximum allowed object separation error for identification (in pixels).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspTrackMaxPosErrCmd( int CamID, float MaxPosErr )
{
	int length = sizeof (uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETIMMAXERR0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETIMMAXERR1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t)(MaxPosErr * 1000.0f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspTrackMaxPosErrData:
///		Gets the image tracking maximum position error command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pMaxPosErr		Maximum allowed object separation error for identification (in pixels).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspTrackMaxPosErrData( float *pMaxPosErr )
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pMaxPosErr = (float)m_Data[m_ReadCount++] * 0.001f;
	}

	// Return status.
	return rval;
}

//	SetAspTrackMinIDCmd:
///		Sets the image tracking minimum valid identified objects command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID				Identifies which camera to send command to (0 or 1).
///		\param[in]		MinID		Minimum number of identified objects for a valid tracking fit.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspTrackMinIDCmd( int CamID, int MinID )
{
	int length = sizeof (uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETTRKMINID0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETTRKMINID1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t)MinID;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspTrackMinIDData:
///		Gets the image tracking minimum valid identified objects command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pMinID		Minimum number of identified objects for a valid tracking fit.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspTrackMinIDData( int *pMinID )
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pMinID = (int)m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetAspTrackWidthCmd:
///		Sets the image tracking minimum object radius and background filter width command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID				Identifies which camera to send command to (0 or 1).
///		\param[in]		MinObjRadius	Minimum radius to integrate an image object over (in pixels).
///		\param[in]		FilterWidth		Running digital filter width for background modelling (in pixels).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspTrackWidthCmd( int CamID, uint16_t MinObjRadius, int FilterWidth )
{
	int length = sizeof (uint16_t ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETTRKWIDTHS0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETTRKWIDTHS1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = MinObjRadius;
		m_Data[m_WriteCount++] = (uint16_t)FilterWidth;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspTrackWidthData:
///		Gets the image tracking minimum object radius and background filter width command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pMinObjRadius	Minimum radius to integrate an image object over (in pixels).
///		\param[out]		pFilterWidth	Running digital filter width for background modelling (in pixels).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspTrackWidthData( uint16_t *pMinObjRadius, int *pFilterWidth )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pMinObjRadius = m_Data[m_ReadCount++];
		*pFilterWidth = (int)m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetAspFocusParamCmd:
///		Sets the focus adjust loop parameters command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		Identifies which camera to send command to (0 or 1).
///		\param[in]		FFigType	Focus figure type identifier (0=star, 1=illuminated).
///		\param[in]		Backlash	Minimum number of steps to remove backlash (in steps).
///		\param[in]		StepSize	Number of motor steps per focus point in this loop.
///		\param[in]		numSteps	Number of points in adjust focus method.
///		\param[in]		MinPos		Minimum allowed position of the motor (in steps).
///		\param[in]		MaxPos		Maximum allowed position of the motor (in steps).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspFocusParamCmd(int CamID, uint8_t FfigType,
		int Backlash, int StepSize, int numSteps, int minPos, int maxPos )
{
	int length = sizeof (uint16_t ) * 7;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETFOCPARAM0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETFOCPARAM1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t)FfigType;
		m_Data[m_WriteCount++] = (uint16_t)Backlash;
		m_Data[m_WriteCount++] = (uint16_t)StepSize;
		m_Data[m_WriteCount++] = (uint16_t)numSteps;
		m_Data[m_WriteCount++] = (uint16_t)minPos;
		m_Data[m_WriteCount++] = (uint16_t)maxPos;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspFocusParamData:
///		Gets the focus adjust loop parameters command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[in]		pFfigType	Focus figure type identifier (0=star, 1=illuminated).
///		\param[out]		pBacklash	Minimum number of steps to remove backlash (in steps).
///		\param[out]		stepSize	Number of motor steps per focus points in this loop.
///		\param[out]		numSteps	Number of points in adjust focus method.
///		\param[out]		minPos		Minimum allowed position of the motor (in steps).
///		\param[out]		maxPos		Maximum allowed position of the motor (in steps).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspFocusParamData( uint8_t *pFfigType,
		int *pBacklash, int *pStepSize, int *numSteps, int *minPos, int *maxPos )
{
	int length = sizeof( uint16_t ) * 6;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pFfigType = (uint8_t)m_Data[m_ReadCount++];
		*pBacklash = (int)m_Data[m_ReadCount++];
		*pStepSize = (int)m_Data[m_ReadCount++];
		*numSteps = (int)m_Data[m_ReadCount++];
		*minPos = (int)m_Data[m_ReadCount++];
		*maxPos = (int)m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetAspFocusMotorRateCmd:
///		Sets the focus motor step rate command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID				Identifies which camera to send command to (0 or 1).
///		\param[in]		StepRate		Stepper motor maximum rate in steps/sec.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspFocusMotorRateCmd( int CamID, int StepRate )
{
	int length = sizeof (uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETFOCRATE0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETFOCRATE1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t)StepRate;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspFocusMotorRateData:
///		Gets the focus motor step rate command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pStepRate		Stepper motor maximum rate in steps/sec.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspFocusMotorRateData( int *pStepRate )
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pStepRate = (int)m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetAspCamOffsetCmd:
///		Sets the camera offset from line of sight command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID				Identifies which camera to send command to (0 or 1).
///		\param[in]		Offset			Offset of camera from line-of-sight (degrees).
///		\param[in]		OffsetZPA		Position angle from zenith of offset (degrees).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspCamOffsetCmd( int CamID, float Offset, float OffsetZPA )
{
	int length = sizeof (uint16_t ) + sizeof( float ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCAMOFFSET0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCAMOFFSET1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		WriteFloat( Offset );
		WriteFloat( OffsetZPA );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspCamOffsetData:
///		Gets the camera offset from line of sight command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pOffset			Offset of camera from line-of-sight (degrees).
///		\param[out]		pOffsetZPA		Position angle from zenith of offset (degrees).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspCamOffsetData( float *pOffset, float *pOffsetZPA )
{
	int length = sizeof( float ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pOffset = ReadFloat();
		*pOffsetZPA = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetAspTrackMethodCmd:
///		Sets the image tracking background removal method command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		UseFlat			Flag to select which method to use (Flat if true, Filter if false).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspTrackMethodCmd( int CamID, bool UseFlat )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
		{
			if ( UseFlat )
				m_Data[m_WriteCount++] = CIP_ASP_SETFINDFLAT0;
			else
				m_Data[m_WriteCount++] = CIP_ASP_SETFINDFILTER0;
		}
		else if ( CamID == 1 )
		{
			if ( UseFlat )
				m_Data[m_WriteCount++] = CIP_ASP_SETFINDFLAT1;
			else
				m_Data[m_WriteCount++] = CIP_ASP_SETFINDFILTER1;
		}
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspTrackCECircParam:
///		Sets the daylight corner-exclusion parameter command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID number of track and focus object to command.
///		\param[in]		XCenter		Center of corner-exclusion circle in x-axis pixels.
///		\param[in]		YCenter		Center of corner-exclusion circle in y-axis pixels.
///		\param[in]		Radius		Radius of corner-exclusion circle in pixels.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspTrackCECircParam( int CamID, uint16_t XCenter, uint16_t YCenter,
												uint16_t Radius )
{
	int length = sizeof (uint16_t ) * 4;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCECIRCPARAM0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCECIRCPARAM1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = XCenter;
		m_Data[m_WriteCount++] = YCenter;
		m_Data[m_WriteCount++] = Radius;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspTrackCECircParam:
///		Gets the corner-exclusion circle parameter command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[in]		pXCenter	Center of corner-exclusion circle in x-axis pixels.
///		\param[in]		pYCenter	Center of corner-exclusion circle in y-axis pixels.
///		\param[in]		pRadius		Radius of corner-exclusion circle in pixels.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspTrackCECircParam( uint16_t *pXCenter, uint16_t *pYCenter,
												uint16_t *pRadius )
{
	int length = sizeof( uint16_t ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the data.
		*pXCenter = m_Data[m_ReadCount++];
		*pYCenter = m_Data[m_ReadCount++];
		*pRadius = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetAspCamCoolerStateCmd:
///		Sets the camera cooler state command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CoolerOn			Flag indicating if cooler is to be turned on or off.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspCamCoolerStateCmd( int CamID, bool CoolerOn)
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
		{
			if ( CoolerOn )
				m_Data[m_WriteCount++] = CIP_ASP_SETCOOLERON0;
			else
				m_Data[m_WriteCount++] = CIP_ASP_SETCOOLEROFF0;
		}
		else if ( CamID == 1 )
		{
			if ( CoolerOn )
				m_Data[m_WriteCount++] = CIP_ASP_SETCOOLERON1;
			else
				m_Data[m_WriteCount++] = CIP_ASP_SETCOOLEROFF1;
		}
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspCamCoolerSetPointCmd:
///		Sets the camera cooler set point command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		SetTemp		Cooler set point temperature (C).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspCamCoolerSetPointCmd( int CamID, float SetTemp )
{
	int length = sizeof (uint16_t ) + sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCOOLERSP0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCOOLERSP1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t)(SetTemp * 10.0f + 2730.0f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspCamCoolerSetPointData:
///		Gets the camera cooler set point command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pSetTemp		Cooler set point temperature (C).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspCamCoolerSetPointData( float *pSetTemp )
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the temperature of the set point.
		*pSetTemp = (float)m_Data[m_ReadCount++] * 0.1f - 273.0f;
	}

	// Return status.
	return rval;
}

//	SetAspCamConnectStateCmd:
///		Sets the camera connection state command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Connect			Flag indicating if camera is to connect or disconnect.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspCamConnectStateCmd( int CamID, bool Connect )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
		{
			if ( Connect )
				m_Data[m_WriteCount++] = CIP_ASP_CAMCONNECT0;
			else
				m_Data[m_WriteCount++] = CIP_ASP_CAMDISCONNECT0;
		}
		else if ( CamID == 1 )
		{
			if ( Connect )
				m_Data[m_WriteCount++] = CIP_ASP_CAMCONNECT1;
			else
				m_Data[m_WriteCount++] = CIP_ASP_CAMDISCONNECT1;
		}
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspImageSaveStateCmd:
///		Sets the camera connection state command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		SaveImages			Flag indicating if images are saved to disk or not.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspImageSaveStateCmd( int CamID, bool SaveImages )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
		{
			if ( SaveImages )
				m_Data[m_WriteCount++] = CIP_ASP_SAVEIMAGEON0;
			else
				m_Data[m_WriteCount++] = CIP_ASP_SAVEIMAGEOFF0;
		}
		else if ( CamID == 1 )
		{
			if ( SaveImages )
				m_Data[m_WriteCount++] = CIP_ASP_SAVEIMAGEON1;
			else
				m_Data[m_WriteCount++] = CIP_ASP_SAVEIMAGEOFF1;
		}
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspCECircStateCmd:
///		Sets the corner-exclusion circle enable state command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		EnableCECirc		Flag indicating if CE circle is to be use (true) or not.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspCECircStateCmd( int CamID, bool EnableCECirc )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
		{
			if ( EnableCECirc )
				m_Data[m_WriteCount++] = CIP_ASP_CECIRCON0;
			else
				m_Data[m_WriteCount++] = CIP_ASP_CECIRCOFF0;
		}
		else if ( CamID == 1 )
		{
			if ( EnableCECirc )
				m_Data[m_WriteCount++] = CIP_ASP_CECIRCON1;
			else
				m_Data[m_WriteCount++] = CIP_ASP_CECIRCOFF1;
		}
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspStopCameraCmd:
///		Sets the stop all camera operations command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspStopCameraCmd( int CamID )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_STOPCAM0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_STOPCAM1;
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspSingleExposureCmd:
///		Sets the single exposure command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspSingleExposureCmd( int CamID )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SINGLEEXP0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SINGLEEXP1;
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspStartTrackingCmd:
///		Sets the start expose/track loop command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspStartTrackingCmd( int CamID )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_STARTTRACK0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_STARTTRACK1;
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspStopTrackingCmd:
///		Sets the stop expose/track loop command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspStopTrackingCmd( int CamID )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_STOPTRACK0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_STOPTRACK1;
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspFocusPosCmd:
///		Sets the manual focus position command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		FocusPos		Manual focus position (in motor steps).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspFocusMotorPosCmd( int CamID, uint32_t FocusPos )
{
	int length = sizeof (uint16_t ) + sizeof( uint32_t );
	uint16_t *pVal = (uint16_t *)&FocusPos;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETFOCUSPOS0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETFOCUSPOS1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = pVal[0];
		m_Data[m_WriteCount++] = pVal[1];

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspFocusPosData:
///		Gets the manual focus position command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pFocusPos		Manual focus position (in motor steps).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspFocusMotorPosData( uint32_t *pFocusPos )
{
	int length = sizeof( uint32_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		uint16_t *pval = (uint16_t *)pFocusPos;

		// Read the data by uint16_t lengths.
		pval[0] = m_Data[m_ReadCount++];
		pval[1] = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetAspAdjustFocusCmd:
///		Sets the start focus adjust loop command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID of Camera track and focus object command will be directed to (0, 1)
///		\param[in]		FocusMode	Camera focus mode (0=Star, 1=Illuminated, 2=Custom).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspAdjustFocusCmd( int CamID, uint16_t FocusMode )
{
	int length = sizeof (uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_ADJFOCUS0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_ADJFOCUS1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = FocusMode;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspAdjustFocusData:
///		Gets the adjust focus mode command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pFocusMode		Camera focus mode.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspAdjustFocusData( uint16_t *pFocusMode )
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Read the data by uint16_t lengths.
		*pFocusMode = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetAspStopFocusCmd:
///		Sets the stop focus command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID of Camera track and focus object command will be directed to (0, 1)
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspStopFocusCmd( int CamID )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_STOPFOCUS0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_STOPFOCUS1;
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspProcessTestImageCmd:
///		Sets the process test image command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID of Camera track and focus object command will be directed to (0, 1)
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspProcessTestImageCmd( int CamID )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_TEST_EXPOSURE0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_TEST_EXPOSURE1;
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspMakeDarkImageCmd:
///		Sets the make dark image command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID of Camera track and focus object command will be directed to (0, 1)
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspMakeDarkImageCmd( int CamID )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_MAKE_DARKIMAGE0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_MAKE_DARKIMAGE1;
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspTxFullImageCmd:
///		Sets the transmit next full raw image command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID of Camera track and focus object command will be directed to (0, 1)
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspTxFullImageCmd( int CamID )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_TXIMFULL0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_TXIMFULL1;
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspTxCenterImageCmd:
///		Sets the transmit central quadrant of raw image command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID of Camera track and focus object command will be directed to (0, 1)
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspTxCenterImageCmd( int CamID )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_TXIMCENTER0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_TXIMCENTER1;
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspUsbCam0ToCamera:
///		Sets the assign camera to USB Cam0 command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID of Camera track and focus object command will be directed to (0, 1)
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspUsbCam0ToCamera( int CamID )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCAM2ZERO0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETCAM2ZERO1;
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetAspImageEdgeParamCmd:
///		Sets the set image edge exclusion parameters command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID of Camera track and focus object command will be directed to (0, 1)
///		\param[in]		LeftOffset	Number of pixels to exclude at left edge.
///		\param[in]		RightOffset	Number of pixels to exclude at right edge.
///		\param[in]		BottomOffset	Number of pixels to exclude at bottom edge.
///		\param[in]		TopOffset	Number of pixels to exclude at top edge.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspImageEdgeParamCmd( int CamID, unsigned int LeftOffset, unsigned int RightOffset,
		unsigned int BottomOffset, unsigned int TopOffset )
{
	int length = sizeof (uint16_t ) * 5;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETEDGEOFFS0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETEDGEOFFS1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t) LeftOffset;
		m_Data[m_WriteCount++] = (uint16_t) RightOffset;
		m_Data[m_WriteCount++] = (uint16_t) BottomOffset;
		m_Data[m_WriteCount++] = (uint16_t) TopOffset;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspImageEdgeParamData:
///		Gets the image edge exclusion parameters command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pLeftOffset		Number of pixels to exclude at left edge.
///		\param[out]		pRightOffset	Number of pixels to exclude at right edge.
///		\param[out]		pBottomOffset	Number of pixels to exclude at bottom edge.
///		\param[out]		pTopOffset		Number of pixels to exclude at top edge.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspImageEdgeParamData( unsigned int *pLeftOffset, unsigned int *pRightOffset,
			unsigned int *pBottomOffset, unsigned int *pTopOffset )
{
	int length = sizeof( uint16_t ) * 4;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the slope.
		*pLeftOffset = (unsigned int)m_Data[m_ReadCount++];
		*pRightOffset = (unsigned int)m_Data[m_ReadCount++];
		*pBottomOffset = (unsigned int)m_Data[m_ReadCount++];
		*pTopOffset = (unsigned int)m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetAspImageBiasParamCmd:
///		Sets the set image dark bias level parameters command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID of Camera track and focus object command will be directed to (0, 1)
///		\param[in]		BiasCount	Dark bias level (in pixel counts).
///		\param[in]		BiasSigma	Standard deviation of dark bias level (in pixel counts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspImageBiasParamCmd( int CamID, double BiasCount, double BiasSigma )
{
	int length = sizeof (uint16_t ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETBIASPARAM0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETBIASPARAM1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t) BiasCount;
		m_Data[m_WriteCount++] = (uint16_t) (BiasSigma * 1000.0);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspImageBiasParamData:
///		Gets the image dark bias level parameters command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pBiasCount	Dark bias level (in pixel counts).
///		\param[out]		pBiasSigma	Standard deviation of dark bias level (in pixel counts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspImageBiasParamData( double *pBiasCount, double *pBiasSigma )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the slope.
		*pBiasCount = (double)m_Data[m_ReadCount++];
		*pBiasSigma = (double)m_Data[m_ReadCount++] * 0.001;
	}

	// Return status.
	return rval;
}

//	SetAspImageSizeCmd:
///		Sets the set image dimensions command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID of Camera track and focus object command will be directed to (0, 1)
///		\param[in]		XSize		Number of horizontal pixels in camera image.
///		\param[in]		YSize		Number of vertical pixels in camera image.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspImageSizeCmd( int CamID, int XSize, int YSize )
{
	int length = sizeof (uint16_t ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
			m_Data[m_WriteCount++] = CIP_ASP_SETIMSIZE0;
		else if ( CamID == 1 )
			m_Data[m_WriteCount++] = CIP_ASP_SETIMSIZE1;
		else
			return CMDPKTERR_USERPARAM;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t) XSize;
		m_Data[m_WriteCount++] = (uint16_t) YSize;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAspImageSizeData:
///		Gets the image dimensions command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		XSize		Number of horizontal pixels in camera image.
///		\param[out]		YSize		Number of vertical pixels in camera image.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAspImageSizeData( int *const XSize, int *const YSize )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the slope.
		*XSize = (int)m_Data[m_ReadCount++];
		*YSize = (int)m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetAspCamCentralQuadCmd:
///		Sets the set camera field size setting command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CamID		ID of Camera track and focus object command will be directed to (0, 1)
///		\param[in]		UseCenterQuad	Flag to use only central quadrant (true) or full field (false) of CCD.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAspCamCentralQuadCmd( int CamID, bool UseCenterQuad )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( CamID == 0 )
		{
			if ( UseCenterQuad )
				m_Data[m_WriteCount++] = CIP_ASP_SETCAMQUAD0;
			else
				m_Data[m_WriteCount++] = CIP_ASP_SETCAMFULL0;
		}
		else if ( CamID == 1 )
		{
			if ( UseCenterQuad )
				m_Data[m_WriteCount++] = CIP_ASP_SETCAMQUAD1;
			else
				m_Data[m_WriteCount++] = CIP_ASP_SETCAMFULL1;
		}
		else
			return CMDPKTERR_USERPARAM;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetGyroConversionData:
///		Gets the EHK gyro ADC conversion parameter command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pSlope			Slope of linear conversion.
///		\param[out]		pOffset			Offset of linear conversion.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetGyroConversionData( float *pSlope, float *pOffset )
{
	int length = sizeof( float ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the slope.
		*pSlope = ReadFloat();

		// Get the offset.
		*pOffset = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetAxisContributionCmd:
///		Sets the EHK gondola axis contribution parameter command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Axis			ID of axis whose parameters are being set.
///		\param[in]		ElFrac			Fraction of elevation gyro signal contributing to this axis.
///		\param[in]		EYFrac			Fraction of elevation yaw gyro signal contributing to this axis.
///		\param[in]		ERFrac			Fraction of elevation roll gyro signal contributing to this axis.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetAxisContributionCmd( GON_AXIS Axis, float ElFrac, float EYFrac, float ERFrac )
{
	int length = sizeof( uint16_t ) + sizeof( float ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		switch( Axis )
		{
		case GONAXIS_EL:
			m_Data[m_WriteCount++] = CIP_EHK_ELCONTRIB;
			break;

		case GONAXIS_AZ:
			m_Data[m_WriteCount++] = CIP_EHK_AZCONTRIB;
			break;

		case GONAXIS_ROLL:
			m_Data[m_WriteCount++] = CIP_EHK_ROLLCONTRIB;
			break;
		}

		// Set the contribution fractions.
		WriteFloat( ElFrac );
		WriteFloat( EYFrac );
		WriteFloat( ERFrac );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetAxisContributionData:
///		Gets the EHK gondola axis contribution parameter command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pElFrac			Fraction of elevation gyro signal contributing to this axis.
///		\param[out]		pEYFrac			Fraction of elevation yaw gyro signal contributing to this axis.
///		\param[out]		pERFrac			Fraction of elevation roll gyro signal contributing to this axis.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetAxisContributionData( float *pElFrac, float *pEYFrac, float *pERFrac )
{
	int length = sizeof( float ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the contribution fractions.
		*pElFrac = ReadFloat();
		*pEYFrac = ReadFloat();
		*pERFrac = ReadFloat();
	}

	// Return status.
	return rval;
}

/**
 *  Sets the control state, lower and upper temperature threshold for a given heater.
 */
int HeroCommandPacket::setEhkHeaterControlParamCmd( int heater, int ctlState,
  float lThresh, float uThresh )
{
  int rval, length = sizeof( uint16_t ) * 4;

  if ( !(rval = VerifyPacketForWrite( length )) )
  {
    m_Data[m_WriteCount++] = CIP_EHK_HTRCTLPARAM;

    // Write combined heater ID and control state word.
    m_Data[m_WriteCount++] = (heater & 0x07) << 8 | (ctlState & 0x03);

    // Write the thresholds.
    m_Data[m_WriteCount++] = (uint16_t)(int16_t)(lThresh * 100.f);
    m_Data[m_WriteCount++] = (uint16_t)(int16_t)(uThresh * 100.f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
  }

  return rval;
}

int HeroCommandPacket::getEhkHeaterControlParamData( int &heater,
  int &ctlState, float &lThresh, float &uThresh )
{
  int rval, length = sizeof( uint16_t ) * 3;

  if ( !(rval = VerifyPacketForRead( length )) )
  {
    uint16_t val = m_Data[m_ReadCount++];
    heater = (val >> 8) & 0x07;
    ctlState = val & 0x03;
    lThresh = (float)(short)m_Data[m_ReadCount++] * 0.01f;
    uThresh = (float)(short)m_Data[m_ReadCount++] * 0.01f;
  }

  return rval;
}

/**
 *  Sets the position to move the EHK balance motor to.
 */
int HeroCommandPacket::setEhkBalanceMotorPosCmd( int position )
{
  int rval, length = sizeof( uint16_t ) * 2;

  if ( !(rval = VerifyPacketForWrite( length )) )
  {
    // Write the command.
    m_Data[m_WriteCount++] = CIP_EHK_SETBALMOTORPOS;

    // Write the data.
    m_Data[m_WriteCount++] = (uint16_t)(int16_t)position;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
  }

  return rval;
}

/**
 *  Gets the position data from a motor move command.
 */
int HeroCommandPacket::getMotorPosData( int &position )
{
  int rval, length = sizeof( uint16_t );

  if ( !(rval = VerifyPacketForRead( length )) )
  {
    position = (int)(int16_t)m_Data[m_ReadCount++];
  }

  return rval;
}

/**
 *  Sets the EHK balance motor rate in steps/sec.
 */
int HeroCommandPacket::setEhkBalanceMotorRateCmd( int stepRate )
{
  int rval, length = sizeof( uint16_t ) * 2;

  if ( !(rval = VerifyPacketForWrite( length )) )
  {
    // Write the command.
    m_Data[m_WriteCount++] = CIP_EHK_SETBALMOTORRATE;

    // Write the data.
    m_Data[m_WriteCount++] = (uint16_t)stepRate;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
  }

  return rval;
}

/**
 *  Gets the position data from a motor move command.
 */
int HeroCommandPacket::getMotorRateData( int &stepRate )
{
  int rval, length = sizeof( uint16_t );

  if ( !(rval = VerifyPacketForRead( length )) )
  {
    stepRate = (int)m_Data[m_ReadCount++];
  }

  return rval;
}

/**
 *  Sets the EHK balance motor position bounds.
 */
int HeroCommandPacket::setEhkBalanceMotorBoundCmd( int minPos, int maxPos )
{
  int rval, length = sizeof( uint16_t ) * 3;

  if ( !(rval = VerifyPacketForWrite( length )) )
  {
    // Write the command.
    m_Data[m_WriteCount++] = CIP_EHK_SETBALMOTORBOUND;

    // Write the data.
    m_Data[m_WriteCount++] = (uint16_t)(int16_t)minPos;
    m_Data[m_WriteCount++] = (uint16_t)(int16_t)maxPos;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
  }

  return rval;
}

/**
 *  Gets the position data from a motor move command.
 */
int HeroCommandPacket::getMotorBoundData( int &minPos, int &maxPos )
{
  int rval, length = sizeof( uint16_t ) * 2;

  if ( !(rval = VerifyPacketForRead( length )) )
  {
    minPos = (int)(int16_t)m_Data[m_ReadCount++];
    maxPos = (int)(int16_t)m_Data[m_ReadCount++];
  }

  return rval;
}

//	setEhkBalMotorStopCmd:
///		Sets the FGG digital filter cutoff frequency command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CutoffFreqHz		Cutoff frequency in Hertz.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::setEhkBalMotorStopCmd()
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		m_Data[m_WriteCount++] = CIP_EHK_BALMOTORSTOP;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

/**
 *  Sets the EHK balance motor home stop parameters command and data.
 */
int HeroCommandPacket::setEhkBalMotorHomeParam( bool reset, int position )
{
  int rval, length = sizeof( uint16_t ) * 3;

  if ( !(rval = VerifyPacketForWrite( length )) )
  {
    // Write the command.
    m_Data[m_WriteCount++] = CIP_EHK_BALMOTORHOMEPARAM;

    // Write the data.
    m_Data[m_WriteCount++] = (uint16_t)reset;
    m_Data[m_WriteCount++] = (uint16_t)(int16_t)position;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
  }

  return rval;
}

/**
 *  Sets the EHK balance motor midpoint stop parameters command and data.
 */
int HeroCommandPacket::setEhkBalMotorMidParam( bool reset, int position )
{
  int rval, length = sizeof( uint16_t ) * 3;

  if ( !(rval = VerifyPacketForWrite( length )) )
  {
    // Write the command.
    m_Data[m_WriteCount++] = CIP_EHK_BALMOTORMIDPARAM;

    // Write the data.
    m_Data[m_WriteCount++] = (uint16_t)reset;
    m_Data[m_WriteCount++] = (uint16_t)(int16_t)position;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
  }

  return rval;
}

/**
 *  Sets the EHK balance motor end stop parameters command and data.
 */
int HeroCommandPacket::setEhkBalMotorEndParam( bool reset, int position )
{
  int rval, length = sizeof( uint16_t ) * 3;

  if ( !(rval = VerifyPacketForWrite( length )) )
  {
    // Write the command.
    m_Data[m_WriteCount++] = CIP_EHK_BALMOTORENDPARAM;

    // Write the data.
    m_Data[m_WriteCount++] = (uint16_t)reset;
    m_Data[m_WriteCount++] = (uint16_t)(int16_t)position;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
  }

  return rval;
}

/**
 *  Gets the position data from a motor stop parameter command.
 */
int HeroCommandPacket::getEhkBalMotorStopParam( bool &reset, int &position )
{
  int rval, length = sizeof( uint16_t ) * 2;

  if ( !(rval = VerifyPacketForRead( length )) )
  {
    reset = (bool)m_Data[m_ReadCount++];
    position = (int)(int16_t)m_Data[m_ReadCount++];
  }

  return rval;
}

//	SetEhkFggCutoffFreq:
///		Sets the FGG digital filter cutoff frequency command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CutoffFreqHz		Cutoff frequency in Hertz.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetEhkFggCutoffFreq( float CutoffFreqHz )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		m_Data[m_WriteCount++] = CIP_EHK_FGGCUTOFF;

		// Set the slope.
		WriteFloat( CutoffFreqHz );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetEhkHrgCutoffFreq:
///		Sets the HRG digital filter cutoff frequency command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		CutoffFreqHz		Cutoff frequency in Hertz.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetEhkHrgCutoffFreq( float CutoffFreqHz )
{
	int length = sizeof( uint16_t ) + sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		m_Data[m_WriteCount++] = CIP_EHK_HRGCUTOFF;

		// Set the slope.
		WriteFloat( CutoffFreqHz );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetFilterCutoffFreq:
///		Gets the digital filter cutoff frequency command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pCutoffFreqHz		Cutoff frequency in Hertz.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetFilterCutoffFreq( float *pCutoffFreqHz )
{
	int length = sizeof( float );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the contribution fractions.
		*pCutoffFreqHz = ReadFloat();
	}

	// Return status.
	return rval;
}

//////////////////////////////////////////////////////////////////////////////////////
//	Commands for the FDR system.

//	SetFdrFileSavingStateCmd:
///		Sets the FDR file save command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param			SaveFiles		Flag indicating if files should be saved (TRUE) or not.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetFdrFileSavingStateCmd( bool SaveFiles )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		if ( SaveFiles )
			m_Data[m_WriteCount++] = CIP_FDR_FILEON;
		else
			m_Data[m_WriteCount++] = CIP_FDR_FILEOFF;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetFdrForceTimeSetCmd:
///		Forces the FDR to set the system clock with the GPS time
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetFdrForceTimeSetCmd()
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_FORCE_TIMESET;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}


//	SetFdrRawDetectorCmd:
///		Sets the FDR detector to send raw data to ground command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		RawDet		Indicates which detector should send raw data to ground.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetFdrRawDetectorCmd( int RawDet )
{
	int length = sizeof (uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_FDR_FWDRAWDET;
		m_Data[m_WriteCount++] = (uint16_t)RawDet;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetFdrRawDetectorData:
///		Gets the FDR detector to send raw data to ground command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pRawDet		Indicates which detector should send raw data to ground.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetFdrRawDetectorData( int *pRawDet )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Set the command and data.
		*pRawDet = (int)m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetFdrEventsPerFileCmd:
///		Sets the FDR events per file command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		FileEvents		Maximum number of events per FDR file.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetFdrEventsPerFileCmd( int FileEvents )
{
	int length = sizeof (uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_FDR_FILEEVENTS;
		m_Data[m_WriteCount++] = (uint16_t)FileEvents;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetFdrEventsPerFileData:
///		Gets the FDR events per file command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pFileEvents		Maximum number of events per FDR file.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetFdrEventsPerFileData( int *pFileEvents )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Set the command and data.
		*pFileEvents = (int)m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetFdrEventProcessingCmd:
///		Sets the FDR detector event processing flags command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Flags		Processing flag for each detector (bit number = detector number).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetFdrEventProcessingCmd( uint16_t Flags )
{
	int length = sizeof (uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_FDR_EVPROC;
		m_Data[m_WriteCount++] = Flags;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetFdrEventProcessingData:
///		Gets the FDR detector event processing flags command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pFlags		Processing flag for each detector (bit number = detector number).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetFdrEventProcessingData( uint16_t *pFlags )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Set the command and data.
		*pFlags = (int)m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetFdrStopRawSendCmd:
///		Sets the FDR stop sending raw data command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetFdrStopRawSendCmd()
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_FDR_RAWOFF;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//////////////////////////////////////////////////////////////////////////////////////
//	Commands for the detector systems.

//	SetDetLThreshV:
///		Sets the detector lower threshold voltage command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command uint16_t and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\param[in]		Volts		Lower threhold voltage setting (Volts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetLThreshVCmd( int Det, float Volts )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command uint16_t.
		if ( Det == 0 )
			m_Data[m_WriteCount++] = CIP_DPS_SETLTHRESH1;
		else
			m_Data[m_WriteCount++] = CIP_DPS_SETLTHRESH2;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t)(Volts * 1000.0f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetDetUThreshVCmd:
///		Sets the detector upper threshold voltage command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\param[in]		Volts		Upper threhold voltage setting (Volts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetUThreshVCmd( int Det, float Volts )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		if ( Det == 0 )
			m_Data[m_WriteCount++] = CIP_DPS_SETUTHRESH1;
		else
			m_Data[m_WriteCount++] = CIP_DPS_SETUTHRESH2;

		// Set the data.
		m_Data[m_WriteCount++] = (uint16_t)(Volts * 1000.0f);

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetDetThreshVData:
///		Gets the detector upper or lower threshold voltage command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pVolts		Threshold voltage in volts.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetDetThreshVData( float *pVolts )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Set the command and data.
		*pVolts = (float)m_Data[m_ReadCount++] * 0.001f;
	}

	// Return status.
	return rval;
}

//	GetDetThreshMvData:
///		Gets the detector upper or lower threshold voltage command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		mVolts		Threshold voltage in millivolts.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetDetThreshMvData( uint16_t *mVolts )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Set the command and data.
		*mVolts = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetDetGridVCmd:
///		Sets the detector grid voltage command and data inot the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\param[in]		Volts		Grid voltage setting (Volts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetGridVCmd( int Det, uint16_t Volts )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		if ( Det == 0 )
			m_Data[m_WriteCount++] = CIP_DPS_SETGRID1;
		else
			m_Data[m_WriteCount++] = CIP_DPS_SETGRID2;

		// Set the data.
		m_Data[m_WriteCount++] = Volts;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetDetDriftVCmd:
///		Sets the detector drift voltage command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\param[in]		Volts		Drift voltage setting (Volts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetDriftVCmd( int Det, uint16_t Volts )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		if ( Det == 0 )
			m_Data[m_WriteCount++] = CIP_DPS_SETDRIFT1;
		else
			m_Data[m_WriteCount++] = CIP_DPS_SETDRIFT2;

		// Set the data.
		m_Data[m_WriteCount++] = Volts;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetDetPmtVCmd:
///		Sets the detector PMT voltage command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\param[in]		Volts		PMT voltage setting (Volts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetPmtVCmd( int Det, uint16_t Volts )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		if ( Det == 0 )
			m_Data[m_WriteCount++] = CIP_DPS_SETPMT1;
		else
			m_Data[m_WriteCount++] = CIP_DPS_SETPMT2;

		// Set the data.
		m_Data[m_WriteCount++] = Volts;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetDetHighVData:
///		Gets the detector grid, drift, or PMT voltage command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pVolts		High voltage voltage in volts.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetDetHighVData( uint16_t *pVolts )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Set the command and data.
		*pVolts = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetDetChanCoeffCmd:
///		Sets the channel peak detection parameters command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\param[in]		Step		Number of channels between leading and trailing ADC samples.
///		\param[in]		Thresh		Threshold of difference between leading and trailing ADC samples (counts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetChanCoeffCmd( int Det, uint16_t Step, uint16_t Thresh )
{
	int length = sizeof( uint16_t ) * 4;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_FDR_CHANCOEFF;

		// Set the detector number.
		m_Data[m_WriteCount++] = (uint16_t)Det;

		// Set the step size and threshold.
		m_Data[m_WriteCount++] = Step;
		m_Data[m_WriteCount++] = Thresh;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetDetChanCoeffData:
///		Gets the channel peak detection parameters command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[in]		pDet		Identifies which detector command pertains to.
///		\param[out]		pStep		Number of channels between leading and trailing ADC samples.
///		\param[out]		pThresh		Threshold of difference between leading and trailing ADC samples (counts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetDetChanCoeffData( int *pDet, uint16_t *pStep, uint16_t *pThresh )
{
	int length = sizeof (uint16_t ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the detector number.
		*pDet = (int)m_Data[m_ReadCount++];

		// Set the command and data.
		*pStep = m_Data[m_ReadCount++];
		*pThresh = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetDetCenterOffsetCmd:
//		Sets the detector radial fit center x,y offset command and data.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\param[in]		XOffset		Offset of model center from the physical detector center (mm).
///		\param[in]		YOffset		Offset of model center from the physical detector center (mm).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetCenterOffsetCmd( int Det, float XOffset, float YOffset )
{
	int length = sizeof( uint16_t ) * 2 + sizeof( float ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_FDR_POSORIGIN;

		// Set the detector number.
		m_Data[m_WriteCount++] = (uint16_t)Det;

		// Set the offsets.
		WriteFloat( XOffset );
		WriteFloat( YOffset );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetDetCenterOffsetData:
///		Gets the channel peak detection parameters command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[in]		pDet		Identifies which detector command pertains to.
///		\param[out]		pXOffset	Offset of model center from the physical detector center (mm).
///		\param[out]		pYOffset	Offset of model center from the physical detector center (mm).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetDetCenterOffsetData( int *pDet, float *pXOffset, float *pYOffset )
{
	int length = sizeof( uint16_t ) + sizeof ( float ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the detector number.
		*pDet = (int)m_Data[m_ReadCount++];

		// Set the command and data.
		*pXOffset = ReadFloat();
		*pYOffset = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetDetPosCoeffCmd:
///		Sets the detector radial position binomial fit coefficients command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\param[in]		ACoeff		The R**2 coefficient.
///		\param[in]		BCoeff		The R coefficient.
///		\param[in]		CCoeff		The constant coefficient.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetPosCoeffCmd( int Det, float ACoeff, float BCoeff, float CCoeff )
{
	int length = sizeof( uint16_t ) * 2 + sizeof( float ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_FDR_POSCOEFF;

		// Set the detector number.
		m_Data[m_WriteCount++] = (uint16_t)Det;

		// Set the coefficients.
		WriteFloat( ACoeff );
		WriteFloat( BCoeff );
		WriteFloat( CCoeff );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetDetPosCoeffData:
///		Gets the detector radial position binomial fit coefficients command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[in]		pDet		Identifies which detector command pertains to.
///		\param[out]		pACoeff		The R**2 coefficient.
///		\param[out]		pBCoeff		The R coefficient.
///		\param[out]		pCCoeff		The constant coefficient.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetDetPosCoeffData( int *pDet, float *pACoeff, float *pBCoeff, float *pCCoeff )
{
	int length = sizeof( uint16_t ) + sizeof ( float ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the detector number.
		*pDet = (int)m_Data[m_ReadCount++];

		// Set the command and data.
		*pACoeff = ReadFloat();
		*pBCoeff = ReadFloat();
		*pCCoeff = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetDetEnergyCoeffCmd:
///		Sets the detector energy binomial fit coefficients command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\param[in]		ACoeff		The R**2 coefficient.
///		\param[in]		BCoeff		The R coefficient.
///		\param[in]		CCoeff		The constant coefficient.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetEnergyCoeffCmd( int Det, float ACoeff, float BCoeff, float CCoeff )
{
	int length = sizeof( uint16_t ) * 2 + sizeof( float ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_FDR_ECOEFF;

		// Set the detector number.
		m_Data[m_WriteCount++] = (uint16_t)Det;

		// Set the coefficients.
		WriteFloat( ACoeff );
		WriteFloat( BCoeff );
		WriteFloat( CCoeff );

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetDetEnergyCoeffData:
///		Gets the detector energy binomial fit coefficients command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[in]		pDet		Identifies which detector command pertains to.
///		\param[out]		pACoeff		The R**2 coefficient.
///		\param[out]		pBCoeff		The R coefficient.
///		\param[out]		pCCoeff		The constant coefficient.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetDetEnergyCoeffData( int *pDet, float *pACoeff, float *pBCoeff, float *pCCoeff )
{
	int length = sizeof( uint16_t ) + sizeof ( float ) * 3;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Get the detector number.
		*pDet = (int)m_Data[m_ReadCount++];

		// Set the command and data.
		*pACoeff = ReadFloat();
		*pBCoeff = ReadFloat();
		*pCCoeff = ReadFloat();
	}

	// Return status.
	return rval;
}

//	SetDetDefaultVoltages:
///		Sets the detector default voltage settings command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\param[in]		LThresh		Detector trigger lower threshold voltage (Volts).
///		\param[in]		UThresh		Detector upper level veto threshold voltage (Volts).
///		\param[in]		Grid		Detector grid voltage (Volts).
///		\param[in]		Drift		Detector drift voltage (Volts).
///		\param[in]		Pmt			Detector PMT voltage (Volts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetDefaultVoltagesCmd( int Det, float LThresh, float UThresh, uint16_t Grid,
										uint16_t Drift, uint16_t Pmt )
{
	int length = sizeof( uint16_t ) * 6;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		if ( Det == 0 )
			m_Data[m_WriteCount++] = CIP_DPS_SETDEFV1;
		else
			m_Data[m_WriteCount++] = CIP_DPS_SETDEFV2;

		// Set the five voltages.
		m_Data[m_WriteCount++] = (uint16_t)(LThresh * 1000.0f);
		m_Data[m_WriteCount++] = (uint16_t)(UThresh * 1000.0f);
		m_Data[m_WriteCount++] = Grid;
		m_Data[m_WriteCount++] = Drift;
		m_Data[m_WriteCount++] = Pmt;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetDetDefaultVoltagesData:
///		Gets the detector default voltage settings command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pLThresh	Detector trigger lower threshold voltage (Volts).
///		\param[out]		pUThresh	Detector upper level veto threshold voltage (Volts).
///		\param[out]		pGrid		Detector grid voltage (Volts).
///		\param[out]		pDrift		Detector drift voltage (Volts).
///		\param[out]		pPmt		Detector PMT voltage (Volts).
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetDetDefaultVoltagesData( float *pLThresh, float *pUThresh, uint16_t *pGrid,
		uint16_t *pDrift, uint16_t *pPmt )
{
	int length = sizeof ( uint16_t ) * 5;

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Set the command and data.
		*pLThresh = (float)m_Data[m_ReadCount++] * 0.001f;
		*pLThresh = (float)m_Data[m_ReadCount++] * 0.001f;
		*pGrid = m_Data[m_ReadCount++];
		*pDrift = m_Data[m_ReadCount++];
		*pPmt = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

//	SetDetRestoreDefaultsCmd:
///		Sets the current voltages to the saved defaults command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetRestoreDefaultsCmd( int Det )
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word, based on which detector we're sending to.
		if ( Det == 0 )
			m_Data[m_WriteCount++] = CIP_DPS_GETDEFAULT1;
		else
			m_Data[m_WriteCount++] = CIP_DPS_GETDEFAULT2;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetDetSaveDefaultsCmd:
///		Sets the save the current parameters and default voltages command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetSaveDefaultsCmd( int Det )
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word, based on which detector we're sending to.
		if ( Det == 0 )
			m_Data[m_WriteCount++] = CIP_DPS_SETDEFAULT1;
		else
			m_Data[m_WriteCount++] = CIP_DPS_SETDEFAULT2;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetDetFifoResetCmd:
///		Sets the detector reset both event FIFOs command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetFifoResetCmd()
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_DPS_SETFIFORST;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetDetZeroHvCmd:
///		Sets the zero a single detector's high voltages command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det			Identifies which detector command pertains to.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetZeroHvCmd( int Det )
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word, based on which detector we're sending to.
		if ( Det == 0 )
			m_Data[m_WriteCount++] = CIP_DPS_SETZERO1;
		else
			m_Data[m_WriteCount++] = CIP_DPS_SETZERO2;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetDetZeroDpsHvCmd:
///		Sets the zero both of this DPS's detectors' high voltages command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetZeroDpsHvCmd()
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word.
		m_Data[m_WriteCount++] = CIP_DPS_SETHVZERO;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetDetProcessingStateCmd:
///		Sets a single detector's event processing state command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		Det				Identifies which detector command pertains to.
///		\param[in]		ProcessEvents	Flag indicating if events are to be processed (TRUE) or not.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetDetProcessingStateCmd( int Det, bool ProcessEvents )
{
	int length = sizeof( uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word, based on which detector we're sending to.
		if ( Det == 0 )
		{
			if ( ProcessEvents )
				m_Data[m_WriteCount++] = CIP_DPS_SETDET1PROCON;
			else
				m_Data[m_WriteCount++] = CIP_DPS_SETDET1PROCOFF;
		}
		else
		{
			if ( ProcessEvents )
				m_Data[m_WriteCount++] = CIP_DPS_SETDET2PROCON;
			else
				m_Data[m_WriteCount++] = CIP_DPS_SETDET2PROCOFF;
		}

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetDetEventControl:
///		Sets a single detector's event processing state command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		idet				Identifies which detector command pertains to.
///		\param[in]		eventEnable	Flag to enable event triggers.
///   \param[in]    keepVetoed  Flag to retain events after veto is triggered.
///   \param[in]    intEnable   Flag to enable event FIFO interrupts.
///   \param[in]    calEnable   Flag to enable calibration input.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::setDetEventControl( int idet, bool eventEnable,
    bool keepVetoed, bool intEnable, bool calEnable )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word, based on which detector we're sending to.
		if ( idet == 0 )
      m_Data[m_WriteCount++] = CIP_DPS_SETEVCTL1;
		else
      m_Data[m_WriteCount++] = CIP_DPS_SETEVCTL2;

    // Calculate the output value based on the flags.
    uint16_t val = 0;
    if ( eventEnable )
      val |= 0x8000;
    if ( keepVetoed )
      val |= 0x2000;
    if ( intEnable )
      val |= 0x1000;
    if ( calEnable )
      val |= 0x0800;

    // Set the output value.
    m_Data[m_WriteCount++] = val;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetDetEventIntThreshold:
///		Sets a single detector's event FIFO interrupt threshold command into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		idet			Identifies which detector command pertains to.
///		\param[in]		thresh    # event in FIFO before it issues interrupt.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::setDetEventIntThreshold( int idet, int thresh )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word, based on which detector we're sending to.
		if ( idet == 0 )
      m_Data[m_WriteCount++] = CIP_DPS_SETITHRESH1;
		else
      m_Data[m_WriteCount++] = CIP_DPS_SETITHRESH2;

    // Set the output value.
    m_Data[m_WriteCount++] = (uint16_t)thresh;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	setDetTriggerHoldoff:
///		Sets a single detector's trigger holdoff.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		idet			Identifies which detector command pertains to.
///		\param[in]		trigHoldoff    Trigger holdoff selection.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::setDetTriggerHoldoff( int idet, int trigHoldoff )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word, based on which detector we're sending to.
		if ( idet == 0 )
      m_Data[m_WriteCount++] = CIP_DPS_SETHOLDOFF1;
		else
      m_Data[m_WriteCount++] = CIP_DPS_SETHOLDOFF2;

    // Set the output value.
    m_Data[m_WriteCount++] = (uint16_t)trigHoldoff;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	setDetEventIntLowThreshold:
///		Sets a single detector's event FIFO interrupt re-enable threshold into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\param[in]		idet			Identifies which detector command pertains to.
///		\param[in]		thresh    # event left in FIFO before it re-enables interrupt.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::setDetEventIntLowThreshold( int idet, int thresh )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command word, based on which detector we're sending to.
		if ( idet == 0 )
      m_Data[m_WriteCount++] = CIP_DPS_SETILTHRESH1;
		else
      m_Data[m_WriteCount++] = CIP_DPS_SETILTHRESH2;

    // Set the output value.
    m_Data[m_WriteCount++] = (uint16_t)thresh;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	GetDetFpgaRegister:
///		Gets any FPGA register command data from the packet.
//
///		This routine first checks that adequate space is available in the packet, then reads
///		the correct command data.
///		\param[out]		pReg    Register setting.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::GetDetFpgaRegister( uint16_t *pReg )
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// Set the command and data.
		*pReg = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

int HeroCommandPacket::SetDetSwTriggerEnable( bool enable0, bool enable1 )
{
	int length = sizeof( uint16_t ) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
  {
		// Set the commands.
    if (enable0)
      m_Data[m_WriteCount++] = CIP_DPS_SETSWTRIGON1;
    else
      m_Data[m_WriteCount++] = CIP_DPS_SETSWTRIGOFF1;

    if (enable1)
      m_Data[m_WriteCount++] = CIP_DPS_SETSWTRIGON2;
    else
      m_Data[m_WriteCount++] = CIP_DPS_SETSWTRIGOFF2;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetShutdownCmd:
///		Sets the universal shutdown command and data into the packet.
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetShutdownCmd()
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_SHUTDOWN;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetFdrTurnHeaterOn:
///		Turns the Heater inside FDR on
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetFdrTurnHeaterOn()
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_FDR_HEATERON;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

//	SetFdrTurnHeaterOff:
///		Turns the Heater inside FDR on
//
///		This routine first checks that adequate space is available in the packet, then inserts
///		the correct command word and data.
///		\return			If the packet exists and has room, this routine returns 0, otherwise
///						it returns a non-zero error code.
int HeroCommandPacket::SetFdrTurnHeaterOff()
{
	int length = sizeof (uint16_t );

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command.
		m_Data[m_WriteCount++] = CIP_FDR_HEATEROFF;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;
	}

	// Return status.
	return rval;
}

int HeroCommandPacket::SetHkGatewayIoConfig(char hk, char cip)
{
	int length = sizeof (uint16_t) * 2;
	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// Set the command
		m_Data[m_WriteCount++] = CIP_HK_CONFIG_IO;
		m_Data[m_WriteCount++] = hk;
		m_Data[m_WriteCount++] = cip;

		// Increment the lengths.
		m_Header->Length += length;
		m_PacketLength += length;

	}

	// Return status.
	return rval;
}

int HeroCommandPacket::GetHkGatewayIoConfig(char* hk, char* cip)
{
	int length = sizeof (uint16_t);

	int rval;
	if ( !(rval = VerifyPacketForRead( length )) )
	{
		// get the data.
		*hk = m_Data[m_ReadCount++];
		*cip = m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}

int HeroCommandPacket::SetClockSetThresh(uint16_t thresh)
{
	int length = sizeof (uint16_t) * 2;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// set the command
		m_Data[m_WriteCount++] = CIP_TIME_THRESHOLD;
		m_Data[m_WriteCount++] = thresh;
	}

	// Increment the lengths.
	m_Header->Length += length;
	m_PacketLength += length;

	// Return status.
	return rval;
}

int HeroCommandPacket::GetClockSetThresh(uint16_t *thresh)
{
	int length = sizeof (uint16_t) * 2;

	if(thresh == NULL)
		return -1;

	int rval;
	if ( !(rval = VerifyPacketForWrite( length )) )
	{
		// get the data
		*thresh =  m_Data[m_ReadCount++];
	}

	// Return status.
	return rval;
}
