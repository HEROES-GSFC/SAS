//	HeroGsePacket.h:
///		\file HeroGsePacket.h
///		C++ header file declaring the HeroGsePacket class.

#if !defined(AFX_ECHEROGSEPACKET_H__5171B519_D2BC_49E6_88D3_07B383A81BE9__INCLUDED_)
#define AFX_ECHEROGSEPACKET_H__5171B519_D2BC_49E6_88D3_07B383A81BE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
#include "GsePacket.h"
#include "HeroGsePacketStruct.h"
#else
#include <packets/GsePacket.h>
#include <packets/HeroGsePacketStruct.h>
#endif

#include <ShortBuffer.h>

///	Maximum number of converted data values.
#define HERO_MAXDATA 20

/// Valid HERO packet types.
enum HERO_PKT_TYPE
{
	// The Housekeeping (HK) data types.
	GSEPTYPE_HKCMDREPLY = 0x01,		///< Successful CIP command string processed. (2)
	GSEPTYPE_HKCMDWORD,   			///< Echo of each word received on the CIP.(2)
	GSEPTYPE_IDLE,					///< A IDLE packet to keep the synchronous link alive (2)

	// The control (CTL) word types.
	GSEPTYPE_CTLSYSDATA = 0x10,		///< Control system data (temperatures, pressures, etc). (46)
	GSEPTYPE_CTLASPDATA,			///< Control system aspect/control data. (60)
	GSEPTYPE_CTLCTLDATA,			///< Control system control loop data. (52)
	GSEPTYPE_CTLADU5,				///< Control system ADU5 GPS packet. (128 max)
	GSEPTYPE_MOTGPS,				///< Motorola GPS packet. (294 max)
	GSEPTYPE_CTLPARAM,				///< Control loop Parameters. (76)
	GSEPTYPE_CTLBRS,				///< Control report of Bearing Rotation Sensor. (2)
	GSEPTYPE_H3000MAG,				///< Honeywell H3000 Magnetometer packet (256 max).
	GSEPTYPE_PPSSYNC,				///< PPS Sync Information packet
	GSEPTYPE_CTLTANS,      			///< Trimble TANS DGPS packet.
	GSEPTYPE_PPSSYNC2,				///< 2011 PPS Sync Information Packet
  GSEPTYPE_CTLPARAM2,

	// The Aspect (ASP) system types.
	GSEPTYPE_ASPSYSDATA = 0x20,		///< Aspect system status. (30)
	GSEPTYPE_ASPPARAM,				///< Aspect parameters. (70)
	GSEPTYPE_ASPIMAGE,				///< Image information sent to ground. (372 max).
	GSEPTYPE_ASPFOCUS,				///< Focus loop positions and focus figures. (68)
	GSEPTYPE_ASPRAWIMAGE,			///< Raw image data from aspect camera (variable)
	GSEPTYPE_ASPCAMSTATUS,			///< Aspect camera status.
	GSEPTYPE_ASPMODECHANGE,			///< Aspect track mode change packet.
  
	GSEPTYPE_EHKSYSDATA = 0x30, ///< EHK status, temp, pressure, etc.
	GSEPTYPE_EHKPARAM,        		///< EHK parameters.

	// The Flight Data Recorder (FDR) types.
	GSEPTYPE_FDRSYSDATA = 0x40,	///< FDR Status, temp, pressure, etc.
	GSEPTYPE_FDRPARAM,			///< Deprecated.
	GSEPTYPE_FDRGPS,			///< Status of the FDR connected GPS
	GSEPTYPE_FDRDETFILE,		///< Current detector file sizes being recorded by the FDR

	// The Detector-Pair Stack (DPS) system types.
	GSEPTYPE_DPSSTATUS = 0x50,		///< Detector statistics & measured HV
	GSEPTYPE_DPSCURSET,				///< Detector current settings & parameters.
	GSEPTYPE_DPSDEFSET,				///< Detector default settings & parameters.
	GSEPTYPE_DPSTEMPRS,				///< Detector temperatures & pressures.
	GSEPTYPE_DPSEVENT,				///< Detector processed event data.
	GSEPTYPE_DPSCLKSYNC,			///< Clock synchronization data.

	// GSE Gateway Statistics
	GSEPTYPE_GSEGATE_DATA = 0x60,	///< GSE Gateway Statistics
	GSEPTYPE_CIPGATE_DATA,			///< CIP Gateway Statistics
	GSEPTYPE_HDLC_STATS,			///< HDLC Receive Statistics

	// Types common to all or most systems.
	GSEPTYPE_CLOCKSYNC = 0x80,		///< Sent by all systems when clock is synchronized (8).
  GSEPTYPE_NTCSYNC,             ///< Send by all systems' NetworkTimeClient when synchronizing.

	// The error types.
	GSEPTYPE_ERROR = 0xe0,			///< Error messages (8).
	GSEPTYPE_CMDACK,				///< Command acknowledgement (6).

	GSEPTYPE_DIAG = 0xd0,			///< Generic diagnostic packet for temporary use by any system. (34)
};

/// Valid HERO packet lengths.
enum HERO_PKT_LENGTH
{
	GSEPLEN_IDLE = 2,

	// The control (CTL) word types.
	GSEPLEN_CTLADU5 = 256,    ///< Control system ADU5 packet. (256 max)
	GSEPLEN_MOTGPS = 294,     ///< Motorola GPS packet. (294 max)
	GSEPLEN_H3000MAG = 256,   ///< CTL Honeywell H3000 magnetometer packet. (256 max)
	GSEPLEN_PPSSYNC = 29,     ///< PPS Sync packet (29)
  GSEPLEN_CTLTANS = 128,    ///< Trimble TANS DPGS packet (128 max ).

	// GSE Gateway Statistics
	GSEPLEN_GSEGATE_MAX_DATA = sizeof (PKT_GSEGATE_DATA_STRUCT),	///< Max length of a GSE Gateway Stat
	GSEPLEN_GSEGATE_DATA = GSEPLEN_GSEGATE_MAX_DATA - 128,		///< GSE Gateway Statistics (Min Size)
	GSEPLEN_CIPGATE_DATA = 27,		///< CIP Gateway Statistics

	// The error types.
	GSEPLEN_ERROR = 8,				///< Error messages (8).

	GSEPLEN_DIAG = 34,			///< Generic diagnostic packet for temporary use by any system. (34)
};

/// Packet receipt messages.
enum WM_HEROMSG
{
	// HK packet messages.
	WM_GSEP_HKCMDREPLY = (0x400),
	WM_GSEP_HKCMDWORD,
	WM_GSEP_IDLE,

	// CTL packet messages.
	WM_GSEP_CTLSYSDATA,
	WM_GSEP_CTLASPDATA,
	WM_GSEP_CTLCTLDATA,
	WM_GSEP_CTLADU5,
	WM_GSEP_MOTGPS,
	WM_GSEP_CTLPARAM,
	WM_GSEP_CTLBRS,
	WM_GSEP_H3000MAG,
	WM_GSEP_PPSSYNC,
	WM_GSEP_CTLTANS,

	// ASP packet messages.
	WM_GSEP_ASPSYSDATA,
	WM_GSEP_ASPPARAM,
	WM_GSEP_ASPIMAGE,
	WM_GSEP_ASPFOCUS,
	WM_GSEP_ASPALTAZ,
	WM_GSEP_ASPPOS,
	WM_GSEP_ASPRAWIMAGE,
	WM_GSEP_ASPCAMSTATUS,
	WM_GSEP_ASPMODECHANGE,

	// EHK packet messages.
	WM_GSEP_EHKSYSDATA,
	WM_GSEP_EHKPARAM,

	// FDR packet messages.
	WM_GSEP_FDRSYSDATA,
	WM_GSEP_FDRPARAM,
	WM_GSEP_FDRGPS,
	WM_GSEP_FDRDETFILEDATA,

	// DPS packet messages.
	WM_GSEP_DPSSTATUS,        ///< DPS status and rates.
	WM_GSEP_DPSCURSET,        ///< DPS current settings.
	WM_GSEP_DPSDEFSET,        ///< DPS default settings.
	WM_GSEP_DPSTEMPRS,        ///< DPS temperature and pressure
	WM_GSEP_DPSEVENT,         ///< DPS processed event data.
	WM_GSEP_DPSCLKSYNC,       ///< DPS clock synch data.

	// All systems packet messages.
	WM_GSEP_CLOCKSYNC,
  WM_GSEP_NTCSYNC,

	// Error packet messages.
	WM_GSEP_ERROR,
	WM_GSEP_CMDACK,

	// Diagnostic packet.
	WM_GSEP_DIAG,

	// GSE Gateway
	WM_GSEP_GSEGATE_DATA,	///< GSE Gateway Statistics
	WM_GSEP_CIPGATE_DATA,	///< CIP Gateway Statistics
	WM_GSEP_HDLC_STAT,		///< HDLC Receive Statistics

	// New PPS Packet
	WM_GSEP_PPSSYNC2,		///< 2011 PPS Sync Packet
	WM_GSEP_CTLPARAM2,
};

//	HeroGsePacket:
///		EcGsePacket-based class to handle HERO-specific packets.
//
///		This class provides for HERO-specific packet type creation, as well as
///		custom packet-building and packet data-conversion functions.
class HeroGsePacket : public EcGsePacket  
{
// Construction/destruction
public:
	HeroGsePacket();
	HeroGsePacket( int type );
	virtual ~HeroGsePacket();

// Protected data members.
protected:

// Public member functions.
public:
	/// Command reply type functions.
	int SetCmdReply( uint16_t seqNum );
	int GetCmdReply( uint16_t *pSeqNum );
  void SetDiagLength() {m_Header->DataLength = GSEPLEN_DIAG;};

	/// Command word echo functions.
	int SetCmdWord( uint16_t CmdWord );
	int GetCmdWord( uint16_t *pCmdWord );

	/// CTL system data get and set functions.
	int	SetCtlSysMuxTempData( int Index, uint16_t Data );
	int SetCtlSysTempData( uint16_t cpuTemp, uint16_t tankTemp, uint16_t azDacTemp,
			uint16_t elDacTemp, uint16_t azGyroTemp, uint16_t elGyroTemp );
	int SetCtlSysPressData( uint16_t azGyroP, uint16_t elGyroP, uint16_t elDacP,
			uint16_t azDacP, uint16_t tankP );
	int SetCtlSysRelayData( uint8_t RelayState );
	int SetCtlSysDacSelectData( uint8_t DacSelect );
	int SetCtlSysDioData( uint8_t dioState );
	int GetCtlSysData( GSE_CTLSYSDATA_STRUCT &SysData );

	/// CTL system aspect data get and set functions.
	int SetCtlAspRawAdcData( uint16_t RollIncl, uint16_t PitchIncl,
			uint16_t azTach, uint16_t muxAdcValue );
	int SetCtlAspFlags( bool MagValid, bool DgpsValid, bool GpsValid,
		bool SaeValid, bool Im0Valid, bool EfrValid, bool AfrValid,
		bool RfrValid, bool StartCam, bool StopCam, bool CamActive,
		bool CamStarted, bool CamReceived, bool bearingState, bool Im1Valid );
	int SetCtlAspModeAddr( uint8_t PointingMode, uint8_t AzMode, uint8_t ElMode, uint8_t MuxAddr );
	int SetCtlAspSaeElevation( uint16_t SaeEl );
	int SetCtlAspMagData( float Az, float Pitch, float Roll, uint16_t state );
	int SetCtlAspDgpsData( float Az, float Pitch, float Roll );
	int SetCtlAspGyroData( int16_t AzRate, int16_t ElRate, int16_t RollRate,
    int16_t SpareRate, int16_t AzHrgRate );
	int SetCtlAspGeoData( float Longitude, float Latitude );
	int SetCtlAspFggIntData( double Az, double El, double Roll, float azValue,
    float elValue );
	int GetCtlAspData( GSE_CTLASPDATA_STRUCT &AspData );

	/// CTL system control data get and set functions.
	int SetCtlCtlData(  uint16_t PMode, uint16_t AzMode, uint16_t ElMode,
    int AzExplain, int ElExplain, float AzUTarg, float ElUTarg, float AzPTarg,
		float ElPTarg, float AzRTarg, float ElRTarg, float RaTarg, float DecTarg,
    float AzP, float AzI, float AzD, float ElP, float ElI, float ElD,
    float ElV, float AzWV, float AzSV, float WheelP, float WheelI,
    float WheelD );
	int GetCtlCtlData( GSE_CTLCTLDATA_STRUCT &CtlData );

	/// CTL parameter data get and set functions.
	int SetCtlParamMiscSettings( int GeoPosSrc, bool CamTracking, bool AllowDgps,
			bool UseCam0, bool UseCam1 );
	int SetCtlParamElTrimFlags( bool SaeUseCam, bool FggUseCam,
    bool FggUseSae );
	int SetCtlParamAzTrimFlags( bool DgpsUseCam, bool MagUseCam,
    bool MagUseDgps, bool FggUseCam, bool FggUseDgps, bool FggUseMag );
  int SetCtlParamAzCamSensor( int numPoints, double maxAge );
  int SetCtlParamAzDgpsSensor( int numPoints, double maxAge );
  int SetCtlParamAzMagSensor( int numPoints, double maxAge );
  int SetCtlParamAzFggSensor( int numPoints, double maxAge );
  int SetCtlParamElCamSensor( int numPoints, double maxAge );
  int SetCtlParamElSaeSensor( int numPoints, double maxAge );
  int SetCtlParamElFggSensor( int numPoints, double maxAge );
  int SetCtlParamAzFggRipParam( float slope, float offset );
  int SetCtlParamElFggRipParam( float slope, float offset );
	int SetCtlParamRollTrimFlags( bool TrimRollInc, bool TrimRollFgg );
	int SetCtlParamElStowPos( float Position );
	int SetCtlParamElSaeOffset( float Offset );
	int SetCtlParamElIncOffset( float Offset );
	int SetCtlParamElSaeIncOffset( float Offset );
	int SetCtlParamElRateFggOffset( float Offset );
	int SetCtlParamAzDgpsOffset( float Offset );
	int SetCtlParamAzMagOffset( float Offset );
	int SetCtlParamAzRateFggOffset( float Offset );
	int SetCtlParamAzRateHrgOffset( float Offset );
	int SetCtlParamRollIncOffset( float Offset );
	int SetCtlParamRollRateFggOffset( float Offset );
	int SetCtlParamHrgFilterCutoff( float CutoffFreqHz );
	int SetCtlParamFggFilterCutoff( float CutoffFreqHz );
	int SetCtlParamGyroIntOffsets( float ElFggIntOffset, float AzFggIntOffset,
		float RollFggIntOffset );
	int SetCtlParamImErrThresh( float ImErrThresh );
	int SetCtlParamWheelDumpData( float PGain, float IGain, float DGain,
		float MaxOut, float ILimit, float OmegaMax );
	int SetCtlParamModeData( int Mode, float PGain, float IGain, float DGain,
			float ILimit, int MaxOut, float PBound, float DBound, float MaxRate,
			float Accel );
  int SetCtlParamGyroTweakData( float azXSlope, float azXOffset,
    float azYSlope, float azYOffset, float elXSlope, float elXOffset,
    float elYSlope, float elYOffset );
	int GetCtlParamData( GSE_CTLPARAMDATA_STRUCT &ParamData );

	int GetCtlParam2Data( GSE_CTLPARAMDATA2_STRUCT &ParamData );

	// CTL parameter

	/// CTL system bearing rate sensor get and set functions.
	int SetCtlBearingRate( uint16_t Rate );
	int GetCtlBearingRate( uint16_t *pRate );

	/// CTL system GPS and TANS get functions.
	int GetMotGpsPsdData( GSE_MOTGPSPSDDATA_STRUCT& PsdData );
	int GetM12GpsPsdData( GSE_M12GPSPSDDATA_STRUCT& PsdData );

	/// Honeywell H3000 Magnetometer get functions.
	int GetH3000MagData( char *data, int *length );

	/// PPS Sync functions
	int GetPpsSyncData( PKT_PPSSYNCDATA_STRUCT& ppsData);
	int SetPpsSyncData( PKT_PPSSYNCDATA_STRUCT& ppsData);
	int GetPpsSync2Data( PKT_PPSSYNC2DATA_STRUCT& ppsData);
	int SetPpsSync2Data( PKT_PPSSYNC2DATA_STRUCT& ppsData);

	/// ASP system data get and set functions.
	int	SetAspSysDataAdcValues( int16_t cpuPressure, int16_t cpuTankTemp,
			int16_t cpuSpare0, int16_t cpuSpare1, int16_t camSensorTemp0,
      int16_t camAirTemp0, int16_t camPressure0, int16_t focusPos0,
      int16_t camSensorTemp1, int16_t camAirTemp1, int16_t camPressure1,
      int16_t focusPos1 );
	int SetAspSysDataStatus( uint16_t RelayBits, int GeoSrc );
	int SetAspSysDataFreeSpace( uint32_t blocksFree, uint32_t blocksAvail,
			uint32_t blockSize );

	int SetAspSysDataCamStatus( int Camera, int Mode, bool CamConn,
			bool FocusAdj, bool ImSave, bool MotorMove, int CatSize, int Activity,
			int FocusStepCount );
	int GetAspSysData( USR_ASPSYSDATA_STRUCT &Data );

	// ASP parameter data get and set functions.
	int SetAspParamCamID( uint8_t CamID );
	int SetAspParamTrkMode( int Mode );
	int	SetAspParamFlags( int GeoPosSrc, bool CamXFlip, bool CamYFlip,
			bool CamCoolerOn, bool CamConnect, bool TrkFlatFlag, bool UseCECirc );
	int SetAspParamCamExposure( float ExpSec );
	int SetAspParamCamSetpoint( float SetPoint );
	int SetAspParamCamCenter( uint16_t CamXCenter, uint16_t CamYCenter,
			float RotAngle );
	int SetAspParamCamParam( float ImScale, float EPerCount );
	int SetAspParamTrkThresh( float PixThresh, float ObjThresh );
	int SetAspParamTrkMaxErr( float MaxPosErr );
	int SetAspParamTrkMinID( int MinID );
	int SetAspParamTrkWidths( uint16_t MinObjRadius, int FilterWidth );
	int SetAspParamCatTarget( float RA, float Dec );
	int SetAspParamCatParam( float Radius, float VMin, float VMax );
	int SetAspParamGeoPos( float Longitude, float Latitude );
	int	SetAspParamFocusMotorRate( int RateHz );
	int	SetAspParamFocusParam( uint8_t FfigType, int MinPos, int MaxPos,
			int Backlash, int NSteps, int StepSize );
	int SetAspParamOffsetParam( float Offset, float OffsetZPA );
	int SetAspParamUsbEnum( uint8_t UsbEnum );
	int SetAspParamFocusMode( uint8_t FocusMode );
	int SetAspParamCECircParam( uint16_t XCenter, uint16_t YCenter,
			uint16_t Radius );
	int SetAspParamEdgeOffsets( uint32_t LeftOffset, uint32_t RightOffset,
			uint32_t BottomOffset, uint32_t TopOffset );
	int SetAspParamBiasParam( double BiasCount, double BiasSigma );
	int SetAspParamImSize( int XSize, int YSize );
	int SetAspParamExtFlags( bool UseCenterQuad, bool saveImages );
	int SetAspParamData( USR_ASPPARAMDATA_STRUCT &Data );
	int GetAspParamData( USR_ASPPARAMDATA_STRUCT &Data );

	//	ASP image data get and set functions.
	int	SetAspImCamID( int CamID );
	int SetAspImTargetData( float RA, float Dec, float Azimuth,
			float Altitude );
	int SetAspImTrackData( float XOff, float YOff, float RA, float Dec,
			float RASigma, float DecSigma, float Azimuth, float Altitude,
			float Roll, int NObjFound, int NObjID );
	int SetAspImStatData( uint16_t PixMin, uint16_t PixMax, float PixMean,
			float PixSigma, float BgdMean, float BgdSigma );
	int SetAspImCenterData( uint16_t XCenter, uint16_t YCenter );
	int SetAspImObjectData( int Index, double XPos, double YPos, uint16_t ID,
			double Counts );
	int	GetAspImData( USR_ASPIMDATA_STRUCT &Data );

	//	ASP focus data get and set functions.
	int	SetAspFocusCamID( int CamID );
	int	SetAspFocusDataParam( int NPoints, double centroid );
	int SetAspFocusDataPoint( int IPoint, int Position, double FFig );
	int GetAspFocusData( USR_ASPFOCUSDATA_STRUCT &Data );

	// 	ASP camera status get and set functions.
	int SetAspCamStatus( int camID, bool connected, bool coolerOn,
			int coolStatus, float coolPower, float ccdTemp );
	int GetAspCamStatus( USR_ASPCAMSTATUS_STRUCT &Data );

	// ASP mode change get and set functions.
	int SetAspModeChange( int camID, int newMode, int oldMode, int modeStatus,
			int modeActivity );
	int GetAspModeChange( USR_ASPMODECHANGE_STRUCT &Data );

	//	EHK system data get and set functions.
  int setEhkSysMuxData( short tempVal, short inampVal, int tempID,
    int inampID );
  int setEhkSysAdcData( ShortBuffer &data, int tempID, int inampID );
	int setEhkSysRelayData( uint16_t relayBits );
  int setEhkSysDioData( uint8_t dioBits, bool motorMoving );
  int setEhkSysMotorData( int motorPos );
	int GetEhkSysData( USR_EHKSYSDATA_STRUCT &Data );

  // EHK parameter data set and get functions.
  int setEhkBalMotorParamData( int stepRate, int minPos, int maxPos,
    bool homeReset, int homePos, bool midReset, int midPos,
    bool endReset, int endPos  );
  int setEhkHeaterParamData( int heater, int ctlState, float lThresh,
    float uThresh );
  int getEhkParamData( USR_EHKHTRPARAMDATA_STRUCT &htrData,
    USR_EHKBALMOTORPARAMDATA_STRUCT &motorData );

	//	FDR system data get and set functions.
	int SetFdrSysRawStatData( uint32_t RawTxEvents, uint32_t MaxFileSize, uint8_t RawTxDet,
		uint32_t Det0RxEvents, uint32_t Det1RxEvents, uint32_t Det2RxEvents, uint32_t Det3RxEvents,
		uint32_t Det4RxEvents, uint32_t Det5RxEvents, uint32_t Det6RxEvents, uint32_t Det7RxEvents );
	int SetFdrSysMiscStatData( uint32_t TotalRxBytes, uint32_t TotalTxBytes,
		uint32_t KbFreeP, uint32_t KbFreeS );
	int SetFdrSysAdcData( uint16_t TankTemp, uint16_t TankPress, uint16_t Spare1, uint16_t Spare2, uint16_t BatteryV, uint16_t dioState );
	int SetFdrSysGseData( uint32_t RxBytes, uint16_t RxPkts, uint16_t RxErrors, uint16_t RxLastError,
		uint32_t TxBytes, uint16_t TxPkts, uint16_t TxErrors, uint16_t TxLastError );
	int SetFdrSysImRxData( uint32_t RxBytes, uint16_t RxPkts, uint16_t RxImPkts, uint16_t QImPkts );
	int SetFdrSysImTxData( uint16_t QImPkts, uint16_t TxPkts, uint32_t TxBytes, uint16_t TxErrors, uint16_t TxLastError );
	int GetFdrSysData( USR_FDRSYSDATA_STRUCT &Data );
	int SetFdrDetFileData( PKT_FDRDETFILEDATA_STRUCT &Data);
	int GetFdrDetFileData( PKT_FDRDETFILEDATA_STRUCT &Data);

//	/// FDR parameter data get and set functions.
//	int SetFdrParamData( uint8_t ProcFlags, bool FwdRaw, uint8_t RawDet, uint16_t EventsPerFile, uint8_t ParamDet,
//		uint16_t ChanStep, uint16_t ChanThresh, float XOff, float YOff, float PCoeff0, float PCoeff1,
//		float PCoeff2, float ECoeff0, float ECoeff1, float ECoeff2 );
//	int GetFdrParamData( USR_FDRPARAMDATA_STRUCT &Data );

	///	DPS status data get and set functions.
	int	SetDpsStatusDetRateData( int detID, uint16_t isrCount, uint16_t readCount,
    uint16_t writeCount, uint32_t ppsCount, uint16_t triggerCount,
    uint16_t swTriggerCount, uint16_t calTriggerCount, uint16_t ultVetoCount,
    uint16_t calVetoCount, uint16_t hdrErrCount );
	int SetDpsStatusDetVData( int detID, uint16_t pmtV, uint16_t driftV,
    uint16_t gridV );
  int SetDpsEventQRateData( uint16_t readCount0, uint16_t writeCount0,
    uint16_t readCount1, uint16_t writeCount1 );
	int GetDpsStatusData( PKT_DETSTATUS_STRUCT &data0,
    PKT_DETSTATUS_STRUCT &data1 );
  int GetDpsQStatusData( PKT_QSTATUS_STRUCT &data0,
    PKT_QSTATUS_STRUCT &data1 );

	/// DPS parameter data get and set functions.
	int	SetDpsParamVSetData( int detID, uint16_t lThreshMv, uint16_t uThreshMv,
    uint16_t pmtV, uint16_t driftV, uint16_t gridV );
  int SetDpsParamFpgaData( int detID, uint16_t eventCtl,
    uint16_t fifoIntThresh, uint16_t trigHoldoff, uint16_t fifoLowThresh,
    uint16_t verMoDay, uint16_t verYrVer );
  int SetDpsParamSwTrigData( bool swTrigEnable0, bool swTrigEnable1 );
	int GetDpsParamData( USR_DETPARAM_STRUCT &Data0,
    USR_DETPARAM_STRUCT &Data1 );

	/// DPS temperature and pressure data get and set functions.
	int	SetDpsTempPresData( uint16_t DpsTemp, uint16_t DpsPress,
    uint16_t DpsRegTemp, uint16_t DpsSpareV, uint16_t DetTemp0,
    uint16_t DetPress0, uint16_t DetAuxTemp0, uint16_t DetTemp1,
    uint16_t DetPress1, uint16_t DetAuxTemp1 );
	int	GetDpsTempPresData( USR_DPSTPDATA_STRUCT &Data );

	/// DPS processed event data get and set functions.
	int SetDpsEventData( DET_EVENT_DATA &Data );
	int GetDpsEventData( DET_EVENT_DATA &Data );

	/// DPS clock sync data get and set functions.
	int SetDpsClkSyncDetTimeData( uint32_t Time0, uint32_t Time1 );
	int GetDpsClkSyncDetTimeData( double *pTime0, double *pTime1 );

	/// Get and set functions for clock sync packets.
	int SetClkSyncData( double ClkDiff, uint16_t Thresh, bool Reset );
	int SetClkSyncData( int ClkDiffMs, uint16_t Thresh, bool Reset );
	int GetClkSyncData( GSE_CLKSYNC_STRUCT &SyncData );

  int SetNTCSyncData( PKT_NTCDATA_STRUCT &data );
  int GetNTCSyncData( PKT_NTCDATA_STRUCT &data );

	/// Get and set functions for error types.
	int	SetErrorData( uint16_t ErrVal1, uint16_t ErrVal2=0, uint16_t ErrVal3=0,
			uint16_t ErrVal4=0 );
	int GetErrorData( uint16_t *pError, uint16_t *pType, uint16_t *pDataValue,
			uint16_t *pNominalValue );

	/// Get and set functions for command acknowledgement packets.
	int SetCmdAckSeqNum( uint16_t SeqNum );
	int SetCmdAckCmdID( uint16_t CmdID );
	int SetCmdAckCmdResult( uint16_t CmdResult );
	int GetCmdAckData( GSE_CMDACK_STRUCT &AckData );
	int SetIdle();

	/// Get and set functions for GSE Gateway packets.
	int SetGseStatistics( PKT_GSEGATE_DATA_STRUCT &GseData);
	int GetGseStatistics( PKT_GSEGATE_DATA_STRUCT &GseData);
	int SetCipStatistics( PKT_CIPGATE_DATA_STRUCT &CipData);
	int GetCipStatistics( PKT_CIPGATE_DATA_STRUCT &CipData);
	int	SetHdlcStatistics( PKT_HDLC_DATA_STRUCT &HdlcData);
	int GetHdlcStatistics( PKT_HDLC_DATA_STRUCT &HdlcData);

	/// Set functions used by class CGseHandler.
	virtual bool HandleError( uint16_t ErrVal1, uint16_t ErrVal2=0,
			uint16_t ErrVal3=0, uint16_t ErrVal4=0 );

protected:
	/// Reset, creation.
	virtual int VerifyType( uint8_t type, int *pDataLength, uint32_t *pMessage,
			bool *pVariableLength );
	virtual int VerifyPacketForSet( uint8_t type );
	virtual int VerifyPacketForGet( uint8_t type, int length );
	uint16_t ReverseBytes( uint16_t val );
	short ReverseBytes( short val );
	uint32_t ReverseBytes( uint32_t val );
	long ReverseBytes( long val );
};

#endif // !defined(AFX_ECHEROGSEPACKET_H__5171B519_D2BC_49E6_88D3_07B383A81BE9__INCLUDED_)
