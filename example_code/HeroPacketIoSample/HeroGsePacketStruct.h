///	\file HeroGsePacketStruct.h
///		This file defines the structures that overlay the EcGsePacket payload for certain
///		packet types.  It also defines the structures used to retrieve data from the packet
///		for certain packet types.

#ifndef HEROGSEPACKETSTRUCT_H
#define HEROGSEPACKETSTRUCT_H

#include <Emb.h>

//	PKT_CTLSYSDATA_STRUCT:
///		Structure to hold the CTL system data (temp, pressure, etc.) in the packet.
#pragma pack( push, ambient )
#pragma pack( 1 )
struct PKT_CTLSYSDATA_STRUCT
{
	uint16_t	MuxTemp[16];	///< MUX Temperatures.
	uint16_t	CtlTemp[6];		///< Other CTL temperatures.
	uint16_t	Press[5];			///< Pressures.
	uint8_t		DioState;			///< DMM DIO input states.
	uint8_t		RelayState;		///< Relay states as bits.
	uint8_t		DacSelect;		///< DAC selections as bits.
};
#pragma pack( pop, ambient )

//	GSE_CTLSYSDATA_STRUCT:
///		Structure to hold the CTL system data (temp, pressure, etc.)
struct GSE_CTLSYSDATA_STRUCT
{
	double	MuxTemp[16];	///< MUX Temperatures.
	double	CtlTemp[6];		///< Other CTL temperatures.
	double	Press[5];			///< Pressures.
  bool    EhkPower;     ///< DIO state of EHK power.
  bool    BearingPower; ///< DIO state of Bearing motor power.
	bool		SciTx;			  ///< State of HERO sience transmitter power relay.
  bool    AspPower;     ///< DIO state of ASP power.
  bool    FdrPower;     ///< DIO state of FDR power.
  bool    WheelPower;   ///< DIO state of wheel motor power.
  bool    ShaftPower;   ///< DIO state of shaft motor power.
  bool    ElPower;      ///< DIO state of elevation motor power.
	bool		AzShaftBackup;		///< Flag indicating if azimuth shaft backup DAC is enabled (TRUE) or not.
	bool		AzWheelBackup;		///< Flag indicating if azimuth wheel backup DAC is enabled (TRUE) or not.
	bool		ElBackup;			///< Flag indicating if elevation backup DAC is enabled (TRUE) or not.
	uint16_t	RelayState;			///< Bit-wise relay settings.
	uint16_t	DacSelect;			///< DAC selections as bits.
};

//	PKT_CTLASPDATA_STRUCT:
///		Structure to hold the CTL aspect data inside the packet.
#pragma pack( push, ambient )
#pragma pack( 1 )
struct PKT_CTLASPDATA_STRUCT
{
	uint16_t	Flags;				///< Flag bitfield.
	uint16_t	ModeAddr;			///< Modes and addresses.
	uint16_t	SaeElevation;	///< Raw Shaft angle encoder elevation.
	int16_t	ElPitch;				///< Raw elevation pitch inclinometer ADC reading.
	int16_t	ElRoll;					///< Raw elevation roll inclinometer ADC reading.
	int16_t	ElvIncl;				///< EHK elevation inclination angle.
	uint16_t	MagAzimuth;		///< Magnetometer azimuth.
	int16_t	MagPitch;				///< Magnetometer pitch angle.
	int16_t	MagRoll;				///< Magnetometer roll angle.
	uint16_t	MagState;     ///< Magnetometer data status word.
	uint16_t	DgpsAzimuth;	///< TANS azimuth.
	int16_t	DgpsPitch;			///< TANS pitch angle.
	int16_t	DgpsRoll;				///< TANS roll angle.
	int16_t	WheelTach;			///< Wheel tachometer value.
	int16_t	GyroAzRate;			///< Azimuth angular speed from gyro.
	int16_t	GyroElRate;			///< Elevation angular speed from gyros.
	int16_t	GyroRollRate;		///< Roll angular speed from gyros.
  int16_t GyroSpareRate;  ///< Non-elevation angular speed from elevation gyro.
	uint16_t	MuxValue;			///< Mux raw ADC value (Either shaft, wheel, elevation current or Az SAE).
	int32_t	FggIntAz;				///< Fine-guidance gyro rate-integrated azimuth.
	int32_t	FggIntEl;				///< Fine-guidance gyro rate-integrated elevation.
	int32_t	FggIntRoll;			///< Fine-guidance gyro rate-integrated roll.
	int16_t	Longitude;			///< Longitude currently set by preferred geographic position source.
	int16_t	Latitude;				///< Latitude currently set by preferred geographic position source.
  int16_t HrgAzRate;      ///< Azimuth high-rate gyro.
  int16_t ElFggRipVal;    ///< Trimmed elevation FGG RIP value.
  int16_t AzFggRipVal;    ///< Trimmed azimuth FGG RIP value.
};
#pragma pack( pop, ambient )

//	GSE_CTLASPDATA_STRUCT:
///		Structure to hold the converted CTL system aspect data for the GSE.
struct GSE_CTLASPDATA_STRUCT
{
	// Sensor valid flags.
	bool	MagValid;				///< Flag indicating magnetometer value is valid (updated).
	bool	DgpsValid;			///< Flag indicating TANS value is valid (updated).
	bool	GpsValid;				///< Flag indicating GPS values are valid.
	bool	EhkValid;				///< Flag indicating data from EHK are valid (updated).
	bool	SaeValid;				///< Flag indicating shaft angle encoder value is valid.
	bool	ImPosValid0;		///< Flag indicating image data from camera 0 is valid.
  bool  ImPosValid1;    ///< Flag indicating image data from camera 1 is valid.

	// Gyro rate-integrated position flags.
	bool	ElFggRipValid;			///< Flag indicating fine-guidance gyro inertial integrated position is valid.
	bool	AzFggRipValid;			///< Flag indicating fine-guidance gyro inertial integrated position is valid.
	bool	RollFggRipValid;		///< Flag indicating fine-guidance gyro inertial integrated position is valid.

	// Camera operation flags.
	bool	StartCam;				///< Flag for commanding ASP to begin camera tracking.
	bool	StopCam;				///< Flag for commanding ASP to stop camera tracking.
	bool	CamActive;			///< Flag indicating camera trim to be used.
	bool	CamStarted;			///< Flag indicating ASP received command to start camera tracking.
	bool	CamReceived;		///< Flag indicating camera image data was received.
	bool	BearingState;		///< Bearing state.

	//	Modes and mux address.
	int		PointMode;
	int		AzMode;
	int		ElMode;
	int		MuxAddr;
	int		MagState;

	double	SaeElevation;
	double	ElPitch;
	double	ElRoll;
	double	MagAzimuth;
	double	MagPitch;
	double	MagRoll;
	double	DgpsAzimuth;
	double	DgpsPitch;
	double	DgpsRoll;
	double	GyroAzRate;
	double	GyroElRate;
	double	GyroRollRate;
  double  GyroSpareRate;
	double	ElvIncl;
	double	AzTach;
	double	AzSaeAngle;
	double	ShaftCurrent;
	double	WheelCurrent;
	double	ElCurrent;
	double	FggIntAz;
	double	FggIntEl;
	double	FggIntRoll;
	double	Longitude;
	double	Latitude;
  double  HrgAzRate;
  double  ElFggRipVal;
  double  AzFggRipVal;
};

//	CTL_CTLDATA_STRUCT:
///		Structure to hold the raw CTL system control loop data in the packet.
#pragma pack( push, ambient )
#pragma pack( 1 )
struct CTL_CTLDATA_STRUCT
{
	uint16_t		Modes;			///< Mode bits for Az, El, and pointing mode.
	uint16_t		AzUTarget;		///< Ultimate azimuth target.
	int16_t		ElUTarget;		///< Ultimate elevation target.
	uint16_t		AzTarget;		///< Current (or immediate) azimuth position target.
	int16_t		AzRTarget;		///< Current (or immediate) azimuth rate target.
	int16_t		ElTarget;		///< Current (or immediate) azimuth position target.
	int16_t		ElRTarget;		///< Current (or immediate) azimuth rate target.
	int16_t		ElVOut;			///< Elevation motor voltage output (mVolt).
	int16_t		AzWVOut;		///< Azimuth wheel motor voltage output (mVolt).
	int16_t		AzSVOut;		///< Azimuth shaft motor voltage output (mVolt).
	float		RaTarget;		///< Current right-ascension target (degrees).
	float		DecTarget;		///< Current declination target (degrees).
	float		AzPValue;		///< Current azimuth PID loop P value.
	float		AzIValue;		///< Current azimuth PID loop I value.
	float		AzDValue;		///< Current azimuth PID loop D value.
	float		ElPValue;		///< Current elevation PID loop P value.
	float		ElIValue;		///< Current elevation PID loop I value.
	float		ElDValue;		///< Current elevation PID loop D value.
  float   WPValue;    ///< Wheel control PID loop P value.
  float   WIValue;    ///< Wheel control PID loop I value.
  float   WDValue;    ///< Wheel control PID loop D value.
};
#pragma pack( pop, ambient )

//	GSE_CTLCTLDATA_STRUCT:
///		Structure to hold the converted CTL system variables.
struct GSE_CTLCTLDATA_STRUCT
{
	int			AzMode;			///< Azimuth control mode.
	int			ElMode;			///< Elevation control mode.
	int			PMode;			///< Overall pointing mode.
  int     AzExplain;  ///< Explanation of Azimuth control mode demotion.
  int     ElExplain;  ///< Explanation of Elevation control mode demotion.
	double		AzUTarget;		///< Ultimate azimuth target (deg).
	double		ElUTarget;		///< Ultimate elevation target (deg).
	double		AzTarget;		///< Current (or immediate) azimuth position target.
	double		AzRTarget;		///< Current (or immediate) azimuth rate target.
	double		ElTarget;		///< Current (or immediate) azimuth position target.
	double		ElRTarget;		///< Current (or immediate) azimuth rate target.
	double		ElVOut;			///< Elevation motor voltage output (mVolt).
	double		AzWVOut;		///< Azimuth wheel motor voltage output (mVolt).
	double		AzSVOut;		///< Azimuth shaft motor voltage output (mVolt).
	double		RaTarget;		///< Current right-ascension target (degrees).
	double		DecTarget;		///< Current declination target (degrees).
	double		AzPValue;		///< Current azimuth PID loop P value.
	double		AzIValue;		///< Current azimuth PID loop I value.
	double		AzDValue;		///< Current azimuth PID loop D value.
	double		ElPValue;		///< Current elevation PID loop P value.
	double		ElIValue;		///< Current elevation PID loop I value.
	double		ElDValue;		///< Current elevation PID loop D value.
  double    WPValue;    ///< Wheel control PID loop P value.
  double    WIValue;    ///< Wheel control PID loop I value.
  double    WDValue;    ///< Wheel control PID loop D value.
};

//	CTL_PARAMDATA_STRUCT:
///		Structure to hold the CTL system parameter data in the packet.
#pragma pack( push, ambient )
#pragma pack( 1 )
struct CTL_PARAMDATA_STRUCT
{
	uint8_t		MiscSettings;			///< Bitfield settings.
	uint8_t		ElTrimFlags;			///< Bitfield settings of elevation auto-trim flags.
	uint8_t		AzTrimFlags;			///< Bitfield settings of elevation auto-trim flags.
	uint8_t		RollTrimFlags;			///< Bitfield settings of elevation auto-trim flags.
	uint16_t		StowPos;				///< Raw SAE reading of stow position.
	float		ElSaeOffset;			///< Offset angle of SAE elevation from true elevation (deg).
	float		ElIncOffset;			///< Offset angle of elevation inclinometer from true level (deg).
	float		ElSaeIncOffset;			///< Offset angle of SAE + inclinometer from true elevation. (deg)
	float		ElFggIntOffset;			///< Offset angle of fine-guidance gyro RIP elevation from true. (deg)
	float		ElRateFggOffset;		///< Offset of elevation fine-guidance gyro rate from true. (deg/s).
	float		AzDgpsOffset;			///< Offset angle of Dgps azimuth from true azimuth (deg).
	float		AzMagOffset;			///< Offset angle of magnetometer azimuth from true azimuth (deg).
	float		AzFggIntOffset;			///< Offset angle of fine-guidance gyro RIP azimuth from true. (deg)
	float		AzRateFggOffset;		///< Offset of azimuth fine-guidance gyro rate from true. (deg/s).
	float		RollIncOffset;			///< Offset angle of roll inclinometer from true level (deg).
	float		RollFggIntOffset;		///< Offset angle of fine-guidance gyro RIP roll from true. (deg)
	float		RollRateFggOffset;		///< Offset of roll fine-guidance gyro rate from true. (deg/s).
	uint16_t		ImErrThresh;			///< ASP image position error threshold.
	float				WDPGain;				///< Wheel momentum-dump PID loop P gain.
	float				WDIGain;				///< Wheel momentum-dump PID loop I gain.
	float				WDDGain;				///< Wheel momentum-dump PID loop D gain.
	uint16_t		WDMaxOut;				///< Wheel momentum-dump PID loop maximum shaft voltage output.
	uint16_t		WDILimit;				///< Wheel momentum-dump PID loop I limit.
	uint16_t		WDMaxOmega;			///< Wheel momentum-dump PID loop maximum angular speed.
	uint16_t		Mode;						///< Control mode for following parameters.
	float				PGain;					///< P gain of current mode's PID loop.
	float				IGain;					///< I gain of current mode's PID loop.
	float				DGain;					///< D gain of current mode's PID loop.
	float				ILimit;					///< Current mode's PID loop I limit.
	float				PBound;					///< Current mode's position (P) error bound.
	float				DBound;					///< Current mode's rate (D) error bound.
	uint16_t		MaxOut;					///< Current mode's PID loop maximum output.
	uint16_t		SlewMaxRate;			///< Current mode's slew angular speed maximum.
	uint16_t		SlewAccel;				///< Current mode's slew acceleration.
  float       FggCutoff;      ///< FGG filter cutoff frequency.
  float       HrgCutoff;      ///< HRG filter cutoff frequency.
  float       AzRateHrgOffset;  ///< Offset of azimuth high-rate gyro rate from true (deg/s).
  uint16_t    AzCamNPoints;       ///< Number of points in Azimuth CamSensor.
  uint16_t    AzCamMaxAge;        ///< Maximum age of fit for Azimuth CamSensor (sec).
  uint16_t    AzDgpsNPoints;      ///< Number of points in DgpsAzimuthSensor.
  uint16_t    AzDgpsMaxAge;       ///< Maximum age of fit for DgpsAzimuthSensor.
  uint16_t    AzMagNPoints;       ///< Number of points in MagAzimuthSensor.
  uint16_t    AzMagMaxAge;        ///< Maximum age of fit for MagAzimuthSensor.
  uint16_t    AzFggNPoints;       ///< Number of points in FggRipAzimuthSensor.
  uint16_t    AzFggMaxAge;        ///< Maximum age of fit for FggRipAzimuthSensor.
  uint16_t    ElCamNPoints;       ///< Number of points in Elevation CamSensor.
  uint16_t    ElCamMaxAge;        ///< Maximum age of fit for Elevation CamSensor.
  uint16_t    ElSaeNPoints;       ///< Number of points in SaeIncElevationSensor.
  uint16_t    ElSaeMaxAge;        ///< Maximum age of fit for SaeIncElevationSensor.
  uint16_t    ElFggNPoints;       ///< Number of points in FggRipElevationSensor.
  uint16_t    ElFggMaxAge;        ///< Maximum age of fit for FggRipElevationSensor.
  float       AzFggRipSlope;
  float       AzFggRipOffset;
  float       ElFggRipSlope;
  float       ElFggRipOffset;
  float       GyroTweak[4][2];
};
#pragma pack( pop, ambient )

//	CTL_PARAMDATA_STRUCT:
///		Structure to hold the CTL system parameter data in the packet.
#pragma pack( push, ambient )
#pragma pack( 1 )
struct CTL_PARAMDATA2_STRUCT
{
	uint8_t		MiscSettings;			///< Bitfield settings.
	uint8_t		ElTrimFlags;			///< Bitfield settings of elevation auto-trim flags.
	uint8_t		AzTrimFlags;			///< Bitfield settings of elevation auto-trim flags.
	uint8_t		RollTrimFlags;			///< Bitfield settings of elevation auto-trim flags.
	uint16_t		StowPos;				///< Raw SAE reading of stow position.
	float		ElSaeOffset;			///< Offset angle of SAE elevation from true elevation (deg).
	float		ElIncOffset;			///< Offset angle of elevation inclinometer from true level (deg).
	float		ElSaeIncOffset;			///< Offset angle of SAE + inclinometer from true elevation. (deg)
	float		ElFggIntOffset;			///< Offset angle of fine-guidance gyro RIP elevation from true. (deg)
	float		ElRateFggOffset;		///< Offset of elevation fine-guidance gyro rate from true. (deg/s).
	float		AzDgpsOffset;			///< Offset angle of Dgps azimuth from true azimuth (deg).
	float		AzMagOffset;			///< Offset angle of magnetometer azimuth from true azimuth (deg).
	float		AzFggIntOffset;			///< Offset angle of fine-guidance gyro RIP azimuth from true. (deg)
	float		AzRateFggOffset;		///< Offset of azimuth fine-guidance gyro rate from true. (deg/s).
	float		RollIncOffset;			///< Offset angle of roll inclinometer from true level (deg).
	float		RollFggIntOffset;		///< Offset angle of fine-guidance gyro RIP roll from true. (deg)
	float		RollRateFggOffset;		///< Offset of roll fine-guidance gyro rate from true. (deg/s).
	uint16_t		ImErrThresh;			///< ASP image position error threshold.
	float				WDPGain;				///< Wheel momentum-dump PID loop P gain.
	float				WDIGain;				///< Wheel momentum-dump PID loop I gain.
	float				WDDGain;				///< Wheel momentum-dump PID loop D gain.
	uint16_t		WDMaxOut;				///< Wheel momentum-dump PID loop maximum shaft voltage output.
	uint16_t		WDILimit;				///< Wheel momentum-dump PID loop I limit.
	uint16_t		WDMaxOmega;			///< Wheel momentum-dump PID loop maximum angular speed.
	uint16_t		Mode;						///< Control mode for following parameters.
	float				PGain;					///< P gain of current mode's PID loop.
	float				IGain;					///< I gain of current mode's PID loop.
	float				DGain;					///< D gain of current mode's PID loop.
	float				ILimit;					///< Current mode's PID loop I limit.
	float				PBound;					///< Current mode's position (P) error bound.
	float				DBound;					///< Current mode's rate (D) error bound.
	uint16_t		MaxOut;					///< Current mode's PID loop maximum output.
	uint16_t		SlewMaxRate;			///< Current mode's slew angular speed maximum.
	uint16_t		SlewAccel;				///< Current mode's slew acceleration.
  float       FggCutoff;      ///< FGG filter cutoff frequency.
  float       HrgCutoff;      ///< HRG filter cutoff frequency.
  float       AzRateHrgOffset;  ///< Offset of azimuth high-rate gyro rate from true (deg/s).
  uint16_t    AzCamNPoints;       ///< Number of points in Azimuth CamSensor.
  uint16_t    AzCamMaxAge;        ///< Maximum age of fit for Azimuth CamSensor (sec).
  uint16_t    AzDgpsNPoints;      ///< Number of points in DgpsAzimuthSensor.
  uint16_t    AzDgpsMaxAge;       ///< Maximum age of fit for DgpsAzimuthSensor.
  uint16_t    AzMagNPoints;       ///< Number of points in MagAzimuthSensor.
  uint16_t    AzMagMaxAge;        ///< Maximum age of fit for MagAzimuthSensor.
  uint16_t    AzFggNPoints;       ///< Number of points in FggRipAzimuthSensor.
  uint16_t    AzFggMaxAge;        ///< Maximum age of fit for FggRipAzimuthSensor.
  uint16_t    ElCamNPoints;       ///< Number of points in Elevation CamSensor.
  uint16_t    ElCamMaxAge;        ///< Maximum age of fit for Elevation CamSensor.
  uint16_t    ElSaeNPoints;       ///< Number of points in SaeIncElevationSensor.
  uint16_t    ElSaeMaxAge;        ///< Maximum age of fit for SaeIncElevationSensor.
  uint16_t    ElFggNPoints;       ///< Number of points in FggRipElevationSensor.
  uint16_t    ElFggMaxAge;        ///< Maximum age of fit for FggRipElevationSensor.
  float       AzFggRipSlope;
  float       AzFggRipOffset;
  float       ElFggRipSlope;
  float       ElFggRipOffset;
  float       GyroTweak[4][2];
  uint16_t    ElEntryBound;
  uint16_t    ElEntryCount;
  uint16_t    AzEntryBound;
  uint16_t    AzEntryCount;
};
#pragma pack( pop, ambient )

//	GSE_CTLPARAMDATA_STRUCT:
///		Structure to hold converted CTL system parameter data for ground processing.
struct GSE_CTLPARAMDATA_STRUCT
{
	int			  GeoPosSrc;				///< Geographic position source (0-2: GPS, TANS, Manual).
	bool		  CamTracking;			///< Flag indicating if camera has been commanded to track.
	bool		  UseCam0;				  ///< Flag indicating camera 0 is used for tracking.
  bool      UseCam1;          ///< Flag indicating camera 1 is used for tracking.
	bool		  AllowDgps;				///< Flag allowing/disallowing TANS in pointing control.
	double		StowPos;				///< Raw SAE reading of stow position.
	double		ElSaeOffset;			///< Offset angle of SAE elevation from true elevation (deg).
	double		ElIncOffset;			///< Offset angle of elevation inclinometer from true level (deg).
	double		ElSaeIncOffset;			///< Offset angle of SAE + inclinometer from true elevation. (deg)
	double		ElFggIntOffset;			///< Offset angle of fine-guidance gyro RIP elevation from true. (deg)
	double		ElRateFggOffset;		///< Offset of elevation fine-guidance gyro rate from true. (deg/s).
	double		AzDgpsOffset;			///< Offset angle of TANS azimuth from true azimuth (deg).
	double		AzMagOffset;			///< Offset angle of magnetometer azimuth from true azimuth (deg).
	double		AzFggIntOffset;			///< Offset angle of fine-guidance gyro RIP azimuth from true. (deg)
	double		AzRateFggOffset;		///< Offset of azimuth fine-guidance gyro rate from true. (deg/s).
	double		RollIncOffset;			///< Offset angle of roll inclinometer from true level (deg).
	double		RollFggIntOffset;		///< Offset angle of fine-guidance gyro RIP roll from true. (deg)
	double		RollRateFggOffset;		///< Offset of roll fine-guidance gyro rate from true. (deg/s).
	double		ImErrThresh;			///< ASP image position error threshold.
	double		WDPGain;				///< Wheel momentum-dump PID loop P gain.
	double		WDIGain;				///< Wheel momentum-dump PID loop I gain.
	double		WDDGain;				///< Wheel momentum-dump PID loop D gain.
	double		WDMaxOut;				///< Wheel momentum-dump PID loop maximum shaft voltage output.
	double		WDILimit;				///< Wheel momentum-dump PID loop I limit.
	double		WDMaxOmega;				///< Wheel momentum-dump PID loop maximum angular speed.
	int				Mode;					///< Current control mode for PID gains, etc. at end of packet.
	double		PGain;					///< P gain of current mode's PID loop.
	double		IGain;					///< I gain of current mode's PID loop.
	double		DGain;					///< D gain of current mode's PID loop.
	int				MaxOut;					///< Current mode's PID loop maximum output.
	double		ILimit;					///< Current mode's PID loop I limit.
	double		PBound;					///< Current mode's position (P) error bound.
	double		DBound;					///< Current mode's rate (D) error bound.
	double		SlewMaxRate;			///< Current mode's slew angular speed maximum.
	double		SlewAccel;				///< Current mode's slew acceleration.
  double    FggCutoff;      ///< FGG filter cutoff frequency.
  double    HrgCutoff;      ///< HRG filter cutoff frequency.
  double    AzRateHrgOffset;  ///< Offset of azimuth high-rate gyro rate from true (deg/s).
  int       AzCamNPoints;       ///< Number of points in Azimuth CamSensor.
  double    AzCamMaxAge;        ///< Maximum age of fit for Azimuth CamSensor.
  int       AzDgpsNPoints;      ///< Number of points in DgpsAzimuthSensor.
  double    AzDgpsMaxAge;       ///< Maximum age of fit for DgpsAzimuthSensor.
  bool      AzDgpsUseCam;       ///< Flag to use camera in DgpsAzimuthSensor.
  int       AzMagNPoints;       ///< Number of points in MagAzimuthSensor.
  double    AzMagMaxAge;        ///< Maximum age of fit for MagAzimuthSensor.
  bool      AzMagUseCam;        ///< Flag to use camera in MagAzimuthSensor.
  bool      AzMagUseDgps;       ///< Flag to use DPGS in MagAzimuthSensor.
  int       AzFggNPoints;       ///< Number of points in FggRipAzimuthSensor.
  double    AzFggMaxAge;        ///< Maximum age of fit for FggRipAzimuthSensor.
  bool      AzFggUseCam;        ///< Flag to use camera in FggRipAzimuthSensor.
  bool      AzFggUseDgps;       ///< Flag to use DGPS in FggRipAzimuthSensor.
  bool      AzFggUseMag;        ///< Flag to use magnetometer in FggRipAzimuthSensor.
  int       ElCamNPoints;       ///< Number of points in Elevation CamSensor.
  double    ElCamMaxAge;        ///< Maximum age of fit for Elevation CamSensor.
  int       ElSaeNPoints;       ///< Number of points in SaeIncElevationSensor.
  double    ElSaeMaxAge;        ///< Maximum age of fit for SaeIncElevationSensor.
  bool      ElSaeUseCam;        ///< Flag to use camera in SaeIncElevationSensor.
  int       ElFggNPoints;       ///< Number of points in FggRipElevationSensor.
  double    ElFggMaxAge;        ///< Maximum age of fit for FggRipElevationSensor.
  bool      ElFggUseCam;        ///< Flag to use camera in FggRipElevationSensor.
  bool      ElFggUseSae;        ///< Flag to use SAE+Inc in FggRipElevationSensor.
  double    AzFggRipSlope;
  double    AzFggRipOffset;
  double    ElFggRipSlope;
  double    ElFggRipOffset;
  float     GyroTweak[4][2];
};

#pragma pack( push, ambient )
#pragma pack( 1 )

//	GSE_CTLPARAMDATA2_STRUCT:
///		Structure to hold converted CTL system parameter data for ground processing.
struct GSE_CTLPARAMDATA2_STRUCT
{
	int			  GeoPosSrc;				///< Geographic position source (0-2: GPS, TANS, Manual).
	bool		  CamTracking;			///< Flag indicating if camera has been commanded to track.
	bool		  UseCam0;				  ///< Flag indicating camera 0 is used for tracking.
  bool      UseCam1;          ///< Flag indicating camera 1 is used for tracking.
	bool		  AllowDgps;				///< Flag allowing/disallowing TANS in pointing control.
	double		StowPos;				///< Raw SAE reading of stow position.
	double		ElSaeOffset;			///< Offset angle of SAE elevation from true elevation (deg).
	double		ElIncOffset;			///< Offset angle of elevation inclinometer from true level (deg).
	double		ElSaeIncOffset;			///< Offset angle of SAE + inclinometer from true elevation. (deg)
	double		ElFggIntOffset;			///< Offset angle of fine-guidance gyro RIP elevation from true. (deg)
	double		ElRateFggOffset;		///< Offset of elevation fine-guidance gyro rate from true. (deg/s).
	double		AzDgpsOffset;			///< Offset angle of TANS azimuth from true azimuth (deg).
	double		AzMagOffset;			///< Offset angle of magnetometer azimuth from true azimuth (deg).
	double		AzFggIntOffset;			///< Offset angle of fine-guidance gyro RIP azimuth from true. (deg)
	double		AzRateFggOffset;		///< Offset of azimuth fine-guidance gyro rate from true. (deg/s).
	double		RollIncOffset;			///< Offset angle of roll inclinometer from true level (deg).
	double		RollFggIntOffset;		///< Offset angle of fine-guidance gyro RIP roll from true. (deg)
	double		RollRateFggOffset;		///< Offset of roll fine-guidance gyro rate from true. (deg/s).
	double		ImErrThresh;			///< ASP image position error threshold.
	double		WDPGain;				///< Wheel momentum-dump PID loop P gain.
	double		WDIGain;				///< Wheel momentum-dump PID loop I gain.
	double		WDDGain;				///< Wheel momentum-dump PID loop D gain.
	double		WDMaxOut;				///< Wheel momentum-dump PID loop maximum shaft voltage output.
	double		WDILimit;				///< Wheel momentum-dump PID loop I limit.
	double		WDMaxOmega;				///< Wheel momentum-dump PID loop maximum angular speed.
	int				Mode;					///< Current control mode for PID gains, etc. at end of packet.
	double		PGain;					///< P gain of current mode's PID loop.
	double		IGain;					///< I gain of current mode's PID loop.
	double		DGain;					///< D gain of current mode's PID loop.
	int				MaxOut;					///< Current mode's PID loop maximum output.
	double		ILimit;					///< Current mode's PID loop I limit.
	double		PBound;					///< Current mode's position (P) error bound.
	double		DBound;					///< Current mode's rate (D) error bound.
	double		SlewMaxRate;			///< Current mode's slew angular speed maximum.
	double		SlewAccel;				///< Current mode's slew acceleration.
  double    FggCutoff;      ///< FGG filter cutoff frequency.
  double    HrgCutoff;      ///< HRG filter cutoff frequency.
  double    AzRateHrgOffset;  ///< Offset of azimuth high-rate gyro rate from true (deg/s).
  int       AzCamNPoints;       ///< Number of points in Azimuth CamSensor.
  double    AzCamMaxAge;        ///< Maximum age of fit for Azimuth CamSensor.
  int       AzDgpsNPoints;      ///< Number of points in DgpsAzimuthSensor.
  double    AzDgpsMaxAge;       ///< Maximum age of fit for DgpsAzimuthSensor.
  bool      AzDgpsUseCam;       ///< Flag to use camera in DgpsAzimuthSensor.
  int       AzMagNPoints;       ///< Number of points in MagAzimuthSensor.
  double    AzMagMaxAge;        ///< Maximum age of fit for MagAzimuthSensor.
  bool      AzMagUseCam;        ///< Flag to use camera in MagAzimuthSensor.
  bool      AzMagUseDgps;       ///< Flag to use DPGS in MagAzimuthSensor.
  int       AzFggNPoints;       ///< Number of points in FggRipAzimuthSensor.
  double    AzFggMaxAge;        ///< Maximum age of fit for FggRipAzimuthSensor.
  bool      AzFggUseCam;        ///< Flag to use camera in FggRipAzimuthSensor.
  bool      AzFggUseDgps;       ///< Flag to use DGPS in FggRipAzimuthSensor.
  bool      AzFggUseMag;        ///< Flag to use magnetometer in FggRipAzimuthSensor.
  int       ElCamNPoints;       ///< Number of points in Elevation CamSensor.
  double    ElCamMaxAge;        ///< Maximum age of fit for Elevation CamSensor.
  int       ElSaeNPoints;       ///< Number of points in SaeIncElevationSensor.
  double    ElSaeMaxAge;        ///< Maximum age of fit for SaeIncElevationSensor.
  bool      ElSaeUseCam;        ///< Flag to use camera in SaeIncElevationSensor.
  int       ElFggNPoints;       ///< Number of points in FggRipElevationSensor.
  double    ElFggMaxAge;        ///< Maximum age of fit for FggRipElevationSensor.
  bool      ElFggUseCam;        ///< Flag to use camera in FggRipElevationSensor.
  bool      ElFggUseSae;        ///< Flag to use SAE+Inc in FggRipElevationSensor.
  double    AzFggRipSlope;
  double    AzFggRipOffset;
  double    ElFggRipSlope;
  double    ElFggRipOffset;
  float     GyroTweak[4][2];
  float     ElEntryBound;
  uint16_t  ElEntryCount;
  float     AzEntryBound;
  uint16_t  AzEntryCount;
};

//	PKT_MOTGPSSATSTAT_STRUCT:
///		Structure to hold satellite status information from Motorola GPS in raw form.
struct PKT_MOTGPSSATSTAT_STRUCT
{
	uint8_t	Id;							///< Satellite identifier.
	uint8_t	TrkMode;					///< Channel tracking mode.
	uint8_t	Snr;						///< Carrier to noise density ratio (dB-Hz).
	uint8_t	StatFlag;					///< Channel status flag.
};

//	PKT_MOTGPSPSDDATA_STRUCT:
///		Structure to hold position/status/data packet from Motorola GPS in raw form.
struct PKT_MOTGPSPSDDATA_STRUCT
{
	uint16_t	Header;						///< Header (equal to "@@" characters for all valid packets).
	uint16_t	Type;						///< Type (equal "Ea" for PSD packet).
	uint8_t	Month;						///< Month field.
	uint8_t	Day;						///< Day of the month.
	uint16_t	Year;						///< Year field.
	uint8_t	Hour;						///< Hour of the day.
	uint8_t	Minute;						///< Minutes after the hour.
	uint8_t	Second;						///< Seconds after the minute.
	uint32_t	NanoSec;					///< Nano seconds after the second.
	int32_t	Latitude;					///< Latitude in milli-arcseconds.
	int32_t	Longitude;					///< Longitude in milli-arcseconds.
	int32_t	Height;						///< Ellipsoidal height in cm.
	uint32_t	Unused;						///< Unused value.
	uint16_t	Velocity;					///< Velocity in cm/sec.
	uint16_t	Heading;					///< Heading in 0.1 degree units.
	uint16_t	Dop;						///< Degree of precision.
	uint8_t	DopType;					///< Flag bits to indicate mode and DOP type.
	uint8_t	SatVis;						///< Number of satellites visible.
	uint8_t	SatTrk;						///< Number of satellites tracked (0-8).
	PKT_MOTGPSSATSTAT_STRUCT SatStat[8];	///< Satellite status structures (see above).
	uint8_t	RcvrStatus;					///< Receiver status flags.
	uint8_t	CheckSum;					///< GPS Checksum.
};
#pragma pack( pop, ambient )

//	GSE_MOTGPSPSDDATA_STRUCT:
///		Structure to hold Motorola GPS position/status/data information in user form.
struct GSE_MOTGPSPSDDATA_STRUCT
{
	uint8_t	Month;						///< Month field.
	uint8_t	Day;						///< Day of the month.
	uint16_t	Year;						///< Year field.
	uint8_t	Hour;						///< Hour of the day.
	uint8_t	Minute;						///< Minutes after the hour.
	double	Second;						///< Seconds after the minute.
	double	Latitude;					///< Latitude in degrees.
	double	Longitude;					///< Longitude in degrees.
	double	Height;						///< Ellipsoidal height in meters.
	float	Velocity;					///< Velocity in m/sec.
	float	Heading;					///< Heading in degrees..
	uint16_t	Dop;						///< Degree of precision.
	uint8_t	DopType;					///< Flag bits to indicate mode and DOP type.
	uint8_t	SatVis;						///< Number of satellites visible.
	uint8_t	SatTrk;						///< Number of satellites tracked (0-8).
	PKT_MOTGPSSATSTAT_STRUCT SatStat[8];	///< Satellite status structures (see above).
	uint8_t	RcvrStatus;					///< Receiver status flags.
};

#pragma pack( push, ambient )
#pragma pack( 1 )

//	PKT_M12GPSSATSTAT_STRUCT:
///		Structure to hold satellite status information from Motorola M12 GPS in raw form.
struct PKT_M12GPSSATSTAT_STRUCT
{
	uint8_t	Id;							///< Satellite identifier.
	uint8_t	TrkMode;					///< Channel tracking mode.
	uint8_t	Snr;						///< Carrier to noise density ratio (dB-Hz).
	uint8_t	Iode;						///< Channel IODE.
	uint16_t	StatFlag;					///< Channel status flag.
};

//	PKT_M12GPSPSDDATA_STRUCT:
///		Structure to hold position/status/data packet from Motorola M12 GPS in raw form.
struct PKT_M12GPSPSDDATA_STRUCT
{
	uint16_t	Header;						///< Header (equal to "@@" characters for all valid packets).
	uint16_t	Type;						///< Type (equal "Ha" for PSD packet).
	uint8_t	Month;						///< Month field.
	uint8_t	Day;						///< Day of the month.
	uint16_t	Year;						///< Year field.
	uint8_t	Hour;						///< Hour of the day.
	uint8_t	Minute;						///< Minutes after the hour.
	uint8_t	Second;						///< Seconds after the minute.
	uint32_t	NanoSec;					///< Nano seconds after the second.
	int32_t	Latitude;					///< Latitude in milli-arcseconds.
	int32_t	Longitude;					///< Longitude in milli-arcseconds.
	int32_t	Height;						///< Ellipsoidal height in cm.
	int32_t	MslHeight;					///< MSL height in cm.
	int32_t	UfLatitude;					///< Unfiltered Latitude in milli-arcseconds.
	int32_t	UfLongitude;				///< Unfiltered Longitude in milli-arcseconds.
	int32_t	UfHeight;					///< Unfiltered Ellipsoidal height in cm.
	int32_t	UfMslHeight;				///< Unfiltered MSL height in cm.
	uint16_t	Velocity3D;					///< 3D Velocity in cm/sec.
	uint16_t	Velocity2D;					///< 2D Velocity in cm/sec.
	uint16_t	Heading;					///< Heading in 0.1 degree units.
	uint16_t	Dop;						///< Degree of precision.
	uint8_t	SatVis;						///< Number of satellites visible.
	uint8_t	SatTrk;						///< Number of satellites tracked (0-8).
	PKT_M12GPSSATSTAT_STRUCT SatStat[12];	///< Satellite status structures (see above).
	uint16_t	RcvrStatus;					///< Receiver status flags.
	uint16_t	Reserved;					///< Reserved word.
	int16_t	ClkBias;					///< Clock bias in nanoseconds.
	uint32_t	OscOffset;					///< Oscillator offset in Hz.
	int16_t	OscTemp;					///< Oscillator temp in 1/2 degree C increments.
	uint8_t	UtcParam;					///< Time mode/UTC parameter (flags).
	char	GmtOffsetSign;				///< Signed byte of GMT offset.
	uint8_t	GmtOffsetHour;				///< Hour offset from GMT.
	uint8_t	GmtOffsetMinute;			///< Minutes offset from GMT.
	char	IdTag[6];					///< 6-character ASCII ID tag.
	uint8_t	CheckSum;					///< GPS Checksum.
};
#pragma pack( pop, ambient )

//	GSE_M12GPSPSDDATA_STRUCT:
///		Structure to hold Motorola M12 format GPS position/status/data information in user form.
struct GSE_M12GPSPSDDATA_STRUCT
{
	uint8_t	Month;						///< Month field.
	uint8_t	Day;						///< Day of the month.
	uint16_t	Year;						///< Year field.
	uint8_t	Hour;						///< Hour of the day.
	uint8_t	Minute;						///< Minutes after the hour.
	double	Second;						///< Seconds after the minute.
	double	Latitude;					///< Latitude in degrees.
	double	Longitude;					///< Longitude in degrees.
	double	Height;						///< Ellipsoidal height in meters.
	float	Velocity;					///< Velocity in m/sec.
	float	Heading;					///< Heading in degrees..
	uint16_t	Dop;						///< Degree of precision.
	uint8_t	SatVis;						///< Number of satellites visible.
	uint8_t	SatTrk;						///< Number of satellites tracked (0-8).
	PKT_M12GPSSATSTAT_STRUCT SatStat[12];	///< Satellite status structures (see above).
	uint16_t	RcvrStatus;					///< Receiver status flags.
};

// PPSSTATUS:
/// Used to define the values or PKT_PPSSYNCDATA_STRUCT.state

enum PPSSTATUS {
	PPS_OK = 0,				// Everything is OK and we are measuring drift.
	PPS_NO_GPS_TIME,		// No GPS time available to correct offset.
	PPS_SET_TIME,			// Corrected the offset by setting the clock to the correct time.
	PPS_OFFSET_TOO_LARGE,	// Offset too large to automatically correct.
	PPS_COMP_DRIFT,			// Compensate for the drift of the SYSTEM clock.
};

enum NTPSTATUS {
	NTP_NSET,	/* freq not set */
	NTP_FSET,	/* freq set */
	NTP_SPIK, 	/* spike detect */
	NTP_FREQ,	/* freq mode */
	NTP_SYNC,	/* clock sync */
};

// PKT_PPSSYNCDATA_STRUCT
/// Structure to hold PPS Sync information in the packet.
#pragma pack( push, ambient )
#pragma pack( 1 )
struct PKT_PPSSYNCDATA_STRUCT
{
	int32_t	pps_sec;	///< PPS Timestamp : Seconds
	int32_t	pps_nsec;	///< PPS Timestamp : Nanoseconds
	int32_t	gps_sec;	///< GPS Timestamp : Seconds
	int32_t	gps_nsec;	///< GPS Timestamp : Nanoseconds
	int32_t	rtc_sec;	///< RTC Timestamp : Seconds
	double	offset;		///< Exponential Average of the Phase difference between GPS and SYSTEM clocks (secs)
	double	drift;		///< Exponential Average of the Period difference between GPS and SYSTEM clocks (secs)
	double	offset2;	///< Exponential Average of the square of the Phase difference between GPS and SYSTEM clocks (secs^2)
	uint8_t	actionTaken;///< Action Taken by FDR Time Handler
	uint8_t ntp_status; ///< Current state of the NTP clock discipline
	uint8_t spare[2];	///< Spare bytes reserved for later use.
};
#pragma pack( pop, ambient )

// PKT_PPSSYNC2DATA_STRUCT
/// Structure to hold PPS Sync information in the packet.
#pragma pack( push, ambient )
#pragma pack( 1 )
struct PKT_PPSSYNC2DATA_STRUCT
{
	int32_t	pps_sec;	///< PPS Timestamp : Seconds
	int32_t	pps_nsec;	///< PPS Timestamp : Nanoseconds
	int32_t	gps_sec;	///< GPS Timestamp : Seconds
	int32_t	gps_nsec;	///< GPS Timestamp : Nanoseconds
	int32_t mono_sec;	///< Monotonic Timestamp : Seconds
	int32_t mono_nsec;	///< Monotonic Timestamp : Nanoseconds
	int32_t	rtc_sec;	///< RTC Timestamp : Seconds
	int32_t	counter;	///< Current Timehandler Counter value
	double	fit_slope;	///< Slope of the linear fit of GPS - PPS (drift)
	double	fit_offset;	///< Offset of the linear fit of GPS - PPS
	double	fit_phase;	///< Phase difference calculated from linear fit
	uint8_t	status;		///< Status of the FDR Time Handler
	uint8_t ntp_status; ///< Current state of the NTP clock discipline
};
#pragma pack( pop, ambient )

//	PKT_ASPSYSDATA_STRUCT:
///		Structure to hold ASP system data in the packet.
#pragma pack( push, ambient )
#pragma pack( 1 )

/// Structure to hold system data for a single camera.
struct PKT_CAMSYSDATA_STRUCT
{
  int16_t   focusPos;
  int16_t   pressure;
  int16_t   sensorTemp;
  int16_t   airTemp;
  int16_t   ccdTemp;
  int16_t   coolerPower;
  uint16_t  catSize;
  uint16_t  focusStepPos;
  uint8_t   status;
  uint8_t   activity;
};

/// Structure to hold entire system data packet.
struct PKT_ASPSYSDATA_STRUCT
{
	uint32_t	blocksFree;				///< Space remaining on "/" filesystem (blocks).
	uint32_t	blocksAvail;			///< Blocks available to non-system user.
	uint32_t	blockSize;					///< Size of blocks on disk (bytes).
	uint8_t		RelayBits;					///< ASP relay states.
	uint8_t		StatusBits;					///< ASP system status bits.
	int16_t		CpuPressure;				///< CPU tank pressure raw ADC value.
	int16_t		CpuTankTemp;				///< CPU tank temperature raw ADC value.
	int16_t		CpuSpare[2];				///< CPU spare raw ADC value.
  PKT_CAMSYSDATA_STRUCT camData[2]; ///< Camera-specific data (see above).
};
#pragma pack( pop, ambient )

//	USR_ASPCAMDATA_STRUCT
///		Structure to hold camera-specific data for GSE and user use.
struct USR_ASPCAMDATA_STRUCT
{
	int		  Mode;						///< Track/focus/expose loop mode.
	int		  Activity;				///< Current activity in mode.
	bool	  FocusAdj;				///< Flag indicating if focus is being adjusted (TRUE) or not.
	bool	  SavingImages;		///< Flag indicating if images are being saved (TRUE) or not.
	bool	  MotorMoving;		///< Flag indicating if motor is moving (TRUE) or not.
	bool	  Connected;  		///< Flag indicating if camera is connected (TRUE) or not.
	int		  FocusStepPos;		///< Focus motor position step count (relative position).
	int		  CatSize;				///< Number of objects in current target catalog.
	double	SensorTemp;	  	///< Camera tank rear temperature raw ADC value.
	double	AirTemp;    		///< Camera tank rear temperature raw ADC value.
	double	Pressure;   		///< Camera/Lens tank pressure raw ADC value.
	double	FocusPos;				///< Measured camera motor position.
  double  CcdTemp;        ///< Camera CCD temperature.
  double  CoolerPower;    ///< Fraction of total cooler power.
};

//	USR_ASPSYSDATA_STRUCT:
///		Structure to hold ASP system data for GSE use.
struct USR_ASPSYSDATA_STRUCT
{
	uint32_t	blocksFree;				///< Space remaining on "/" filesystem (blocks).
	uint32_t	blocksAvail;			///< Blocks available to non-system user.
	uint32_t	blockSize;				///< Size of a disk block (bytes)
	int			RelayBits;					///< Relay bit states.
	int			GeoPosSrc;					///< Current geographic position source.
	double	CpuPressure;				///< CPU tank pressure (millibar).
	double	CpuTankTemp;				///< CPU tank temperature.
	double	CpuTemp;						///< CPU temperature (C).
	USR_ASPCAMDATA_STRUCT	CamData[2];	///< Camera data for each camera.
};

//	PKT_ASPPARAMDATA_STRUCT:
///		Structure to hold ASP parameter data in the packet.
#pragma pack( push, ambient )
#pragma pack( 1 )
struct PKT_ASPPARAMDATA_STRUCT
{
	uint8_t		Mode;						///< Camera tracking mode.
	uint8_t		CamID;					///< Identifies which camera's parameters are being sent.
	uint8_t		Flags;					///< Flag bits.
	uint8_t	  ExtendedFlags;		///< Extended flags.
	uint16_t	CamExposure;		///< Camera image exposure time in milliseconds.
	uint16_t	CamSetPoint;		///< Camera cooler set point (Celsius + 273.0 * 10)
	uint16_t	CamXCenter;			///< Dynamical x-axis center pixel of images.
	uint16_t	CamYCenter;			///< Dynamical y-axis center pixel of images.
	float			CamImScale;			///< Image scale in deg/pixel.
	float			CamEPerCount;		///< Number of electrons per CCD count.
	float			CamImRotAngle;	///< Angle to rotate images to get Az/Alt aligned with X/Y.
	uint8_t		ImTxMode;				///< Value indicating whether and how to transmit next image.
	uint8_t		TrkMinID;				///< Minimum number of identified objects for a valid aspect solution.
	uint8_t		TrkFilterWidth;	///< Background model running filter (avg) width in pixels.
	uint8_t		TrkMinRadius;		///< Minimum object radius in pixels.
	uint16_t	TrkMaxErr;			///< Maximum catalog - image separation error (* 1000 pixels).
	uint16_t	TrkPixThresh;		///< Minimum significance level of a valid object pixel (* 100 sigma).
	uint16_t	TrkObjThresh;		///< Minimum significance level of a valid object (* 100 sigma).
	float			CatTargetRA;		///< Right ascension of current target.
	float			CatTargetDec;		///< Declination of current target.
	uint16_t	CatRadius;			///< Target catalog radius (* 1000 degrees).
	uint8_t		CatVMin;				///< Minimum visual magnitude of catalog field.
	uint8_t		CatVMax;				///< Maximum visual magnitude of catalog field.
	uint16_t	CatNObj;				///< Number of objects in target catalog.
	float			Longitude;			///< Currently set longitude (deg).
	float			Latitude;				///< Currently set latitude (deg).
	uint16_t	FocusMotorRate;	///< Motor rate in Hz.
	int16_t		FocusMin;				///< Minimum focus position (in steps).
	int16_t		FocusMax;				///< Maximum focus position (in steps).
	uint8_t		FocusBacklash;	///< Minimum number of steps to remove backlash.
	uint8_t		FocusNSteps;		///< Number of loops in focus algorithm.
	uint16_t	FocusStepSize;	///< Number of steps per between focus images for each loop.
	float			CamOffset;			///< Offset of camera from line of sight (degrees).
	float			OffsetZPA;			///< Position angle (relative to Zenith) of offset (degrees).
	uint8_t		UsbEnum;				///< Enumerated USB ID of camera to conneted to
	uint8_t		FocusFlags;			///< Type of focus figure used in custom focus mode.
	uint16_t	CeCircXCenter;	///< Corner exclusion circle x-axis center pixel.
	uint16_t	CeCircYCenter;	///< Corner exclusion circle y-axis center pixel.
	uint16_t	CeCircRadius;		///< Corner exclusion circle radius.
	uint16_t	LeftEdgeOffset;	///< Number of pixels to exclude on left edge of image.
	uint16_t	RightEdgeOffset;///< Number of pixels to exclude on right edge of image.
	uint16_t	BottomEdgeOffset;	///< Number of pixels to exclude on bottom edge of image.
	uint16_t	TopEdgeOffset;	///< Number of pixels to exclude on top edge of image.
	uint16_t	BiasCount;			///< Bias level in pixel counts.
	uint16_t	BiasSigma;			///< Bias standard deviation in pixel counts.
	uint16_t	ImageXSize;			///< Number of horizontal pixels in camera image.
	uint16_t	ImageYSize;			///< Number of vertical pixels in camera image.
};
#pragma pack( pop, ambient )

//	USR_ASPPARAMDATA_STRUCT:
///		Structure to hold the ASP parameters in user units.
struct USR_ASPPARAMDATA_STRUCT
{
	/// Processing control parameters.
	uint8_t		Mode;						///< x Track thread operating mode.
	bool			SaveIm;					///< Flag for saving images to disk (true) or not.

	/// Camera control parameters.
	int				CamID;					///< x Identifies which camera's parameters are being received.
	bool			OffsetCam;			///< x Flag indicating if this is the offset camera.
	bool			CamXFlip;				///< x Flag indicating if image X-axis must be flipped (TRUE) to transform to Alt/Az.
	bool			CamYFlip;				///< x Flag indicating if image Y-axis must be flipped (TRUE) to transform to Alt/Az.
	bool			CamCoolerOn;		///< x Flag indicating if camera cooler is on (TRUE).
	bool			CamConnect;			///< x Flag indicating if camera is connected (TRUE).
	float			CamExposure;		///< x Camera exposure time in seconds.
	float			CamSetPoint;		///< x Camera cooler set point.
	uint8_t		UsbEnum;				///< x Enumerated USB ID of camera to conneted to.
	uint8_t		ImTxMode;				///< x Value indicating whether and how to transmit next image.
	bool			UseCenterQuad;	///< Flag determining if only central quadrant of camera is used (TRUE).

	/// Focus control parameters
	int				FocusMotorRate;	///< Motor rate in Hz.
	int				FocusMin;				///< Minimum focus position (in steps).
	int				FocusMax;				///< Maximum focus position (in steps).
	int				FocusBacklash;	///< Minimum number of steps to remove backlash.
	int				FocusNSteps;		///< Number of loops in focus algorithm.
	int				FocusStepSize;	///< Number of steps per between focus images for each loop.
	int				FocusFfigType;	///< Type of focus figure used in custom focus mode.
	int				MotorDmmPort;		///< x Port number for motor step and direction bits.
	int				MotorStepBit;		///< x Bits number of step bit.
	int				MotorDirBit;		///< x Bit number of motor direction bit.

	/// Image processing parameters.
	uint16_t	CamXCenter;			///< x Dynamical x-axis center pixel of images.
	uint16_t	CamYCenter;			///< x Dynamical y-axis center pixel of images.
	float			CamImScale;			///< x Image scale in deg/pixel.
	float			CamEPerCount;		///< x Number of electrons per CCD count.
	float			CamImRotAngle;	///< x Angle to rotate images to get Az/Alt aligned with X/Y.
	bool			TrkFlatFlag;		///< x Flag indicating if background model is flat (TRUE).
	uint32_t	TrkMinID;				///< x Object tracking minimum # of objects for aspect solution.
	float			TrkMaxError;		///< x Maximum allowed discrepancy between image and catalog distances (pixels).
	int				TrkFilterWidth;	///< x Running filter object detection method filter width in pixels.
	int				TrkMinRadius;		///< x Object detection minimum object radius.
	float			TrkPixThresh;		///< x Object detection minimum valid pixel amplitude in std. dev. above bgd.
	float			TrkObjThresh;		///< x Object detection minimum valid object amplitude in std. dev. above bgd.
	uint16_t	CeCircXCenter;	///< x Corner exclusion circle x-axis center pixel.
	uint16_t	CeCircYCenter;	///< x Corner exclusion circle y-axis center pixel.
	uint16_t	CeCircRadius;		///< x Corner exclusion circle radius.
	bool			CeCircEnable;		///< x Flag enabling corner exclusion circle use.
	uint32_t	LeftEdgeOffset;	///< Number of pixels to exclude on left edge of image.
	uint32_t	RightEdgeOffset;///< Number of pixels to exclude on right edge of image.
	uint32_t	BottomEdgeOffset;	///< Number of pixels to exclude on bottom edge of image.
	uint32_t	TopEdgeOffset;	///< Number of pixels to exclude on top edge of image.
	double		BiasCount;			///< x Bias level in pixel counts.
	double		BiasSigma;			///< x Bias standard deviation in pixel counts.
	int				ImageXSize;			///< x Number of horizontal pixels in camera image.
	int				ImageYSize;			///< x Number of vertical pixels in camera image.

	/// Target/catalog parameters.
	float			CatTargetRA;		///< x Right ascension of current target.
	float			CatTargetDec;		///< x Declination of current target.
	float			CatRadius;			///< x Radius of catalog target field in degrees.
	float			CatVMin;				///< x Minimum V magnitude of target catalog field.
	float			CatVMax;				///< x Minimum V magnitude of target catalog field.
	int				CatNObj;				///< x Number of objects in target catalog.

	/// Geographic positioning parameters.
	int				GeoPosSrc;			///< Preferred geographical position source.
	float			Longitude;			///< x Current longitude in degrees.
	float			Latitude;				///< x Current latitude in degrees.

	// Offset camera parameters.
	float			CamOffset;			///< x Offset of camera from line of sight (degrees).
	float			OffsetZPA;			///< x Position angle (relative to Zenith) of offset (degrees).
};

#pragma pack( push, ambient )
#pragma pack( 1 )
/// Packet reporting the current status of the onboard cameras
struct PKT_ASPCAMSTATUS_STRUCT
{
	uint8_t		CamID;					///< Camera ID.
	uint8_t		StatusBits;			///< Connect and cooler on status bits.
	int16_t		CcdTemp;				///< Camera CCD temperature.
	uint16_t	CoolPower;			///< Camera cooler power level (%).
};

#pragma pack( pop, ambient )
/// Structure used by the GSE to report the current status of onboard cameras
struct USR_ASPCAMSTATUS_STRUCT
{
	int				CamID;					///< Camera ID.
	bool			CoolerOn;				///< Flag indicating if cooler is on.
	bool			Connected;			///< Connect and cooler on status bits.
	int				CoolingStatus;	///< Cooling status.
	double		CcdTemp;				///< Camera CCD temperature.
	double		CoolPower;				///< Camera cooler power level (%).
};

#pragma pack( push, ambient )
#pragma pack( 1 )
/// Packet sent whenever an onboard camera changes operating mode
struct PKT_ASPMODECHANGE_STRUCT
{
	uint8_t		CamID;				///< Camera ID.
	uint8_t		oldMode;			///< Connect and cooler on status bits.
	uint8_t		newMode;			///< Camera CCD temperature.
	uint8_t		modeStatus;		///< Camera cooler power level (%).
	uint8_t		modeActivity;	///< Activity occuring when update occured.
};

#pragma pack( pop, ambient )
/// Structure used by the GSE to report that an onboard camera has changed operating mode
struct USR_ASPMODECHANGE_STRUCT
{
	int		CamID;				///< Camera ID.
	int		oldMode;			///< Connect and cooler on status bits.
	int		newMode;			///< Camera CCD temperature.
	int		modeStatus;		///< Camera cooler power level (%).
	int		modeActivity;	///< Activity occuring when update occured.
};

#pragma pack( push, ambient )
#pragma pack( 1 )

#define ASPIM_MAXNOBJ 49

//	PKT_ASPIMOBJ_STRUCT:
///		Bitfield structure to hold object data in the packet.
struct PKT_ASPIMOBJ_STRUCT
{
	uint16_t	X;				///< X-coordinate of object position.
	uint16_t	Y;				///< Y-coordinate of object position.
	uint16_t	ID;				///< Count of image in catalog (or zero, if unidentified).
	uint16_t	Count;			///< Total - background CCD counts (counts = exp( Count/4000 );
};

//	PKT_ASPIMDATA_STRUCT:
///		Structure to hold ASP image data in the packet.
struct PKT_ASPIMDATA_STRUCT
{
	uint8_t		CamID;				///< Identifies source camera.
	float		TargetRA;			///< Right ascension of target.
	float		TargetDec;			///< Declination of target.
	float		TargetAz;			///< Azimuth of target.
	float		TargetAlt;			///< Altitude of target.
	float		XOffset;			///< X axis offset of target from center of image.
	float		YOffset;			///< Y axis offset of target from center of image.
	float		CenterRA;			///< Right ascension of center of image.
	float		CenterDec;			///< Declination of center of image.
	float		SigmaRA;			///< Standard deviation of CenterRA.
	float		SigmaDec;			///< Standard deviation of CenterDec;
	float		CenterAz;			///< Azimuth of center of image.
	float		CenterAlt;			///< Altitude of center of image.
	float		CenterRoll;			///< Roll angle of center of image from Az/Alt coordinates.
	uint16_t		PixMin;				///< Minimum value of all pixels in image.
	uint16_t		PixMax;				///< Maximum value of all pixels in image.
	uint16_t		PixMean;			///< Mean value of all pixels in image.
	uint16_t		PixSigma;			///< Standard deviation of all pixels in image * 10.
	uint16_t		BgdMean;			///< Mean of valid background pixels.
	uint16_t		BgdSigma;			///< Standard deviation of valid background pixels * 10.
	uint16_t		NObjFound;			///< Number of objects found in image.
	uint16_t		NObjID;				///< Number of objects identified from image.
	uint16_t		CenterX;			///< Dynamical center of image x-coordinates.
	uint16_t		CenterY;			///< Dynamical center of image y-coordinate.
};

#pragma pack( pop, ambient )

//	USR_ASPIMOBJ_STRUCT:
///		Structure to hold object data for users.
struct USR_ASPIMOBJ_STRUCT
{
	uint16_t	X;
	uint16_t	Y;
	uint16_t	ID;
	double		Count;
	double		Size;
};

//	USR_ASPIMDATA_STRUCT:
///		Structure to hold converted ASP image data for users.
struct USR_ASPIMDATA_STRUCT
{
	int			CamID;				///< Identifies source camera.
	float		TargetRA;			///< Right ascension of target.
	float		TargetDec;			///< Declination of target.
	float		TargetAz;			///< Azimuth of target.
	float		TargetAlt;			///< Altitude of target.
	float		XOffset;			///< X axis offset of target from center of image.
	float		YOffset;			///< Y axis offset of target from center of image.
	float		CenterRA;			///< Right ascension of center of image.
	float		CenterDec;			///< Declination of center of image.
	float		SigmaRA;			///< Standard deviation of CenterRA.
	float		SigmaDec;			///< Standard deviation of CenterDec;
	float		CenterAz;			///< Azimuth of center of image.
	float		CenterAlt;			///< Altitude of center of image.
	float		CenterRoll;			///< Roll angle of center of image from Az/Alt coordinates.
	uint16_t		PixMin;				///< Minimum value of all pixels in image.
	uint16_t		PixMax;				///< Maximum value of all pixels in image.
	uint16_t		PixMean;			///< Mean value of all pixels in image.
	float		PixSigma;			///< Standard deviation of all pixels in image.
	uint16_t		BgdMean;			///< Mean of valid background pixels.
	float		BgdSigma;			///< Standard deviation of valid background pixels.
	uint16_t		NObjFound;			///< Number of objects found in image.
	uint16_t		NObjID;				///< Number of objects identified from image.
	uint16_t		CenterX;			///< Dynamical center of image x-coordinates.
	uint16_t		CenterY;			///< Dynamical center of image y-coordinate.
	USR_ASPIMOBJ_STRUCT	ObjList[ASPIM_MAXNOBJ];		///< List of object positions, ID's and total Counts.
};

#pragma pack( push, ambient )
#pragma pack( 1 )

#define ASPIM_MAXNFPOINTS 11

//	PKT_ASPFPOINT_STRUCT:
///		Structure to hold each focus point data in the packet.
struct PKT_ASPFPOINT_STRUCT
{
	uint16_t	Position;			///< Stepper motor position when image was taken (steps).
	float			FocusFig;			///< Focus figure calculated from image.
};

//	PKT_ASPFOCUSDATA_STRUCT:
///		Structure to hold ASP focus data in the packet.
struct PKT_ASPFOCUSDATA_STRUCT
{
	uint8_t		CamID;			///< Identifies source camera.
	uint8_t		NPoints;		///< Number of valid points in the following array.
	uint16_t	Centroid;		///< Centroid calculated for the focus operation.
	PKT_ASPFPOINT_STRUCT	Point[ASPIM_MAXNFPOINTS];	///< Array of position and focus figures for this loop.
};

#pragma pack( pop, ambient )

//	USR_ASPFOCUSDATA_STRUCT:
///		Structure to hold ASP focus data in user format.
struct USR_ASPFOCUSDATA_STRUCT
{
	int				CamID;				///< Identifies source camera.
	int				Centroid;			///< Index of focus loop (indicates step size).
	int				NPoints;			///< Number of valid points in the following array.
	double		Position[ASPIM_MAXNFPOINTS];	///< Array of motor steps positions for this loop.
	double		FocusFig[ASPIM_MAXNFPOINTS];	///< Array of focus figures for this loop.
};

#pragma pack( push, ambient )
#pragma pack( 1 )

/// Structure to hold EHK system data in packet.
struct PKT_EHKSYSDATA_STRUCT
{
  short   inampTemp;      ///< INAMP MUX temperature.
  short   muxTemp;        ///< Temperature MUX temperature.
  short   balMotorTemp;   ///< Balance motor temperture.
  short   balBearingTemp; ///< Balance system bearing temperature.
  short   obRearTopTemp;  ///< Optical bench rear top temp.
  short   obRearPortTemp; ///< Optical bench rear port side temp.
  short   tankTemp;       ///< EHK tank temperature.
  short   tankPress;      ///< EHK tank presure.
  short   spare[6];       ///< Spare measurements.
  uint16_t relayImage;    ///< Latest relay output image.
  int16_t motorPos;       ///< Balance motor position.
  uint8_t tempID;         ///< Temperature MUX address ID.
  uint8_t inampID;        ///< INAMP MUX address ID.
  uint8_t dioInput;       ///< Digital input byte.
};

//	PKT_EHKHTRPARAMDATA_STRUCT:
///		Structure to hold EHK heater control parameter data in packet.
struct PKT_EHKHTRPARAMDATA_STRUCT
{
  int16_t tCtlLThresh[8];   ///< Thermal control lower threshold temperatures.
  int16_t tCtlUThresh[8];   ///< Thermal control upper threshold temperatures.
  uint8_t tCtlState[8];     ///< Thermal control state.
};

//  PKT_EHKBALMOTORPARAM_STRUCT
///   Structure to hold EHK balance motor parameter data in packet.
struct PKT_EHKBALMOTORPARAMDATA_STRUCT
{
  uint16_t  stepRateHz;     ///< Step rate in Hertz.
  int16_t   minPos;         ///< Minimum allowed position.
  int16_t   maxPos;         ///< Maximum allowed position.
  int16_t   homePos;        ///< Home stop position.
  int16_t   midPos;         ///< Midpoint stop position.
  int16_t   endPos;         ///< End stop position.
  uint8_t   resetBits;      ///< Combined "reset on trigger" enable bits.
};

#pragma pack( pop, ambient )

///		Structure to hold EHK system data in user-friendly form.
struct USR_EHKSYSDATA_STRUCT
{
  double  inampTemp;      ///< INAMP MUX temperature.
  double  muxTemp;        ///< Temperature MUX temperature.
  double  balMotorTemp;   ///< Balance motor temperture.
  double  balBearingTemp; ///< Balance system bearing temperature.
  double  obRearTopTemp;  ///< Optical bench rear top temp.
  double  obRearPortTemp; ///< Optical bench rear port side temp.
  double  tankTemp;       ///< EHK tank temperature.
  double  tankPress;      ///< EHK tank presure.
  double  spare[6];       ///< Spare measurements.
  int     tempID;         ///< Temperature MUX address ID.
  int     inampID;        ///< INAMP MUX address ID.
  int     dioInput;       ///< Digital input byte.
  int     relayImage;     ///< Latest relay output image.
  int     motorPosition;  ///< Balance motor position.
  bool    motorMoving;    ///< Flag to indicate motor is moving.
};

//	PKT_EHKHTRPARAMDATA_STRUCT:
///		Structure to hold EHK heater control parameter data in user-friendly form.
struct USR_EHKHTRPARAMDATA_STRUCT
{
  float   tCtlLThresh[8];   ///< Thermal control lower threshold temperatures.
  float   tCtlUThresh[8];   ///< Thermal control upper threshold temperatures.
  int     tCtlState[8];     ///< Thermal control state.
};

//  USR_EHKBALMOTORPARAM_STRUCT
///   Structure to hold EHK balance motor parameter data in user-friendly form.
struct USR_EHKBALMOTORPARAMDATA_STRUCT
{
  int     stepRateHz;     ///< Step rate in Hertz.
  int     minPos;         ///< Minimum allowed position.
  int     maxPos;         ///< Maximum allowed position.
  int     homePos;        ///< Home stop position.
  int     midPos;         ///< Midpoint stop position.
  int     endPos;         ///< End stop position.
  bool    homeReset;      ///< Home position reset flag.
  bool    midReset;       ///< Midpoint position reset flag.
  bool    endReset;       ///< End position reset flag.
};

// Enumerated EHK gyros.
enum EHK_GYRO_INDEX
{
	EGYRO_EL1 = 0,						///< Elevation gyro 1.
	EGYRO_ELYAW,						///< Elevation yaw gyro.
	EGYRO_EL2,							///< Elevation gyro 2.
	EGYRO_ELROLL						///< Elevation roll gyro.
};

// Enumerated EHK axes.
enum EHK_AXIS_INDEX
{
	EHKAXIS_EL = 0,						///< Elevation axis.
	EHKAXIS_AZ,							///< Azimuth axis.
	EHKAXIS_ROLL,						///< Roll axis.
};

//	PKT_FDRSYSDATA_STRUCT:
///		Structure to hold the FDR system data in the packet.
#pragma pack( push, ambient )
#pragma pack( 1 )
struct PKT_FDRSYSDATA_STRUCT
{
	uint16_t		RawRxRate[NUM_HERO_DETECTORS];		///< Raw events received from each detector.
	uint16_t		RawTxRate;			///< Raw events transmitted to ground.
	uint16_t		TotalTxRate;		///< Number of bytes sent to ground.
	uint32_t		TotalRxRate;		///< Number of bytes received (all packets, all systems).
	uint32_t		KbFreePrimary;		///< Free disk space remaining on primary volume.
	uint32_t		KbFreeSecondary;	///< Free disk space remaining on secondary volume.
	short			FdrTemp;			///< Raw ADC reading of FDR tank temperature.
	short			FdrPress;			///< Raw ADC reading of FDR tank pressure.
	short			DcDcTemp;			///< Temp of DC/DC converter
	short			SpareV1;			///< Spare ADC value.
	short			BatteryV;			///< FDR battery voltage reading.
	uint32_t		GseRxBytes;			///< Number of bytes read from GSE stream input.
	uint16_t		GseRxPkts;			///< Number of packets read from GSE stream input.
	uint16_t		GseRxErrors;		///< Number of RX errors on GSE stream input.
	uint16_t		GseRxLastError;		///< Last RX error on GSE stream input.
	uint32_t		GseTxBytes;			///< Number of bytes written to GSE stream output.
	uint16_t		GseTxPkts;			///< Number of packets written to GSE stream output.
	uint16_t		GseTxErrors;		///< Number of TX errors on GSE stream output.
	uint16_t		GseTxLastError;		///< Last TX error on GSE stream output.
	uint32_t		ImRxBytes;			///< Number of bytes read from image stream input.
	uint16_t		ImRxPkts;			///< Number of packets read from image stream input.
	uint16_t		ImRxImPkts;			///< Number of image packets read from image stream input.
	uint16_t		ImRxQPkts;			///< Number of image packets written to queue.
	uint16_t		ImTxQPkts;			///< Number of image packets read from queue.
	uint16_t		ImTxPkts;			///< Number of image packets written to the ouput stream.
	uint32_t		ImTxBytes;			///< Number of bytes written to the output stream.
	uint16_t		ImTxErrors;			///< Number of image output stream errors.
	uint16_t		ImTxLastError;		///< ID of last image output error.
	uint16_t    	DioState;       	///< DIO state bits read from Uio48 device.
	uint32_t		MaxFileSize;		///< Number of raw events per file stored on FDR.
	uint8_t			RawTxDet;			///< Detector from which raw events are being transmitted to ground.
};
#pragma pack( pop, ambient )

//	PKT_FDRDETFILEDATA_STRUCT:
#pragma pack( push, ambient )
#pragma pack( 1 )

/// Used to hold the file data for each detector being recorded.
struct DETFILEDATA_STRUCT
{
	uint32_t		CreationTime;		///< Time the file was created
	uint32_t		CurrentSize;		///< Current Size of the File
};

///		Structure to hold the FDR detector file data in the packet.
struct PKT_FDRDETFILEDATA_STRUCT
{
	uint32_t		MaxFileSize;		///< Max size of each detector file in bytes
	DETFILEDATA_STRUCT	detFileData[8];	///< Array of detector data. see DETFILEDATA_STRUCT
};

#pragma pack( pop, ambient )

//	USR_FDRSYSDATA_STRUCT:
///		Structure to hold converted FDR system data in user units.
struct USR_FDRSYSDATA_STRUCT
{
	uint32_t		RawRxRate[NUM_HERO_DETECTORS];		///< Raw events received from each detector.
	uint32_t		RawTxRate;			///< Raw events transmitted to ground.
	uint32_t		TotalTxRate;		///< Number of bytes sent to ground.
	uint32_t		TotalRxRate;		///< Number of bytes received (all packets, all systems).
	double			KbFreePrimary;			///< Free disk space remaining on FDR C: drive.
	double			KbFreeSecondary;			///< Free disk space remaining on FDR C: drive.
	float			FdrTemp;			///< Raw ADC reading of FDR tank temperature.
	float			FdrPress;			///< Raw ADC reading of FDR tank pressure.
	float			DcDcTemp;			///< Temp of DC/DC Converter
	float			SpareV1;			///< Spare ADC value.
	float			BatteryV;			///< FDR battery voltage reading.
	uint32_t		GseRxBytes;			///< Number of bytes read from GSE stream input.
	uint32_t		GseRxPkts;			///< Number of packets read from GSE stream input.
	uint32_t		GseRxErrors;		///< Number of RX errors on GSE stream input.
	uint32_t		GseRxLastError;		///< Last RX error on GSE stream input.
	uint32_t		GseTxBytes;			///< Number of bytes written to GSE stream output.
	uint32_t		GseTxPkts;			///< Number of packets written to GSE stream output.
	uint32_t		GseTxErrors;		///< Number of TX errors on GSE stream output.
	uint32_t		GseTxLastError;		///< Last TX error on GSE stream output.
	uint32_t		ImRxBytes;			///< Number of bytes read from image stream input.
	uint32_t		ImRxPkts;			///< Number of packets read from image stream input.
	uint32_t		ImRxImPkts;			///< Number of image packets read from image stream input.
	uint32_t		ImRxQPkts;			///< Number of image packets written to queue.
	uint32_t		ImTxQPkts;			///< Number of image packets read from queue.
	uint32_t		ImTxPkts;			///< Number of image packets written to the ouput stream.
	uint32_t		ImTxBytes;			///< Number of bytes written to the output stream.
	uint32_t		ImTxErrors;			///< Number of image output stream errors.
	uint32_t		ImTxLastError;		///< ID of last image output error.
	uint16_t    	DioState; ///< Dio state bits from Uio48 device.
	uint32_t		MaxFileSize;		///< Number of bytes per file stored on FDR.
	uint8_t			RawTxDet;			///< Detector from which raw events are being transmitted to ground.
	bool		StowArm;			///< Stow pin arm status.
	bool		StowMove;			///< Stow pin moving status.
	bool		StowOpen;			///< Stow pin latch open state.
	bool		ObaAtStow;		///< Optical bench in stow position.
};

////	PKT_FDRPARAMDATA_STRUCT:
/////		Structure to hold the FDR parameter data in the packet.
//#pragma pack( push, ambient )
//#pragma pack( 1 )
//struct PKT_FDRPARAMDATA_STRUCT
//{
//	uint8_t		ProcFlags;			///< Bits 0-7 detector processing flags,.
//	uint8_t		RawTxDet;			///< Detector from which raw events are being transmitted to ground.
//	uint16_t		RawNPerFile;		///< Number of raw events per file stored on FDR.
//	uint8_t		ParamDet;			///< ID of detector to which the rest of the parameters apply.
//	uint8_t		ChanStep;			///< Channel processing step width (channels).
//	uint16_t		ChanThresh;			///< Channel processing threshold (ADC counts).
//	float		XOffset;			///< Model detector center offset (mm).
//	float		YOffset;			///< Model detector center offset (mm).
//	float		PCoeff[3];			///< Position fit binomial coefficients.
//	float		ECoeff[3];			///< Energy fit binomial coefficients.
//};
//#pragma pack( pop, ambient )
//
////	USR_FDRPARAMDATA_STRUCT:
/////		Structure to hold converted FDR parameter data in user units.
//struct USR_FDRPARAMDATA_STRUCT
//{
//	bool		ProcFlag[NUM_HERO_DETECTORS];		///< Detector processing flags.
//	int			RawTxDet;			///< Detector from which raw events are being transmitted to ground.
//	bool		RawTxFlag;			///< Flag indicating if any raw data is transmitted to ground.
//	int			RawNPerFile;		///< Number of raw events per file stored on FDR.
//	int			ParamDet;			///< ID of detector to which the rest of the parameters apply.
//	uint16_t		ChanStep;			///< Channel processing step width (channels).
//	uint16_t		ChanThresh;			///< Channel processing threshold (ADC counts).
//	float		XOffset;			///< Model detector center offset (mm).
//	float		YOffset;			///< Model detector center offset (mm).
//	float		PCoeff[3];			///< Position fit binomial coefficients.
//	float		ECoeff[3];			///< Energy fit binomial coefficients.
//};

#pragma pack( push, ambient )
#pragma pack( 1 )
//	PKT_DETSTATUS_STRUCT:
///		Structure to hold a single detectors' status data.
struct PKT_DETSTATUS_STRUCT
{
  uint32_t  ppsCount;         ///< FPGA pulse-per-second count since system start.
  uint16_t  isrCount;         ///< # calls to the detector event data ISR since last report.
  uint16_t  readCount;        ///< # events read from the FPGA since last report.
  uint16_t  hdrErrCount;      ///< # events read from the FPGA with bad header.
  uint16_t  writeCount;       ///< # events written to event queue since last report.
  uint16_t  triggerCount;     ///< FPGA Trigger count since last report.
  uint16_t  swTriggerCount;   ///< FPGA Software-forced trigger count since last report.
  uint16_t  calTriggerCount;  ///< FPGA Calibration trigger count since last report.
  uint16_t  ultVetoCount;     ///< FPGA Upper-level threshold veto count since last report.
  uint16_t  calVetoCount;     ///< FPGA Calibration assertion veto count since last report.
	int16_t	  pmtVReadback; 		///< Voltage read from PMT (volts)
	int16_t   driftVReadback;		///< Voltage read from drift section (volts).
	int16_t   gridVReadback; 		///< Voltage read from grid (volts).
};

//  PKT_QSTATUS_STRUCT
/// Structure to hold the detector queue handler's status data.
struct PKT_QSTATUS_STRUCT
{
  uint16_t  readCount;        ///< # events read from the event queue since last report.
  uint16_t  writeCount;       ///< # events sent to FDR since last report.
};

//	PKT_DPSSTATUSDATA_STRUCT:
///		Structure to hold DPS status data in the packet.
struct PKT_DPSSTATUSDATA_STRUCT
{
	PKT_DETSTATUS_STRUCT	status[2];	///< Status data for each detector.
  PKT_QSTATUS_STRUCT    qStatus[2]; ///< Status data for each detector.
};
#pragma pack( pop, ambient )

#pragma pack( push, ambient )
#pragma pack( 1 )

///		Structure to hold a single detector's parameter data in the packet.
struct PKT_DETPARAM_STRUCT
{
	uint16_t	lThreshMv;        ///< Lower threshold millivolts.
	uint16_t	uThreshMv;        ///< Upper threshold millivolts.
	uint16_t	pmtV;             ///< PMT volts.
	uint16_t	driftV;           ///< Drift volts.
	uint16_t	gridV;            ///< Grid volts.
  uint16_t  eventCtl;         ///< Event control settings.
  uint16_t  fifoEventThresh;  ///< FIFO event threshold setting.
  uint16_t  trigHoldoff;      ///< Trigger hold off setting.
  uint16_t  fifoLowThresh;    ///< FIFO lower threshold setting.
  uint16_t  fpgaVMonthDay;    ///< Month and day of FPGA version.
  uint16_t  fpgaVYearVer;     ///< Year and version number of FPGA version.
  uint16_t  flags;            ///< Various processing flags.
};

#pragma pack( pop, ambient )

///		Structure to hold converted DPS parameter data.
struct USR_DETPARAM_STRUCT
{
	float     lThreshV;         ///< Lower threshold volts.
	float     uThreshV;         ///< Upper threshold volts.
	int     	pmtV;             ///< PMT volts.
	int     	driftV;           ///< Drift volts.
	int     	gridV;            ///< Grid volts.
  int       fpgaVYear;        ///< Year of FPGA version.
  int       fpgaVMonth;       ///< Month of FPGA version.
  int       fpgaVDay;         ///< Day of the month of FPGA version.
  int       fpgaVNum;         ///< FPGA version number for the given date.
  bool      eventEnable;      ///< Events enable flag.
  bool      vetoEnable;       ///< Veto enable flag.
  bool      intEnable;        ///< Interrupt enable flag.
  bool      calEnable;        ///< Calibration input signal enable flag.
  bool      swTrigEnable;     ///< Software trigger enable flag.
  uint16_t  fifoEventThresh;  ///< Fifo event threshold setting.
  uint16_t  trigHoldoff;      ///< Trigger holdoff in microseconds.
  uint16_t  fifoLowThresh;    ///< FIFO lower threshold setting.
  bool      swTrigEnabled;    ///< Software triggering is enabled.

};

#pragma pack( push, ambient )
#pragma pack( 1 )

///		Structure to hold DPS temperature and pressure data in the packet.
struct PKT_DPSTPDATA_STRUCT
{
	uint16_t	  dpsTemp;			  ///< DPS tank temperature raw ADC.
	uint16_t		dpsRegTemp;			///< DPS +5V regulator temperature raw ADC.
	uint16_t		dpsPressure;		///< DPS tank pressure raw ADC.
	uint16_t		dpsSpareV;			///< DPS tank spare analog input.
	uint16_t		detTemp0;			  ///< Detector 0 tank temperature raw ADC.
	uint16_t		detAuxTemp0;		///< Detector 0 auxilliary temperature raw ADC.
	uint16_t		detPressure0;		///< Detector 0 tank pressure raw ADC.
	uint16_t		detTemp1;			  ///< Detector 1 tank temperature raw ADC.
	uint16_t		detAuxTemp1;		///< Detector 1 auxilliary temperature raw ADC.
	uint16_t		detPressure1;		///< Detector 1 tank pressure raw ADC.
};

#pragma pack( pop, ambient )

///		Structure to hold converted DPS temperature and pressure data.
struct USR_DPSTPDATA_STRUCT
{
	double		dpsTemp;			  ///< DPS tank temperature raw ADC.
	double		dpsRegTemp;			///< DPS +5V regulator temperature raw ADC.
	double		dpsPressure;		///< DPS tank pressure raw ADC.
	double		dpsSpareV;			///< DPS tank spare analog input.
	double		detTemp0;			  ///< Detector 0 tank temperature raw ADC.
	double		detAuxTemp0;		///< Detector 0 auxilliary temperature raw ADC.
	double		detPressure0;		///< Detector 0 tank pressure raw ADC.
	double		detTemp1;			  ///< Detector 1 tank temperature raw ADC.
	double		detAuxTemp1;		///< Detector 1 auxilliary temperature raw ADC.
	double		detPressure1;		///< Detector 1 tank pressure raw ADC.
};

#pragma pack( push, ambient )
#pragma pack( 1 )
/// The HERO processed event data structure.
struct DET_EVENT_DATA
{
	uint8_t   detector;			///< Indicates which detector the event is from.
	uint8_t 	nEvent;				///< Number of subevents in this event.
	uint16_t	flags;				///< Processing flags and error bits.
	uint16_t	energy[2];		///< Total energy of event (KeV).
	uint16_t	rise[2];			///< Rise time (in channels) of each sub-event;
	uint32_t	time;					///< Event timetag.
	float	    x[2];					///< X-axis position (mm).
	float	    y[2];					///< Y-axis position (mm).
};

//	PKT_DPSCLKSYNC_STRUCT:
///		Structure to hold the extra clock/time information from the DPS modules.
struct PKT_DPSCLKSYNC_STRUCT
{
	uint32_t	detTime[2];					///< Current event timer count from each detector.
};

#pragma pack( pop, ambient )

#pragma pack( push, ambient )
#pragma pack( 1 )

//	PKT_CLKSYNC_STRUCT:
///		Structure to hold clock synchronization data in the packet.
struct PKT_CLKSYNC_STRUCT
{
	int		TimeDiff;					///< Difference between local clock and master clock (millisec).
	uint16_t	ThreshReset;				///< Bits 0-14: Reset threshold.  Bit 15: Local clock reset flag.
};

#pragma pack( pop, ambient )

//	GSE_CLKSYNC_STRUCT:
///		Structure to hold clock synchronization data for GSE use.
struct GSE_CLKSYNC_STRUCT
{
	double	TimeDiff;					///< Difference between local clock and master clock (sec).
	double	ResetThresh;				///< Local clock reset threshold (sec).
	bool	Reset;						///< Flag indicating if local clock was reset.
};

//	GSE_CMDACK_STRUCT:
///		Structure to hold command acknowledgement data in the packet and for GSE use.
#pragma pack( push, ambient )
#pragma pack( 1 )
struct GSE_CMDACK_STRUCT
{
	uint16_t		SeqNum;					///< Sequence number of packet containing command.
	uint16_t		ID;						///< ID of command (command token).
	uint16_t		Result;					///< Error (or non-error) result returned from parsing or executing.
};
#pragma pack( pop, ambient )

//	GSE_DATA_STRUCT:
///		Structure to hold various types of converted packet data.
struct GSE_DATA_STRUCT
{
	int		NInt;						///< Number of integer items in the structure.
	int		NDouble;					///< Number of double precision items in the structure.
	int		IArray[EXIST_MAX_DATA];		///< Array of integer data items.
	double	DArray[EXIST_MAX_DATA];		///< Array of double precision data items.
};


#pragma pack( push, ambient )
#pragma pack( 1 )
//	PKT_GSEGATE_DATA_STRUCT:
/// Structure to hold the GseGateway statistical data.
struct PKT_GSEGATE_DATA_STRUCT {

	// Inbound HK stats
	uint32_t	RxRate;				///< Input rate in bytes/sec.
	uint32_t	RxBytes;			///< Total number of bytes received.
	uint32_t	RxValidPackets;		///< Total number of valid packets received.
	uint32_t	RxIdlePackets;		///< Total number of Idle packets received.
	uint32_t	RxPacketErrors;		///< Total number of packet errors generated from input.
	uint32_t	RxErrors;			///< Total number of receive errors.
	uint32_t	RxLastError;		///< Last receive error.
	uint32_t	TxBytes;			///< Total number of transmitted bytes.
	uint32_t	TxPackets;			///< Total number of transmitted packets.
	uint32_t	TxErrors;			///< Total number of transmit errors.
	uint32_t	TxLastError;		///< Last transmission error.
	uint32_t	FileBytes;			///< Number of bytes written to the current file.
	uint32_t	FileLimit;			///< Maximum number of bytes per file.
	uint32_t	FileErrors;			///< Number of file I/O errors.
	uint32_t	FileLastError;		///< Last file I/O error.
	char		FileName[128];		///< Name of current file.

};
#pragma pack( pop, ambient )

#pragma pack( push, ambient )
#pragma pack( 1 )
//	PKT_CIPGATE_DATA_STRUCT:
/// Structure to hold statistical CIP Gateway data.
struct PKT_CIPGATE_DATA_STRUCT
{
	uint16_t	RxBytes;			///< Total input bytes.
	uint16_t	RxPackets;			///< Total input raw packets.
	uint16_t	RxErrors;			///< Total input errors.
	uint16_t	RxLastError;		///< Last input error value.
	uint32_t	RxLastAddress;		///< Address of host that sent last command.
	uint16_t	PktValid;			///< Total input valid packets.
	uint16_t	PktErrors;			///< Total input packet errors.
	uint16_t	PktLastError;		///< Last input packet error.
	uint16_t	TxBytes;			///< Total output bytes.
	uint16_t	TxPackets;			///< Total output packets.
	uint16_t	TxErrors;			///< Total output errors.
	uint16_t	TxLastError;		///< Last output error.
	uint8_t		CommandPath;		///< CIP command path (0 = CIP, 1 = Net)
};
#pragma pack( pop, ambient )

#pragma pack( push, ambient )
#pragma pack( 1 )
/// Packet sent by GseGatewaySci that reports HDLC driver stats to the SciGateway GUI.
struct PKT_HDLC_DATA_STRUCT
{
	uint32_t	RxShort;			///< Number of received HDLC short frames
	uint32_t	RxLong;				///< Number of received HDLC larger than 4096 bytes (discarded)
	uint32_t	RxAbort;			///< Number of received HDLC abort sequences detected
	uint32_t	RxOver;				///< Number of received HDLC termination due to overruns
	uint32_t	RxCrc;				///< Number of HDLC frames discarded from CRC errors
	uint32_t	RxOk;				///< Number of HDLC frames received OK
	uint32_t	ExitHunt;			///< Number of times the receiver exited hunt mode
	uint32_t	RxIdle;				///< Number of times the receiver detected an idle sequence
};
#pragma pack( pop, ambient)

#pragma pack( push, ambient )
#pragma pack( 1 )

/// Packet sent by NetworkTimeClient objects to update time status.
struct PKT_NTCDATA_STRUCT
{
  double    clientTime;   ///< Raw time (seconds) on client.
  double    serverTime;   ///< Corresponding time (seconds) on server.
  double    offsetTime;   ///< Offset corrected time (seconds) on client.
  double    linearTime;   ///< Linear corrected time (seconds) on client.
  double    grossOffset;  ///< Averaged offset of raw client time from server time.
  double    netOffset;    ///< Averaged offset of offset-correct client time from server time.
  double    linearSlope;  ///< Latest linear fit slope.
  double    linearOffset; ///< Latest linear fit offset.
  double    linearError;  ///< Offset of latest time difference from latest linear model.
  uint8_t   actionTaken;  ///< Bits indicating actions taken.
  uint16_t  offsetCount;  ///< Count from offset average function.
  uint16_t  linearCount;  ///< Count from linear fit function.
};

#pragma pack( pop, ambient )


#endif //HEROGSEPACKETSTRUCT_H
