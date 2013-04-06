//	CipCom.h:
//		C/C++ header file defining the values and meanings of the CIP commands.

#pragma once

// Define the CIP command types, # of data words follows in parentheses.
enum HERO_CIP_CMD {
	// Commands for general use.
	CIP_STOW_IN = 1,			///< Send stow pin in. (0)
	CIP_STOW_OUT,				///< Send stow pin out. (0)
	CIP_GPSTIME_ON,				///< Use GPS as timing source. (0)
	CIP_GPSTIME_OFF,			///< Do not use GPS as timing source. (0)
	CIP_RELAY_SETBITS,			///< Sets relay bits (up to 16 bits) on any machine (1).
	CIP_GEOSRC_FDRGPS,				///< Sets the FDR GPS as the source of the geographic position. (0)
  CIP_GEOSRC_CTLGPS,    ///< Sets the CTL GPS as the source of geographic position (0).
	CIP_GEOSRC_DGPS,			///< Sets the DGPS as the source of the geographic position. (0)
	CIP_GEOSRC_MANUAL,			///< Sets manual update as the source of the geographic position. (0)
	CIP_GEOPOS_GPS,				///< Sets the FDR GPS geographic position. (4)
	CIP_GEOPOS_DGPS,			///< Sets the DGPS geographic position. (4)
	CIP_GEOPOS_MANUAL,			///< Sets the geographic position manually. (4)
	CIP_SAVE_DEFAULTS,			///< Saves current system parameters as defaults. (0)
	CIP_LOAD_DEFAULTS,			///< Loads saved system default parameters (abandons changes). (0)
	CIP_FORCE_TIMESET,			///< Causes the time handler to set the time to GPS time regardless of offset once. (0)
	CIP_GEOPOS_CTLGPS,     ///< Sets the CTL GPS position.

	// CTL-only general commands.
	CIP_RESET_DGPS = 0x0100,	///< Sends command to reset the DGPS softly. (0)
	CIP_GPS_RESET,				///< Issue software reset of GPS system. (0)
	CIP_MAG_SETFWD,				///< Set magnetometer packet forwarding states (1).
	CIP_MAG_RAWCMD,				///< Send a raw magnetometer command packet. (<80).

	// CTL-only pointing commands.
	CIP_NO_POINT = 0x0110,		///< Stop rough and fine aspect control loop, set motors to zero voltage. (0).
	CIP_SAFE_POINT,				///< Start safe-mode pointing at current Alt-Az position. (0)
	CIP_STOW_POINT,				///< Use safe-mode pointing to hold current Az, go to Alt stow. (0)
	CIP_STATIC_POINT,			///< Using rough aspect, acquire a given alt-az position (2).
	CIP_INERTIAL_POINT,			///< Acquire a given RA & Dec and track via gyros. (4)
	CIP_CELESTIAL_POINT,		///< Acquire a given RA & Dec and track via gyros and images. (4)

	// CTL-only direct motor control (NO_POINT mode) commands.
	CIP_WHEEL_VOLTAGE = 0x0120,	///< Sets the Azimuth wheel motor voltage. (1)
	CIP_SHAFT_VOLTAGE,			///< Sets the Azimuth shaft motor voltage. (1)
	CIP_ELEVATION_VOLTAGE,		///< Sets the elevation motor voltage. (1)
	CIP_BEARING_VOLTAGE,		///< Sets the bearing motor voltage. (1)

	// CTL-only aspect sensor manual offset commands.
	CIP_CTL_SAEOFFSET=0x0130,	///< Sets True-measured offset for Elevation - SAE. (2)
	CIP_CTL_EINCOFFSET,			///< Sets True-measured offset for Elevation Inclination - inclinometer. (2)
	CIP_CTL_SAEINCOFFSET,		///< Sets True-measured offset for Elevation - SAE + Inc + Offsets. (2)
	CIP_CTL_EHKINCOFFSET,		///< Sets True-measured offset for Elevation Inclination - EHK inclinometer. (2)
	CIP_CTL_DGPSOFFSET,			///< Sets True-measured offset for Azimuth - DGPS Az. (2)
	CIP_CTL_MAGOFFSET,			///< Sets True-measured offset for Azimuth - Mag Az. (2)
	CIP_CTL_ROLLINCOFFSET,		///< Sets true-measured offset for Roll - Roll inclinometer. (2)
	CIP_CTL_ELFGGOFFSET,		///< Sets True-measured offset for elevation rate - fine guidance gyro. (2)
	CIP_CTL_ELHRGOFFSET,		///< Sets True-measured offset for elevation rate - high-rate gyro. (2)
	CIP_CTL_AZFGGOFFSET,		///< Sets True-measured offset for azimuth rate - fine guidance gyro. (2)
	CIP_CTL_AZHRGOFFSET,		///< Sets True-measured offset for azimuth rate - high-rate gyro. (2)
	CIP_CTL_ROLLFGGOFFSET,		///< Sets True-measured offset for roll rate - fine guidance gyro. (2)
	CIP_CTL_ROLLHRGOFFSET,		///< Sets True-measured offset for roll rate - high-rate gyro. (2)
	CIP_CTL_CUR2STOW,			///< Sets current SAE reading as the target stow position. (0)
	CIP_CTL_STOWPOS,			///< Sets the raw SAE reading of the stow position. (2)

	// CTL-only aspect sensor auto-trim commands.
	CIP_CTL_ELTRIM=0x0140,		///< Sets the auto-trim flags for all elevation sensors. (1)
	CIP_CTL_AZTRIM,				///< Sets the auto-trim flags for all azimuth sensors. (1)
	CIP_CTL_ROLLTRIM,			///< Sets the auto-trim flags for all elevation sensors. (1)
	CIP_CTL_DGPSINVALID,		///< Invalidates the DGPS data to protect against undetected bad data. (0)
	CIP_CTL_DGPSVALID,			///< Allows DGPS sensor to detect good and bad data. (0)
	CIP_CTL_FGGINVALID,			///< Invalidates data from fine-guidance gyros. (0)
	CIP_CTL_FGGVALID,			///< Allows system to determine if fine-guidance gyro data is good. (0)
	CIP_CTL_HRGCUTOFF,			///< Sets the high-rate gyro digital filter cutoff frequency. (4)
  CIP_CTL_FGGCUTOFF,    ///< Sets the fine-guidance gyro digital filter cutoff frequency. (4)

	// CTL-only aspect update commands.
	CIP_CTL_SETIMERRTHRESH=0x0150,///< Sets the image position-fit error threshold. (1)
	CIP_CTL_SETIMASP,			///< Sets the el, az, roll, error and time from the latest aspect image. (20)
	CIP_GYRO_UPDATE,			///< Packet from EHK with latest gyro measurements (6).
	CIP_CTL_ASPACKTRKON,		///< Acknowledges that ASP has received start tracking command.
	CIP_CTL_ASPACKTRKOFF,		///< Acknowledges that ASP has received stop tracking command.
	CIP_CTL_ASPACKTRKERR,		///< Acknowledges that ASP encountered and error starting or stopping track loop.

	// CTL-only control loop parameter commands.
	CIP_SET_PIDPARAM = 0x0160,	///< Set PID parameters for a given control mode. (8)
	CIP_SET_MODEBOUNDS,			///< Set the boundaries for the given mode (5).
	CIP_SET_SLEWPARAM,			///< Set slew parameters for a given control mode. (3)
	CIP_CTL_WHEELDUMPPARM,		///< Sets the wheel momentum-dump control parameters. (7)
	CIP_CTL_DACSELECTPARAM,		///< Sets the motor DAC selection parameters. (1)

  // CTL-only sensor trim parameters.
  CIP_CTL_SETAZCAMSENPARAM=0x170, ///< Sets the paramters for Azimuth CameraSensor. (2)
  CIP_CTL_SETAZDGPSSENPARAM,      ///< Sets the parameters for the DgpsAzimuthSensor. (2)
  CIP_CTL_SETAZMAGSENPARAM,       ///< Sets the parameters for the MagAzimuthSensor. (2)
  CIP_CTL_SETAZFGGSENPARAM,       ///< Sets the parameters for the FggRipAzimuthSensor. (2)
  CIP_CTL_SETELCAMSENPARAM,       ///< Sets the parameters for the Elevation CamSensor. (2)
  CIP_CTL_SETELSAESENPARAM,       ///< Sets the parameters for the SaeIncElevationSensor. (2)
  CIP_CTL_SETELFGGSENPARAM,       ///< Sets the parameters for the FggRipElevationSensor. (2)

  // CTL-only gyro tweak parameter commands.
  CIP_CTL_SETGTAZXPARAM=0x180,    ///< Sets the AZ X gyro tweak parameters.
  CIP_CTL_SETGTAZYPARAM,          ///< Sets the AX Y gyro tweak parameters.
  CIP_CTL_SETGTELXPARAM,          ///< Sets the EL X gyro tweak parameters.
  CIP_CTL_SETGTELYPARAM,          ///< Sets the EL Y gyro tweak parameters.

	// ASP-only camera/tracking/focus parameters (Primary camera).
	CIP_ASP_SETEXP0 = 0x0200,	///< Set the exposure time. (1)
	CIP_ASP_SETIMCENTER0,		///< Sets the defined image center for tracking. (2)
	CIP_ASP_SETCAMPARAM0,		///< Sets the image scale, electrons per count, orientation and rotation angle.
	CIP_ASP_SETCATPARAM0,		///< Sets catalog region parameters (radius, vmin, vmax). (2)
	CIP_ASP_SETTRKTHRESH0,		///< Sets the image object pixel thresh, obj thresh. (2)
	CIP_ASP_SETIMMAXERR0,		///< Sets the maximum image - catalog separation tolerance (in pixels). (1).
	CIP_ASP_SETTRKMINID0,		///< Sets the minimum number of ID'ed objected needed for an aspect solution. (1)
	CIP_ASP_SETTRKWIDTHS0,		///< Sets the minimum object radius and running filter width in pixels. (2)
	CIP_ASP_SETFINDFLAT0,		///< Set the tracking algorithm to assume a flat background. (0)
	CIP_ASP_SETFINDFILTER0,		///< Set the tracking algorithm to use a running filter. (0)
	CIP_ASP_SETFOCPARAM0,		///< Sets the focus parameters (bounds, backlash, nLoop, steps). (8)
	CIP_ASP_SETFOCRATE0,			///< Sets the focus motor rate (steps/sec). (1)
	CIP_ASP_SETCAMOFFSET0,		///< Sets the camera offset from line of sight, and zenith-position angle. (4)
	CIP_ASP_SETCAM2ZERO0,		///< Sets camera 0 to USB enumerated 0 camera. (0)
	CIP_ASP_SETCECIRCPARAM0,	///< Sets daylight corner exclusion parameters. (3)

	// ASP-only camera/tracking/focus parameters (Secondary camera).
	CIP_ASP_SETEXP1 = 0x0210,	///< Set the exposure time. (1)
	CIP_ASP_SETIMCENTER1,		///< Sets the defined image center for tracking. (2)
	CIP_ASP_SETCAMPARAM1,		///< Sets the image scale, electrons per count, orientation and rotation angle.
	CIP_ASP_SETCATPARAM1,		///< Sets catalog region parameters (radius, vmin, vmax). (2)
	CIP_ASP_SETTRKTHRESH1,		///< Sets the image object pixel thresh, obj thresh. (2)
	CIP_ASP_SETIMMAXERR1,		///< Sets the maximum image - catalog separation tolerance (in pixels). (1).
	CIP_ASP_SETTRKMINID1,		///< Sets the minimum number of ID'ed objected needed for an aspect solution. (1)
	CIP_ASP_SETTRKWIDTHS1,		///< Sets the minimum object radius and running filter width in pixels. (2)
	CIP_ASP_SETFINDFLAT1,		///< Set the tracking algorithm to assume a flat background. (0)
	CIP_ASP_SETFINDFILTER1,		///< Set the tracking algorithm to use a running filter. (0)
	CIP_ASP_SETFOCPARAM1,		///< Sets the focus parameters (bounds, backlash, nLoop, steps). (8)
	CIP_ASP_SETFOCRATE1,		///< Sets the focus motor rate (steps/sec). (1)
	CIP_ASP_SETCAMOFFSET1,		///< Sets the camera offset from line of sight, and zenith-position angle. (4)
	CIP_ASP_SETCAM2ZERO1,		///< Sets camera 1 to USB enumerated 0 camera. (0)
	CIP_ASP_SETCECIRCPARAM1,	///< Sets daylight corner exclusion parameters. (3)

	// ASP-only camera control commands (Primary camera).
	CIP_ASP_SETCOOLERON0=0x0220,///< Turns on the camera cooler. (0)
	CIP_ASP_SETCOOLEROFF0,		///< Turns off the camera cooler. (0)
	CIP_ASP_SETCOOLERSP0,		///< Sets the camera cooler setpoint. (1)
	CIP_ASP_CAMCONNECT0,		///< Command to connect to USB camera. (0)
	CIP_ASP_CAMDISCONNECT0,		///< Command to disconnect from USB camera. (0)
	CIP_ASP_SAVEIMAGEON0,		///< Turns on the saving of images to the hard disk. (0)
	CIP_ASP_SAVEIMAGEOFF0,		///< Turns off the saving of images to the hard disk. (0)
	CIP_ASP_TXIMFULL0,			///< Transmit next full raw image to ground. (0)
	CIP_ASP_TXIMCENTER0,		///< Transmit central quadrant of next raw image to ground. (0)
	CIP_ASP_CECIRCON0,			///< Enables use of corner-exclusion circle. (0)
	CIP_ASP_CECIRCOFF0,			///< Disables use of corner-exclusion circle. (0)

	// ASP-only camera control commands (Secondary camera).
	CIP_ASP_SETCOOLERON1=0x0230,///< Turns on the camera cooler. (0)
	CIP_ASP_SETCOOLEROFF1,		///< Turns off the camera cooler. (0)
	CIP_ASP_SETCOOLERSP1,		///< Sets the camera cooler setpoint. (1)
	CIP_ASP_CAMCONNECT1,		///< Command to connect to USB camera. (0)
	CIP_ASP_CAMDISCONNECT1,		///< Command to disconnect from USB camera. (0)
	CIP_ASP_SAVEIMAGEON1,		///< Turns on the saving of images to the hard disk. (0)
	CIP_ASP_SAVEIMAGEOFF1,		///< Turns off the saving of images to the hard disk. (0)
	CIP_ASP_TXIMFULL1,			///< Transmit next full raw image to ground. (0)
	CIP_ASP_TXIMCENTER1,		///< Transmit central quadrant of next raw image to ground. (0)
	CIP_ASP_CECIRCON1,			///< Enables use of corner-exclusion circle. (0)
	CIP_ASP_CECIRCOFF1,			///< Disables use of corner-exclusion circle. (0)

	// ASP-only track/focus mode commands (Primary camera).
	CIP_ASP_STOPCAM0 = 0x0240,	///< Aborts any focusing or expose/track loop. (0)
	CIP_ASP_SINGLEEXP0,			///< Takes and tracks one exposure. (0)
	CIP_ASP_STARTTRACK0,		///< Starts expose/track loop. (0)
	CIP_ASP_STOPTRACK0,			///< Stops expose/track loop. (0)
	CIP_ASP_SETFOCUSPOS0,		///< Set the current focus position. (2)
	CIP_ASP_ADJFOCUS0,			///< Starts find focus procedure from home position. (1)
	CIP_ASP_STOPFOCUS0,			///< Aborts the focus procedure. (0)
	CIP_TEST_EXPOSURE0,			///< Use tracking algorithm on test exposure stored on disk. (0)
	CIP_MAKE_DARKIMAGE0,		///< Expose camera without opening shutter. (0)
	CIP_ASP_SETAUTOFOCUSPOS0,

	// ASP-only track/focus mode commands (Secondary camera).
	CIP_ASP_STOPCAM1 = 0x0250,	///< Aborts any focusing or expose/track loop. (0)
	CIP_ASP_SINGLEEXP1,			///< Takes and tracks one exposure. (0)
	CIP_ASP_STARTTRACK1,		///< Starts expose/track loop. (0)
	CIP_ASP_STOPTRACK1,			///< Stops expose/track loop. (0)
	CIP_ASP_SETFOCUSPOS1,		///< Set the current focus position. (2)
	CIP_ASP_ADJFOCUS1,			///< Starts find focus procedure from home position. (1)
	CIP_ASP_STOPFOCUS1,			///< Aborts the focus procedure. (0)
	CIP_TEST_EXPOSURE1,			///< Use tracking algorithm on test exposure stored on disk. (0)
	CIP_MAKE_DARKIMAGE1,		///< Expose camera without opening shutter. (0)
	CIP_ASP_SETAUTOFOCUSPOS1,

	// ASP-only camera/tracking/focus additional parameters (Primary camera).
	CIP_ASP_SETEDGEOFFS0 = 0x0260,	///< Set the image edge exclusion parameters. (4)
	CIP_ASP_SETBIASPARAM0,		///< Set dark bias parameters. (2)
	CIP_ASP_SETIMSIZE0,			///< Sets the default camera image dimensions. (2)
	CIP_ASP_SETCAMQUAD0,		///< Sets camera to use only central quadrant of CCD. (0)
	CIP_ASP_SETCAMFULL0,		///< Sets camera to use full field of CCD. (0)

	// ASP-only camera/tracking/focus additional parameters (Secondary camera).
	CIP_ASP_SETEDGEOFFS1 = 0x0270,	///< Set the image edge exclusion parameters. (4)
	CIP_ASP_SETBIASPARAM1,		///< Set dark bias parameters. (2)
	CIP_ASP_SETIMSIZE1,			///< Sets the default camera image dimensions. (2)
	CIP_ASP_SETCAMQUAD1,		///< Sets camera to use only central quadrant of CCD. (0)
	CIP_ASP_SETCAMFULL1,		///< Sets camera to use full field of CCD. (0)

	// EHK-only commands.  From ground.
  CIP_EHK_HTRCTLPARAM = 0x300,  ///< Sets the heater control parameters for a given heater. (3)
	CIP_EHK_SETBALMOTORPOS,		    ///< Set the balance motor position (2).
  CIP_EHK_SETBALMOTORRATE,      ///< Set the balance motor rate (steps/sec) (1).
  CIP_EHK_SETBALMOTORBOUND,     ///< Set the balance motor bounds (4).
  CIP_EHK_BALMOTORSTOP,         ///< Stop the balance motor. (0)
  CIP_EHK_BALMOTORHOMEPARAM,    ///< Set balance motor home stop parameters. (2)
  CIP_EHK_BALMOTORMIDPARAM,     ///< Set balance motor midpoint stop parameters. (2)
  CIP_EHK_BALMOTORENDPARAM,     ///< Set balance motor end stop parameters. (2)

	CIP_EHK_EL1CNV = 0x310,				///< Sets the elevation gyro 1 conversion slope and offset. (4)
	CIP_EHK_EL2CNV,				///< Sets the elevation gyro 2 conversion slope and offset. (4)
	CIP_EHK_ELYAWCNV,			///< Sets the elevation yaw gyro conversion slope and offset. (4)
	CIP_EHK_ELROLLCNV,			///< Sets the elevation roll gyro conversion slope and offset. (4)
	CIP_EHK_ELCONTRIB,			///< Sets the contribution of EYR gyros to true elevation rate. (6)
	CIP_EHK_AZCONTRIB,			///< Sets the contribution of EYR gyros to true azimuth rate. (6)
	CIP_EHK_ROLLCONTRIB,		///< Sets the contribution of EYR gyros to true roll rate. (6)
	CIP_EHK_FGGCUTOFF,			///< Sets the digital filter cutoff frequency for the FGG. (4).
	CIP_EHK_HRGCUTOFF,			///< Sets the digital filter cutoff frequency for the HRG. (4).
	
	// FDR-only commands.
	CIP_FDR_FILEOFF = 0x0400,	///< Toggles saving detector data to file OFF. (0)
	CIP_FDR_FILEON,				///< Toggles saving detector data to file ON. (0)
	CIP_FDR_FWDRAWDET,			///< Sets which detector will have raw SCI data forwarded to transmitter. (1)
	CIP_FDR_FILEEVENTS,			///< Sets the filesize (max # of events per file) for all detectors' save files. (1)
	CIP_FDR_EVPROC,				///< Sets the event processing flag for each detector. (1)
	CIP_FDR_RAWOFF,				///< Turns off sending of raw data altogether. (0)
	CIP_FDR_CHANCOEFF,			///< Sets the channel processing coefficients. (2)
	CIP_FDR_POSORIGIN,			///< Sets position/energy correction fit origin x,y. (4)
	CIP_FDR_POSCOEFF,			///< Sets 3 position correction coefficients. (6)
	CIP_FDR_ECOEFF,				///< Sets 3 energy correction coefficients. (6)
	CIP_FDR_HEATERON,			///< Toggles the FDR heater to ON (0)
	CIP_FDR_HEATEROFF,			///< Toggles the FDR heater to OFF (0)


	// DPS-only commands for detector 1.
	CIP_DPS_SETLTHRESH1 = 0x0500,	///< Sets lower threshold of Detector1. (1)
	CIP_DPS_SETUTHRESH1,	///< Sets upper threshold of Detector1. (1)
	CIP_DPS_SETGRID1,			///< Sets grid voltage of Detector1. (1)
	CIP_DPS_SETDRIFT1,		///< Sets drift voltage of Detector1. (1)
	CIP_DPS_SETPMT1,			///< Sets PMT voltage of Detector1. (1)
	CIP_DPS_SETZERO1,			///< Sets grid, drift and PMT voltages to zero for Detector1. (0)
	CIP_DPS_SETDEFV1,			///< Sets all five default voltages, but not active voltage. (5)
  CIP_DPS_SETEVCTL1,    ///< Set FPGA event control register. (1)
  CIP_DPS_SETITHRESH1,  ///< Set FPGA FIFO interrupt event threshold. (1)
  CIP_DPS_SETHOLDOFF1,  ///< Set FPGA trigger threshold holdoff. (1)
  CIP_DPS_SETILTHRESH1, ///< Set FPGA FIFO interrupt lower threshold. (1)
	CIP_DPS_GETDEFAULT1,	///< Gets the settings above from defaults and sets them. (0)
	CIP_DPS_SETDEFAULT1,	///< Saves current settings as default values. (0)
	CIP_DPS_SETDET1PROCON,  ///< Enable event processing on detector 1. (0)
	CIP_DPS_SETDET1PROCOFF,	///< Disable event processing on detector 1. (0)
  CIP_DPS_SETSWTRIGON1,   ///< Sets the software trigger state ON (0)
  CIP_DPS_SETSWTRIGOFF1,  ///< Sets the software trigger state OFF (0)

  // DPS-only commands for detector 2.
	CIP_DPS_SETLTHRESH2 = 0x0540,	///< Sets lower threshold of Detector2. (1)
	CIP_DPS_SETUTHRESH2,	///< Sets upper threshold of Detector2. (1)
	CIP_DPS_SETGRID2,			///< Sets grid voltage of Detector2. (1)
	CIP_DPS_SETDRIFT2,		///< Sets drift voltage of Detector2. (1)
	CIP_DPS_SETPMT2,			///< Sets PMT voltage of Detector2. (1)
	CIP_DPS_SETZERO2,			///< Sets grid, drift and PMT voltages to zero for Detector1. (0)
	CIP_DPS_SETDEFV2,			///< Sets all five default voltages, but not active voltage. (5)
  CIP_DPS_SETEVCTL2,    ///< Set FPGA event control register. (1)
  CIP_DPS_SETITHRESH2,  ///< Set FPGA FIFO interrupt event threshold. (1)
  CIP_DPS_SETHOLDOFF2,  ///< Set FPGA trigger threshold holdoff. (1)
  CIP_DPS_SETILTHRESH2, ///< Set FPGA FIFO interrupt lower threshold. (1)
	CIP_DPS_GETDEFAULT2,	///< Gets the settings above from defaults and sets them. (0)
	CIP_DPS_SETDEFAULT2,	///< Saves current settings as default values. (0)
	CIP_DPS_SETDET2PROCON,	///< Enable event processing on detector 2. (0)
	CIP_DPS_SETDET2PROCOFF,	///< Disable event processing on detector 2. (0)
  CIP_DPS_SETSWTRIGON2,   ///< Sets the software trigger state ON (0)
  CIP_DPS_SETSWTRIGOFF2,  ///< Sets the software trigger state OFF (0)

  // DPS-only commands for both detectors.
	CIP_DPS_SENDPARAM = 0x0580,				// Requests a parameter packet. (0)
	CIP_DPS_SETHVZERO,				// Sets all detector HV settings to zero. (0)
	CIP_DPS_SETFIFORST,			// Resets the FIFOs on both detector FPGA boards. (0)

	// GSE Gateway only commands
	CIP_HK_CONFIG_IO = 0x0600,	///< Sets the Communication Device flags on HK Gateway (0)
	CIP_GSE_NEWLOGFILE = 0x0601, ///< Causes the GSE Gateway to start a new logfile(0)
	CIP_GSE_RESETSTATS = 0x0602, ///< Causes the GSE to reset the stats

	// Graceful shutdown command.
	CIP_SHUTDOWN = 0x0a15,		// Tells all systems to shutdown gracefully. (0)

	// Time sync command.
	CIP_TIME_SYNC = 0x0f00,		// Sends time to be set at next time-sync pulse (2).
	CIP_TIME_THRESHOLD,		// Tells the system to set the threshold for clock set (2).
};

// Define the lengths of the data following the commands above.
enum {
	CIP_RELAY_STATE_LEN = 1,
	CIP_ACQUIRE_STATIC_LEN = 4,
	CIP_STOP_GUIDING_LEN = 0,

	CIP_TIME_SYNC_LEN = 2,
	CIP_TIME_THRESHHOLD_LEN = 2,
};

// Define the lower 4 bits in the Set PID loop gain commands.
enum {
	SETPID_ROUGH = 1,
	SETPID_INERTIAL = 2,
};

// Define the lower 4 bits in the Set DAC bounds command.
enum {
	SETDACB_AZW = 1,		// Azimuth wheel is included.
	SETDACB_AZS = 2,		// Azimuth shaft is included.
	SETDACB_EL = 4,			// Elevation is included.
};

// Define the lower 4 bits for the trajectory, PIDLIM, & modeLim commands.
enum {
	SETAXIS_AZ = 1,			// Azimuth axis.
	SETAXIS_EL = 2,			// Elevation axis.
};

