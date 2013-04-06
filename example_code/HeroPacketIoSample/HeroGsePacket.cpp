//	HeroGsePacket.cpp:
///		\file HeroGsePacket.cpp
///		C++ source file defining the behavior of the HeroGsePacket class.
//		This class encapsulates all the Gse packet behavior specific to the AmpStar
//		project, leaving the EcGsePacket class generic for easier inclusion into
//		other projects.

#include "StdAfx.h"
#include "HeroGsePacket.h"
#include "ErrorCodes.h"
//#include "GpsData.h"
#include "CtlAdcConverter.h"
#include "AspAdcConverter.h"
#include "EhkAdcConverter.h"
#include <math.h>
#include <string.h>

//	HeroGsePacket:
///		Default constructor.  Calls the base class constructor.
HeroGsePacket::HeroGsePacket() : EcGsePacket()
{
	// Initially, packet is reset.
	m_Reset = true;
}

//	~HeroGsePacket:
///		Destructor.
HeroGsePacket::~HeroGsePacket()
{
}

//	HeroGsePacket:
///		More intelligent constructor.  Calls the base class constructor.
HeroGsePacket::HeroGsePacket( int type ) : EcGsePacket()
{
	m_Reset = true;

	// Reset packet and create it.
	Create( type );
}

//	VerifyType:
///		Verifies the type and returns the packet length and notification message, if any.
//
///		This routine overriddes to base class to return 0 and fill in the two return parameters
///		with valid information if the type is valid; otherwise, it will return a non-zero error
///		code.  This version also sets the internal m_VariableLength flag.
///		\param[in]		type			Type of GSE packet to create.
///		\param[out]		pDataLength		Pointer to the data length of the packet.
///		\param[out]		pMessage		Pointer to the notification message.
///		\param[out]		pVariableLength	Flag indicating if packet can vary in length less than max length.
///		\return			This version always returns HGSEPKTERR_TYPE.
int HeroGsePacket::VerifyType( uint8_t type, int *pDataLength, uint32_t *pMessage, bool *pVariableLength )
{
	int rval = 0;

	// Most commands do not have variable length.
	*pVariableLength = false;

	// Set the data length and message for each valid type.
	switch (type )
	{

	case GSEPTYPE_IDLE:
		*pDataLength = GSEPLEN_IDLE;
		*pMessage = WM_GSEP_IDLE;
		break;

	case GSEPTYPE_HKCMDREPLY:
		*pDataLength = sizeof( uint16_t );
		*pMessage = WM_GSEP_HKCMDREPLY;
		break;

	case GSEPTYPE_HKCMDWORD:		// Echo of each word received on the CIP.
		*pDataLength = sizeof( uint16_t );
		*pMessage = WM_GSEP_HKCMDWORD;
		break;

	case GSEPTYPE_CTLSYSDATA:		// Control system data (temps, pressures, etc).
		*pDataLength = sizeof( PKT_CTLSYSDATA_STRUCT );
		*pMessage = WM_GSEP_CTLSYSDATA;
		break;

	case GSEPTYPE_CTLASPDATA:		// Control system aspect data.
		*pDataLength = sizeof( PKT_CTLASPDATA_STRUCT );
		*pMessage = WM_GSEP_CTLASPDATA;
		break;

	case GSEPTYPE_CTLCTLDATA:		// Control system control loop data data.
		*pDataLength = sizeof( CTL_CTLDATA_STRUCT );
		*pMessage = WM_GSEP_CTLCTLDATA;
		break;

	case GSEPTYPE_CTLADU5:			// Control system ADU5 DGPS packet.
		*pDataLength = GSEPLEN_CTLADU5;
		*pMessage = WM_GSEP_CTLADU5;
		*pVariableLength = true;
		break;

	case GSEPTYPE_MOTGPS:			// Motorola GPS packet.
		*pDataLength = GSEPLEN_MOTGPS;
		*pMessage = WM_GSEP_MOTGPS;
		*pVariableLength = true;
		break;

	case GSEPTYPE_CTLPARAM:			// Control system control loop Parameters.
		*pDataLength = sizeof( CTL_PARAMDATA_STRUCT);
		*pMessage = WM_GSEP_CTLPARAM;
		m_Data[0] = 0;
		m_Data[1] = 0;
		break;

	case GSEPTYPE_CTLPARAM2:			// Control system control loop Parameters.
		*pDataLength = sizeof( CTL_PARAMDATA2_STRUCT);
		*pMessage = WM_GSEP_CTLPARAM2;
		m_Data[0] = 0;
		m_Data[1] = 0;
		break;

	case GSEPTYPE_CTLBRS:			// Control system report of Bearing Rotation Sensor.
		*pDataLength = sizeof( uint16_t );
		*pMessage = WM_GSEP_CTLBRS;
		break;

	case GSEPTYPE_H3000MAG:			// CTL Honeywell H3000 magnetometer packet.
		*pDataLength = GSEPLEN_H3000MAG;
		*pMessage = WM_GSEP_H3000MAG;
		*pVariableLength = true;
		break;

	case GSEPTYPE_CTLTANS:			// Control system Trimble TANS packet.
		*pDataLength = GSEPLEN_CTLTANS;
		*pMessage = WM_GSEP_CTLTANS;
		*pVariableLength = true;
		break;

	case GSEPTYPE_ASPSYSDATA:		// Aspect system status.
		*pDataLength = sizeof( PKT_ASPSYSDATA_STRUCT );
		*pMessage = WM_GSEP_ASPSYSDATA;
		break;

	case GSEPTYPE_ASPPARAM:			// Aspect parameters.
		*pDataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );
		*pMessage = WM_GSEP_ASPPARAM;
		break;

	case GSEPTYPE_ASPIMAGE:			// Image information sent to ground.
		*pDataLength = sizeof( PKT_ASPIMDATA_STRUCT ) + sizeof( PKT_ASPIMOBJ_STRUCT ) * ASPIM_MAXNOBJ;
		*pMessage = WM_GSEP_ASPIMAGE;
		*pVariableLength = true;
		break;

	case GSEPTYPE_ASPFOCUS:			// Focus loop data.
		*pDataLength = sizeof( PKT_ASPFOCUSDATA_STRUCT );
		*pMessage = WM_GSEP_ASPFOCUS;
		break;

	case GSEPTYPE_ASPCAMSTATUS:			// Focus loop data.
		*pDataLength = sizeof( PKT_ASPCAMSTATUS_STRUCT );
		*pMessage = WM_GSEP_ASPCAMSTATUS;
		break;

	case GSEPTYPE_ASPMODECHANGE:			// Focus loop data.
		*pDataLength = sizeof( PKT_ASPMODECHANGE_STRUCT );
		*pMessage = WM_GSEP_ASPMODECHANGE;
		break;

	case GSEPTYPE_ASPRAWIMAGE:		// Aspect Camera's raw image
		*pDataLength = GSEPKT_MAX_DATA_SIZE;
		*pMessage = WM_GSEP_ASPRAWIMAGE;
		*pVariableLength = true;
		break;

	case GSEPTYPE_EHKSYSDATA:
		*pDataLength = sizeof( PKT_EHKSYSDATA_STRUCT );
		*pMessage = WM_GSEP_EHKSYSDATA;
		break;

	case GSEPTYPE_EHKPARAM:
		*pDataLength = sizeof( PKT_EHKHTRPARAMDATA_STRUCT ) +
      sizeof( PKT_EHKBALMOTORPARAMDATA_STRUCT);
		*pMessage = WM_GSEP_EHKPARAM;
		break;

	case GSEPTYPE_DPSSTATUS:		// Detector system status and V readings.
		*pDataLength = sizeof( PKT_DPSSTATUSDATA_STRUCT );
		*pMessage = WM_GSEP_DPSSTATUS;
		break;

	case GSEPTYPE_DPSCURSET:		// Detector current V settings and parameters.
		*pDataLength = sizeof( PKT_DETPARAM_STRUCT ) * 2;
		*pMessage = WM_GSEP_DPSCURSET;
		break;

	case GSEPTYPE_DPSDEFSET:		// Detector default V settings and parameters.
		*pDataLength = sizeof( PKT_DETPARAM_STRUCT ) * 2;
		*pMessage = WM_GSEP_DPSDEFSET;
		break;

	case GSEPTYPE_DPSTEMPRS:		// Detector temperatures and pressures.
		*pDataLength = sizeof( PKT_DPSTPDATA_STRUCT );
		*pMessage = WM_GSEP_DPSTEMPRS;
		break;

	case GSEPTYPE_DPSEVENT:			// Detector processed event data.
		*pDataLength = sizeof( DET_EVENT_DATA );
		*pMessage = WM_GSEP_DPSEVENT;
		break;

	case GSEPTYPE_DPSCLKSYNC:		// Clock synchronization data for DPS only.
		*pDataLength = sizeof( PKT_DPSCLKSYNC_STRUCT );
		*pMessage = WM_GSEP_DPSCLKSYNC;
		break;

	case GSEPTYPE_CLOCKSYNC:		// Clock synchronization data for all other systems.
		*pDataLength = sizeof( PKT_CLKSYNC_STRUCT );
		*pMessage = WM_GSEP_CLOCKSYNC;
		break;

	case GSEPTYPE_NTCSYNC:		  // NetworkTimeClient synchronization data for all other systems.
		*pDataLength = sizeof( PKT_NTCDATA_STRUCT );
		*pMessage = WM_GSEP_NTCSYNC;
		break;

	case GSEPTYPE_ERROR:			// Error messages.
		*pDataLength = GSEPLEN_ERROR;
		*pMessage = WM_GSEP_ERROR;
		break;

	case GSEPTYPE_CMDACK:			// Command acknowledgement.
		*pDataLength = sizeof( GSE_CMDACK_STRUCT );
		*pMessage = WM_GSEP_CMDACK;
		break;

	case GSEPTYPE_DIAG:				// Generic diagnostic packet.
		*pDataLength = GSEPLEN_DIAG;
		*pMessage = WM_GSEP_DIAG;
		break;

	case GSEPTYPE_GSEGATE_DATA:		// GSE Gateway Statistics
		*pDataLength = GSEPLEN_GSEGATE_MAX_DATA;
		*pMessage = WM_GSEP_GSEGATE_DATA;
		*pVariableLength = true;
		break;

	case GSEPTYPE_CIPGATE_DATA:		// GSE Gateway Statistics
		*pDataLength = GSEPLEN_CIPGATE_DATA;
		*pMessage = WM_GSEP_CIPGATE_DATA;
		*pVariableLength = false;
		break;

	case GSEPTYPE_FDRSYSDATA:	// FDR Stats
		*pDataLength = sizeof ( PKT_FDRSYSDATA_STRUCT );
		*pMessage = WM_GSEP_FDRSYSDATA;
		break;

	case GSEPTYPE_PPSSYNC:		// PPS Sync status
		*pDataLength = sizeof ( PKT_PPSSYNCDATA_STRUCT );
		*pMessage = WM_GSEP_PPSSYNC;
		break;

	case GSEPTYPE_PPSSYNC2:		// PPS Sync2 status
		*pDataLength = sizeof ( PKT_PPSSYNC2DATA_STRUCT );
		*pMessage = WM_GSEP_PPSSYNC2;
		break;

	case GSEPTYPE_HDLC_STATS:
		*pDataLength = sizeof ( PKT_HDLC_DATA_STRUCT );
		*pMessage = WM_GSEP_HDLC_STAT;
		break;

	case GSEPTYPE_FDRDETFILE:
		*pDataLength = sizeof (PKT_FDRDETFILEDATA_STRUCT);
		*pMessage = WM_GSEP_FDRDETFILEDATA;
		break;

	// Unknown packet type case.
	default:
		rval = HGSEPKTERR_TYPE;
	}

	// Return the code.
	return rval;
}

//	VerifyPacketForSet:
///		Verifies that a packet of the given type exists.
int HeroGsePacket::VerifyPacketForSet( uint8_t type )
{
	// If no packet, return an error.
	if ( m_Packet == NULL )
		return HGSEPKTERR_NOPKT;

	// If the packet length isn't at least the size of the header, send an error.
	if ( m_PacketLength < (int)sizeof( EcGsePacket::Header ) )
		return HGSEPKTERR_INCOMPLETE;

	// If the packet type isn't correct, return the error.
	if ( m_Header->Type != type )
		return HGSEPKTERR_OPTYPE;

	// Return without error.
	return 0;
}

//	VerifyPacketForGet:
///		Verifies that a packet of the given type exists.
int HeroGsePacket::VerifyPacketForGet( uint8_t type, int length )
{
	// If no packet, return an error.
	if ( m_Packet == NULL )
		return HGSEPKTERR_NOPKT;

	// If the packet length isn't at least the size of the header, send an error.
	if ( m_PacketLength < (int)sizeof( EcGsePacket::Header ) )
		return HGSEPKTERR_INCOMPLETE;

	// If the packet type isn't correct, return the error.
	if ( m_Header->Type != type )
		return HGSEPKTERR_OPTYPE;

	// If the length isn't correct, return the error.
	if ( (m_VariableLength && (length > m_DataLength || m_Header->DataLength > length)) ||
		(!m_VariableLength && m_Header->DataLength != length) )
		return HGSEPKTERR_LENGTH;

	// Return without error.
	return 0;
}

//	SetCmdReply:
///		Sets the sequence number of the command packet into the command reply packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then inserts the command packet
///		sequence number into the packet in the correct position, updates the data length and
///		packet length of the packet, and any flags necessary.
///		\param[in]		seqNum		Sequence number to insert into packet.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCmdReply( uint16_t seqNum )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_HKCMDREPLY )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		uint16_t *Data = (uint16_t *)m_Data;
		Data[0] = seqNum;
		m_Header->DataLength = sizeof( uint16_t );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;
	}

	// Return without error.
	return rval;
}

//	GetCmdReply:
///		Gets the sequence number of the command packet from the command reply packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the command packet
///		sequence number from the packet in the correct position.
///		\param[out]		pSeqNum		Sequence number read from packet.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetCmdReply( uint16_t* pSeqNum )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_HKCMDREPLY, sizeof( uint16_t ) )) )
	{
		// Get the data.
		uint16_t *Data = (uint16_t *)m_Data;
		*pSeqNum = Data[0];
	}

	// Return status.
	return rval;
}

//	SetCmdWord:
///		Sets the packet word from an incoming command packet into the command word packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then inserts the data int the
///		packet, updates the data length and packet length of the packet, and any flags necessary.
///		\param[in]		CmdWord		Command packet word to insert into packet.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCmdWord( uint16_t CmdWord )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_HKCMDWORD )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		uint16_t *Data = (uint16_t *)m_Data;
		Data[0] = CmdWord;
		m_Header->DataLength = sizeof( uint16_t );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;
	}

	// Return status.
	return rval;
}

//	GetCmdWord:
///		Gets the command packet word from the command word packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		pCmdWord		Command word read from packet.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetCmdWord( uint16_t* pCmdWord )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_HKCMDWORD, sizeof( uint16_t ) )) )
	{
		// Get the data.
		uint16_t *Data = (uint16_t *)m_Data;
		*pCmdWord = Data[0];
	}

	// Return status.
	return rval;
}

//	SetCtlSysMuxTempData:
///		Sets a single mux temperature into the CTL system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then inserts the data int the
///		packet, updates the data length and packet length of the packet, and any flags necessary.
///		\param[in]		Index		Index of current temperature in MUX array.
///		\param[in]		Data		Raw thermistor data for this MUX temperature.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlSysMuxTempData( int Index, uint16_t Data )
{
	// Constrain the index.
  Index = Index > 0 ? Index : 0;
	Index = Index < 15 ? Index : 15;

	// Verify the packet is the correct type.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLSYSDATA )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		m_Header->DataLength = sizeof( PKT_CTLSYSDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;

		// Get a pointer to the packet data.
		PKT_CTLSYSDATA_STRUCT *pData = (PKT_CTLSYSDATA_STRUCT *)m_Data;

		// Set the data.
		pData->MuxTemp[Index] = Data;
	}

	// Return status.
	return rval;
}

//	SetCtlSysTempData:
///		Sets several raw ADC Temp values into the CTL system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then inserts the data int the
///		packet, updates the data length and packet length of the packet, and any flags necessary.
///		\param[in]		cpuTemp		Raw ADC reading of CTL CPU heat sink temperature.
///		\param[in]		tankTemp	Raw ADC reading of CTL tank temperature.
///		\param[in]		azDacTemp	Raw ADC reading of Azimuth DAC tank temperature.
///		\param[in]		elDacTemp	Raw ADC reading of Elevation DAC tank temperature.
///		\param[in]		azGyroTemp	Raw ADC reading of azimuth gyro tank temperature.
///		\param[in]		elGyroTemp	Raw ADC reading of elevation gyro tank pressure.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlSysTempData( uint16_t cpuTemp, uint16_t tankTemp,
		uint16_t azDacTemp, uint16_t elDacTemp, uint16_t azGyroTemp, uint16_t elGyroTemp )
{
	// Verify the packet is the correct type.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLSYSDATA )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		m_Header->DataLength = sizeof( PKT_CTLSYSDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;

		// Get a pointer to the packet data.
		PKT_CTLSYSDATA_STRUCT *pData = (PKT_CTLSYSDATA_STRUCT *)m_Data;

		// Set the data.
		pData->CtlTemp[0] = cpuTemp;
		pData->CtlTemp[1] = tankTemp;
		pData->CtlTemp[2] = azDacTemp;
		pData->CtlTemp[3] = elDacTemp;
		pData->CtlTemp[4] = azGyroTemp;
		pData->CtlTemp[5] = elGyroTemp;
	}

	// Return status.
	return rval;
}

//	SetCtlSysMiscData:
///		Sets several miscellaneous raw ADC values into the CTL system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then inserts the data int the
///		packet, updates the data length and packet length of the packet, and any flags necessary.
///		\param[in]		azGyroP		Raw ADC reading of Azimuth gyro tank pressure.
///		\param[in]		elGyroP		Raw ADC reading of Elevation gyro tank pressure.
///		\param[in]		elDacP		Raw ADC reading of Elevation DAC tank pressure.
///		\param[in]		azDacP		Raw ADC reading of Azimuth DAC tank pressure.
///		\param[in]		tankP			Raw ADC reading of CTL tank pressure.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlSysPressData( uint16_t azGyroP, uint16_t elGyroP,
		uint16_t elDacP, uint16_t azDacP, uint16_t tankP )
{
	// Verify the packet is the correct type.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLSYSDATA )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		m_Header->DataLength = sizeof( PKT_CTLSYSDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;

		// Get a pointer to the packet data.
		PKT_CTLSYSDATA_STRUCT *pData = (PKT_CTLSYSDATA_STRUCT *)m_Data;

		// Set the data.
		pData->Press[0] = azGyroP;
		pData->Press[1] = elGyroP;
		pData->Press[2] = elDacP;
		pData->Press[3] = azDacP;
		pData->Press[4] = tankP;
	}

	// Return status.
	return rval;
}

//	SetCtlSysRelayData:
///		Sets the relay state bits into the CTL system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then inserts the data int the
///		packet, updates the data length and packet length of the packet, and any flags necessary.
///		\param[in]		RelayState		Relay states as bits.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlSysRelayData( uint8_t RelayState )
{
	// Verify the packet is the correct type.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLSYSDATA )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		m_Header->DataLength = sizeof( PKT_CTLSYSDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;

		// Get a pointer to the packet data.
		PKT_CTLSYSDATA_STRUCT *pData = (PKT_CTLSYSDATA_STRUCT *)m_Data;

		// Set the data.
		pData->RelayState = RelayState;
	}

	// Return status.
	return rval;
}

//	SetCtlSysDacSelectData:
///		Sets the DAC selection bits into the CTL system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then inserts the data int the
///		packet, updates the data length and packet length of the packet, and any flags necessary.
///		\param[in]		DacSelect	DAC selections as bits.
///		\param[in]		Data		Raw thermistor data for this MUX temperature.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlSysDacSelectData( uint8_t DacSelect )
{
	// Verify the packet is the correct type.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLSYSDATA )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		m_Header->DataLength = sizeof( PKT_CTLSYSDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;

		// Get a pointer to the packet data.
		PKT_CTLSYSDATA_STRUCT *pData = (PKT_CTLSYSDATA_STRUCT *)m_Data;

		// Set the data.
		pData->DacSelect = DacSelect;
	}

	// Return status.
	return rval;
}

//	SetCtlSysDioData:
///		Sets the DIO input bits into the CTL system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then inserts the data int the
///		packet, updates the data length and packet length of the packet, and any flags necessary.
///		\param[in]		flags		DIO bits for stow and EXIST power.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlSysDioData( uint8_t dioState )
{
	// Verify the packet is the correct type.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLSYSDATA )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		m_Header->DataLength = sizeof( PKT_CTLSYSDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;

		// Get a pointer to the packet data.
		PKT_CTLSYSDATA_STRUCT *pData = (PKT_CTLSYSDATA_STRUCT *)m_Data;

		// Set the data.
		pData->DioState = dioState;
	}

	// Return status.
	return rval;
}

//	GetCtlSysData:
///		Gets the CTL system data structure, converted into physical units, from the packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		pSysData		GSE system data structure to read from packet.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetCtlSysData( GSE_CTLSYSDATA_STRUCT &SysData )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_CTLSYSDATA, sizeof( PKT_CTLSYSDATA_STRUCT ) )) )
	{
		// Get a pointer to the raw data structure.
		PKT_CTLSYSDATA_STRUCT *pCtlData = (PKT_CTLSYSDATA_STRUCT *)m_Data;


		// Convert AD590 temps (temporary fit, not calibrated).
		double TSlope = 251.23 / 32768.0, TOffset = -55.0;
    int i;

		for ( i = 0; i < 16; i++ )
		{
			SysData.MuxTemp[i] = (double)(short)pCtlData->MuxTemp[i] * TSlope + TOffset;
		}

		// Convert the two additional temperatures.
		double tSlope = 500.0 / 32768.0, tOffset = -50.0;
		for ( i = 0; i < 6; i++ )
		{
			SysData.CtlTemp[i] = (double)(short)pCtlData->CtlTemp[i] * tSlope + tOffset;
		}

		// Convert the pressures.
		double pSlope = 72.516 / 32768.0, pOffset = 1.4503;
		for ( i = 0; i < 6; i++ )
		{
			SysData.Press[i] = (double)(short)pCtlData->Press[i] * pSlope + pOffset;
		}

		// Separate the flag values into the stow status and relay bits.
		SysData.EhkPower = (pCtlData->DioState & 0x01) == 0;
		SysData.BearingPower = (pCtlData->DioState & 0x02) == 0;
		SysData.SciTx = (pCtlData->DioState & 0x04) == 0;
    SysData.AspPower = (pCtlData->DioState & 0x08) == 0;
    SysData.FdrPower = (pCtlData->DioState & 0x10) == 0;
    SysData.WheelPower = (pCtlData->DioState & 0x20) == 0;
    SysData.ShaftPower = (pCtlData->DioState & 0x40) == 0;
    SysData.ElPower = (pCtlData->DioState & 0x80) == 0;

		SysData.RelayState = pCtlData->RelayState;

		SysData.AzShaftBackup = (pCtlData->DacSelect & 0x04) > 0;
		SysData.AzWheelBackup = (pCtlData->DacSelect & 0x08) > 0;
		SysData.ElBackup = (pCtlData->DacSelect & 0x10) > 0;
		SysData.DacSelect = pCtlData->DacSelect;
	}

	// Return status.
	return rval;
}

//	SetCtlAspRawAdcData:
///		Sets raw 16-bit ADC readings into the data structure.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets several raw ADC values into the
///		data packet using a PKT_CTLASPDATA_STRUCT pointer to access the packet data.
///		\param[in]		RollIncl		Raw reading from the roll inclinometer.
///		\param[in]		PitchIncl		Raw reading from the pitch inclinometer.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlAspRawAdcData( uint16_t RollIncl,
		uint16_t PitchIncl, uint16_t azTach, uint16_t muxAdcValue )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLASPDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_CTLASPDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		// Set the values.
		PKT_CTLASPDATA_STRUCT *pData = (PKT_CTLASPDATA_STRUCT *)m_Data;
		pData->ElPitch = (short)PitchIncl;
		pData->ElRoll = (short)RollIncl;
		pData->WheelTach = (int16_t)azTach;
		pData->MuxValue = muxAdcValue;

		m_Altered = true;
	}

	// Return status.
	return rval;
}

//	SetCtlAspFlags:
///		Sets status and mode information into the flag word of the ASP data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the flag value of the ASP data packet.
///		\param[in]		MagValid		Flag indicating magnetometer data are valid.
///		\param[in]		DgpsValid		Flag indicating DGPS data are valid.
///		\param[in]		GpsValid		Flag indicating GPS data are valid.
///		\param[in]		SaeValid		Flag indicating elevation SAE data are valid.
///		\param[in]		Im0Valid		Flag indicating camera 0 image data are valid.
///		\param[in]		EfrValid		Elevation FGG RIP valid flag.
///		\param[in]		AfrValid		Azimuth FGG RIP valid flag.
///		\param[in]		StartCam		Flag to command camera to start.
///		\param[in]		StopCam			Flag to command camera to stop.
///		\param[in]		CamActive		Camera data valid flag (for trimming positions).
///		\param[in]		CamStarted		Flag indicating ASP received command to start exposing.
///		\param[in]		CamReceived		Flag indicating camera image aspect data has been received.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlAspFlags( bool MagValid, bool DgpsValid,
		bool GpsValid, bool SaeValid, bool Im0Valid, bool EfrValid,
		bool AfrValid, bool RfrValid, bool StartCam, bool StopCam,
		bool CamActive, bool CamStarted, bool CamReceived, bool bearingState,
    bool Im1Valid )
{
	// Verify the packet type is correct, etc.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLASPDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_CTLASPDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		// Get a pointer to the data.
		PKT_CTLASPDATA_STRUCT *pData = (PKT_CTLASPDATA_STRUCT *)m_Data;

		// Set the flag values.
		pData->Flags = 0;
		if ( MagValid )
			pData->Flags |= 0x0001;
		if ( DgpsValid )
			pData->Flags |= 0x0002;
		if ( GpsValid )
			pData->Flags |= 0x0004;
		if ( SaeValid )
			pData->Flags |= 0x0008;
		if ( Im0Valid )
			pData->Flags |= 0x0010;
		if ( EfrValid )
			pData->Flags |= 0x0020;
		if ( AfrValid )
			pData->Flags |= 0x0040;
		if ( RfrValid )
			pData->Flags |= 0x0080;
		if ( StartCam )
			pData->Flags |= 0x0100;
		if ( StopCam )
			pData->Flags |= 0x0200;
		if ( CamActive )
			pData->Flags |= 0x0400;
		if ( CamStarted )
			pData->Flags |= 0x0800;
		if ( CamReceived )
			pData->Flags |= 0x1000;
		if ( bearingState )
			pData->Flags |= 0x2000;
		if ( Im1Valid )
			pData->Flags |= 0x4000;

		m_Altered = true;
	}

	// Return status.
	return rval;
}

//	SetCtlAspFlags:
///		Sets status and mode information into the flag word of the ASP data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the flag value of the ASP data packet.
///		\param[in]		PointingMode	Currently active pointing mode.
///		\param[in]		AzMode			Current pointing mode of azimuth axis.
///		\param[in]		ElMode			Current pointing mode of elevation axis.
///		\param[in]		MuxAddr			INAMP Mux address (0-3). 
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlAspModeAddr( uint8_t PointingMode, uint8_t AzMode,
									   uint8_t ElMode, uint8_t MuxAddr )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLASPDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_CTLASPDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		PKT_CTLASPDATA_STRUCT *pData = (PKT_CTLASPDATA_STRUCT *)m_Data;

		// Zero the value.
		pData->ModeAddr = 0;

		// Set the pointing mode value.
		pData->ModeAddr |= (0x0007 & PointingMode);

		// Set the azimuth mode.
		pData->ModeAddr |= (0x0038 & (AzMode << 3));

		// Set the elevation mode.
		pData->ModeAddr |= (0x01c0 & (ElMode << 6));

		// Set the mux address.
		pData->ModeAddr |= (0x0600 & (MuxAddr << 9));

		m_Altered = true;
	}

	// Return status.
	return rval;
}

//	SetCtlAspSaeElevation:
///		Sets elevation SAE data into the packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data in the packet.
///		\param[in]		SaeEl		Raw 16-bit value from elevation shaft angle encoder.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlAspSaeElevation( uint16_t SaeEl )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLASPDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_CTLASPDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		PKT_CTLASPDATA_STRUCT *pData = (PKT_CTLASPDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->SaeElevation = SaeEl;
	}

	// Return status.
	return rval;
}

//	SetCtlAspMagData:
///		Sets the magnetometer azimuth, pitch, roll and temperature into the ASP data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the ASP data packet.
///		\param[in]		Az				Magnetometer azimuth angle.
///		\param[in]		Pitch			Magnetometer pitch angle.
///		\param[in]		Roll			Magnetometer roll angle.
///		\param[in]		Temp			Magnetometer housing temperature.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlAspMagData( float Az, float Pitch, float Roll,
		uint16_t State )
{
	// If no packet, return an error.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLASPDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_CTLASPDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		PKT_CTLASPDATA_STRUCT *pData = (PKT_CTLASPDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		if ( State == 0 )
		{
			pData->MagAzimuth = (uint16_t)(Az * 100.0f);
			pData->MagPitch = (short)(Pitch * 100.0f);
			pData->MagRoll = (short)(Roll * 100.0f);
		}
		pData->MagState = State;
	}

	// Return status.
	return rval;
}

//	SetCtlAspDgpsData:
///		Sets the TANS azimuth, pitch, and roll into the ASP data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the ASP data packet.
///		\param[in]		Az				TANS azimuth angle.
///		\param[in]		Pitch			TANS pitch angle.
///		\param[in]		Roll			TANS roll angle.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlAspDgpsData( float Az, float Pitch, float Roll )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLASPDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_CTLASPDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		PKT_CTLASPDATA_STRUCT *pData = (PKT_CTLASPDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->DgpsAzimuth = (uint16_t)(Az * 100.0f);
		pData->DgpsPitch = (short)(Pitch * 100.0f);
		pData->DgpsRoll = (short)(Roll * 100.0f);
	}

	// Return status.
	return rval;
}

//	SetCtlAspGyroData:
///		Sets the gyro azimuth, elevation, and roll rates into the ASP data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the ASP data packet.
///		\param[in]		AzRate			Gyro azimuth rate (deg/s).
///		\param[in]		ElRate			Gyro elevation rate (deg/s).
///		\param[in]		RollRate		Gyro roll rate (deg/s).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlAspGyroData( int16_t AzRate, int16_t ElRate,
		int16_t RollRate, int16_t SpareRate, int16_t AzHrgRate )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLASPDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_CTLASPDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		PKT_CTLASPDATA_STRUCT *pData = (PKT_CTLASPDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->GyroAzRate = AzRate;
		pData->GyroElRate = ElRate;
		pData->GyroRollRate = RollRate;
    pData->GyroSpareRate = SpareRate;
    pData->HrgAzRate = AzHrgRate;
	}

	// Return status.
	return rval;
}

//	SetCtlAspGeoData:
///		Sets the longitude and latitude into the ASP data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the ASP data packet.
///		\param[in]		Longitude		Longitude in degrees.
///		\param[in]		Latitude		Latitude in degrees.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlAspGeoData( float Longitude, float Latitude )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLASPDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_CTLASPDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		PKT_CTLASPDATA_STRUCT *pData = (PKT_CTLASPDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->Longitude = (short)(Longitude * 100.0f);
		pData->Latitude = (short)(Latitude * 100.0f);
	}

	// Return status.
	return rval;
}

//	SetCtlAspFggIntData:
///		Sets the integrated fine-guidance gyro azimuth, pitch, and roll into the ASP data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the ASP data packet.
///		\param[in]		Az				fine-guidance gyro rate-integrated azimuth (deg).
///		\param[in]		El				fine-guidance gyro rate-integrated elevation (deg).
///		\param[in]		Roll			fine-guidance gyro rate-integrated roll (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlAspFggIntData( double Az, double El, double Roll,
    float azValue, float elValue )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLASPDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_CTLASPDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		PKT_CTLASPDATA_STRUCT *pData = (PKT_CTLASPDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->FggIntAz = (long)(Az * 10000.0);
		pData->FggIntEl = (long)(El * 10000.0);
		pData->FggIntRoll = (long)(Roll * 10000.0);
    pData->AzFggRipVal = (int16_t)(azValue * 1000.0f);
    pData->ElFggRipVal = (int16_t)(elValue * 1000.0f);
	}

	// Return status.
	return rval;
}

//	GetCtlAspData:
///		Gets the CTL system aspect data structure, converted into physical units, from the packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		pAspData		GSE system data structure to read from packet.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetCtlAspData( GSE_CTLASPDATA_STRUCT &AspData )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_CTLASPDATA, sizeof( PKT_CTLASPDATA_STRUCT ) )) )
	{
		// Get a pointer to the raw data structure.
		PKT_CTLASPDATA_STRUCT *pCtlData = (PKT_CTLASPDATA_STRUCT *)m_Data;

		// Set the flag values.
		AspData.MagValid = (pCtlData->Flags & 0x0001) > 0;
		AspData.DgpsValid = (pCtlData->Flags & 0x0002) > 0;
		AspData.GpsValid = (pCtlData->Flags & 0x0004) > 0;
		AspData.SaeValid = (pCtlData->Flags & 0x0008) > 0;
		AspData.ImPosValid0 = (pCtlData->Flags & 0x0010) > 0;
		AspData.ElFggRipValid = (pCtlData->Flags & 0x0020) > 0;
		AspData.AzFggRipValid = (pCtlData->Flags & 0x0040) > 0;
		AspData.RollFggRipValid = (pCtlData->Flags & 0x0080) > 0;
		AspData.StartCam = (pCtlData->Flags & 0x0100) > 0;
		AspData.StopCam = (pCtlData->Flags & 0x0200) > 0;
		AspData.CamActive = (pCtlData->Flags & 0x0400) > 0;
		AspData.CamStarted = (pCtlData->Flags & 0x0800) > 0;
		AspData.CamReceived = (pCtlData->Flags & 0x1000) > 0;
		AspData.BearingState = (pCtlData->Flags & 0x2000) > 0;
    AspData.ImPosValid1 = (pCtlData->Flags & 0x4000) > 0;

		// Get the modes and mux address.
		AspData.PointMode = (pCtlData->ModeAddr & 0x0007);
		AspData.AzMode = (pCtlData->ModeAddr >> 3) & 0x0007;
		AspData.ElMode = (pCtlData->ModeAddr >> 6) & 0x0007;
		AspData.MuxAddr = (pCtlData->ModeAddr >> 9) & 0x0003;

		AspData.MagState = pCtlData->MagState;

		// Convert the other values.
		AspData.SaeElevation = (double)pCtlData->SaeElevation * 360.0 / 65535.0;
		AspData.ElPitch = CtlAdcConverter::cnvInclPitch(pCtlData->ElPitch);
		AspData.ElRoll = CtlAdcConverter::cnvInclRoll(pCtlData->ElRoll);
//		AspData.ElPitch = (double)pCtlData->ElPitch * 8.715951 / 3276.7;
//		AspData.ElRoll = (double)pCtlData->ElRoll * 8.715951 / 3276.7;
		AspData.MagAzimuth = (double)pCtlData->MagAzimuth * 0.01;
		AspData.MagPitch = (double)pCtlData->MagPitch * 0.01;
		AspData.MagRoll = (double)pCtlData->MagRoll * 0.01;
		AspData.DgpsAzimuth = (double)pCtlData->DgpsAzimuth * 0.01;
		AspData.DgpsPitch = (double)(short)pCtlData->DgpsPitch * 0.01;
		AspData.DgpsRoll = (double)(short)pCtlData->DgpsRoll * 0.01;
		AspData.GyroAzRate =
				CtlAdcConverter::cnvFggAzXAxis(pCtlData->GyroAzRate);
		AspData.GyroElRate =
				CtlAdcConverter::cnvFggElXAxis(pCtlData->GyroElRate);
		AspData.GyroRollRate =
				CtlAdcConverter::cnvFggAzYAxis(pCtlData->GyroRollRate);
    AspData.GyroSpareRate = 
      CtlAdcConverter::cnvFggElYAxis(pCtlData->GyroSpareRate);
    AspData.HrgAzRate = CtlAdcConverter::cnvHrgAzAxis(pCtlData->HrgAzRate);
//		AspData.GyroAzRate = (double)(short)pCtlData->GyroAzRate / 3600.0;
//		AspData.GyroElRate = (double)(short)pCtlData->GyroElRate / 3600.0;
//		AspData.GyroRollRate = (double)(short)pCtlData->GyroRollRate / 3600.0;
//		AspData.AzTach = (double)(short)pCtlData->AzWheelTach * 0.00828608 + 0.55;
		AspData.FggIntAz = (double)pCtlData->FggIntAz * 0.0001;
		AspData.FggIntEl = (double)pCtlData->FggIntEl * 0.0001;
		AspData.FggIntRoll = (double)pCtlData->FggIntRoll * 0.0001;
    AspData.AzFggRipVal = (double)pCtlData->AzFggRipVal * 0.001f;
    AspData.ElFggRipVal = (double)pCtlData->ElFggRipVal * 0.001f;
		AspData.Longitude = (double)(short)pCtlData->Longitude * 0.01;
		AspData.Latitude = (double)(short)pCtlData->Latitude * 0.01;
		AspData.AzTach =
				CtlAdcConverter::cnvAzWheelTach(pCtlData->WheelTach);

		// Convert the INAMP Mux value.
		switch( AspData.MuxAddr )
		{
		// Shaft current case.
		case 0:
			AspData.ShaftCurrent =
					CtlAdcConverter::cnvShaftEm19Current(pCtlData->MuxValue);
//			AspData.ShaftCurrent = (double)(short)pCtlData->MuxValue * 0.00149488 + 0.1;
			break;

		// Wheel current case.
		case 1:
			AspData.WheelCurrent =
					CtlAdcConverter::cnvWheelEm19Current(pCtlData->MuxValue);
//			AspData.WheelCurrent = (double)(short)pCtlData->MuxValue * 0.00149488 + 0.1;
			break;

		// Elevation current case.
		case 2:
			AspData.ElCurrent =
					CtlAdcConverter::cnvElEm19Current(pCtlData->MuxValue);
//			AspData.ElCurrent = (double)(short)pCtlData->MuxValue * 0.0340708 + 0.3;
			break;

		// Azimuth shaft angle encoder.
		case 3:
  		AspData.AzSaeAngle = 
        CtlAdcConverter::cnvShaftEncoder(pCtlData->MuxValue);
//			AspData.AzSaeAngle = (double)pCtlData->MuxValue * 50 / 3276.7;
			break;
		}
	}

	// Return status.
	return rval;
};

//	SetCtlCtlData:
///		Sets the CTL system target, PID, and output data into the CTL data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the CTL data packet.
///		\param[in]		PMode			Overall pointing mode.
///		\param[in]		AzMode			Azimuth control mode.
///		\param[in]		ElMode			Elevation control mode.
///		\param[in]		AzUTarg			Azimuth ultimate target (deg).
///		\param[in]		ElUTarg			Elevation ultimate target (deg).
///		\param[in]		AzPTarg			Azimuth immediate position target (deg).
///		\param[in]		ElPTarg			Elevation immediate position target (deg).
///		\param[in]		AzRTarg			Azimuth immediate rate target (deg/s).
///		\param[in]		ElRTarg			Elevation immediate rate target (deg/s).
///		\param[in]		RaTarg			Right-ascension target (deg).
///		\param[in]		DecTarg			Declination target (deg).
///		\param[in]		AzP,			Azimuth PID loop P component.
///		\param[in]		AzI,			Azimuth PID loop I component.
///		\param[in]		AzD,			Azimuth PID loop D component.
///		\param[in]		ElP,			Elevation PID loop P component.
///		\param[in]		ElI,			Elevation PID loop I component.
///		\param[in]		ElD,			Elevation PID loop D component.
///		\param[in]		ElV,			Elevation voltage output (mV).
///		\param[in]		AzWV,			Azimuth wheel voltage output (mV).
///		\param[in]		AzSV,			Azimuth shaft voltage output (mV).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlCtlData( uint16_t PMode, uint16_t AzMode,
    uint16_t ElMode, int AzExplain, int ElExplain, float AzUTarg,
    float ElUTarg, float AzPTarg, float ElPTarg, float AzRTarg, float ElRTarg,
    float RaTarg, float DecTarg, float AzP, float AzI, float AzD, float ElP,
    float ElI, float ElD, float ElV, float AzWV, float AzSV, float WheelP,
    float WheelI, float WheelD )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLCTLDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_CTLDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_CTLDATA_STRUCT *pData = (CTL_CTLDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		// Set the mode word packet.
//		pData->Modes = (PMode << 8) | ((ElMode & 0x000f) << 4) | (AzMode & 0x000f);
    pData->Modes = ((PMode & 0x0007) << 12) | ((ElMode & 0x0007) << 8) |
      ((AzMode & 0x0007) << 4) | ((ElExplain & 0x0003) << 2) |
      (AzExplain & 0x0003);

		// Set the az-el targets.
		pData->AzUTarget = (uint16_t)((AzUTarg * 65535.0f)/360.0f);
		pData->ElUTarget = (short)((ElUTarg * 65535.0f)/360.0f);
		pData->AzTarget = (uint16_t)((AzPTarg * 65535.0f)/360.0f);
		pData->AzRTarget = (short)(AzRTarg * 3600.0f);
		pData->ElTarget = (short)((ElPTarg * 65535.0f)/360.0f);
		pData->ElRTarget = (short)(ElRTarg * 3600.0f);

		// Set the output voltages.
		pData->ElVOut = (short)ElV;
		pData->AzWVOut = (short)AzWV;
		pData->AzSVOut = (short)AzSV;

		// Set the RA,Dec targets.
		pData->RaTarget = RaTarg;
		pData->DecTarget = DecTarg;

		// Set the Azimuth PID values.
		pData->AzPValue = AzP;
		pData->AzIValue = AzI;
		pData->AzDValue = AzD;

		// Set the Elevation PID values.
		pData->ElPValue = ElP;
		pData->ElIValue = ElI;
		pData->ElDValue = ElD;

    pData->WPValue = WheelP;
    pData->WIValue = WheelI;
    pData->WDValue = WheelD;
	}

	// Return status.
	return rval;
}

//	GetCtlCtlData:
///		Gets the CTL system control loop data structure, converted into physical units, from the packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		CtlData		GSE control loop data structure to read from packet.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetCtlCtlData( GSE_CTLCTLDATA_STRUCT &CtlData )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_CTLCTLDATA, sizeof( CTL_CTLDATA_STRUCT ) )) )
	{
		// Get a pointer to the raw data structure.
		CTL_CTLDATA_STRUCT *pData = (CTL_CTLDATA_STRUCT *)m_Data;

		// Set the flag values.
		CtlData.PMode = pData->Modes >> 12;
		CtlData.ElMode = (pData->Modes >> 8) & 0x0007;
		CtlData.AzMode = (pData->Modes >> 4) & 0x0007;
    CtlData.ElExplain = (pData->Modes >> 2) & 0x0003;
    CtlData.AzExplain = pData->Modes & 0x0003;

		// Convert the targets.
		CtlData.AzUTarget = (double)pData->AzUTarget * 360.0 / 65535.0;
		CtlData.ElUTarget = (double)pData->ElUTarget * 360.0 / 65535.0;
		CtlData.AzTarget = (double)pData->AzTarget * 360.0 / 65535.0;
		CtlData.AzRTarget = (double)pData->AzRTarget / 3600.0;
		CtlData.ElTarget = (double)pData->ElTarget * 360.0 / 65535.0;
		CtlData.ElRTarget = (double)pData->ElRTarget / 3600.0;

		// Convert the output voltages to volts.
		CtlData.ElVOut = (double)pData->ElVOut * 0.001;
		CtlData.AzWVOut = (double)pData->AzWVOut * 0.001;
		CtlData.AzSVOut = (double)pData->AzSVOut * 0.001;

		// Simply cast the RA and Dec targets from float to double.
		CtlData.RaTarget = (double)pData->RaTarget;
		CtlData.DecTarget = (double)pData->DecTarget;

		// Simply cast the PID values from float to double.
		CtlData.AzPValue = (double)pData->AzPValue;
		CtlData.AzIValue = (double)pData->AzIValue;
		CtlData.AzDValue = (double)pData->AzDValue;
		CtlData.ElPValue = (double)pData->ElPValue;
		CtlData.ElIValue = (double)pData->ElIValue;
		CtlData.ElDValue = (double)pData->ElDValue;
		CtlData.WPValue = (double)pData->WPValue;
		CtlData.WIValue = (double)pData->WIValue;
		CtlData.WDValue = (double)pData->WDValue;
	}

	// Return status.
	return rval;
};

//	SetCtlParamMscSettings:
///		Sets several CTL system flag parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		GeoPosSrc		Preferred geographic position source.
///		\param[in]		CamTracking		Flag indicating if camera has been commanded to track stars.
///		\param[in]		AllowDgps		Flag indicating if DGPS is allowed in pointing control.
///		\param[in]		UseCam0			Flag indicating camera 0 is used in image tracking.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamMiscSettings( int GeoPosSrc, bool CamTracking,
		bool AllowDgps, bool UseCam0, bool UseCam1 )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Start data at zero.
		pData->MiscSettings = 0;

		// OR in the geographic position source bits.
		pData->MiscSettings |= (GeoPosSrc & 0x03);

		// If tans preferred, OR in bit 2.
		if ( CamTracking )
			pData->MiscSettings |= 0x04;

		// If the DGPS is allowed, OR in bit 3.
		if ( AllowDgps )
			pData->MiscSettings |= 0x08;

		// If camera 0 is used for tracking, OR in bit 4.
		if ( UseCam0 )
			pData->MiscSettings |= 0x10;

    if ( UseCam1 )
      pData->MiscSettings |= 0x20;
	}

	// Return status.
	return rval;
}

//	SetCtlParamElTrimFlags:
///		Sets several CTL system elevation auto-trim flag parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		TrimElSaeInc	Flag to auto-trim the elevation SAE + inclinometer value.
///		\param[in]		TrimElFgg		Flag to auto-trim the elevation fine-guidance gyro rate.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamElTrimFlags( bool SaeUseCam, bool FggUseCam,
    bool FggUseSae )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Start data at zero.
		pData->ElTrimFlags = 0;

		// OR in the bits, if set.
		if ( SaeUseCam )
			pData->ElTrimFlags |= 0x01;
		if ( FggUseCam )
			pData->ElTrimFlags |= 0x02;
    if ( FggUseSae )
      pData->ElTrimFlags |= 0x04;
	}

	// Return status.
	return rval;
}

//	SetCtlParamAzTrimFlags:
///		Sets several CTL system azimuth auto-trim flag parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		TrimAzDgps		Flag to auto-trim the azimuth TANS.
///		\param[in]		TrimAzMag		Flag to auto-trim the azimuth magnetometer.
///		\param[in]		TrimAzFgg		Flag to auto-trim the azimuth fine-guidance gyro rate.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamAzTrimFlags( bool DgpsUseCam, bool MagUseCam,
    bool MagUseDgps, bool FggUseCam, bool FggUseDgps, bool FggUseMag )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Start data at zero.
		pData->AzTrimFlags = 0;

		// OR in the bits, if set.
		if ( DgpsUseCam )
			pData->AzTrimFlags |= 0x01;
		if ( MagUseCam )
			pData->AzTrimFlags |= 0x02;
		if ( MagUseDgps )
			pData->AzTrimFlags |= 0x04;
		if ( FggUseCam )
			pData->AzTrimFlags |= 0x08;
		if ( FggUseDgps )
			pData->AzTrimFlags |= 0x10;
		if ( FggUseMag )
			pData->AzTrimFlags |= 0x20;
	}

	// Return status.
	return rval;
}

//	SetCtlParamRollTrimFlags:
///		Sets several CTL system roll auto-trim flag parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		TrimRollInc		Flag to auto-trim the roll inclinometer value.
///		\param[in]		TrimRollFgg		Flag to auto-trim the roll fine-guidance gyro rate.
///		\param[in]		TrimRollHrg		Flag to auto-trim the roll high-rate gyro rate.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamRollTrimFlags( bool TrimRollInc,
		bool TrimRollFgg )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Start data at zero.
		pData->RollTrimFlags = 0;

		// OR in the bits, if set.
		if ( TrimRollInc )
			pData->RollTrimFlags |= 0x01;
		if ( TrimRollFgg )
			pData->RollTrimFlags |= 0x02;
	}

	// Return status.
	return rval;
}

///		Sets sensor object parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		numPoints   Number of points in trim object fit.
///		\param[in]		maxAge      Maximum age of fit in seconds before it is invalid.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamAzCamSensor( int numPoints, double maxAge )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->AzCamNPoints = (uint16_t)numPoints;
    pData->AzCamMaxAge = (uint16_t)maxAge;
	}

	// Return status.
	return rval;
}

///		Sets sensor object parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		numPoints   Number of points in trim object fit.
///		\param[in]		maxAge      Maximum age of fit in seconds before it is invalid.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamAzDgpsSensor( int numPoints, double maxAge )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->AzDgpsNPoints = (uint16_t)numPoints;
    pData->AzDgpsMaxAge = (uint16_t)maxAge;
	}

	// Return status.
	return rval;
}

///		Sets sensor object parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		numPoints   Number of points in trim object fit.
///		\param[in]		maxAge      Maximum age of fit in seconds before it is invalid.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamAzMagSensor( int numPoints, double maxAge )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->AzMagNPoints = (uint16_t)numPoints;
    pData->AzMagMaxAge = (uint16_t)maxAge;
	}

	// Return status.
	return rval;
}

///		Sets sensor object parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		numPoints   Number of points in trim object fit.
///		\param[in]		maxAge      Maximum age of fit in seconds before it is invalid.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamAzFggSensor( int numPoints, double maxAge )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->AzFggNPoints = (uint16_t)numPoints;
    pData->AzFggMaxAge = (uint16_t)maxAge;
	}

	// Return status.
	return rval;
}

///		Sets sensor object parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		numPoints   Number of points in trim object fit.
///		\param[in]		maxAge      Maximum age of fit in seconds before it is invalid.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamElCamSensor( int numPoints, double maxAge )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->ElCamNPoints = (uint16_t)numPoints;
    pData->ElCamMaxAge = (uint16_t)maxAge;
	}

	// Return status.
	return rval;
}

///		Sets sensor object parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		numPoints   Number of points in trim object fit.
///		\param[in]		maxAge      Maximum age of fit in seconds before it is invalid.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamElSaeSensor( int numPoints, double maxAge )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->ElSaeNPoints = (uint16_t)numPoints;
    pData->ElSaeMaxAge = (uint16_t)maxAge;
	}

	// Return status.
	return rval;
}

///		Sets sensor object parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		numPoints   Number of points in trim object fit.
///		\param[in]		maxAge      Maximum age of fit in seconds before it is invalid.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamElFggSensor( int numPoints, double maxAge )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->ElFggNPoints = (uint16_t)numPoints;
    pData->ElFggMaxAge = (uint16_t)maxAge;
	}

	// Return status.
	return rval;
}

///		Sets sensor object parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		numPoints   Number of points in trim object fit.
///		\param[in]		maxAge      Maximum age of fit in seconds before it is invalid.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamAzFggRipParam( float slope, float offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->AzFggRipSlope = slope;
    pData->AzFggRipOffset = offset;
	}

	// Return status.
	return rval;
}

///		Sets sensor object parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		numPoints   Number of points in trim object fit.
///		\param[in]		maxAge      Maximum age of fit in seconds before it is invalid.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamElFggRipParam( float slope, float offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->ElFggRipSlope = slope;
    pData->ElFggRipOffset = offset;
	}

	// Return status.
	return rval;
}

//	SetCtlParamElStowPos:
///		Sets the CTL system sensor offset parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			SAE reading of stow position.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamElStowPos( float Position )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->StowPos = (uint16_t)(Position * 65535.0f / 360.0f);
	}

	// Return rval.
	return rval;
}

//	SetCtlParamElSaeOffset:
///		Sets the CTL system sensor offset parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of SAE from true elevation (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamElSaeOffset( float Offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->ElSaeOffset = Offset;
	}

	// Return rval.
	return rval;
}
	
//	SetCtlParamElIncOffset:
///		Sets the CTL system sensor offset parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of pitch inclinometer from true level (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamElIncOffset( float Offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->ElIncOffset = Offset;
	}

	// Return rval.
	return rval;
}

//	SetCtlParamElSaeIncOffset:
///		Sets the CTL system sensor offset parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of SAE + pitch inclinometer from true elevation (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamElSaeIncOffset( float Offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->ElSaeIncOffset = Offset;
	}

	// Return rval.
	return rval;
}

//	SetCtlParamElRateFggOffset:
///		Sets the CTL system sensor offset parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of elevation fine-guidance gyro from true rate (deg/s).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamElRateFggOffset( float Offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->ElRateFggOffset = Offset;
	}

	// Return rval.
	return rval;
}

//	SetCtlParamAzDgpsOffset:
///		Sets the CTL system sensor offset parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of TANS azimuth from true azimuth (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamAzDgpsOffset( float Offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->AzDgpsOffset = Offset;
	}

	// Return rval.
	return rval;
}

//	SetCtlParamAzMagOffset:
///		Sets the CTL system sensor offset parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of magnetometer azimuth from true azimuth (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamAzMagOffset( float Offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->AzMagOffset = Offset;
	}

	// Return rval.
	return rval;
}

//	SetCtlParamAzRateFggOffset:
///		Sets the CTL system sensor offset parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of azimuth fine-guidance gyro from true rate (deg/s).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamAzRateFggOffset( float Offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->AzRateFggOffset = Offset;
	}

	// Return rval.
	return rval;
}

//	SetCtlParamAzRateHrgOffset:
///		Sets the CTL system sensor offset parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of azimuth fine-guidance gyro from true rate (deg/s).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamAzRateHrgOffset( float Offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->AzRateHrgOffset = Offset;
	}

	// Return rval.
	return rval;
}

//	SetCtlParamRollIncOffset:
///		Sets the CTL system sensor offset parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of roll inclinometer from true level (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamRollIncOffset( float Offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->RollIncOffset = Offset;
	}

	// Return rval.
	return rval;
}

//	SetCtlParamRollRateFggOffset:
///		Sets the CTL system sensor offset parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of roll fine-guidance gyro from true rate (deg/s).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamRollRateFggOffset( float Offset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->RollRateFggOffset = Offset;
	}

	// Return rval.
	return rval;
}

//	SetCtlParamGyroIntOffsets:
///		Sets the CTL system sensor offsets parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		ElFggOffset			Offset of elevation fine-guidance gyro RIP from true (deg).
///		\param[in]		AzFggOffset			Offset of elevation fine-guidance gyro RIP from true (deg).
///		\param[in]		RollFggOffset		Offset of elevation fine-guidance gyro RIP from true (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamGyroIntOffsets( float ElFggIntOffset,
		float AzFggIntOffset, float RollFggIntOffset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->ElFggIntOffset = ElFggIntOffset;
		pData->AzFggIntOffset = AzFggIntOffset;
		pData->RollFggIntOffset = RollFggIntOffset;
	}

	// Return rval.
	return rval;
}

///		Sets the CTL system filter cutoff frequency into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of roll fine-guidance gyro from true rate (deg/s).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamHrgFilterCutoff( float CutoffFreqHz )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->HrgCutoff = CutoffFreqHz;
	}

	// Return rval.
	return rval;
}

///		Sets the CTL system filter cutoff frequency into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Position			Offset of roll fine-guidance gyro from true rate (deg/s).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamFggFilterCutoff( float CutoffFreqHz )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;


		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->FggCutoff = CutoffFreqHz;
	}

	// Return rval.
	return rval;
}

//	SetCtlParamImErrThresh:
///		Sets the CTL system image error threshold into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		ImErrThresh			Image error threshold (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamImErrThresh( float ImErrThresh )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->ImErrThresh = (uint16_t)(ImErrThresh * 10000.0f);
	}

	// Return status.
	return rval;
}

//	SetCtlParamWheelDumpData:
///		Sets the CTL system torque wheel PID loop parameters into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		PGain			Proportional error gain in PID loop.
///		\param[in]		IGain			Integral error gain in PID loop.
///		\param[in]		DGain			Differential error gain in PID loop.
///		\param[in]		MaxOff			Maximum offset (at which P err * P Gain = MaxOut).
///		\param[in]		MaxOut			Maximum output voltage to shaft (millivolts).
///		\param[in]		ILimit			Fraction of MaxOut which I err * I Gain is limited to.
///		\param[in]		OmegaMax		Maximum wheel angular speed allowed.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamWheelDumpData( float PGain, float IGain,
		float DGain, float MaxOut, float ILimit, float OmegaMax )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->WDPGain = PGain;
		pData->WDIGain = IGain;
		pData->WDDGain = DGain;
		pData->WDMaxOut = (uint16_t)MaxOut;
		pData->WDILimit = (uint16_t)(ILimit * 10000.0f);
		pData->WDMaxOmega = (uint16_t)(OmegaMax * 1000.0f);
	}

	// Return status.
	return rval;
}

//	SetCtlParamModeData:
///		Sets the CTL system PID loop parameters of a given control mode into the parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Mode			Control mode to which the following parameters pertain.
///		\param[in]		PGain			Proportional error gain in PID loop.
///		\param[in]		IGain			Integral error gain in PID loop.
///		\param[in]		DGain			Differential error gain in PID loop.
///		\param[in]		ILimit			Fraction of MaxOut which I err * I Gain is limited to.
///		\param[in]		MaxOut			Maximum output voltage to shaft (millivolts).
///		\param[in]		PBound			Maximum value of P-error for this mode (deg).
///		\param[in]		DBound			Maximum value of D-error for this mode (deg/s).
///		\param[in]		MaxRate			Slew operation maximum rate (deg/s).
///		\param[in]		Accel			Slew operation acceleration (deg/s*s).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlParamModeData( int Mode, float PGain, float IGain,
		float DGain, float ILimit, int MaxOut, float PBound, float DBound,
		float MaxRate, float Accel )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->Mode = (uint16_t)Mode;
		pData->PGain = PGain;
		pData->IGain = IGain;
		pData->DGain = DGain;
		pData->ILimit = ILimit;
		pData->PBound = PBound;
		pData->DBound = DBound;
		pData->MaxOut = (uint16_t)MaxOut;
		pData->SlewMaxRate = (uint16_t)(MaxRate * 10000.0f);
		pData->SlewAccel = (uint16_t)(Accel * 10000.0f);
	}

	// Return status.
	return rval;
}

int HeroGsePacket::SetCtlParamGyroTweakData( float azXSlope, float azXOffset,
    float azYSlope, float azYOffset, float elXSlope, float elXOffset,
    float elYSlope, float elYOffset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( CTL_PARAMDATA_STRUCT );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;

		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
    pData->GyroTweak[0][0] = azXSlope;
    pData->GyroTweak[0][1] = azXOffset;
    pData->GyroTweak[1][0] = azYSlope;
    pData->GyroTweak[1][1] = azYOffset;
    pData->GyroTweak[2][0] = elXSlope;
    pData->GyroTweak[2][1] = elXOffset;
    pData->GyroTweak[3][0] = elYSlope;
    pData->GyroTweak[3][1] = elYOffset;
	}

	// Return status.
	return rval;
}

//	GetCtlParamData:
///		Gets the CTL parameter data from a validated packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		ParamData		Structure to hold the converted parameter data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetCtlParamData( GSE_CTLPARAMDATA_STRUCT &ParamData )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_CTLPARAM, sizeof( CTL_PARAMDATA_STRUCT) )) )
	{
		// Get a pointer to the raw data structure.
		CTL_PARAMDATA_STRUCT *pData = (CTL_PARAMDATA_STRUCT *)m_Data;

		// Get the settings out of the miscellaneous settings byte.
		ParamData.GeoPosSrc = pData->MiscSettings & 0x03;
		ParamData.CamTracking = (pData->MiscSettings & 0x04) > 0;
		ParamData.AllowDgps = (pData->MiscSettings & 0x08) > 0;
		ParamData.UseCam0 = (pData->MiscSettings & 0x10) > 0;
		ParamData.UseCam1 = (pData->MiscSettings & 0x20) > 0;

		// Get the elevation trim flags out of the byte.
		ParamData.ElSaeUseCam = (pData->ElTrimFlags & 0x01) > 0;
		ParamData.ElFggUseCam = (pData->ElTrimFlags & 0x02) > 0;
    ParamData.ElFggUseSae = (pData->ElTrimFlags & 0x04) > 0;

		// Get the azimuth trim flags out of the byte.
		ParamData.AzDgpsUseCam = (pData->AzTrimFlags & 0x01) > 0;
		ParamData.AzMagUseCam = (pData->AzTrimFlags & 0x02) > 0;
		ParamData.AzMagUseDgps = (pData->AzTrimFlags & 0x04) > 0;
    ParamData.AzFggUseCam = (pData->AzTrimFlags & 0x08) > 0;
    ParamData.AzFggUseDgps = (pData->AzTrimFlags & 0x10) > 0;
    ParamData.AzFggUseMag = (pData->AzTrimFlags & 0x20) > 0;

		// Get the offsets.
		ParamData.StowPos = (double)pData->StowPos * 360.0 / 65535.0;
		ParamData.ElSaeOffset = (double)pData->ElSaeOffset;
		ParamData.ElIncOffset = (double)pData->ElIncOffset;
		ParamData.ElSaeIncOffset = (double)pData->ElSaeIncOffset;
		ParamData.ElFggIntOffset = (double)pData->ElFggIntOffset;
		ParamData.ElRateFggOffset = (double)pData->ElRateFggOffset;
		ParamData.AzDgpsOffset = (double)pData->AzDgpsOffset;
		ParamData.AzMagOffset = (double)pData->AzMagOffset;
		ParamData.AzFggIntOffset = (double)pData->AzFggIntOffset;
		ParamData.AzRateFggOffset = (double)pData->AzRateFggOffset;
		ParamData.RollIncOffset = (double)pData->RollIncOffset;
		ParamData.RollFggIntOffset = (double)pData->RollFggIntOffset;
		ParamData.RollRateFggOffset = (double)pData->RollRateFggOffset;
    ParamData.FggCutoff = (double)pData->FggCutoff;
    ParamData.HrgCutoff = (double)pData->HrgCutoff;

		// Get the image error threshold.
		ParamData.ImErrThresh = (double)pData->ImErrThresh * 0.0001;

		// Get the wheel momentum dump PID loop parametrs.
		ParamData.WDPGain = (double)pData->WDPGain;
		ParamData.WDIGain = (double)pData->WDIGain;
		ParamData.WDDGain = (double)pData->WDDGain;
		ParamData.WDMaxOut = (double)pData->WDMaxOut;
		ParamData.WDILimit = (double)pData->WDILimit * 0.0001;
		ParamData.WDMaxOmega = (double)pData->WDMaxOmega * 0.001;

		// Get the control mode paramters.
		ParamData.Mode = (int)pData->Mode;
		ParamData.PGain = (int)pData->PGain;
		ParamData.IGain = (int)pData->IGain;
		ParamData.DGain = (int)pData->DGain;
		ParamData.ILimit = (double)pData->ILimit;
		ParamData.PBound = (double)pData->PBound;
		ParamData.DBound = (double)pData->DBound;
		ParamData.MaxOut = (int)pData->MaxOut;
		ParamData.SlewMaxRate = (double)pData->SlewMaxRate * 0.0001;
		ParamData.SlewAccel = (double)pData->SlewAccel * 0.0001;

    // Convert the HRG data.
    ParamData.AzRateHrgOffset = (double)pData->AzRateHrgOffset;

    // Convert the sensor object data.
    ParamData.AzCamNPoints = (int)pData->AzCamNPoints;
    ParamData.AzCamMaxAge = (double)pData->AzCamMaxAge;
    ParamData.AzDgpsNPoints = (int)pData->AzDgpsNPoints;
    ParamData.AzDgpsMaxAge = (double)pData->AzDgpsMaxAge;
    ParamData.AzMagNPoints = (int)pData->AzMagNPoints;
    ParamData.AzMagMaxAge = (double)pData->AzMagMaxAge;
    ParamData.AzFggNPoints = (int)pData->AzFggNPoints;
    ParamData.AzFggMaxAge = (double)pData->AzFggMaxAge;
    ParamData.ElCamNPoints = (int)pData->ElCamNPoints;
    ParamData.ElCamMaxAge = (double)pData->ElCamMaxAge;
    ParamData.ElSaeNPoints = (int)pData->ElSaeNPoints;
    ParamData.ElSaeMaxAge = (double)pData->ElSaeMaxAge;
    ParamData.ElFggNPoints = (int)pData->ElFggNPoints;
    ParamData.ElFggMaxAge = (double)pData->ElFggMaxAge;
    ParamData.AzFggRipSlope = (double)pData->AzFggRipSlope;
    ParamData.AzFggRipOffset = (double)pData->AzFggRipOffset;
    ParamData.ElFggRipSlope = (double)pData->ElFggRipSlope;
    ParamData.ElFggRipOffset = (double)pData->ElFggRipOffset;

    memcpy( &ParamData.GyroTweak[0][0], &pData->GyroTweak[0][0],
      sizeof(float)*8 );
	}

	// Return without error.
	return 0;
}

//	GetCtlParamData2:
///		Gets the CTL parameter data from a validated packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		ParamData		Structure to hold the converted parameter data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetCtlParam2Data( GSE_CTLPARAMDATA2_STRUCT &ParamData )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_CTLPARAM2, sizeof( CTL_PARAMDATA2_STRUCT) )) )
	{
		// Get a pointer to the raw data structure.
		CTL_PARAMDATA2_STRUCT *pData = (CTL_PARAMDATA2_STRUCT *)m_Data;

		// Get the settings out of the miscellaneous settings byte.
		ParamData.GeoPosSrc = pData->MiscSettings & 0x03;
		ParamData.CamTracking = (pData->MiscSettings & 0x04) > 0;
		ParamData.AllowDgps = (pData->MiscSettings & 0x08) > 0;
		ParamData.UseCam0 = (pData->MiscSettings & 0x10) > 0;
		ParamData.UseCam1 = (pData->MiscSettings & 0x20) > 0;

		// Get the elevation trim flags out of the byte.
		ParamData.ElSaeUseCam = (pData->ElTrimFlags & 0x01) > 0;
		ParamData.ElFggUseCam = (pData->ElTrimFlags & 0x02) > 0;
    ParamData.ElFggUseSae = (pData->ElTrimFlags & 0x04) > 0;

		// Get the azimuth trim flags out of the byte.
		ParamData.AzDgpsUseCam = (pData->AzTrimFlags & 0x01) > 0;
		ParamData.AzMagUseCam = (pData->AzTrimFlags & 0x02) > 0;
		ParamData.AzMagUseDgps = (pData->AzTrimFlags & 0x04) > 0;
    ParamData.AzFggUseCam = (pData->AzTrimFlags & 0x08) > 0;
    ParamData.AzFggUseDgps = (pData->AzTrimFlags & 0x10) > 0;
    ParamData.AzFggUseMag = (pData->AzTrimFlags & 0x20) > 0;

		// Get the offsets.
		ParamData.StowPos = (double)pData->StowPos * 360.0 / 65535.0;
		ParamData.ElSaeOffset = (double)pData->ElSaeOffset;
		ParamData.ElIncOffset = (double)pData->ElIncOffset;
		ParamData.ElSaeIncOffset = (double)pData->ElSaeIncOffset;
		ParamData.ElFggIntOffset = (double)pData->ElFggIntOffset;
		ParamData.ElRateFggOffset = (double)pData->ElRateFggOffset;
		ParamData.AzDgpsOffset = (double)pData->AzDgpsOffset;
		ParamData.AzMagOffset = (double)pData->AzMagOffset;
		ParamData.AzFggIntOffset = (double)pData->AzFggIntOffset;
		ParamData.AzRateFggOffset = (double)pData->AzRateFggOffset;
		ParamData.RollIncOffset = (double)pData->RollIncOffset;
		ParamData.RollFggIntOffset = (double)pData->RollFggIntOffset;
		ParamData.RollRateFggOffset = (double)pData->RollRateFggOffset;
    ParamData.FggCutoff = (double)pData->FggCutoff;
    ParamData.HrgCutoff = (double)pData->HrgCutoff;

		// Get the image error threshold.
		ParamData.ImErrThresh = (double)pData->ImErrThresh * 0.0001;

		// Get the wheel momentum dump PID loop parametrs.
		ParamData.WDPGain = (double)pData->WDPGain;
		ParamData.WDIGain = (double)pData->WDIGain;
		ParamData.WDDGain = (double)pData->WDDGain;
		ParamData.WDMaxOut = (double)pData->WDMaxOut;
		ParamData.WDILimit = (double)pData->WDILimit * 0.0001;
		ParamData.WDMaxOmega = (double)pData->WDMaxOmega * 0.001;

		// Get the control mode paramters.
		ParamData.Mode = (int)pData->Mode;
		ParamData.PGain = (int)pData->PGain;
		ParamData.IGain = (int)pData->IGain;
		ParamData.DGain = (int)pData->DGain;
		ParamData.ILimit = (double)pData->ILimit;
		ParamData.PBound = (double)pData->PBound;
		ParamData.DBound = (double)pData->DBound;
		ParamData.MaxOut = (int)pData->MaxOut;
		ParamData.SlewMaxRate = (double)pData->SlewMaxRate * 0.0001;
		ParamData.SlewAccel = (double)pData->SlewAccel * 0.0001;

    // Convert the HRG data.
    ParamData.AzRateHrgOffset = (double)pData->AzRateHrgOffset;

    // Convert the sensor object data.
    ParamData.AzCamNPoints = (int)pData->AzCamNPoints;
    ParamData.AzCamMaxAge = (double)pData->AzCamMaxAge;
    ParamData.AzDgpsNPoints = (int)pData->AzDgpsNPoints;
    ParamData.AzDgpsMaxAge = (double)pData->AzDgpsMaxAge;
    ParamData.AzMagNPoints = (int)pData->AzMagNPoints;
    ParamData.AzMagMaxAge = (double)pData->AzMagMaxAge;
    ParamData.AzFggNPoints = (int)pData->AzFggNPoints;
    ParamData.AzFggMaxAge = (double)pData->AzFggMaxAge;
    ParamData.ElCamNPoints = (int)pData->ElCamNPoints;
    ParamData.ElCamMaxAge = (double)pData->ElCamMaxAge;
    ParamData.ElSaeNPoints = (int)pData->ElSaeNPoints;
    ParamData.ElSaeMaxAge = (double)pData->ElSaeMaxAge;
    ParamData.ElFggNPoints = (int)pData->ElFggNPoints;
    ParamData.ElFggMaxAge = (double)pData->ElFggMaxAge;
    ParamData.AzFggRipSlope = (double)pData->AzFggRipSlope;
    ParamData.AzFggRipOffset = (double)pData->AzFggRipOffset;
    ParamData.ElFggRipSlope = (double)pData->ElFggRipSlope;
    ParamData.ElFggRipOffset = (double)pData->ElFggRipOffset;

    memcpy( &ParamData.GyroTweak[0][0], &pData->GyroTweak[0][0],
      sizeof(float)*8 );

    ParamData.ElEntryBound = (float)pData->ElEntryBound * 0.001f;
    ParamData.ElEntryCount = pData->ElEntryCount;
    ParamData.AzEntryBound = (float)pData->AzEntryBound * 0.001f;
    ParamData.AzEntryCount = pData->AzEntryCount;
	}

	// Return without error.
	return 0;
}

//	SetCtlBearingRate:
///		Sets the bearing rate sensor reading into the packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then inserts the data int the
///		packet, updates the data length and packet length of the packet, and any flags necessary.
///		\param[in]		Rate		Number of bearing case bolts detected in period.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCtlBearingRate( uint16_t Rate )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CTLBRS )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		uint16_t *Data = (uint16_t *)m_Data;
		Data[0] = Rate;
		m_Header->DataLength = sizeof( uint16_t );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;
	}

	// Return without error.
	return 0;
}

//	GetCtlBearingRate:
///		Gets the bearing rate sensor reading from a validated packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		pRate		Number of bearing case bolts detected in period.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetCtlBearingRate( uint16_t* pRate )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_CTLBRS, sizeof( uint16_t ) )) )
	{
		// Get the data.
		uint16_t *Data = (uint16_t *)m_Data;
		*pRate = Data[0];
	}

	// Return without error.
	return rval;
}

/// ReverseBytes:
///		Reverses the bytes of a uint16_t value.
uint16_t HeroGsePacket::ReverseBytes( uint16_t val )
{
	uint16_t rval;
	uint8_t *prval = (uint8_t *)&rval;
	uint8_t *pval = (uint8_t *)&val;
	prval[0] = pval[1];
	prval[1] = pval[0];

	return rval;
}

/// ReverseBytes:
///		Reverses the bytes of a uint32_t value.
uint32_t HeroGsePacket::ReverseBytes( uint32_t val )
{
	uint32_t rval;
	uint8_t *prval = (uint8_t *)&rval;
	uint8_t *pval = (uint8_t *)&val;
	prval[0] = pval[3];
	prval[1] = pval[2];
	prval[2] = pval[1];
	prval[3] = pval[0];

	return rval;
}
///	GetMotGpsPsdData:
///		Gets the position/status/data information from a validated packet.
//
///		This function first checks that the packet exists, has been created correctly with the correct
///		type, is the correct length and GPS packet type, and then reads the data from the packet,
///		converting some of the values to user units.
///		\param[out]		PsdData			Structure to hold GPS position/status/data packet data.
///		\return			Returns 0 if packet exists, is correct type, length and GPS type.
///						Returns a non-zero error code otherwise.
int HeroGsePacket::GetMotGpsPsdData( GSE_MOTGPSPSDDATA_STRUCT& PsdData )
{
	int rval;

	// Verify the packet type and length.
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_MOTGPS, 76 ))  )
	{
		// Cast the pointer to a GPS PSD data packet structure.
		PKT_MOTGPSPSDDATA_STRUCT *pData = (PKT_MOTGPSPSDDATA_STRUCT *)m_Data;

		// Verify the GPS packet type.
		if ( pData->Type == 0x6145 )
		{
			// Get date and time.
			PsdData.Month = pData->Month;
			PsdData.Day = pData->Day;
			PsdData.Year = pData->Year;
			PsdData.Hour = pData->Hour;
			PsdData.Minute = pData->Minute;
			PsdData.Second = (double)pData->Second + (double)pData->NanoSec * 0.000000001;

			// Get geographic position.
			PsdData.Latitude = (double)pData->Latitude / 3600000.0;
			PsdData.Longitude = (double)pData->Longitude / 3600000.0;
			PsdData.Height = (double)pData->Height * 0.01;

			// Get velocity and heading.
			PsdData.Velocity = (float)pData->Velocity * 0.01f;
			PsdData.Heading = (float)pData->Heading * 0.1f;

			// Get satellite data.
			PsdData.Dop = pData->Dop;
			PsdData.DopType = pData->DopType;
			PsdData.SatVis = pData->SatVis;
			PsdData.SatTrk = pData->SatTrk;

			// Get specific satellite status stuff.
			for ( int i = 0; i < 8; i++ )
			{
				PsdData.SatStat[i].Id = pData->SatStat[i].Id;
				PsdData.SatStat[i].TrkMode = pData->SatStat[i].TrkMode;
				PsdData.SatStat[i].Snr = pData->SatStat[i].Snr;
				PsdData.SatStat[i].StatFlag = pData->SatStat[i].StatFlag;
			}

			// Get the receiver status.
			PsdData.RcvrStatus = pData->RcvrStatus;
		}
		// If wrong type, set an error.
		else
			rval = HGSEPKTERR_GPSTYPE;
	}

	return rval;
}

///	GetM12GpsPsdData:
///		Gets the position/status/data information from a validated packet.
//
///		This function first checks that the packet exists, has been created correctly with the correct
///		type, is the correct length and GPS packet type, and then reads the data from the packet,
///		converting some of the values to user units.
///		\param[out]		PsdData			Structure to hold GPS position/status/data packet data.
///		\return			Returns 0 if packet exists, is correct type, length and GPS type.
///						Returns a non-zero error code otherwise.
int HeroGsePacket::GetM12GpsPsdData( GSE_M12GPSPSDDATA_STRUCT& PsdData )
{
	int rval;

	// Verify the packet type and length.
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_MOTGPS, 154 )) )
	{
		// Cast the pointer to a GPS PSD data packet structure.
		PKT_M12GPSPSDDATA_STRUCT *pData = (PKT_M12GPSPSDDATA_STRUCT *)m_Data;

		// Verify the GPS packet type.
		if ( pData->Type == 0x6148 )
		{
			// Get date and time.
			PsdData.Month = pData->Month;
			PsdData.Day = pData->Day;
			PsdData.Year = ReverseBytes( pData->Year );
			PsdData.Hour = pData->Hour;
			PsdData.Minute = pData->Minute;
			PsdData.Second = (double)pData->Second + (double)ReverseBytes( pData->NanoSec ) * 0.000000001;

			// Get geographic position.
			PsdData.Latitude = (double)(int32_t)ReverseBytes( (uint32_t)pData->Latitude ) / 3600000.0;
			PsdData.Longitude = (double)(int32_t)ReverseBytes( (uint32_t)pData->Longitude ) / 3600000.0;
			PsdData.Height = (double)(int32_t)ReverseBytes( (uint32_t)pData->Height ) * 0.01;

			// Get velocity and heading.
			PsdData.Velocity = (float)ReverseBytes( pData->Velocity3D ) * 0.01f;
			PsdData.Heading = (float)ReverseBytes( pData->Heading ) * 0.1f;

			// Get satellite data.
			PsdData.Dop = ReverseBytes( pData->Dop );
			PsdData.SatVis = pData->SatVis;
			PsdData.SatTrk = pData->SatTrk;

			// Get specific satellite status stuff.
			for ( int i = 0; i < 12; i++ )
			{
				PsdData.SatStat[i].Id = pData->SatStat[i].Id;
				PsdData.SatStat[i].TrkMode = pData->SatStat[i].TrkMode;
				PsdData.SatStat[i].Snr = pData->SatStat[i].Snr;
				PsdData.SatStat[i].Iode = pData->SatStat[i].Iode;
				PsdData.SatStat[i].StatFlag = ReverseBytes( pData->SatStat[i].StatFlag );
			}

			// Get the receiver status.
			PsdData.RcvrStatus = ReverseBytes( pData->RcvrStatus );
		}
		// If wrong type, set an error.
		else
			rval = HGSEPKTERR_GPSTYPE;
	}

	return rval;
}

/**
 * Returns Honeywell H3000 Magnetometer ASCII output data.
 * \param[out]			buffer		Buffer to hold data.
 * \param[in,out]		length		Input is length of buffer, output is # bytes read.
 * \return					0 if packet is the right type.  Non-zero error otherwise.
 */
int HeroGsePacket::GetH3000MagData(char *data, int *length)
{
	int rval;

	// Verify the packet type and length.
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_H3000MAG, GSEPLEN_H3000MAG )) )
	{
		// Don't try to read more than there is.
		*length = *length < m_Header->DataLength ? *length : m_Header->DataLength;

		memcpy(data, m_Data, *length);
	}

	return rval;
}

int HeroGsePacket::GetPpsSyncData(PKT_PPSSYNCDATA_STRUCT& ppsData)
{
	int rval;

	// Verify the packet type and length.
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_PPSSYNC, sizeof(PKT_PPSSYNCDATA_STRUCT))) )
	{
		memcpy(&ppsData, m_Data, sizeof(PKT_PPSSYNCDATA_STRUCT));
	}

	return rval;
}

int HeroGsePacket::SetPpsSyncData(PKT_PPSSYNCDATA_STRUCT& ppsData)
{
	int rval;

	// Verify the packet type and length.
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_PPSSYNC )))
	{
		m_Header->DataLength = sizeof( PKT_PPSSYNCDATA_STRUCT );
		memcpy(m_Data, &ppsData, sizeof(PKT_PPSSYNCDATA_STRUCT));
		m_Altered = true;
	}

	return rval;
}

int HeroGsePacket::GetPpsSync2Data(PKT_PPSSYNC2DATA_STRUCT& ppsData)
{
	int rval;

	// Verify the packet type and length.
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_PPSSYNC2, sizeof(PKT_PPSSYNC2DATA_STRUCT))) )
	{
		memcpy(&ppsData, m_Data, sizeof(PKT_PPSSYNC2DATA_STRUCT));
	}

	return rval;
}

int HeroGsePacket::SetPpsSync2Data(PKT_PPSSYNC2DATA_STRUCT& ppsData)
{
	int rval;

	// Verify the packet type and length.
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_PPSSYNC2 )))
	{
		m_Header->DataLength = sizeof( PKT_PPSSYNC2DATA_STRUCT );
		memcpy(m_Data, &ppsData, sizeof(PKT_PPSSYNC2DATA_STRUCT));
		m_Altered = true;
	}

	return rval;
}


//	SetAspSysDataAdcValues:
///		Sets the raw ADC values into the ASP system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		FocusPos0		Focus position of camera 0 readback ADC value.
///		\param[in]		LensPress0		Lens tank pressure of camera 0 ADC value.
///		\param[in]		LensTemp0a		Lens tank temperature of camera 0 ADC value.
///		\param[in]		LensTemp0b		Lens tank temperature of camera 0 ADC value.
///		\param[in]		CpuPress		CPU tank pressure ADC value.
///		\param[in]		CpuTemp			CPU tank temperature ADC value.
///		\param[in]		CpuSpare1		CPU tank spare voltage ADC value.
///		\param[in]		CpuSpare2		CPU tank spare voltage ADC value.
///		\param[in]		FocusPos1		Focus position of camera 1 readback ADC value.
///		\param[in]		LensPress1		Lens tank pressure of camera 1 ADC value.
///		\param[in]		LensTemp1a		Lens tank temperature of camera 1 ADC value.
///		\param[in]		LensTemp1b		Lens tank temperature of camera 1 ADC value.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspSysDataAdcValues( int16_t cpuPressure, int16_t cpuTankTemp,
			int16_t cpuSpare0, int16_t cpuSpare1, int16_t camSensorTemp0,
      int16_t camAirTemp0, int16_t camPressure0, int16_t focusPos0,
      int16_t camSensorTemp1, int16_t camAirTemp1, int16_t camPressure1,
      int16_t focusPos1 )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPSYSDATA_STRUCT );

		PKT_ASPSYSDATA_STRUCT *pData = (PKT_ASPSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the raw ADC values.
		pData->CpuPressure = cpuPressure;
		pData->CpuTankTemp = cpuTankTemp;
		pData->CpuSpare[0] = cpuSpare0;
		pData->CpuSpare[1] = cpuSpare1;
    pData->camData[0].sensorTemp = camSensorTemp0;
    pData->camData[0].airTemp = camAirTemp0;
    pData->camData[0].pressure = camPressure0;
    pData->camData[0].focusPos = focusPos0;
    pData->camData[1].sensorTemp = camSensorTemp1;
    pData->camData[1].airTemp = camAirTemp1;
    pData->camData[1].pressure = camPressure1;
    pData->camData[1].focusPos = focusPos1;
	}

	// Return without error.
	return rval;
}

//	SetAspSysDataStatus:
///		Sets the system status data into the ASP system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		RelayBits		Relay states (bits 0-7 used).
///		\param[in]		GeoSrc			Geographic position source (0-2 = GPS, TANS, Manual).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspSysDataStatus( uint16_t RelayBits, int GeoSrc )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPSYSDATA_STRUCT );

		PKT_ASPSYSDATA_STRUCT *pData = (PKT_ASPSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the flag value.
		pData->RelayBits = (uint8_t)RelayBits & 0x00ff;
		pData->StatusBits = (uint8_t)(GeoSrc & 0x0003);
	}

	// Return without error.
	return rval;
}

//	SetAspSysDataFreeSpace:
///		Sets the system free disk space into the ASP system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		FreeSpace		Disk space left on image save drive.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspSysDataFreeSpace( uint32_t blocksFree,
		uint32_t blocksAvail, uint32_t blockSize )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPSYSDATA_STRUCT );

		PKT_ASPSYSDATA_STRUCT *pData = (PKT_ASPSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the disk space.
		pData->blocksFree = (uint32_t)(blocksFree);
		pData->blocksAvail = (uint32_t)(blocksAvail);
		pData->blockSize = (uint32_t)(blockSize);
	}

	// Return without error.
	return rval;
}

//	SetAspSysDataCamStatus:
///		Sets the camera status data into the ASP system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Camera			Identifies which camera status is being updated.
///		\param[in]		Mode			Expose/track/focus loop mode.
///		\param[in]		CamConn			Flag indicating if camera is connected (true) or not.
///		\param[in]		FocusAdj		Flag indicating if focus adjustment is active (true) or not.
///		\param[in]		ImSave			Flag indicating if images are being saved (true) or not.
///		\param[in]		MotorMove		Flag indicating if motor is moving (true) or not.
///		\param[in]		CamCoolStatus	Camera cooler status.
///		\return			Returns a non-zero error code if the Camera identifier is invalid, if
///						the packet does not exist, or if it is the wrong type.
///						Otherwise, returns zero to indicate success.
int HeroGsePacket::SetAspSysDataCamStatus( int Camera, int Mode, bool CamConn,
		bool FocusAdj, bool ImSave, bool MotorMove, int CatSize, int Activity,
		int FocusStepCount )
{
	// Return if the camera is not a valid identifier.
	if ( Camera < 0 || Camera > 1 )
		return HGSEPKTERR_USERPARAM;

	// If the packet type is correct, set the data.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPSYSDATA_STRUCT );

		PKT_ASPSYSDATA_STRUCT *pData = (PKT_ASPSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		pData->camData[Camera].status = Mode & 0x03;

		if ( FocusAdj )
			pData->camData[Camera].status |= 0x10;
		if ( ImSave )
			pData->camData[Camera].status |= 0x20;
		if ( MotorMove )
			pData->camData[Camera].status |= 0x40;
		if ( CamConn )
			pData->camData[Camera].status |= 0x80;

		pData->camData[Camera].activity = Activity;
		pData->camData[Camera].focusStepPos = FocusStepCount;
		pData->camData[Camera].catSize = CatSize;
	}

	// Return without error.
	return rval;
}

//	GetAspSysData:
///		Gets the converted user data from the ASP system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		Data			Structure holding converted ASP system data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetAspSysData( USR_ASPSYSDATA_STRUCT &Data )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_ASPSYSDATA, sizeof( PKT_ASPSYSDATA_STRUCT) )) )
	{
		// Get a pointer to the data.
		PKT_ASPSYSDATA_STRUCT *pData = (PKT_ASPSYSDATA_STRUCT *)m_Data;

		// Get the system values (non-ADC).
		Data.blocksFree = (int)pData->blocksFree;
		Data.blocksAvail = (int)pData->blocksAvail;
		Data.blockSize = (int)pData->blockSize;
		Data.RelayBits = (int)pData->RelayBits;
		Data.GeoPosSrc = (int)(pData->StatusBits & 0x03);
	  Data.CpuPressure = AspAdcConverter::cnvPressure(pData->CpuPressure);
    Data.CpuTankTemp = AspAdcConverter::cnvLM50Tempr( pData->CpuTankTemp );

		// Convert the raw ADC values.
    for ( int i = 0; i < 2; i++ )
    {
		  Data.CamData[i].SensorTemp = 
        AspAdcConverter::cnvLM50Tempr(pData->camData[i].sensorTemp);
		  Data.CamData[i].AirTemp = 
        AspAdcConverter::cnvLM50Tempr(pData->camData[i].airTemp);
		  Data.CamData[i].Pressure = 
        AspAdcConverter::cnvPressure(pData->camData[i].pressure);
		  Data.CamData[i].FocusPos = 
        AspAdcConverter::cnvFocusPos(pData->camData[i].focusPos);

		  // Get the camera values (non-ADC) for each camera.
		  Data.CamData[i].Mode = (pData->camData[i].status & 0x03);
		  Data.CamData[i].FocusAdj = (pData->camData[i].status & 0x10) > 0;
		  Data.CamData[i].SavingImages = (pData->camData[i].status & 0x20) > 0;
		  Data.CamData[i].MotorMoving = (pData->camData[i].status & 0x40) > 0;
		  Data.CamData[i].Connected = (pData->camData[i].status & 0x80) > 0;
		  Data.CamData[i].FocusStepPos = (int)pData->camData[i].focusStepPos;
		  Data.CamData[i].CatSize = pData->camData[i].catSize;
		  Data.CamData[i].Activity = pData->camData[i].activity;
    }
	}

	// Return without error.
	return rval;
}

//	SetAspParamCamID:
///		Sets the camera ID into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		CamID			The camera identification number.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamCamID( uint8_t CamID )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CamID = CamID;
	}

	// Return without error.
	return rval;
}

//	SetAspParamCamID:
///		Sets the camera ID into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		CamID			The camera identification number.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamTrkMode( int Mode )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->Mode = (uint8_t)Mode;
	}

	// Return without error.
	return rval;
}

//	SetAspParamFlags:
///		Sets the flag bits into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		GeoPosSrc		Two-bit value indicating the geographic position source.
///		\param[in]		CamXFlip		Flag indicating if camera x-axis is flipped for analysis.
///		\param[in]		CamYFlip		Flag indicating if camera x-axis is flipped for analysis.
///		\param[in]		CamCoolerOn		Flag indicating if camera cooler is on.
///		\param[in]		CamConnected	Flag indicating if camera is connected.
///		\param[in]		TrkFlatFlag		Flag indicating if flag background model is used in image analysis.
///		\param[in]		UseCECirc		Flag controlling use of corner exclusion circle.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspParamFlags( int GeoPosSrc, bool CamXFlip, bool CamYFlip,
		bool CamCoolerOn, bool CamConnect, bool TrkFlatFlag, bool UseCECirc )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->Flags = GeoPosSrc & 0x03;
		if ( CamXFlip )
			pData->Flags |= 0x04;
		if ( CamYFlip )
			pData->Flags |= 0x08;
		if ( CamCoolerOn )
			pData->Flags |= 0x10;
		if ( CamConnect )
			pData->Flags |= 0x20;
		if ( TrkFlatFlag )
			pData->Flags |= 0x40;
		if ( UseCECirc )
			pData->Flags |= 0x80;
	}

	// Return without error.
	return rval;
}

//	SetAspParamCamExposure:
///		Sets the camera exposure time into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		ExpSec			The camera exposure time in seconds.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamCamExposure( float ExpSec )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CamExposure = (uint16_t)(ExpSec * 1000.0f);
	}

	// Return without error.
	return rval;
}

//	SetAspCamSetpoint:
///		Sets the camera cooler setpoint the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		SetPoint		Camera cooler set point (C).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamCamSetpoint( float SetPoint )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CamSetPoint = uint16_t((SetPoint * 10.0f) + 2730.0f);
	}

	// Return without error.
	return rval;
}

//	SetAspParamCamCenter:
///		Sets the camera image dynamical center into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		SetPoint		Camera cooler set point (C).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamCamCenter( uint16_t CamXCenter, uint16_t CamYCenter,
		float RotAngle )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CamXCenter = CamXCenter;
		pData->CamYCenter = CamYCenter;
		pData->CamImRotAngle = RotAngle;
	}

	// Return without error.
	return rval;
}

//	SetAspParamCamParam:
///		Sets the camera image scale, electrons per count and rotation angle into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		ImScale			Camera + lens image scale (deg/pixel).
///		\param[in]		EPerCount		Camera electrons per CCD count.
///		\param[in]		RotAngle		Rotation angle between camera axes and Alt/Az axes (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamCamParam( float ImScale, float EPerCount )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CamImScale = ImScale;
		pData->CamEPerCount = EPerCount;
	}

	// Return without error.
	return rval;
}

//	SetAspParamTrkThresh:
///		Sets the tracking pixel and object significance thresholds into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		PixThresh		Number of sigma above background for a pixel to be a valid object pixel.
///		\param[in]		ObjThresh		Number of sigma above background for an object to be valid0.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamTrkThresh( float PixThresh, float ObjThresh )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->TrkPixThresh = (uint16_t)(PixThresh * 100.0f);
		pData->TrkObjThresh = (uint16_t)(ObjThresh * 100.0f);
	}

	// Return without error.
	return rval;
}

//	SetAspParamTrkMaxErr:
///		Sets the maximum catalog - image position error into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		MaxPosErr		Maximum catalog - image distance considered a match (pixels).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamTrkMaxErr( float MaxPosErr )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->TrkMaxErr = (uint16_t)(MaxPosErr * 1000.0f);
	}

	// Return without error.
	return rval;
}

//	SetAspParamTrkMinID:
///		Sets the minimum number of identified objects into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		MinID		Minimum number of identified objects for a valid aspect fit.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamTrkMinID( int MinID )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->TrkMinID = (uint8_t)MinID;
	}

	// Return without error.
	return rval;
}

//	SetAspParamTrkWidths:
///		Sets the minimum object radius and background filter width into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		MinObjRadius		Minimum object radius for photometery (pixels).
///		\param[in]		FilterWidth			Width of digital low-pass filter used to model background (pixels).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamTrkWidths( uint16_t MinObjRadius, int FilterWidth )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->TrkMinRadius = (uint8_t)MinObjRadius;
		pData->TrkFilterWidth = (uint8_t)FilterWidth;
	}

	// Return without error.
	return rval;
}

//	SetAspParamCatTarget:
///		Sets the target RA and Dec into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		RA				Right ascension of target (deg).
///		\param[in]		Dec				Delination of target (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamCatTarget( float RA, float Dec )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CatTargetRA = RA;
		pData->CatTargetDec = Dec;
	}

	// Return without error.
	return rval;
}

//	SetAspParamCatParam:
///		Sets the catalog parameters into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Radius			Radius of the target subcatalog (deg).
///		\param[in]		VMin			Minimum visual magnitude of the target subcatalog.
///		\param[in]		VMax			Maximum visual magnitude of the target subcatalog.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamCatParam( float Radius, float VMin, float VMax )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CatRadius = (uint16_t)(Radius * 1000.0f);
		pData->CatVMin = (uint8_t)(VMin * 10.0f);
		pData->CatVMax = (uint8_t)(VMax * 10.0f);
	}

	// Return without error.
	return rval;
}

//	SetAspParamGeoPos:
///		Sets the longitude and latitude into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Longitude		Current longitude (deg).
///		\param[in]		Latitude		Current latitude (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamGeoPos( float Longitude, float Latitude )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->Longitude = Longitude;
		pData->Latitude = Latitude;
	}

	// Return without error.
	return rval;
}

//	SetAspParamFocusMotorRate:
///		Sets the focus motor step rate into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		RateHz			Focus motor step rate (Hz).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspParamFocusMotorRate( int RateHz )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->FocusMotorRate = (uint16_t)RateHz;
	}

	// Return without error.
	return rval;
}

//	SetAspParamFocusParam:
///		Sets the auto-focus algorithm parameters into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		FFigType		Focus figure type indicator.
///		\param[in]		MinPos			Minimum valid focus stepper motor position (steps).
///		\param[in]		MaxPos			Maximum valid focus stepper motor position (steps).
///		\param[in]		Backlash		Number of steps to remove motor backlash (steps).
///		\param[in]		NLoops			Number of loops in auto-focus algorithm.
///		\param[in]		StepSize1		Number of motor steps between images in this loop.
///		\param[in]		StepSize2		Number of motor steps between images in this loop.
///		\param[in]		StepSize3		Number of motor steps between images in this loop.
///		\param[in]		StepSize4		Number of motor steps between images in this loop.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspParamFocusParam( uint8_t FfigType, int MinPos,
		int MaxPos, int Backlash, int NSteps, int StepSize )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->FocusFlags &= 0xf0;
		pData->FocusFlags |= (0x0f & FfigType);
		pData->FocusMin = (short)MinPos;
		pData->FocusMax = (short)MaxPos;
		pData->FocusBacklash = (uint8_t)Backlash;
		pData->FocusNSteps = (uint8_t)NSteps;
		pData->FocusStepSize = StepSize;
	}

	// Return without error.
	return rval;
}

//	SetAspParamOffsetParam:
///		Sets the camera offset parameters into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Offset			Offset of camera from line of sight (degrees).
///		\param[in]		OffsetZPA		Position angle (relative to zenith) of offset (degrees).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspParamOffsetParam( float Offset, float OffsetZPA )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CamOffset = Offset;
		pData->OffsetZPA = OffsetZPA;
	}

	// Return without error.
	return rval;
}

//	SetAspParamUsbEnum:
///		Sets the camera USB enumeration parameter into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		UsbEnum			System-enumerated USB ID of camera.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspParamUsbEnum( uint8_t UsbEnum )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->UsbEnum = UsbEnum;
	}

	// Return without error.
	return rval;
}

//	SetAspParamFocusMode:
///		Sets the camera focus mode into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		FocusMode			Focus mode for the given camera.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspParamFocusMode( uint8_t FocusMode )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->FocusFlags &= 0x0f;
		pData->FocusFlags |= (0x0f & FocusMode) << 4;
	}

	// Return without error.
	return rval;
}

//	SetAspParamCECircParam:
///		Sets the image daylight corner exclusion paramers into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		FocusMode			Focus mode for the given camera.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspParamCECircParam( uint16_t XCenter, uint16_t YCenter,
											uint16_t Radius )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CeCircXCenter = XCenter;
		pData->CeCircYCenter = YCenter;
		pData->CeCircRadius = Radius;
	}

	// Return without error.
	return rval;
}

//	SetAspParamEdgeOffsets:
///		Sets the image edge exclusion paramers into the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		LeftOffset			Number of pixels excluded on left edge.
///		\param[in]		RightOffset			Number of pixels excluded on right edge.
///		\param[in]		BottomOffset		Number of pixels excluded on bottom edge.
///		\param[in]		TopOffset			Number of pixels excluded on top edge.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspParamEdgeOffsets( uint32_t LeftOffset, uint32_t RightOffset, uint32_t BottomOffset,
		uint32_t TopOffset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->LeftEdgeOffset = (uint16_t)LeftOffset;
		pData->RightEdgeOffset = (uint16_t)RightOffset;
		pData->BottomEdgeOffset = (uint16_t)BottomOffset;
		pData->TopEdgeOffset = (uint16_t)TopOffset;
	}

	// Return without error.
	return rval;
}

//	SetAspParamBiasParam:
///		Sets the image dark bias level and standard deviation.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		BiasCount			Dark bias level (in pixel counts).
///		\param[in]		BiasSigma			Dark bias level standard deviation (in pixel counts).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspParamBiasParam( double BiasCount, double BiasSigma )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->BiasCount = (uint16_t)BiasCount;
		pData->BiasSigma = (uint16_t)(BiasSigma * 1000.0);
	}

	// Return without error.
	return rval;
}

//	SetAspParamImSize:
///		Sets the image default dimensions.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		XSize				Number of horizontal pixels in image.
///		\param[in]		YSize				Number of vertical pixels in image.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspParamImSize( int XSize, int YSize )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->ImageXSize = (uint16_t)XSize;
		pData->ImageYSize = (uint16_t)YSize;
	}

	// Return without error.
	return rval;
}

//	SetAspParamExtFlags:
///		Sets the extended parameter flags.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		UseCenterQuad		Flag to use only central quadrant of camera (true) or entire CCD (false).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspParamExtFlags( bool UseCenterQuad, bool saveImages )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->ExtendedFlags = 0;

		// Set the data bits based on the input.
		if ( UseCenterQuad )
			pData->ExtendedFlags |= 0x01;
		if ( saveImages )
		  pData->ExtendedFlags |= 0x02;
	}

	// Return without error.
	return rval;
}

//	GetAspParamData:
///		Gets the converted user data from the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		Data			Structure holding converted ASP parameter data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspParamData( USR_ASPPARAMDATA_STRUCT &Data )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPPARAMDATA_STRUCT );

		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Get the aspect camera ID.
		pData->CamID = (uint8_t)Data.CamID;

		// Get the flag values from the flag bits.
		pData->Flags = (Data.GeoPosSrc & 0x03);
		pData->Flags |= Data.CamXFlip ? 0x04 : 0;
		pData->Flags |= Data.CamYFlip ? 0x08 : 0;
		pData->Flags |= Data.CamCoolerOn ? 0x10 : 0;
		pData->Flags |= Data.CamConnect ? 0x20 : 0;
		pData->Flags |= Data.TrkFlatFlag ? 0x40 : 0;
		pData->Flags |= Data.CeCircEnable ? 0x80 : 0;

		// Convert the camera parameters.
		pData->CamExposure = (uint16_t)Data.CamExposure * 1000;
		pData->CamSetPoint = (uint16_t)((Data.CamSetPoint + 273.0f)*10.0f);
		pData->CamXCenter = Data.CamXCenter;
		pData->CamYCenter = Data.CamYCenter;
		pData->CamImScale = Data.CamImScale;
		pData->CamEPerCount = Data.CamEPerCount;
		pData->CamImRotAngle = Data.CamImRotAngle;

		// Convert the track parameters.
		pData->TrkMinID = (uint8_t)Data.TrkMinID;
		pData->TrkMaxErr = (uint8_t)(Data.TrkMaxError * 1000.0f);
		pData->TrkFilterWidth = (uint8_t)Data.TrkFilterWidth;
		pData->TrkMinRadius = Data.TrkMinRadius;
		pData->TrkPixThresh = (uint16_t)(Data.TrkPixThresh * 100.0f);
		pData->TrkObjThresh = (uint16_t)(Data.TrkObjThresh * 100.0f);

		// Convert the catalog parameters.
		pData->CatTargetRA = Data.CatTargetRA;
		pData->CatTargetDec = Data.CatTargetDec;
		pData->CatRadius = (uint16_t)(Data.CatRadius * 1000.0f);
		pData->CatVMin = (uint8_t)(Data.CatVMin * 10.0f);
		pData->CatVMax = (uint8_t)(Data.CatVMax * 10.0f);
		pData->CatNObj = Data.CatNObj;

		// Convert the focus parameters.
		pData->FocusMotorRate = (uint16_t)Data.FocusMotorRate;
		pData->FocusMin = (uint16_t)Data.FocusMin;
		pData->FocusMax = (uint16_t)Data.FocusMax;
		pData->FocusBacklash = (uint8_t)Data.FocusBacklash;
		pData->FocusNSteps = (uint8_t)Data.FocusNSteps;
		pData->FocusStepSize = (uint16_t)Data.FocusStepSize;

		// Copy the longitude and latitude.
		pData->Longitude = Data.Longitude;
		pData->Latitude = Data.Latitude;

		// Set the offset data.
		pData->CamOffset = Data.CamOffset;
		pData->OffsetZPA = Data.OffsetZPA;

		// Set the system-enumerated USB ID.
		pData->UsbEnum = Data.UsbEnum;

		// Get the focus figure type.
		pData->FocusFlags = Data.FocusFfigType & 0x0f;

		// Get the corner exclusion parameters.
		pData->CeCircXCenter = Data.CeCircXCenter;
		pData->CeCircYCenter = Data.CeCircYCenter;
		pData->CeCircRadius = Data.CeCircRadius;

		// Get the edge exclusion parameters.
		pData->LeftEdgeOffset = (uint16_t)Data.LeftEdgeOffset;
		pData->RightEdgeOffset = (uint16_t)Data.RightEdgeOffset;
		pData->BottomEdgeOffset = (uint16_t)Data.BottomEdgeOffset;
		pData->TopEdgeOffset = (uint16_t)Data.TopEdgeOffset;

		// Get the dark bias parameters.
		pData->BiasCount = (uint16_t)Data.BiasCount;
		pData->BiasSigma = (uint16_t)(Data.BiasSigma * 1000.0);

		// Get the image size parameters.
		pData->ImageXSize = (uint16_t)pData->ImageXSize;
		pData->ImageYSize = (uint16_t)pData->ImageYSize;

		// Parse the extended flags.
		pData->ExtendedFlags = Data.UseCenterQuad ? 0x01 : 0;
		pData->ExtendedFlags |= Data.SaveIm ? 0x02 : 0;
	}

	// Return without error.
	return rval;
}

//	GetAspParamData:
///		Gets the converted user data from the ASP parameter data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		Data			Structure holding converted ASP parameter data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetAspParamData( USR_ASPPARAMDATA_STRUCT &Data )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_ASPPARAM, sizeof( PKT_ASPPARAMDATA_STRUCT) )) )
	{
		// Get a pointer to the data.
		PKT_ASPPARAMDATA_STRUCT *pData = (PKT_ASPPARAMDATA_STRUCT *)m_Data;

		// Get the aspect camera ID.
		Data.CamID = (int)pData->CamID;

		// Get the flag values from the flag bits.
		Data.GeoPosSrc = pData->Flags & 0x0003;
		Data.CamXFlip = (pData->Flags & 0x0004) > 0;
		Data.CamYFlip = (pData->Flags & 0x0008) > 0;
		Data.CamCoolerOn = (pData->Flags & 0x0010) > 0;
		Data.CamConnect = (pData->Flags & 0x0020) > 0;
		Data.TrkFlatFlag = (pData->Flags & 0x0040) > 0;
		Data.CeCircEnable = (pData->Flags & 0x0080) > 0;

		// Convert the camera parameters.
		Data.CamExposure = (float)pData->CamExposure * 0.001f;
		Data.CamSetPoint = (float)pData->CamSetPoint * 0.1f - 273.0f;
		Data.CamXCenter = pData->CamXCenter;
		Data.CamYCenter = pData->CamYCenter;
		Data.CamImScale = pData->CamImScale;
		Data.CamEPerCount = pData->CamEPerCount;
		Data.CamImRotAngle = pData->CamImRotAngle;

		// Convert the track parameters.
		Data.TrkMinID = (uint32_t)pData->TrkMinID;
		Data.TrkMaxError = (float)pData->TrkMaxErr * 0.001f;
		Data.TrkFilterWidth = (int)pData->TrkFilterWidth;
		Data.TrkMinRadius = pData->TrkMinRadius;
		Data.TrkPixThresh = (float)pData->TrkPixThresh * 0.01f;
		Data.TrkObjThresh = (float)pData->TrkObjThresh * 0.01f;

		// Convert the catalog parameters.
		Data.CatTargetRA = pData->CatTargetRA;
		Data.CatTargetDec = pData->CatTargetDec;
		Data.CatRadius = (float)pData->CatRadius * 0.001f;
		Data.CatVMin = (float)pData->CatVMin * 0.1f;
		Data.CatVMax = (float)pData->CatVMax * 0.1f;
		Data.CatNObj = (int)pData->CatNObj;

		// Convert the focus parameters.
		Data.FocusMotorRate = (int)pData->FocusMotorRate;
		Data.FocusMin = (int)pData->FocusMin;
		Data.FocusMax = (int)pData->FocusMax;
		Data.FocusBacklash = (int)pData->FocusBacklash;
		Data.FocusNSteps = (int)pData->FocusNSteps;
		Data.FocusStepSize = (int)pData->FocusStepSize;

		// Copy the longitude and latitude.
		Data.Longitude = pData->Longitude;
		Data.Latitude = pData->Latitude;

		// Set the offset data.
		Data.CamOffset = pData->CamOffset;
		Data.OffsetZPA = pData->OffsetZPA;

		// Set the system-enumerated USB ID.
		Data.UsbEnum = pData->UsbEnum;

		// Get the focus figure type.
		Data.FocusFfigType = pData->FocusFlags & 0x0f;

		// Get the corner exclusion parameters.
		Data.CeCircXCenter = pData->CeCircXCenter;
		Data.CeCircYCenter = pData->CeCircYCenter;
		Data.CeCircRadius = pData->CeCircRadius;

		// Get the edge exclusion parameters.
		Data.LeftEdgeOffset = (uint32_t)pData->LeftEdgeOffset;
		Data.RightEdgeOffset = (uint32_t)pData->RightEdgeOffset;
		Data.BottomEdgeOffset = (uint32_t)pData->BottomEdgeOffset;
		Data.TopEdgeOffset = (uint32_t)pData->TopEdgeOffset;

		// Get the dark bias parameters.
		Data.BiasCount = (double)pData->BiasCount;
		Data.BiasSigma = (double)pData->BiasSigma * 0.001;

		// Get the image size parameters.
		Data.ImageXSize = (int)pData->ImageXSize;
		Data.ImageYSize = (int)pData->ImageYSize;

		// Parse the extended flags.
		Data.UseCenterQuad = (pData->ExtendedFlags & 0x01) > 0;
		Data.SaveIm = (pData->ExtendedFlags & 0x02) > 0;
	}

	// Return without error.
	return rval;
}

//	SetAspImCamID:
///		Sets the source camera ID into the ASP image data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		CamID			Idntification number of source camera.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspImCamID( int CamID )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPIMAGE )) )
	{
		// Set the data length.
		m_Header->DataLength = m_Header->DataLength > sizeof( PKT_ASPIMDATA_STRUCT ) ?
      m_Header->DataLength : sizeof( PKT_ASPIMDATA_STRUCT );

		PKT_ASPIMDATA_STRUCT *pData = (PKT_ASPIMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CamID = (uint8_t)CamID;
	}

	// Return without error.
	return rval;
}

//	SetAspImTargetData:
///		Sets the target information into the ASP image data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		RA				Right ascension of target (deg).
///		\param[in]		Dec				Declination of target (deg).
///		\param[in]		Azimuth			Azimuth of target at center of image exposure (deg).
///		\param[in]		Altitude		Altitude of target at ceter of image exposure (deg).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspImTargetData( float RA, float Dec, float Azimuth, float Altitude )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPIMAGE )) )
	{
		// Set the data length.
		m_Header->DataLength = m_Header->DataLength > sizeof( PKT_ASPIMDATA_STRUCT ) ?
      m_Header->DataLength : sizeof( PKT_ASPIMDATA_STRUCT );

		PKT_ASPIMDATA_STRUCT *pData = (PKT_ASPIMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->TargetRA = RA;
		pData->TargetDec = Dec;
		pData->TargetAz = Azimuth;
		pData->TargetAlt = Altitude;
	}

	// Return without error.
	return rval;
}

//	SetAspImTrackData:
///		Sets the image tracking aspect information into the ASP image data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		XOff			X-axis offset of target from dynamical center of image.
///		\param[in]		YOff			Y-axis offset of target from dynamical center of image.
///		\param[in]		RA				Right ascension of dynamical center of image (deg).
///		\param[in]		RASigma			Standard deviation of RA value (deg).
///		\param[in]		Dec				Declination of dynamical center of image (deg).
///		\param[in]		DecSigma		Standard deviation of Dec value (deg).
///		\param[in]		Azimuth			Azimuth of dynamical center of image (deg).
///		\param[in]		Altitude		Altitude of dynamical center of image (deg).
///		\param[in]		Roll			Roll angle about dynamical center of image (deg).
///		\param[in]		NObjFound		Number of objects found in image.
///		\param[in]		NObjID			Number of objects identified in image.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspImTrackData( float XOff, float YOff, float RA, float Dec, float RASigma,
		float DecSigma, float Azimuth, float Altitude, float Roll, int NObjFound, int NObjID )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPIMAGE )) )
	{
		// Set the data length.
		m_Header->DataLength = m_Header->DataLength > sizeof( PKT_ASPIMDATA_STRUCT ) ?
      m_Header->DataLength : sizeof( PKT_ASPIMDATA_STRUCT );

		PKT_ASPIMDATA_STRUCT *pData = (PKT_ASPIMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->XOffset = XOff;
		pData->YOffset = YOff;
		pData->CenterRA = RA;
		pData->CenterDec = Dec;
		pData->SigmaRA = RASigma;
		pData->SigmaDec = DecSigma;
		pData->CenterAz = Azimuth;
		pData->CenterAlt = Altitude;
		pData->CenterRoll = Roll;
		pData->NObjFound = NObjFound;
		pData->NObjID = NObjID;
	}

	// Return without error.
	return rval;
}

//	SetAspImStatData:
///		Sets the image statistical information into the ASP image data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		PixMin			Minimum value of all image pixels.
///		\param[in]		PixMax			Maximum value of all image pixels.
///		\param[in]		PixMean			Arithmetic mean value of all image pixels.
///		\param[in]		PixSigma		Standard deviation of all image pixels.
///		\param[in]		BgdMean			Arithmetic mean of all background pixels in image.
///		\param[in]		BgdSigma		Standard deviation of background pixels.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspImStatData( uint16_t PixMin, uint16_t PixMax, float PixMean, float PixSigma,
								   float BgdMean, float BgdSigma )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPIMAGE )) )
	{
		// Set the data length.
		m_Header->DataLength = m_Header->DataLength > sizeof( PKT_ASPIMDATA_STRUCT ) ?
      m_Header->DataLength : sizeof( PKT_ASPIMDATA_STRUCT );

		PKT_ASPIMDATA_STRUCT *pData = (PKT_ASPIMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->PixMin = PixMin;
		pData->PixMax = PixMax;
		pData->PixMean = (uint16_t)PixMean;
		pData->PixSigma = (uint16_t)(PixSigma * 10.0f);
		pData->BgdMean = (uint16_t)BgdMean;
		pData->BgdSigma = (uint16_t)(BgdSigma * 10.0f);
	}

	// Return without error.
	return rval;
}

//	SetAspImObjectData:
///		Sets the object information for a single object into the ASP image data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Index			Index of object in object array.
///		\param[in]		XPos			X-axis position of object in image (pixels).
///		\param[in]		YPos			Y-axis position of object in image (pixels).
///		\param[in]		ID				ID number of object from target sub-catalog.
///		\param[in]		Counts			Total counts (minus background) of object (CCD counts).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspImObjectData( int Index, double XPos, double YPos, uint16_t ID, double Counts )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPIMAGE )) && Index < ASPIM_MAXNOBJ )
	{
		// Set the data length.
		m_Header->DataLength = m_Header->DataLength > (sizeof( PKT_ASPIMDATA_STRUCT )
			+ sizeof( PKT_ASPIMOBJ_STRUCT ) * (Index + 1) ) ? m_Header->DataLength :
      (sizeof( PKT_ASPIMDATA_STRUCT ) + sizeof( PKT_ASPIMOBJ_STRUCT ) * (Index + 1) );

		PKT_ASPIMOBJ_STRUCT *ObjList =  (PKT_ASPIMOBJ_STRUCT *)&m_Data[sizeof( PKT_ASPIMDATA_STRUCT )];
		m_Altered = true;

		// Set the values.
		ObjList[Index].X = (uint16_t)XPos;
		ObjList[Index].Y = (uint16_t)YPos;
		ObjList[Index].ID = (uint16_t)ID;
		ObjList[Index].Count = (uint16_t)(4000.0 * log( Counts ));
	}

	// Return without error.
	return rval;
}

//	SetAspImCenterData:
///		Sets the image dynamical center into the ASP image data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		XCenter			X-axis coordinate of dynamical center of image.
///		\param[in]		YCenter			Y-axis coordinate of dynamical center of image.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int	HeroGsePacket::SetAspImCenterData( uint16_t XCenter, uint16_t YCenter )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPIMAGE )) )
	{
		// Set the data length.
		m_Header->DataLength = m_Header->DataLength > sizeof( PKT_ASPIMDATA_STRUCT ) ?
      m_Header->DataLength : sizeof( PKT_ASPIMDATA_STRUCT );

		PKT_ASPIMDATA_STRUCT *pData = (PKT_ASPIMDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CenterX = XCenter;
		pData->CenterY = YCenter;
	}

	// Return without error.
	return rval;
}

//	GetAspImData:
///		Gets the converted user data from the ASP image data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		Data			Structure holding converted ASP image data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetAspImData( USR_ASPIMDATA_STRUCT &Data )
{
	int rval;
	int psize = sizeof( PKT_ASPIMDATA_STRUCT ) + sizeof( PKT_ASPIMOBJ_STRUCT ) * ASPIM_MAXNOBJ;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_ASPIMAGE, psize )) )
	{
		// Get a pointer to the data.
		PKT_ASPIMDATA_STRUCT *pData = (PKT_ASPIMDATA_STRUCT *)m_Data;

		// Get the camera ID.
		Data.CamID = pData->CamID;

		// Get the target information.
		Data.TargetRA = pData->TargetRA;
		Data.TargetDec = pData->TargetDec;
		Data.TargetAz = pData->TargetAz;
		Data.TargetAlt = pData->TargetAlt;

		// Get the aspect solution information.
		Data.XOffset = pData->XOffset;
		Data.YOffset = pData->YOffset;
		Data.CenterRA = pData->CenterRA;
		Data.CenterDec = pData->CenterDec;
		Data.SigmaRA = pData->SigmaRA;
		Data.SigmaDec = pData->SigmaDec;
		Data.CenterAz = pData->CenterAz;
		Data.CenterAlt = pData->CenterAlt;
		Data.CenterRoll = pData->CenterRoll;

		// Get the statistics data.
		Data.PixMin = pData->PixMin;
		Data.PixMax = pData->PixMax;
		Data.PixMean = pData->PixMean;
		Data.PixSigma = (float)pData->PixSigma * 0.1f;
		Data.BgdMean = pData->BgdMean;
		Data.BgdSigma = (float)pData->BgdSigma * 0.1f;

		// Get the number of objects.
		Data.NObjFound = pData->NObjFound;
		Data.NObjID = pData->NObjID;

		// Get the center pixels.
		Data.CenterX = pData->CenterX;
		Data.CenterY = pData->CenterY;

		// Get the object data.
		PKT_ASPIMOBJ_STRUCT *ObjList =  (PKT_ASPIMOBJ_STRUCT *)&m_Data[sizeof( PKT_ASPIMDATA_STRUCT )];
		int nobj = Data.NObjFound < ASPIM_MAXNOBJ ? Data.NObjFound : ASPIM_MAXNOBJ;
		for ( int i = 0; i < nobj; i++ )
		{
			Data.ObjList[i].X = ObjList[i].X;
			Data.ObjList[i].Y = ObjList[i].Y;
			Data.ObjList[i].ID = ObjList[i].ID;
			Data.ObjList[i].Count = exp( (double)ObjList[i].Count / 4000.0 );
			Data.ObjList[i].Size = 1.0 > (((sqrt( (double)ObjList[i].Count ) - 120.0) / 6.0 ) + 3.0) ?
        1.0 : (((sqrt( (double)ObjList[i].Count ) - 120.0) / 6.0 ) + 3.0);
		}
	}

	// Return without error.
	return rval;
}

//	SetAspFocusCamID:
///		Sets the source camera ID into ASP focus data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		CamID			Identification number of source camera.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspFocusCamID( int CamID )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPFOCUS )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPFOCUSDATA_STRUCT );

		PKT_ASPFOCUSDATA_STRUCT *pData = (PKT_ASPFOCUSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->CamID = (uint8_t)CamID;
	}

	// Return without error.
	return rval;
}

//	SetAspFocusCountData:
///		Sets the aspect focus loop index and point count into ASP focus data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		NPoints			Number of valid points in focus data array.
///		\param[in]		centroid		Focus centroid of the points.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspFocusDataParam( int NPoints, double centroid )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPFOCUS )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPFOCUSDATA_STRUCT );

		PKT_ASPFOCUSDATA_STRUCT *pData = (PKT_ASPFOCUSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->Centroid = (uint16_t)centroid;
    pData->NPoints = NPoints < ASPIM_MAXNFPOINTS ? NPoints : ASPIM_MAXNFPOINTS;
	}

	// Return without error.
	return rval;
}

//	SetAspFocusPointData:
///		Sets a focus loop motor position and focus figure into an ASP focus data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		IPoint			Index of point in focus data array.
///		\param[in]		Position		Focus motor step position image was taken at.
///		\param[in]		FFig			Focus figure calculated from image at this position.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspFocusDataPoint( int IPoint, int Position, double FFig )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPFOCUS )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPFOCUSDATA_STRUCT );

		PKT_ASPFOCUSDATA_STRUCT *pData = (PKT_ASPFOCUSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Constrain the array size and the index.
		pData->NPoints = pData->NPoints < ASPIM_MAXNFPOINTS ?
				pData->NPoints : ASPIM_MAXNFPOINTS;
    int index = IPoint < pData->NPoints ? IPoint : pData->NPoints;
		index = 0 > IPoint ? 0 : IPoint;

		// Set the data.
		pData->Point[index].Position = (uint16_t)Position;
		pData->Point[index].FocusFig = (float)FFig;
	}

	// Return without error.
	return rval;
}

//	GetAspFocusData:
///		Gets the converted user data from the ASP focus data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		Data			Structure holding converted ASP focus data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetAspFocusData( USR_ASPFOCUSDATA_STRUCT &Data )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_ASPFOCUS, sizeof( PKT_ASPFOCUSDATA_STRUCT ) )) )
	{
		// Get a pointer to the data.
		PKT_ASPFOCUSDATA_STRUCT *pData = (PKT_ASPFOCUSDATA_STRUCT *)m_Data;

    // Get the camera ID.
    Data.CamID = pData->CamID;

		// Get the loop number and number of valid points.
		Data.Centroid = pData->Centroid;
		Data.NPoints = pData->NPoints < ASPIM_MAXNFPOINTS ? pData->NPoints : ASPIM_MAXNFPOINTS;

		// Get the focus information.
		for ( int i = 0; i < Data.NPoints; i++ )
		{
			Data.Position[i] = (double)pData->Point[i].Position;
			Data.FocusFig[i] = (double)pData->Point[i].FocusFig;
		}
	}

	// Return without error.
	return rval;
}

///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspCamStatus( int camID, bool connected, bool coolerOn,
		int coolStatus, float coolPower, float ccdTemp )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPCAMSTATUS )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPCAMSTATUS_STRUCT );

		PKT_ASPCAMSTATUS_STRUCT *pData = (PKT_ASPCAMSTATUS_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->StatusBits = coolStatus & 0x03;
		pData->CamID = (uint8_t)camID;
		if (connected)
			pData->StatusBits |= 0x10;
		if (coolerOn)
			pData->StatusBits |= 0x20;

		pData->CoolPower = (uint16_t)(coolPower);
		pData->CcdTemp = (int16_t)(ccdTemp * 100.0f);
	}

	// Return without error.
	return rval;
}

///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetAspCamStatus( USR_ASPCAMSTATUS_STRUCT &Data )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_ASPCAMSTATUS,
			sizeof( PKT_ASPCAMSTATUS_STRUCT ) )) )
	{
		// Get a pointer to the data.
		PKT_ASPCAMSTATUS_STRUCT *pData = (PKT_ASPCAMSTATUS_STRUCT *)m_Data;

		Data.CamID = pData->CamID;
		Data.Connected = (pData->StatusBits & 0x10) > 0;
		Data.CoolerOn = (pData->StatusBits & 0x20) > 0;
		Data.CoolingStatus = (pData->StatusBits & 0x03);
		Data.CoolPower = (double)pData->CoolPower;
		Data.CcdTemp = (double)pData->CcdTemp * 0.01;
	}

	// Return without error.
	return rval;
}

///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetAspModeChange( int camID, int newMode, int oldMode,
		int modeStatus, int modeActivity )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ASPMODECHANGE )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_ASPMODECHANGE_STRUCT );

		PKT_ASPMODECHANGE_STRUCT *pData = (PKT_ASPMODECHANGE_STRUCT *)m_Data;
		m_Altered = true;

		// Set the values.
		pData->CamID = (uint8_t)camID;
		pData->newMode = (uint8_t)newMode;
		pData->oldMode = (uint8_t)oldMode;
		pData->modeStatus = (uint8_t)modeStatus;
		pData->modeActivity = (uint8_t)modeActivity;
	}

	// Return without error.
	return rval;
}

///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetAspModeChange( USR_ASPMODECHANGE_STRUCT &Data )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_ASPMODECHANGE,
			sizeof( PKT_ASPMODECHANGE_STRUCT ) )) )
	{
		// Get a pointer to the data.
		PKT_ASPMODECHANGE_STRUCT *pData = (PKT_ASPMODECHANGE_STRUCT *)m_Data;

		Data.CamID = (int)pData->CamID;
		Data.newMode = (int)pData->newMode;
		Data.oldMode = (int)pData->oldMode;
		Data.modeStatus = (int)pData->modeStatus;
		Data.modeActivity = (int)pData->modeActivity;
	}

	// Return without error.
	return rval;
}

///		Sets ADC input data into EHK system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		data		  ADC data.
///   \param[in]    tempID    ID (address) of mux temperature in this data.
///   \param[in]    inampID   ID (address) of inamp value in this data.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::setEhkSysMuxData( short tempVal, short inampVal, int tempID,
  int inampID )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_EHKSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_EHKSYSDATA_STRUCT );

		PKT_EHKSYSDATA_STRUCT *pData = (PKT_EHKSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data based on the gyro.
    if ( tempID >= 0 && tempID < 16 )
      pData->muxTemp = tempVal;
//      pData->muxTemp[tempID] = tempVal;

    if ( inampID >= 0 && inampID < 4 )
      pData->inampTemp = inampVal;
//      pData->inampTemp[inampID] = inampVal;
	}

	// Return without error.
	return rval;
}

///		Sets ADC input data into EHK system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		data		  ADC data.
///   \param[in]    tempID    ID (address) of mux temperature in this data.
///   \param[in]    inampID   ID (address) of inamp value in this data.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::setEhkSysAdcData( ShortBuffer &data, int tempID,
  int inampID )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_EHKSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_EHKSYSDATA_STRUCT );

		PKT_EHKSYSDATA_STRUCT *pData = (PKT_EHKSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data based on the gyro.
    pData->balMotorTemp = data[0];
    pData->balBearingTemp = data[1];
    pData->obRearTopTemp = data[2];
    pData->obRearPortTemp = data[3];
    pData->spare[0] = data[4];
    pData->spare[1] = data[5];
    pData->spare[2] = data[6];
    pData->spare[3] = data[7];
    pData->inampTemp = data[8];
    pData->spare[4] = data[9];
    pData->muxTemp = data[10];
    pData->spare[5] = data[11];
    pData->tankTemp = data[12];
    pData->tankPress = data[13];

    pData->tempID = tempID;
    pData->inampID = inampID;
	}

	// Return without error.
	return rval;
}

///		Sets DIO input data into EHK system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		data		  DIO data bits.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::setEhkSysDioData( uint8_t dioBits, bool motorMoving )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_EHKSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_EHKSYSDATA_STRUCT );

		PKT_EHKSYSDATA_STRUCT *pData = (PKT_EHKSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->dioInput = (dioBits & 0x0f);
    pData->dioInput |= motorMoving ? 0x10 : 0;
	}

	// Return without error.
	return rval;
}

///		Sets relay output state data into EHK system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		data		  Relay state bits.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::setEhkSysRelayData( uint16_t relayBits )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_EHKSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_EHKSYSDATA_STRUCT );

		PKT_EHKSYSDATA_STRUCT *pData = (PKT_EHKSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->relayImage = relayBits;
	}

	// Return without error.
	return rval;
}

///		Sets motor position data into EHK system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		data		  Relay state bits.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::setEhkSysMotorData( int motorPos )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_EHKSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_EHKSYSDATA_STRUCT );

		PKT_EHKSYSDATA_STRUCT *pData = (PKT_EHKSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

    pData->motorPos = (int16_t)motorPos;
	}

	// Return without error.
	return rval;
}

//	GetEhkSysData:
///		Gets converted user data from the EHK system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		Data			Structure holding converted EHK parameter data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetEhkSysData( USR_EHKSYSDATA_STRUCT &Data )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_EHKSYSDATA, sizeof( PKT_EHKSYSDATA_STRUCT ) )) )
	{
		// Get a pointer to the data.
		PKT_EHKSYSDATA_STRUCT *pData = (PKT_EHKSYSDATA_STRUCT *)m_Data;

    // Convert the values.
    Data.balMotorTemp = 
      (double)EhkAdcConverter::cnvLM61Tempr(pData->balMotorTemp );
    Data.balBearingTemp = 
      (double)EhkAdcConverter::cnvLM61Tempr(pData->balBearingTemp );
    Data.obRearTopTemp = 
      (double)EhkAdcConverter::cnvLM61Tempr(pData->obRearTopTemp );
    Data.obRearPortTemp = 
      (double)EhkAdcConverter::cnvLM61Tempr(pData->obRearPortTemp );
    Data.tankTemp = 
      (double)EhkAdcConverter::cnvLM50Tempr(pData->tankTemp );
    Data.tankPress = 
      (double)EhkAdcConverter::cnvPressure( pData->tankPress );

    Data.muxTemp = (double)EhkAdcConverter::cnvMuxTemp( pData->tempID,
      pData->muxTemp );
    
    Data.inampTemp = (double)EhkAdcConverter::cnvInampTemp(pData->inampTemp);

    for ( int i = 0; i < 6; i++ )
      Data.spare[i] = (double)EhkAdcConverter::cnvVolts(pData->spare[i]);

    Data.tempID = (int)pData->tempID;
    Data.inampID = (int)pData->inampID;
    Data.dioInput = (int)pData->dioInput;
    Data.relayImage = (int)pData->relayImage;
    Data.motorPosition = (int)pData->motorPos;
    Data.motorMoving = (pData->dioInput & 0x10) > 0;
	}

	// Return without error.
	return rval;
}

/// Set the EHK balance motor paramter data into the packet.
int HeroGsePacket::setEhkBalMotorParamData( int stepRate, int minPos,
    int maxPos, bool homeReset, int homePos, bool midReset, int midPos,
    bool endReset, int endPos )
{
  int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_EHKPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_EHKHTRPARAMDATA_STRUCT ) + 
      sizeof( PKT_EHKBALMOTORPARAMDATA_STRUCT );

    // Get a pointer to the data.
		PKT_EHKBALMOTORPARAMDATA_STRUCT *pData = 
      (PKT_EHKBALMOTORPARAMDATA_STRUCT *)&m_Data[sizeof( PKT_EHKHTRPARAMDATA_STRUCT )];

    // Get the values.
    pData->stepRateHz = (int16_t)stepRate;
    pData->minPos = (int16_t)minPos;
    pData->maxPos = (int16_t)maxPos;
    pData->homePos = (int16_t)homePos;
    pData->midPos = (int16_t)midPos;
    pData->endPos = (int16_t)endPos;

    // Set the reset bits.
    pData->resetBits = homeReset ? 0x01 : 0;
    pData->resetBits |= midReset ? 0x02 : 0;
    pData->resetBits |= endReset ? 0x04 : 0;
  }

  return rval;
}

/// Set the EHK heater contorl parameter data into the packet.
int HeroGsePacket::setEhkHeaterParamData( int heater, int ctlState, float lThresh,
  float uThresh )
{
  int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_EHKPARAM )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_EHKHTRPARAMDATA_STRUCT ) + 
      sizeof( PKT_EHKBALMOTORPARAMDATA_STRUCT );

    // Get a pointer to the data.
		PKT_EHKHTRPARAMDATA_STRUCT *pData = (PKT_EHKHTRPARAMDATA_STRUCT *)m_Data;

    // Get the values.
    if ( heater >= 0 && heater < 8 )
    {
      pData->tCtlLThresh[heater] = (int16_t)(lThresh * 100.f);
      pData->tCtlUThresh[heater] = (int16_t)(uThresh * 100.f);
      pData->tCtlState[heater] = (int8_t)ctlState;
    }
  }

  return rval;
}

//	GetEhkSysData:
///		Gets converted user data from the EHK system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		Data			Structure holding converted EHK parameter data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::getEhkParamData( USR_EHKHTRPARAMDATA_STRUCT &htrData,
    USR_EHKBALMOTORPARAMDATA_STRUCT &motorData )
{
  int rval;
  if ( !(rval = VerifyPacketForGet( GSEPTYPE_EHKPARAM,
    sizeof( PKT_EHKHTRPARAMDATA_STRUCT ) +
    sizeof( PKT_EHKBALMOTORPARAMDATA_STRUCT) )) )
  {
    // Get a pointer to the data.
    PKT_EHKHTRPARAMDATA_STRUCT *pHtrData = (PKT_EHKHTRPARAMDATA_STRUCT*)m_Data;
		PKT_EHKBALMOTORPARAMDATA_STRUCT *pMoData = 
      (PKT_EHKBALMOTORPARAMDATA_STRUCT *)&m_Data[sizeof( PKT_EHKHTRPARAMDATA_STRUCT )];

    // Get the values.
    for ( int i = 0; i < 8; i++ )
    {
      htrData.tCtlLThresh[i] = (float)pHtrData->tCtlLThresh[i] * 0.01f;
      htrData.tCtlUThresh[i] = (float)pHtrData->tCtlUThresh[i] * 0.01f;
      htrData.tCtlState[i] = (int)pHtrData->tCtlState[i];
    }

    motorData.stepRateHz = (int)pMoData->stepRateHz;
    motorData.minPos = (int)pMoData->minPos;
    motorData.maxPos = (int)pMoData->maxPos;
    motorData.homePos = (int)pMoData->homePos;
    motorData.midPos = (int)pMoData->midPos;
    motorData.endPos = (int)pMoData->endPos;

    motorData.homeReset = (pMoData->resetBits & 0x01) > 0;
    motorData.midReset = (pMoData->resetBits & 0x02) > 0;
    motorData.endReset = (pMoData->resetBits & 0x04) > 0;
  }

  return rval;
}

//	SetFdrSysRawStatData:
///		Sets the raw event statistics data into the FDR system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		RawTxEvents		Total number of raw events queued for transmission to ground.
///		\param[in]		Det0RxEvents	Number of raw events received from detector 0 during time period.
///		\param[in]		Det1RxEvents	Number of raw events received from detector 1 during time period.
///		\param[in]		Det2RxEvents	Number of raw events received from detector 2 during time period.
///		\param[in]		Det3RxEvents	Number of raw events received from detector 3 during time period.
///		\param[in]		Det4RxEvents	Number of raw events received from detector 4 during time period.
///		\param[in]		Det5RxEvents	Number of raw events received from detector 5 during time period.
///		\param[in]		Det6RxEvents	Number of raw events received from detector 6 during time period.
///		\param[in]		Det7RxEvents	Number of raw events received from detector 7 during time period.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetFdrSysRawStatData( uint32_t RawTxEvents, uint32_t MaxFileSize, uint8_t RawTxDet,
		uint32_t Det0RxEvents, uint32_t Det1RxEvents, uint32_t Det2RxEvents, uint32_t Det3RxEvents,
		uint32_t Det4RxEvents, uint32_t Det5RxEvents, uint32_t Det6RxEvents, uint32_t Det7RxEvents )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_FDRSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_FDRSYSDATA_STRUCT );

		PKT_FDRSYSDATA_STRUCT *pData = (PKT_FDRSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the reset of the statistics.
		pData->RawTxRate = (uint16_t)RawTxEvents;
		pData->RawRxRate[0] = (uint16_t)Det0RxEvents;
		pData->RawRxRate[1] = (uint16_t)Det1RxEvents;
		pData->RawRxRate[2] = (uint16_t)Det2RxEvents;
		pData->RawRxRate[3] = (uint16_t)Det3RxEvents;
		pData->RawRxRate[4] = (uint16_t)Det4RxEvents;
		pData->RawRxRate[5] = (uint16_t)Det5RxEvents;
		pData->RawRxRate[6] = (uint16_t)Det6RxEvents;
		pData->RawRxRate[7] = (uint16_t)Det7RxEvents;

		pData->MaxFileSize = MaxFileSize;
		pData->RawTxDet = RawTxDet;
	}

	// Return without error.
	return rval;
}


//	SetFdrSysMiscStatData:
///		Sets some miscellaneous statistics data into the FDR system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		TotalRxBytes	Total number of bytes received from all sources.
///		\param[in]		TotalTxBytes	Total number of bytes transmitted from all sources.
///		\param[in]		KbFreeC			Number of KB free on the FDR C Drive.
///		\param[in]		KbFreeD			Number of KB free on the FDR D Drive.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetFdrSysMiscStatData( uint32_t TotalRxBytes, uint32_t TotalTxBytes,
		uint32_t KbFreeP, uint32_t KbFreeS )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_FDRSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_FDRSYSDATA_STRUCT );

		PKT_FDRSYSDATA_STRUCT *pData = (PKT_FDRSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->TotalRxRate = (uint16_t)TotalRxBytes;
		pData->TotalTxRate = (uint16_t)TotalTxBytes;
		pData->KbFreePrimary = KbFreeP;
		pData->KbFreeSecondary = KbFreeS;
	}

	// Return without error.
	return rval;
}

//	SetFdrSysAdcData:
///		Sets the raw ADC values into the FDR system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		TankTemp		Raw ADC reading of FDR tank temperature.
///		\param[in]		TankPress		Raw ADC reading of FDR tank pressure.
///		\param[in]		Spare1			Raw ADC reading of spare voltage.
///		\param[in]		Spare2			Raw ADC reading of spare voltage.
///		\param[in]		BatteryV		Raw ADC reading of FDR battery voltage.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetFdrSysAdcData( uint16_t TankTemp, uint16_t TankPress, uint16_t DcDcTemp, uint16_t Spare1,
									  uint16_t BatteryV, uint16_t dioState )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_FDRSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_FDRSYSDATA_STRUCT );

		PKT_FDRSYSDATA_STRUCT *pData = (PKT_FDRSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->FdrTemp = TankTemp;
		pData->FdrPress = TankPress;
		pData->DcDcTemp = DcDcTemp;
		pData->SpareV1 = Spare1;
		pData->BatteryV = BatteryV;
		pData->DioState = dioState;
	}

	// Return without error.
	return rval;
}

//	SetFdrSysGseData:
///		Sets the GSE stream statistics data into the FDR system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		RxBytes		Bytes received on GSE input stream.
///		\param[in]		RxPkts		Packets received on GSE input stream.
///		\param[in]		RxErrors	Input errors on GSE input stream.
///		\param[in]		RxLastError	ID of last input error on GSE input stream.
///		\param[in]		TxBytes		Bytes sent on GSE output stream.
///		\param[in]		TxPkts		Packets sent on GSE output stream.
///		\param[in]		TxErrors	Input errors on GSE output stream.
///		\param[in]		TxLastError	ID of last output error on GSE output stream.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetFdrSysGseData( uint32_t RxBytes, uint16_t RxPkts, uint16_t RxErrors, uint16_t RxLastError,
		uint32_t TxBytes, uint16_t TxPkts, uint16_t TxErrors, uint16_t TxLastError )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_FDRSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_FDRSYSDATA_STRUCT );

		PKT_FDRSYSDATA_STRUCT *pData = (PKT_FDRSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->GseRxBytes = RxBytes;
		pData->GseRxPkts = RxPkts;
		pData->GseRxErrors = RxErrors;
		pData->GseRxLastError = RxLastError;
		pData->GseTxBytes = TxBytes;
		pData->GseTxPkts = TxPkts;
		pData->GseTxErrors = TxErrors;
		pData->GseTxLastError = TxLastError;
	}

	// Return without error.
	return rval;
}

//	SetFdrSysImRxData:
///		Sets the raw image RX stream statistics data into the FDR system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		RxBytes		Bytes received on GSE input stream.
///		\param[in]		RxPkts		Packets received on GSE input stream.
///		\param[in]		RxImPkts	Image packets received on GSE input stream.
///		\param[in]		QImPkts		Image packets written to packet queue.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetFdrSysImRxData( uint32_t RxBytes, uint16_t RxPkts, uint16_t RxImPkts, uint16_t QImPkts )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_FDRSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_FDRSYSDATA_STRUCT );

		PKT_FDRSYSDATA_STRUCT *pData = (PKT_FDRSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->ImRxBytes = RxBytes;
		pData->ImRxPkts = RxPkts;
		pData->ImRxImPkts = RxImPkts;
		pData->ImRxQPkts = QImPkts;
	}

	// Return without error.
	return rval;
}

//	SetFdrSysImTxData:
///		Sets the raw image TX stream statistics data into the FDR system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		QImPkts		Image packets read from packet queue.
///		\param[in]		TxPkts		Packets transmitted on GSE input stream.
///		\param[in]		TxBytes		Bytes transmitted on GSE input stream.
///		\param[in]		TxErrors	Input errors on GSE output stream.
///		\param[in]		TxLastError	ID of last output error on GSE output stream.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetFdrSysImTxData( uint16_t QImPkts, uint16_t TxPkts, uint32_t TxBytes, uint16_t TxErrors,
									   uint16_t TxLastError )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_FDRSYSDATA )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_FDRSYSDATA_STRUCT );

		PKT_FDRSYSDATA_STRUCT *pData = (PKT_FDRSYSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->ImTxQPkts = QImPkts;
		pData->ImTxPkts = TxPkts;
		pData->ImTxBytes = TxBytes;
		pData->ImTxErrors = TxErrors;
		pData->ImTxLastError = TxLastError;
	}

	// Return without error.
	return rval;
}

//	GetFdrSysData:
///		Gets the converted data from the FDR system data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		Data			Structure holding converted system data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetFdrSysData( USR_FDRSYSDATA_STRUCT &Data )

{
	int i, rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_FDRSYSDATA, sizeof( PKT_FDRSYSDATA_STRUCT ) )) )
	{
		// Get a pointer to the data.
		PKT_FDRSYSDATA_STRUCT *pData = (PKT_FDRSYSDATA_STRUCT *)m_Data;

		// Loop over all detectors.
		for ( i = 0; i < NUM_HERO_DETECTORS; i++ )
		{
			// Get the detector raw input rate.
			Data.RawRxRate[i] = pData->RawRxRate[i];
		}
		Data.RawTxDet = pData->RawTxDet;
		Data.MaxFileSize = pData->MaxFileSize;

		// Get the other event rates.
		Data.RawTxRate = (uint32_t)pData->RawTxRate;
		Data.TotalTxRate = (uint32_t)pData->TotalTxRate;
		Data.TotalRxRate = (uint32_t)pData->TotalRxRate;

		// Convert the free disk space values.
		Data.KbFreePrimary = (double)pData->KbFreePrimary;
		Data.KbFreeSecondary = (double)pData->KbFreeSecondary;

		// Convert the ADC values.
		Data.FdrTemp = (float)pData->FdrTemp * 0.05f - 50.0f;
		Data.FdrPress = (float)pData->FdrPress * 0.0072516f + 1.4503f;
		Data.DcDcTemp = (float)pData->DcDcTemp * 0.05f - 50.0f;;
		Data.SpareV1 = (float)pData->SpareV1 * 0.001f;
		Data.BatteryV = (float)pData->BatteryV * 0.0044638f + 0.0134f;

		// Get the GSE statistics.
		Data.GseRxBytes = (uint32_t)pData->GseRxBytes;
		Data.GseRxPkts = (uint32_t)pData->GseRxPkts;
		Data.GseRxErrors = (uint32_t)pData->GseRxErrors;
		Data.GseRxLastError = (uint32_t)pData->GseRxLastError;
		Data.GseTxBytes = (uint32_t)pData->GseTxBytes;
		Data.GseTxPkts = (uint32_t)pData->GseTxPkts;
		Data.GseTxErrors = (uint32_t)pData->GseTxErrors;
		Data.GseTxLastError = (uint32_t)pData->GseTxLastError;

		// Get the raw image stream statistics.
		Data.ImRxBytes = (uint32_t)pData->ImRxBytes;
		Data.ImRxPkts = (uint32_t)pData->ImRxPkts;
		Data.ImRxImPkts = (uint32_t)pData->ImRxImPkts;
		Data.ImRxQPkts = (uint32_t)pData->ImRxQPkts;
		Data.ImTxQPkts = (uint32_t)pData->ImTxQPkts;
		Data.ImTxBytes = (uint32_t)pData->ImTxBytes;
		Data.ImTxPkts = (uint32_t)pData->ImTxPkts;
		Data.ImTxErrors = (uint32_t)pData->ImTxErrors;
		Data.ImTxLastError = (uint32_t)pData->ImTxLastError;

		// Get the stow pin status states.
		Data.DioState = pData->DioState;
		Data.StowArm = (pData->DioState & 0x02) ? true : false;
		Data.StowMove = (pData->DioState & 0x01) ? true : false;
		Data.StowOpen = (pData->DioState & 0x04) ? false : true;
		Data.ObaAtStow = (pData->DioState & 0x08) ? true : false;


	}

	// Return without error.
	return rval;
}

////	SetFdrParamData:
/////		Sets the raw event statistics data into the FDR system data packet.
////
/////		This function first checks that the packet exists, has been created correctly, and is the
/////		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
/////		\param[in]		ProcFlags		Bitwise status of event processing for each detector (1=processing).
/////		\param[in]		FwdRaw			Flag indicating if raw events are sent to ground.
/////		\param[in]		RawDet			Indicates which detector raw events would be sent from.
/////		\param[in]		EventsPerFile	Number of raw events allowed per FDR file.
/////		\param[in]		ParamDet		Detector ID to which remaining parameters pertain.
/////		\return			Returns 0 if packet exists, has been created with correct type.
/////						Non-zero error code otherwise.
//int HeroGsePacket::SetFdrParamData( uint8_t ProcFlags, bool FwdRaw, uint8_t RawDet, uint16_t EventsPerFile,
//		uint8_t ParamDet, uint16_t ChanStep, uint16_t ChanThresh, float XOff, float YOff, float PCoeff0,
//		float PCoeff1, float PCoeff2, float ECoeff0, float ECoeff1, float ECoeff2 )
//{
//	int rval;
//	if ( !(rval = VerifyPacketForSet( GSEPTYPE_FDRPARAM )) )
//	{
//		// Set the data length.
//		m_Header->DataLength = sizeof( PKT_FDRPARAMDATA_STRUCT );
//
//		PKT_FDRPARAMDATA_STRUCT *pData = (PKT_FDRPARAMDATA_STRUCT *)m_Data;
//		m_Altered = true;
//
//		// Set the processing flags.
//		pData->ProcFlags = ProcFlags;
//
//		// Set the forwarded detector.
//		pData->RawTxDet = RawDet & 0x7f;
//		if ( FwdRaw )
//			pData->RawTxDet |= 0x80;
//
//		// Set the events per file.
//		pData->RawNPerFile = EventsPerFile;
//
//		// Set the detector-specific parameters.
//		pData->ParamDet = ParamDet;
//		pData->ChanStep = (uint8_t)ChanStep;
//		pData->ChanThresh = ChanThresh;
//		pData->XOffset = XOff;
//		pData->YOffset = YOff;
//		pData->PCoeff[0] = PCoeff0;
//		pData->PCoeff[1] = PCoeff1;
//		pData->PCoeff[2] = PCoeff2;
//		pData->ECoeff[0] = ECoeff0;
//		pData->ECoeff[1] = ECoeff1;
//		pData->ECoeff[2] = ECoeff2;
//	}
//
//	// Return without error.
//	return rval;
//}

////	GetFdrParamData:
/////		Gets the converted data from the FDR parameter data packet.
////
/////		This function first checks that the packet exists, has been created correctly, and
/////		is the correct type.  If these checks succeed, it then reads the data from the packet.
/////		\param[out]		Data			Structure holding converted parameter data.
/////		\return			Returns 0 if packet exists, and has been created with the correct type.
/////						Non-zero error code otherwise.
//int HeroGsePacket::GetFdrParamData( USR_FDRPARAMDATA_STRUCT &Data )
//
//{
//	int i, rval;
//	if ( !(rval = VerifyPacketForGet( GSEPTYPE_FDRPARAM, sizeof( PKT_FDRPARAMDATA_STRUCT ) )) )
//	{
//		// Get a pointer to the data.
//		PKT_FDRPARAMDATA_STRUCT *pData = (PKT_FDRPARAMDATA_STRUCT *)m_Data;
//
//		// Loop over all detectors.
//		for ( i = 0; i < NUM_HERO_DETECTORS; i++ )
//		{
//			// Get the processing flag.
//			Data.ProcFlag[i] = ((pData->ProcFlags >> i) & 1) > 0;
//		}
//
//		// Get the detector from which raw data is forwarded, and raw forwarding flag.
//		Data.RawTxFlag = (pData->RawTxDet & 0x80) > 0;
//		Data.RawTxDet = (pData->RawTxDet & 0x7f);
//
//		// Get the other event rates.
//		Data.RawNPerFile = (int)pData->RawNPerFile;
//	}
//
//	// Return without error.
//	return rval;
//}

//	SetDpsStatusDetRateData:
///		Sets the rate data for a single detector into a DPS status data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Det				Index (0 or 1) to which detector data is being set for.
///		\param[in]		TriggerRate		Trigger rate read from FPGA (counts/sec).
///		\param[in]		UThreshRate		Upper level threshold trigger rate read from FPGA (counts/sec).
///		\param[in]		RawReadRate		Raw events read from FPGA FIFO (counts/sec).
///		\param[in]		RawTxRate		Raw events sent to FDR via network (counts/sec).
///		\param[in]		SatRate			Events with a saturated ADC channel (counts/sec).
///		\param[in]		ValidRate		Events processed as valid (counts/sec).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetDpsStatusDetRateData( int detID, uint16_t isrCount,
    uint16_t readCount, uint16_t writeCount, uint32_t ppsCount,
    uint16_t triggerCount, uint16_t swTriggerCount, uint16_t calTriggerCount,
    uint16_t ultVetoCount, uint16_t calVetoCount, uint16_t hdrErrCount )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_DPSSTATUS )) && (detID > 0 || detID < 8) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_DPSSTATUSDATA_STRUCT );

		PKT_DPSSTATUSDATA_STRUCT *pData = (PKT_DPSSTATUSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the rates.
    int idet = detID & 1;
		pData->status[idet].isrCount = isrCount;
		pData->status[idet].readCount = readCount;
		pData->status[idet].writeCount = writeCount;
		pData->status[idet].ppsCount = ppsCount;
		pData->status[idet].triggerCount = triggerCount;
		pData->status[idet].swTriggerCount = swTriggerCount;
		pData->status[idet].calTriggerCount = calTriggerCount;
		pData->status[idet].ultVetoCount = ultVetoCount;
		pData->status[idet].calVetoCount = calVetoCount;
		pData->status[idet].hdrErrCount = hdrErrCount;
	}

	// Return without error.
	return rval;
}

//	SetDpsStatusDetVData:
///		Sets the voltage readback data from a single detector into a DPS status data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Det				Index (0 or 1) to which detector data is being set for.
///		\param[in]		PmtV			PMT readback voltage (volts).
///		\param[in]		DriftV			Drift readback voltage (volts).
///		\param[in]		GridV			Grid readback voltage (volts).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetDpsStatusDetVData( int detID, uint16_t pmtV,
    uint16_t driftV, uint16_t gridV )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_DPSSTATUS )) && (detID > 0 || detID < 8) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_DPSSTATUSDATA_STRUCT );

		PKT_DPSSTATUSDATA_STRUCT *pData = (PKT_DPSSTATUSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the rates.
    int idet = detID & 1;
		pData->status[idet].pmtVReadback = pmtV;
		pData->status[idet].driftVReadback = driftV;
		pData->status[idet].gridVReadback = gridV;
	}

	// Return without error.
	return rval;
}

//	SetDpsEventQRateData:
///		Sets the event queue rate data into a DPS status data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		readCount0    Number of Det0 events read from the event queue.
///   \param[in]    writeCount0   Number of Det0 events sent to FDR.
///		\param[in]		readCount1    Number of Det1 events read from the event queue.
///   \param[in]    writeCount1   Number of Det1 events sent to FDR.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetDpsEventQRateData( uint16_t readCount0,
  uint16_t writeCount0, uint16_t readCount1, uint16_t writeCount1 )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_DPSSTATUS )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_DPSSTATUSDATA_STRUCT );

		PKT_DPSSTATUSDATA_STRUCT *pData = (PKT_DPSSTATUSDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the rates.
		pData->qStatus[0].readCount = readCount0;
    pData->qStatus[0].writeCount = writeCount0;
		pData->qStatus[1].readCount = readCount1;
    pData->qStatus[1].writeCount = writeCount1;
	}

	// Return without error.
	return rval;
}

//	GetDpsStatusData:
///		Gets the converted data from the DPS status data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		Data0			Structure holding converted status data for detector 0.
///		\param[out]		Data1			Structure holding converted status data for detector 1.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetDpsStatusData( PKT_DETSTATUS_STRUCT &Data0, PKT_DETSTATUS_STRUCT &Data1 )

{
	int i, rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_DPSSTATUS, sizeof( PKT_DPSSTATUSDATA_STRUCT ) )) )
	{
		// Get a pointer to the data.
		PKT_DPSSTATUSDATA_STRUCT *pData = (PKT_DPSSTATUSDATA_STRUCT *)m_Data;
		PKT_DETSTATUS_STRUCT *status[2] = {&Data0, &Data1};

		// Loop over both detectors.
		for ( i = 0; i < 2; i++ )
		{
		  // Get the count data.
		  status[i]->isrCount = pData->status[i].isrCount;
		  status[i]->readCount = pData->status[i].readCount;
		  status[i]->writeCount = pData->status[i].writeCount;
		  status[i]->ppsCount = pData->status[i].ppsCount;
		  status[i]->triggerCount = pData->status[i].triggerCount;
		  status[i]->swTriggerCount = pData->status[i].swTriggerCount;
		  status[i]->calTriggerCount = pData->status[i].calTriggerCount;
		  status[i]->ultVetoCount = pData->status[i].ultVetoCount;
		  status[i]->calVetoCount = pData->status[i].calVetoCount;
		  status[i]->hdrErrCount = pData->status[i].hdrErrCount;

			// Get the detector voltage readback data.
			status[i]->pmtVReadback = 
        (short)((double)pData->status[i].pmtVReadback * 10000.0/65535.0);
			status[i]->driftVReadback = 
        (short)((double)pData->status[i].driftVReadback * 10000.0/65535.0);
			status[i]->gridVReadback = 
        (short)((double)pData->status[i].gridVReadback * 20000.0/65535.0);
		}

	}

	// Return without error.
	return rval;
}

//	GetDpsQStatusData:
///		Gets the converted event queue data from the DPS status data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		data0			Structure holding converted status data for detector 0.
///		\param[out]		data1			Structure holding converted status data for detector 1.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetDpsQStatusData( PKT_QSTATUS_STRUCT &data0,
  PKT_QSTATUS_STRUCT &data1 )

{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_DPSSTATUS, sizeof( PKT_DPSSTATUSDATA_STRUCT ) )) )
	{
		// Get a pointer to the data.
		PKT_DPSSTATUSDATA_STRUCT *pData = (PKT_DPSSTATUSDATA_STRUCT *)m_Data;

    // Copy the data from the packet to the return values.
    memcpy( &data0, &pData->qStatus[0], sizeof( PKT_QSTATUS_STRUCT ) );
    memcpy( &data1, &pData->qStatus[1], sizeof( PKT_QSTATUS_STRUCT ) );
	}

	// Return without error.
	return rval;
}

//	SetDpsParamVSetData:
///		Sets a single detector's voltage setting data into a DPS parameter packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		detID				Index (0 or 1) to which detector data is being set for.
///		\param[in]		lThreshMv		Trigger threshold set (mV).
///		\param[in]		uThreshMv		Upper level threshold set (mV).
///		\param[in]		pmtV				Pmt voltage set (Volts).
///		\param[in]		driftV			Drift voltage set (Volts).
///		\param[in]		gridV       Grid voltage set (Volts).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetDpsParamVSetData( int detID, uint16_t lThreshMv,
    uint16_t uThreshMv, uint16_t pmtV, uint16_t driftV, uint16_t gridV )
{
	int rval;
	if ( ( !(rval = VerifyPacketForSet( GSEPTYPE_DPSCURSET )) ||
		!(rval = VerifyPacketForSet( GSEPTYPE_DPSDEFSET )) )
		&& (detID > 0 || detID < 8) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_DETPARAM_STRUCT ) * 2;
		PKT_DETPARAM_STRUCT *Data = (PKT_DETPARAM_STRUCT *)m_Data;

		m_Altered = true;

		// Copy the data.
    int idet = detID & 1;
		Data[idet].lThreshMv = lThreshMv;
		Data[idet].uThreshMv = uThreshMv;
		Data[idet].pmtV = pmtV;
		Data[idet].driftV = driftV;
		Data[idet].gridV = gridV;
	}

	// Return without error.
	return rval;
}

//	SetDpsParamFpgaData:
///		Sets a single detector's FPGA setting data into a DPS parameter packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		detID				Index (0 or 1) to which detector data is being set for.
///   \param[in]    eventCtl    Readback of FPGA event control register.
///   \param[in]    fifoIntThresh Readback of FPGA FIFO interrupt threshold register.
///   \param[in]    trigHoldoff Readback of FPGA trigger holdoff register.
///   \param[in]    fifoLowThresh Readback of FPGA FIFO lower threshold register.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetDpsParamFpgaData( int detID, uint16_t eventCtl,
    uint16_t fifoIntThresh, uint16_t trigHoldoff, uint16_t fifoLowThresh,
    uint16_t verMoDay, uint16_t verYrVer )
{
	int rval;
	if ( ( !(rval = VerifyPacketForSet( GSEPTYPE_DPSCURSET )) ||
		!(rval = VerifyPacketForSet( GSEPTYPE_DPSDEFSET )) )
		&& (detID > 0 || detID < 8) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_DETPARAM_STRUCT ) * 2;
		PKT_DETPARAM_STRUCT *Data = (PKT_DETPARAM_STRUCT *)m_Data;

		m_Altered = true;

		// Copy the data.
    int idet = detID & 1;
		Data[idet].eventCtl = eventCtl;
		Data[idet].fifoEventThresh = fifoIntThresh;
		Data[idet].trigHoldoff = trigHoldoff;
		Data[idet].fifoLowThresh = fifoLowThresh;
    Data[idet].fpgaVMonthDay = verMoDay;
    Data[idet].fpgaVYearVer = verYrVer;
	}

	// Return without error.
	return rval;
}

int HeroGsePacket::SetDpsParamSwTrigData( bool swTrigEnable0,
  bool swTrigEnable1 )
{
	int rval;
	if ( ( !(rval = VerifyPacketForSet( GSEPTYPE_DPSCURSET )) ||
		!(rval = VerifyPacketForSet( GSEPTYPE_DPSDEFSET )) ) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_DETPARAM_STRUCT ) * 2;
		PKT_DETPARAM_STRUCT *Data = (PKT_DETPARAM_STRUCT *)m_Data;

		m_Altered = true;

    // Set the data.
    Data[0].flags = 0;
    if ( swTrigEnable0 )
      Data[0].flags = 1;
    Data[1].flags = 0;
    if ( swTrigEnable1 )
      Data[1].flags = 1;
  }

  return rval;
}

//	GetDpsParamData:
///		Gets the converted parameter data from a validated DPS parameter packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		pRate		Number of bearing case bolts detected in period.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetDpsParamData( USR_DETPARAM_STRUCT &Data0, USR_DETPARAM_STRUCT &Data1 )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_DPSCURSET, sizeof( PKT_DETPARAM_STRUCT ) * 2 )) ||
		!(rval = VerifyPacketForGet( GSEPTYPE_DPSDEFSET, sizeof( PKT_DETPARAM_STRUCT ) * 2 )))
	{
		// Get a pointer to the data.
		PKT_DETPARAM_STRUCT *Data = (PKT_DETPARAM_STRUCT *)m_Data;
		USR_DETPARAM_STRUCT *pData[2] = { &Data0, &Data1 };

		// Get the converted data for each detector.
		for ( int i = 0; i < 2; i++ )
		{
			pData[i]->lThreshV = (float)Data[i].lThreshMv * 0.001f;
			pData[i]->uThreshV = (float)Data[i].uThreshMv * 0.001f;
			pData[i]->pmtV = Data[i].pmtV;
			pData[i]->driftV = Data[i].driftV;
			pData[i]->gridV = Data[i].gridV;
      pData[i]->eventEnable = (Data[i].eventCtl & 0x8000) > 0;
      pData[i]->vetoEnable = (Data[i].eventCtl & 0x2000) > 0;
      pData[i]->intEnable = (Data[i].eventCtl & 0x1000) > 0;
      pData[i]->calEnable = (Data[i].eventCtl & 0x0800) > 0;
      pData[i]->swTrigEnable = (Data[i].eventCtl & 0x0001) > 0;
      pData[i]->fifoEventThresh = Data[i].fifoEventThresh;
      pData[i]->trigHoldoff = Data[i].trigHoldoff;
      pData[i]->fifoLowThresh = Data[i].fifoLowThresh;
      pData[i]->fpgaVYear = ((Data[i].fpgaVYearVer >> 8) / 16) * 10 +
        (Data[i].fpgaVYearVer >> 8) % 16 + 2000;
      pData[i]->fpgaVNum = ((Data[i].fpgaVYearVer & 0x00ff) / 16) * 10 +
        Data[i].fpgaVYearVer % 16;
      pData[i]->fpgaVMonth = ((Data[i].fpgaVMonthDay >> 8) / 16) * 10 +
        (Data[i].fpgaVMonthDay >> 8) % 16;
      pData[i]->fpgaVDay = ((Data[i].fpgaVMonthDay & 0x00ff) / 16) * 10 +
        Data[i].fpgaVMonthDay % 16;
		}
	}

	// Return without error.
	return rval;
}

//	SetDpsEventData:
///		Sets the processed event data into a DPS event data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Data			Structure holding the event data.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetDpsEventData( DET_EVENT_DATA &Data )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_DPSEVENT )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( DET_EVENT_DATA );

		m_Altered = true;

		// Copy the data.
		memcpy( m_Data, &Data, sizeof( DET_EVENT_DATA ) );
	}

	// Return without error.
	return rval;
}

//	GetDpsEventData:
///		Gets the processed event data from a validated packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		pRate		Number of bearing case bolts detected in period.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetDpsEventData( DET_EVENT_DATA &Data )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_DPSEVENT, sizeof( DET_EVENT_DATA ) )) )
	{
		// Get the data.
		memcpy( &Data, m_Data, sizeof( DET_EVENT_DATA ) );
	}

	// Return without error.
	return rval;
}

//	SetDpsTempPresData:
///		Sets the temperature and pressure data into a DPS temperature and pressure data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		DpsTemp			DPS tank temperature (raw ADC counts).
///		\param[in]		DpsPress		DPS tank pressure (raw ADC counts).
///		\param[in]		DpsRegTemp		DPS +5V regulator temperature (raw ADC counts).
///		\param[in]		DetTemp0		Detector 0 tank temperature (raw ADC counts).
///		\param[in]		DetPress0		Detector 0 tank pressure (raw ADC counts).
///		\param[in]		DetAuxTemp0		Detector 0 auxilliary temperature (raw ADC counts).
///		\param[in]		DetTemp1		Detector 1 tank temperature (raw ADC counts).
///		\param[in]		DetPress1		Detector 1 tank pressure (raw ADC counts).
///		\param[in]		DetAuxTemp1		Detector 1 auxilliary temperature (raw ADC counts).
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetDpsTempPresData( uint16_t DpsTemp, uint16_t DpsPress,
    uint16_t DpsRegTemp, uint16_t DpsSpareV, uint16_t DetTemp0,
    uint16_t DetPress0, uint16_t DetAuxTemp0, uint16_t DetTemp1,
    uint16_t DetPress1, uint16_t DetAuxTemp1 )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_DPSTEMPRS )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_DPSTPDATA_STRUCT );

		PKT_DPSTPDATA_STRUCT *pData = (PKT_DPSTPDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the DPS data.
		pData->dpsTemp = DpsTemp;
		pData->dpsPressure = DpsPress;
		pData->dpsRegTemp = DpsRegTemp;
		pData->dpsSpareV = DpsSpareV;

		// Set the detector 0 data.
		pData->detTemp0 = DetTemp0;
		pData->detPressure0 = DetPress0;
		pData->detAuxTemp0 = DetAuxTemp0;

		// Set the detector 1 data.
		pData->detTemp1 = DetTemp1;
		pData->detPressure1 = DetPress1;
		pData->detAuxTemp1 = DetAuxTemp1;
	}

	// Return without error.
	return rval;
}

//	GetDpsTempPresData:
///		Gets the converted temperature and pressure data from a validated DPS packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		Data		Structure to hold converted data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetDpsTempPresData( USR_DPSTPDATA_STRUCT &Data )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_DPSTEMPRS, sizeof( PKT_DPSTPDATA_STRUCT ) )) )
	{
		// Get a pointer to the packet data.
		PKT_DPSTPDATA_STRUCT *pData = (PKT_DPSTPDATA_STRUCT *)m_Data;

		// The conversion factors.
		double TSlope = 0.007629511, TOffset = -50.0;
		double PSlope = 0.07629242, POffset = 99.9965;

		// Convert the DPS data.
		Data.dpsTemp = TSlope * (double)pData->dpsTemp + TOffset;
		Data.dpsPressure = PSlope * (double)pData->dpsPressure + POffset;
		Data.dpsRegTemp = TSlope * (double)pData->dpsRegTemp + TOffset;
		Data.dpsSpareV = (double)pData->dpsSpareV * 0.001;

		// Convert the detector 0 data.
		Data.detTemp0 = (double)pData->detTemp0 * TSlope + TOffset;
		Data.detPressure0 = PSlope * (double)pData->detPressure0 + POffset;
		Data.detAuxTemp0 = (double)pData->detAuxTemp0 * TSlope + TOffset;

		// Convert the detector 1 data.
		Data.detTemp1 = (double)pData->detTemp1 * TSlope + TOffset;
		Data.detPressure1 = PSlope * (double)pData->detPressure1 + POffset;
		Data.detAuxTemp1 = (double)pData->detAuxTemp1 * TSlope + TOffset;
	}

	// Return without error.
	return rval;
}

//	SetDpsClkSyncDetTimeData:
///		Sets the DPS event timer data into a DPSCLKSYNC data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Time0			Event clock/counter read from detector 0 FPGA.
///		\param[in]		Time1			Event clock/counter read from detector 1 FPGA.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetDpsClkSyncDetTimeData( uint32_t Time0, uint32_t Time1 )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_DPSCLKSYNC )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_DPSCLKSYNC_STRUCT );

		PKT_DPSCLKSYNC_STRUCT *pData = (PKT_DPSCLKSYNC_STRUCT*)m_Data;
		m_Altered = true;

		// Set the data.
		pData->detTime[0] = Time0;
		pData->detTime[1] = Time1;
	}

	// Return without error.
	return rval;
}

//	GetDpsClkSyncDetTimeData:
///		Gets the DPS event timer data from a DPSCLKSYNC data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[in]		pTime0			Event clock/counter read from detector 0 FPGA.
///		\param[in]		pTime1			Event clock/counter read from detector 1 FPGA.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetDpsClkSyncDetTimeData( double *pTime0, double *pTime1 )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_DPSCLKSYNC,
    sizeof( PKT_DPSCLKSYNC_STRUCT ) )) )
	{
		// Get a pointer to the data.
		PKT_DPSCLKSYNC_STRUCT *pData = (PKT_DPSCLKSYNC_STRUCT *)m_Data;

		// Get the data.
		*pTime0 = (double)pData->detTime[0] * 0.00005;
		*pTime1 = (double)pData->detTime[1] * 0.00005;
	}

	// Return without error.
	return rval;
}

//	SetClkSyncData:
///		Sets the clock synchronization data into a CLKSYNC data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		ClkDiff			Clock offset from master clock in seconds.
///		\param[in]		Thresh			Clock reset threshold in milliseconds.
///		\param[in]		Reset			Flag indicating if local clock was reset (true) or not.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetClkSyncData( double ClkDiff, uint16_t Thresh, bool Reset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CLOCKSYNC )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_CLKSYNC_STRUCT );

		PKT_CLKSYNC_STRUCT *pData = (PKT_CLKSYNC_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->TimeDiff = (int)(-ClkDiff * 1000.0);
		pData->ThreshReset = Thresh & 0x7fff;
		if ( Reset )
			pData->ThreshReset |= 0x8000;
	}

	// Return without error.
	return rval;
}

//	SetClkSyncData:
///		Sets the clock synchronization data into a CLKSYNC data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		ClkDiffMs			Clock offset from master clock in milliseconds.
///		\param[in]		Thresh			Clock reset threshold in milliseconds.
///		\param[in]		Reset			Flag indicating if local clock was reset (true) or not.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetClkSyncData( int ClkDiffMs, uint16_t Thresh, bool Reset )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CLOCKSYNC )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_CLKSYNC_STRUCT );

		PKT_CLKSYNC_STRUCT *pData = (PKT_CLKSYNC_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->TimeDiff = ClkDiffMs;
		pData->ThreshReset = Thresh & 0x7fff;
		if ( Reset )
			pData->ThreshReset |= 0x8000;
	}

	// Return without error.
	return rval;
}

//	GetClkSyncData:
///		Gets the clock synchronization data from a CLKSYNC data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		SyncData		Structure holding clock synchronization data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetClkSyncData( GSE_CLKSYNC_STRUCT &SyncData )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_CLOCKSYNC, sizeof( PKT_CLKSYNC_STRUCT) )) )
	{
		// Get a pointer to the data.
		PKT_CLKSYNC_STRUCT *pData = (PKT_CLKSYNC_STRUCT *)m_Data;

		// Get the data.
		SyncData.TimeDiff = (double)pData->TimeDiff * 0.001;
		SyncData.ResetThresh = (double)(pData->ThreshReset & 0x7fff) * 0.001;
		SyncData.Reset = (pData->ThreshReset & 0x8000) != 0;
	}

	// Return without error.
	return rval;
}

//	SetNTCSyncData:
///		Sets the NTC clock synchronization data into a data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		Data			NTC clock synchronization data.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetNTCSyncData( PKT_NTCDATA_STRUCT &data )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_NTCSYNC )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( PKT_NTCDATA_STRUCT );

		PKT_NTCDATA_STRUCT *pData = (PKT_NTCDATA_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
    memcpy( pData, &data, sizeof(PKT_NTCDATA_STRUCT) );
	}

	// Return without error.
	return rval;
}

//	GetNTCSyncData:
///		Gets the NTC clock synchronization data from a data packet.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		data		Structure holding NTC clock synchronization data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetNTCSyncData( PKT_NTCDATA_STRUCT &data )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_NTCSYNC, sizeof( PKT_NTCDATA_STRUCT) )) )
	{
		// Get a pointer to the data.
		PKT_NTCDATA_STRUCT *pData = (PKT_NTCDATA_STRUCT *)m_Data;

		// Get the data.
    memcpy( &data, pData, sizeof(PKT_NTCDATA_STRUCT) );
	}

	// Return without error.
	return rval;
}

//	SetErrorData:
///		Sets the packet reception error data if the type is correct.
//
///		The input consists of the type of error encountered, the erroneous
///		data value encountered, and the nominal value expected (if any).
int HeroGsePacket::SetErrorData( uint16_t error, uint16_t type, uint16_t DataValue, uint16_t NominalValue )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_ERROR )) )
	{
		// Set the data length.
		m_Header->DataLength = GSEPLEN_ERROR;

		// Only proceed if the packet type is correct.
		uint16_t *Data = (uint16_t *)m_Data;
		Data[0] = error;
		Data[1] = type;
		Data[2] = DataValue;
		Data[3] = NominalValue;
	}

	// Return without error.
	return rval;
}

//	GetErrorData:
///		Gets the packet reception error data if the type is correct.
//
///		The output consists of the type of error encountered, the erroneous
///		data value encountered, and the nominal value expected (if any).
int HeroGsePacket::GetErrorData( uint16_t *pError, uint16_t *pType, uint16_t *pDataValue,
								   uint16_t *pNominalValue )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_ERROR, GSEPLEN_ERROR )) )
	{
		// Get the values.
		uint16_t *Data = (uint16_t *)m_Data;
		*pError = Data[0];
		*pType = Data[1];
		*pDataValue = Data[2];
		*pNominalValue = Data[3];
	}

	// Return without error.
	return rval;
}

//	SetCmdAckSeqNum:
///		Sets the command packet sequence number into the CMDACK data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		SeqNum			Sequence number of packet containing command.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCmdAckSeqNum( uint16_t SeqNum )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CMDACK )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( GSE_CMDACK_STRUCT);

		GSE_CMDACK_STRUCT *pData = (GSE_CMDACK_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->SeqNum = SeqNum;
	}

	// Return without error.
	return rval;
}

//	SetCmdAckCmdID:
///		Sets the command ID (token) into the CMDACK data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		CmdID			Command ID token of current command.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCmdAckCmdID( uint16_t CmdID )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CMDACK )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( GSE_CMDACK_STRUCT);

		GSE_CMDACK_STRUCT *pData = (GSE_CMDACK_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->ID = CmdID;
	}

	// Return without error.
	return rval;
}

//	SetCmdAckCmdResult:
///		Sets the command result code into the CMDACK data packet.
//
///		This function first checks that the packet exists, has been created correctly, and is the
///		the correct type.  If these checks succeed, it then sets the data into the parameter packet.
///		\param[in]		CmdResult		Result code from command parsing or execution.
///		\return			Returns 0 if packet exists, has been created with correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::SetCmdAckCmdResult( uint16_t CmdResult )
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CMDACK )) )
	{
		// Set the data length.
		m_Header->DataLength = sizeof( GSE_CMDACK_STRUCT);

		GSE_CMDACK_STRUCT *pData = (GSE_CMDACK_STRUCT *)m_Data;
		m_Altered = true;

		// Set the data.
		pData->Result = CmdResult;
	}

	// Return without error.
	return rval;
}

//	GetCmdAckData:
///		Gets the command acknowledgement data if the type is correct.
//
///		This function first checks that the packet exists, has been created correctly, and
///		is the correct type.  If these checks succeed, it then reads the data from the packet.
///		\param[out]		AckData			Structure holding converted command acknowledgement data.
///		\return			Returns 0 if packet exists, and has been created with the correct type.
///						Non-zero error code otherwise.
int HeroGsePacket::GetCmdAckData( GSE_CMDACK_STRUCT &AckData )
{
	int rval;
	if ( !(rval = VerifyPacketForGet( GSEPTYPE_CMDACK, sizeof( GSE_CMDACK_STRUCT ) )) )
	{
		// Copy the structure data from the packet data.
		memcpy( &AckData, m_Data, sizeof( GSE_CMDACK_STRUCT ) );
	}

	// Return without error.
	return rval;
}

///		Handles packet processing errors.
//
///		This version resets the current packet, recreates it as a packet error packet,
///		sets the error information, and returns true.
bool HeroGsePacket::HandleError( uint16_t ErrVal1, uint16_t ErrVal2, uint16_t ErrVal3, uint16_t ErrVal4 )
{
	Reset();

	// Change packet to completed packet error packet.
	Create( GSEPTYPE_ERROR );

	// Set the error data.
	uint16_t *Data = (uint16_t *)m_Data;
	Data[0] = ErrVal1;
	Data[1] = ErrVal2;
	Data[2] = ErrVal3;
	Data[3] = ErrVal4;

	// Ready the packet for sending.
	m_Header->DataLength = 8;
	m_Header->SrcID = CPU_NUM;
	m_PacketLength = m_Header->DataLength + sizeof( EcGsePacket::Header );
	SetPacketTime();
	SetChecksum();

	// Return true.
	return true;
}

//	SetGseStatistics
///		Sets the GSE Statistics Data within the packet
//
int HeroGsePacket::SetGseStatistics(PKT_GSEGATE_DATA_STRUCT &GseData)
{
	// Verify the packet is the correct type.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_GSEGATE_DATA )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		m_Header->DataLength = GSEPLEN_GSEGATE_DATA + ::strlen(GseData.FileName);
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;

		// Get a pointer to the packet data.
		PKT_GSEGATE_DATA_STRUCT *pData = (PKT_GSEGATE_DATA_STRUCT*)m_Data;

		// Copy the values
		unsigned int cpylen = m_Header->DataLength;
		if(cpylen > sizeof (GseData))
			cpylen = sizeof (GseData);

		::memcpy(pData, &GseData, cpylen);

	}

	// Return status.
	return rval;
}

int HeroGsePacket::GetGseStatistics(PKT_GSEGATE_DATA_STRUCT &GseData)
{
	int rval;
		if ( !(rval = VerifyPacketForGet( GSEPTYPE_GSEGATE_DATA, GSEPLEN_GSEGATE_DATA )) )
		{
			// Get the values.
			PKT_GSEGATE_DATA_STRUCT *pData = (PKT_GSEGATE_DATA_STRUCT*)m_Data;

			// Initialize the Destination
			::memset(&GseData, 0, sizeof(GseData));

			// Copy the values
			unsigned int cpylen = m_Header->DataLength;
			if(cpylen > sizeof(GseData))
				cpylen = sizeof(GseData);

			:: memcpy(&GseData, pData, cpylen);
		}

		// Return status
		return rval;

}

//	SetCipStatistics
///		Sets the CIP Statistics Data within the packet
//
int HeroGsePacket::SetCipStatistics(PKT_CIPGATE_DATA_STRUCT &CipData)
{
	// Verify the packet is the correct type.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_CIPGATE_DATA )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		m_Header->DataLength = GSEPLEN_CIPGATE_DATA;
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;

		// Get a pointer to the packet data.
		PKT_CIPGATE_DATA_STRUCT *pData = (PKT_CIPGATE_DATA_STRUCT*)m_Data;

		// Copy the values
		unsigned int cpylen = m_Header->DataLength;
		if(cpylen > sizeof (CipData))
			cpylen = sizeof (CipData);

		::memcpy(pData, &CipData, cpylen);

	}

	// Return status.
	return rval;
}

int HeroGsePacket::GetCipStatistics(PKT_CIPGATE_DATA_STRUCT &CipData)
{
	int rval;
		if ( !(rval = VerifyPacketForGet( GSEPTYPE_CIPGATE_DATA, GSEPLEN_CIPGATE_DATA )) )
		{
			// Get the values.
			PKT_CIPGATE_DATA_STRUCT *pData = (PKT_CIPGATE_DATA_STRUCT*)m_Data;

			// Copy the values
			unsigned int cpylen = m_Header->DataLength;
			if(cpylen > sizeof(CipData))
				cpylen = sizeof(CipData);

			:: memcpy(&CipData, pData, cpylen);
		}

		// Return status
		return rval;

}

int HeroGsePacket::SetHdlcStatistics(PKT_HDLC_DATA_STRUCT &HdlcData)
{
	// Verify the packet is the correct type.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_HDLC_STATS )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		m_Header->DataLength = sizeof (PKT_HDLC_DATA_STRUCT);
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;

		// Get a pointer to the packet data.
		PKT_HDLC_DATA_STRUCT *pData = (PKT_HDLC_DATA_STRUCT*)m_Data;

		// Copy the values
		unsigned int cpylen = m_Header->DataLength;
		if(cpylen > sizeof (HdlcData))
			cpylen = sizeof (HdlcData);

		::memcpy(pData, &HdlcData, cpylen);

	}

	// Return status.
	return rval;
}

int HeroGsePacket::GetHdlcStatistics(PKT_HDLC_DATA_STRUCT &HdlcData)
{
	int rval;
		if ( !(rval = VerifyPacketForGet( GSEPTYPE_HDLC_STATS, sizeof(PKT_HDLC_DATA_STRUCT) )))
		{
			// Get the values.
			PKT_HDLC_DATA_STRUCT *pData = (PKT_HDLC_DATA_STRUCT*)m_Data;

			// Copy the values
			unsigned int cpylen = m_Header->DataLength;
			if(cpylen > sizeof(HdlcData))
				cpylen = sizeof(HdlcData);

			:: memcpy(&HdlcData, pData, cpylen);
		}

		// Return status
		return rval;

}

int HeroGsePacket::SetIdle()
{
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_IDLE )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		uint16_t *Data = (uint16_t *)m_Data;
		Data[0] = 0xFFFF;
		m_Header->DataLength = sizeof( uint16_t );
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;
	}

	// Return without error.
	return rval;
}

int HeroGsePacket::SetFdrDetFileData(PKT_FDRDETFILEDATA_STRUCT &Data)
{
	// Verify the packet is the correct type.
	int rval;
	if ( !(rval = VerifyPacketForSet( GSEPTYPE_FDRDETFILE )) )
	{
		// Set the data, correct the header and packet length, and set altered flag.
		m_Header->DataLength = sizeof (PKT_FDRDETFILEDATA_STRUCT);
		m_PacketLength = sizeof( EcGsePacket::Header ) + m_Header->DataLength;
		m_Altered = true;

		// Get a pointer to the packet data.
		PKT_FDRDETFILEDATA_STRUCT *pData = (PKT_FDRDETFILEDATA_STRUCT*)m_Data;

		// Copy the values
		unsigned int cpylen = m_Header->DataLength;
		if(cpylen > sizeof (PKT_FDRDETFILEDATA_STRUCT))
			cpylen = sizeof (PKT_FDRDETFILEDATA_STRUCT);

		::memcpy(pData, &Data, cpylen);

	}

	// Return status.
	return rval;
}

int HeroGsePacket::GetFdrDetFileData(PKT_FDRDETFILEDATA_STRUCT &Data)
{
	int rval;
		if ( !(rval = VerifyPacketForGet( GSEPTYPE_FDRDETFILE, sizeof(PKT_FDRDETFILEDATA_STRUCT) )))
		{
			// Get the values.
			PKT_FDRDETFILEDATA_STRUCT *pData = (PKT_FDRDETFILEDATA_STRUCT*)m_Data;

			// Copy the values
			unsigned int cpylen = m_Header->DataLength;
			if(cpylen > sizeof(PKT_FDRDETFILEDATA_STRUCT))
				cpylen = sizeof(PKT_FDRDETFILEDATA_STRUCT);

			:: memcpy(&Data, pData, cpylen);
		}

		// Return status
		return rval;

}
