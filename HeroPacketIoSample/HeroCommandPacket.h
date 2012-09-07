//	HeroCommandPacket.h:
///		\file HeroCommandPacket.h
///		C++ header file declaring the HeroCommandPacket class.

#if !defined(AFX_ECHEROCOMMANDPACKET_H__5ADCB7DA_809E_4EEC_AF8E_4029341E77E4__INCLUDED_)
#define AFX_ECHEROCOMMANDPACKET_H__5ADCB7DA_809E_4EEC_AF8E_4029341E77E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Emb.h"

#ifdef _WIN32
#include "CommandPacket.h"
#include "CipCom.h"
#else
#include <packets/CommandPacket.h>
#include <packets/CipCom.h>
#endif


/// Enumerated azimuth trim sources.
enum CTL_AZTRIM_SRC {
	AZTRIM_GPOS=0,			///< Trim azimuth sensor with camera/gyro position.
	AZTRIM_DGPS,			///< Trim azimuth sensor with DGPS azimuth.
	AZTRIM_MAG				///< Trim azimuth sensor with magnetometer azimuth.
};

/// Gondola axes.
enum GON_AXIS {
	GONAXIS_EL=0,			///< Gondola elevation axis.
	GONAXIS_AZ,				///< Gondola azimuth axis.
	GONAXIS_ROLL			///< Gondola roll axis.
};


//	HeroCommandPacket:
///		EcCommandPacket-based class providing customized behavior for the HERO project.
//
///		This class defines the custom commands for the Ampr project, as well as special
///		command-building routines.
class HeroCommandPacket : public EcCommandPacket  
{
// Construction, destruction.
public:
	HeroCommandPacket();
	virtual ~HeroCommandPacket();

	enum {MAGCMD_MAXLEN = 40};

// Public member functions.
public:
	// Stow pin commands.
	int		SetStowPinCmd( bool StowPinIn );

	// GPS timing source commands.
	int		SetGpsTimingCmd( bool UseGps );

	// Set clock set threshhold
	int		SetClockSetThresh(uint16_t thresh);
	int		GetClockSetThresh(uint16_t *thresh);
 
	// GPS reset command.
	int		SetGpsResetCmd();

	// CTL, ASP, EHK relay bit set.
	int		SetRelayBitsCmd( uint16_t RelayBits );
	int		GetRelayBitsData( uint16_t *RelayBits );

	// Geographic position commands.
	int		SetGeoSrcCmd( GEOPOS_SRC Src );
	int		SetGeoPosCmd( GEOPOS_SRC Src, float Lat, float Lon );
	int		GetGeoPosData( float *pLat, float *pLon );

	// Save/restore default commands.
	int		SetDefaultSaveCmd();
	int		SetDefaultRestoreCmd();

	// DGPS reset command.
	int		SetDgpsResetCmd();

	// Magnetometer commands.
	int		SetMagForwarding( bool fwdHeading, bool fwdNonHeading );
	int 	GetMagForwarding( bool &fwdHeading, bool &fwdNonHeading );
	int		SetMagRawCommand( char *cmdBuff, int &cmdLen );
	int		GetMagRawCommand( char *cmdbuff, int &cmdLen );

	// CTL pointing commands.
	int		SetNoPointCmd();
	int		SetSafePointCmd();
	int		SetStowPointCmd();
	int		SetStaticPointCmd( float Azimuth, float Elevation );
	int		GetStaticPointData( float *pAzimuth, float *pElevation );
	int		SetInertialPointCmd( float RA, float Dec );
	int		GetInertialPointData( float *pRA, float *pDec );
	int		SetCelestialPointCmd( bool UseCam0, bool UseCam1, float RA,
    float Dec );
	int		GetCelestialPointData( bool *pUseCam0, bool *pUseCam1, float *pRA,
    float *pDec );

	// CTL non-pointing direct motor voltage setting commands.
	int		SetWheelVoltageCmd( float Volts );
	int		SetShaftVoltageCmd( float Volts );
	int		SetElevationVoltageCmd( float Volts );
	int		SetBearingVoltageCmd( float Volts );
	int		GetCtlMotorVoltageData( float *pmVolts );

	// CTL Manual sensor offset settings.
	int		SetElSaeOffsetCmd( float Offset );
	int		SetElIncOffsetCmd( float Offset );
	int		SetElSaeIncOffsetCmd( float Offset );
	int		SetElStowPosCmd( float StowEl );
	int		SetElCurAsStowCmd();
	int		SetAzDgpsOffsetCmd( float Offset );
	int		SetAzMagOffsetCmd( float Offset );
	int		SetRollIncOffsetCmd( float Offset );
	int		SetElRateFggOffsetCmd( float Offset );
	int		SetAzRateFggOffsetCmd( float Offset );
  int   SetAzRateHrgOffsetCmd( float Offset );
	int		SetRollRateFggOffsetCmd( float Offset );
	int		GetOffsetData( float *pOffset );

	// CTL Automatic azimuth trim settings.
  int   SetFggValidFlag( bool FggValid );
	int		SetElAutoTrimFlags( bool SaeUseCam, bool FggUseCam, bool FggUseSae );
	int		SetAzAutoTrimFlags( bool DgpsUseCam, bool MagUseCam,
    bool MagUseDgps, bool FggUseCam, bool FggUseDgps, bool FggUseMag );
	int		SetRollAutoTrimFlags( bool TrimInc, bool TrimFggRate );
	int		GetElAutoTrimFlags( bool *SaeUseCam, bool *FggUseCam,
    bool *FggUseSae );
	int		GetAzAutoTrimFlags( bool *DgpsUseCam, bool *MagUseCam,
    bool *MagUseDgps, bool *FggUseCam, bool *FggUseDgps, bool *FggUseMag );
	int		GetRollAutoTrimFlags( bool *pTrimInc, bool *pTrimFggRate );
	int		SetDgpsValidFlag( bool DgpsValid );
  int   SetCtlFggCutoff( float FggCutoff );
  int   SetCtlHrgCutoff( float hrgCutoff );
  int   GetFilterCutoffData( float *cutoff );
  int   SetAzCamSensorParam( int numPoints, double maxAge );
  int   SetAzDgpsSensorParam( int numPoints, double maxAge );
  int   SetAzMagSensorParam( int numPoints, double maxAge );
  int   SetAzFggSensorParam( int numPoints, double maxAge );
  int   SetElCamSensorParam( int numPoints, double maxAge );
  int   SetElSaeSensorParam( int numPoints, double maxAge );
  int   SetElFggSensorParam( int numPoints, double maxAge );
  int   GetCtlSensorParam( int &numPoints, double &maxAge );

	// CTL aspect update commands.
	int		SetImageErrorThreshCmd( float ImErrThresh );
	int		GetImageErrorThreshData( float *pImErrThresh );
	int		SetImageAspectCmd( int CamId, double Azimuth, double Elevation,
			double Roll, double Error, double Time );
	int		GetImageAspectData( int *pCamId, double *pAzimuth, double *pElevation,
			double *pRoll, double *pError, double *pTime );
	int		SetGyroUpdateCmd( float ElRate, float AzRate, float RollRate );
	int		GetGyroUpdateData( float *pElRate, float *pAzRate, float *pRollRate );
	int		SetAspAckTrkOnCmd();
	int		SetAspAckTrkOffCmd();
	int		SetAspAckTrkErrCmd();

	// CTL control parameter commands.
	int		SetControlModeParamCmd( uint16_t Mode, float PGain, float IGain,
			float DGain, float MaxOutV, float ILimit );
	int		GetControlModeParamData( int* pMode, float* pPGain, float* pIGain,
			float* pDGain, float *pMaxOutV, float *pILimit );
	int		SetControlModeBoundsCmd( int Mode, float PBound, float DBound );
	int		GetControlModeBoundsData( int *pMode, float *PBound, float *DBound );
	int		SetControlModeSlewParamCmd( int Mode, float MaxRate, float Accel );
	int		GetControlModeSlewParamData( int *pMode, float *pMaxRate,
			float *pAccel );
	int		SetWheelDumpParamCmd( float PGain, float IGain, float DGain,
				float MaxOut, float ILimit, float OmegaMax );
	int		GetWheelDumpParamData( float *pPGain, float *pIGain,
			float *pDGain, float *pMaxOut, float *pILimit, float *pOmegaMax );
	int		SetCtlParamDacSelectCmd( int ShaftSel, int WheelSel, int ElSel );
	int		GetCtlParamDacSelectData( int *pShaftSel, int *pWheelSel, int *pElSel );

  int   SetCtlParamGyroTweakCmd( uint16_t cmdID, float slope, float offset );
  int   GetCtlParamGyroTweakData( float &slope, float &offset );

	// Aspect parameter command routines.
	int		SetAspExposureTimeCmd( int CamID, uint16_t ExpTimeMs );
	int		GetAspExposureTimeData( uint16_t *pExpTimeMs );
	int		SetAspImageCenterCmd( int CamID, uint16_t XCenter, uint16_t YCenter );
	int		GetAspImageCenterData( uint16_t *pXCenter, uint16_t *pYCenter );
	int		SetAspCamParamCmd( int CamID, float Scale, float EPerCount, bool XFlip, bool YFlip,
		float RotAngleDeg );
	int		GetAspCamParamData( float *pScale, float *pEPerCount, bool *pXFlip, bool *pYFlip,
		float *pRotAngleDeg );
	int		SetAspCatalogParamCmd( int CamID, float Radius, float VMin, float VMax );
	int		GetAspCatalogParamData( float *pRadius, float *pVMin, float *pVMax );
	int		SetAspTrackThreshCmd( int CamID, float PixelThresh, float ObjThresh );
	int		GetAspTrackThreshData( float *pPixelThresh, float *pObjThresh );
	int		SetAspTrackMaxPosErrCmd( int CamID, float MaxPosErr );
	int		GetAspTrackMaxPosErrData( float *pMaxPosErr );
	int		SetAspTrackMinIDCmd( int CamID, int MinID );
	int		GetAspTrackMinIDData( int *pMinID );
	int		SetAspTrackWidthCmd( int CamID, uint16_t MinObjRadius, int FilterWidth );
	int		GetAspTrackWidthData( uint16_t *pMinObjRadius, int *pFilterWidth );
	int		SetAspTrackMethodCmd( int CamID, bool UseFlat );
	int		SetAspTrackCECircParam( int CamID, uint16_t XCenter, uint16_t YCenter, uint16_t Radius );
	int		GetAspTrackCECircParam( uint16_t *pXCenter, uint16_t *pYCenter, uint16_t *pRadius );
	int		SetAspCamOffsetCmd( int CamID, float Offset, float OffsetZPA );
	int		GetAspCamOffsetData( float *pOffset, float *pOffsetZPA );

	// Aspect camera/image settings command routines.
	int		SetAspCamCoolerStateCmd( int CamID, bool CoolerOn );
	int		SetAspCamCoolerSetPointCmd( int CamID, float SetTemp );
	int		GetAspCamCoolerSetPointData( float *pSetTemp );
	int		SetAspCamConnectStateCmd( int CamID, bool Connect );
	int		SetAspImageSaveStateCmd( int CamID, bool SaveImages );
	int		SetAspCECircStateCmd( int CamID, bool EnableCECirc );

	// Aspect camera track and focus commands routines.
	int		SetAspStopCameraCmd( int CamID );
	int		SetAspSingleExposureCmd( int CamID );
	int		SetAspStartTrackingCmd( int CamID );
	int		SetAspStopTrackingCmd( int CamID );
	int		SetAspProcessTestImageCmd( int CamID );
	int		SetAspMakeDarkImageCmd( int CamID );
	int		SetAspTxFullImageCmd( int CamID );
	int		SetAspTxCenterImageCmd( int CamID );
	int		SetAspUsbCam0ToCamera( int CamID );

	/// Aspect focus control command routines.
	int		SetAspAdjustFocusCmd( int CamID, uint16_t FocusMode );
	int		GetAspAdjustFocusData( uint16_t *pFocusMode );
	int		SetAspStopFocusCmd( int CamID );
	int		SetAspFocusParamCmd( int CamID, uint8_t FfigType, int Backlash,
			int StepSize, int numSteps, int minPos, int maxPos );
	int		GetAspFocusParamData( uint8_t *pFfigType, int *pBacklash,
		int *pStepSize, int *pNumSteps, int *minPos, int *maxPos );

	/// Focus motor commands.
	int		SetAspFocusMotorPosCmd( int CamID, uint32_t FocusPos );
	int		GetAspFocusMotorPosData( uint32_t *pFocusPos );
	int		SetAspFocusMotorRateCmd( int CamID, int StepRate );
	int		GetAspFocusMotorRateData( int *pStepRate );
	int		SetAspFocusMotorBoundCmd( int CamID, int minPos, int maxPos );
	int		GetMotorBoundData( int *minPos, int *maxPos );

	// Aspect additional parameter command routines.
	int		SetAspImageEdgeParamCmd( int CamID, unsigned int LeftOffset, unsigned int RightOffset,
		unsigned int BottomOffset, unsigned int TopOffset );
	int		GetAspImageEdgeParamData( unsigned int *pLeftOffset, unsigned int *pRightOffset,
		unsigned int *pBottomOffset, unsigned int *pTopOffset );
	int		SetAspImageBiasParamCmd( int CamID, double BiasCount, double BiasSigma );
	int		GetAspImageBiasParamData( double *pBiasCount, double *pBiasSigma );
	int		SetAspImageSizeCmd( int CamID, int XSize, int YSize );
	int		GetAspImageSizeData( int *const XSize, int *const YSize );
	int		SetAspCamCentralQuadCmd( int CamID, bool UseCenterQuad );

	// EHK parameter commands.
  int   setEhkHeaterControlParamCmd( int heater, int ctlState, float lThresh,
    float uThresh );
  int   getEhkHeaterControlParamData( int &heater, int &ctlState, float &lThresh,
    float &uThresh );
  int   setEhkBalanceMotorPosCmd( int position );
  int   getMotorPosData( int &position );
  int   setEhkBalanceMotorRateCmd( int stepRate );
  int   getMotorRateData( int &stepRate );
  int   setEhkBalanceMotorBoundCmd( int minPos, int maxPos );
  int   getMotorBoundData( int &minPos, int &maxPos );

  int setEhkBalMotorStopCmd();
  int setEhkBalMotorHomeParam( bool reset, int position );
  int setEhkBalMotorMidParam( bool reset, int position );
  int setEhkBalMotorEndParam( bool reset, int position );
  int getEhkBalMotorStopParam( bool &reset, int &position );

	int		GetGyroConversionData( float *pSlope, float *pOffset );
	int		SetAxisContributionCmd( GON_AXIS Axis, float ElFrac, float EYFrac, float ERFrac );
	int		GetAxisContributionData( float *pElFrac, float *pEYFrac, float *pERFrac );
	int		SetEhkFggCutoffFreq( float CutoffFreqHz );
	int		SetEhkHrgCutoffFreq( float CutoffFreqHz );
	int		GetFilterCutoffFreq( float *pCutoffFreqHz );

	// FDR parameter commands.
	int		SetFdrForceTimeSetCmd();
	int		SetFdrFileSavingStateCmd( bool SaveFiles );
	int		SetFdrRawDetectorCmd( int RawDet );
	int		GetFdrRawDetectorData( int *pRawDet );
	int		SetFdrEventsPerFileCmd( int FileEvents );
	int		GetFdrEventsPerFileData( int *pFileEvents );
	int		SetFdrEventProcessingCmd( uint16_t Flags );
	int		GetFdrEventProcessingData( uint16_t *pFlags );
	int		SetFdrStopRawSendCmd();
	int		SetFdrTurnHeaterOn();
	int		SetFdrTurnHeaterOff();

	// Detector command and data setting routines.
	int		SetDetLThreshVCmd( int Det, float Volts );
	int		SetDetUThreshVCmd( int Det, float Volts );
	int		GetDetThreshVData( float *pVolts );
  int   GetDetThreshMvData( uint16_t *mVolts );
	int		SetDetGridVCmd( int Det, uint16_t Volts );
	int		SetDetDriftVCmd( int Det, uint16_t Volts );
	int		SetDetPmtVCmd( int Det, uint16_t Volts );
	int		GetDetHighVData( uint16_t *pVolts );
	int		SetDetChanCoeffCmd( int Det, uint16_t Step, uint16_t Thresh );
	int		GetDetChanCoeffData( int *pDet, uint16_t *pStep, uint16_t *pThresh );
	int		SetDetCenterOffsetCmd( int Det, float XOffset, float YOffset );
	int		GetDetCenterOffsetData( int *pDet, float *pXOffset, float *pYOffset );
	int		SetDetPosCoeffCmd( int Det, float ACoeff, float BCoeff, float CCoeff );
	int		GetDetPosCoeffData( int *pDet, float *pACoeff, float *pBCoeff, float *pCCoeff );
	int		SetDetEnergyCoeffCmd( int Det, float ACoeff, float BCoeff, float CCoeff );
	int		GetDetEnergyCoeffData( int *pDet, float *pACoeff, float *pBCoeff, float *pCCoeff );
	int		SetDetDefaultVoltagesCmd( int Det, float LThresh, float UThresh, uint16_t Grid,
		uint16_t Drift, uint16_t Pmt );
	int		GetDetDefaultVoltagesData( float *pLThresh, float *pUThresh, uint16_t *pGrid,
		uint16_t *pDrift, uint16_t *pPmt );
	int		SetDetRestoreDefaultsCmd( int Det );
	int		SetDetSaveDefaultsCmd( int Det );
	int		SetDetFifoResetCmd();
	int		SetDetZeroHvCmd( int Det );
	int		SetDetZeroDpsHvCmd();
	int		SetDetProcessingStateCmd( int Det, bool ProcessEvents );

  // Detector FPGA settings command I/O methods.
  int   setDetEventControl( int idet, bool eventEnable, bool keepVetoed,
    bool intEnable, bool calEnable );
  int   setDetEventIntThreshold( int idet, int thresh );
  int   setDetTriggerHoldoff( int idet, int trigHoldoff );
  int   setDetEventIntLowThreshold( int idet, int thresh );
  int   GetDetFpgaRegister( uint16_t *pReg );

  int   SetDetSwTriggerEnable( bool enable0, bool enable1 );

  // GSE Gateway Commands
  int 	SetHkGatewayIoConfig(char hk, char cip);
  int 	GetHkGatewayIoConfig(char* hk, char* cip);

	//	Shutdown command.
	int		SetShutdownCmd();
};

#endif // !defined(AFX_ECHEROCOMMANDPACKET_H__5ADCB7DA_809E_4EEC_AF8E_4029341E77E4__INCLUDED_)
