//	ErrorCodes.h:
//		Lists error numbers for all classes, making sure they don't overlap, so that
//		each error number is unique.

#ifndef __ERROR_CODES_H__
#define __ERROR_CODES_H__

// Enumerate the Prz32eaDaq errors.
enum {
	PRZERR_REINIT = 1,			///< Attempted to reinitialize object.
	PRZERR_ADCINEVINIT,			///< Error creating ADC input event.
	PRZERR_DIOEVINIT,			///< Error creating DIO event.
	PRZERR_DIOTHRDINIT,			///< Error creating DIO thread.
	PRZERR_TIMEVINIT,			///< Error creating timer event.
	PRZERR_TIMTHRDINIT,			///< Error creating timer thread.
	PRZERR_SETISR,				///< Error setting ISR.
	PRZERR_NOTINIT,				///< Attempted an operation before initializing class.
	PRZERR_WAITTMO,				///< Timed out on a wait operation, or error waiting.
	PRZERR_CHANINVALID,			///< Invalid channel range for ADC scan setup.
	PRZERR_DACARG,				///< Invalid DAC argument (channel or voltage).
  PRZERR_INTDISABLED,   ///< Attempted a wait, or semaphore fired when interrupt disabled.
};

// Enumerate the WordBuffer errors.
enum {
	WBUFERR_WAITFAIL=100,		///< Wait failed.
	WBUFERR_SIZEINVALID,		///< Attempted to set an invalid buffer size.
	WBUFERR_INDEXINVALID,   ///< Attempted to get or set and element outside bounds.
};

// Enumerate the CWordBufferHandler errors.
enum {
	WBHERR_SIZEINVALID=200,		///< Attempted to set an invalid number of buffers.
};

// EcCharCBuff error codes.
enum {
	CBUF_NOBUFFER = 300,
	CBUF_NODATA,
	CBUF_ATEND,
	CBUF_SIZEINVALID,
	CBUF_EXISTS,
	CBUF_LENGTHINVALID,
	CBUF_MUTEXCREATEFAIL,
	CBUF_MUTEXTIMEOUT,
	CBUF_MUTEXFAIL,
};

// EcCipCommandHandler error codes.
enum {
	CIPCOM_ERR_INIT = 400,				///< Attempted operation without initializing.
	CIPCOM_ERR_REINIT,					///< Attempted to re-initialize an existing object.
	CIPCOM_ERR_RELAYINVALID,			///< Passed an invalid relay board.
	CIPCOM_ERR_ASPCTLINVALID,			///< Passed an invalid aspect control object.
	CIPCOM_ERR_GYROHNDINVALID,			///< Invalid gyro handler pointer.
	CIPCOM_ERR_GYRASPINVALID,			///< Invalid EcGyroAspHandler pointer.
	CIPCOM_ERR_KILLED,					///< Owning process of wait object was killed.
	CIPCOM_ERR_TIMEOUT,					///< Wait object timed out.
	CIPCOM_ERR_MUTEXFAIL,				///< Failed to create the waitable timer.
	CIPCOM_ERR_PACKETCPU,				///< Packet received with incorrect CPU ID.
	CIPCOM_ERR_PACKETCRC,				///< A CRC checksum error occurred in the packet.
	CIPCOM_ERR_PACKETLEN,				///< The packet length word was invalid.
	CIPCOM_ERR_TCLIENT,					///< Invalid time client was passed.
	CIPCOM_ERR_DIOHAND,					///< Invalid DIO handler was passed or called.
	CIPCOM_ERR_HDRHND,					///< A NULL HDR handler was passed or called.
	CIPCOM_ERR_TAFCTL,					///< A NULL track and focus control was passed or called.
	CIPCOM_ERR_DATA,					///< An error ocurred getting data from the packet.
	CIPCOM_ERR_RELAY,					///< Relay state indicates resource for command is unpowered.
	CIPCOM_ERR_NOTCMD,					///< Command word not handled by this system.
	CIPCOM_ERR_UIOHAND,					///< Invalid DIO handler was passed or called.
	CIPCOM_ERR_INVALIDPARAM,			///< An invalid parameter was sent in the command data.
	CIPCOM_ERR_DAQHND,					///< DAQ handler pointer was invalid.
	CIPCOM_ERR_RELAYHND,				///< Relay handler pointer was invalid.
	CIPCOM_ERR_MHHND,					///< Mirror heater handler pointer was invalid.
};

// EcCipGateway errors.
enum {
	CGERR_REINIT = 500,			///< Attempted to start an object before stopping it.
	CGERR_NULLSTREAM,				///< Attempted a read or write on a null stream.
};

// EcCommandPacket errors.
enum {
	CMDPKTERR_CRCERR = 600,			///< Indicates packet received, but CRC value is wrong.
	CMDPKTERR_LENGTHERR,			///< Indicates the length passed exceeded max value.
	CMDPKTERR_NOLENGTH,				///< Indicates zero or negative length.
	CMDPKTERR_NOSYNC,				///< No sync word found.
	CMDPKTERR_BUFFSHORT,			///< In CheckPacket, supplied buffer too short for packet.
	CMDPKTERR_INCOMPLETE,			///< Packet not complete.
	CMDPKTERR_NOBUFF,				///< No buffer present.
	CMDPKTERR_FULL,					///< Packet is full (no more command words can be added).
	CMDPKTERR_EOP,					///< End of packet encountered.
	CMDPKTERR_USERPARAM,			///< A user passed an invalid parameter.
};

// EcCipSocketStream errors:
enum {
	CIPSOCK_INSOCK = 700,			///< Attempted to create a CipSocketStream on an input socket.
};

// EcDacHandler errors.
enum {
	DACERR_STREAMP = 800,		///< Passed an invalid communications stream pointer.
  DACERR_REINIT,      ///< Attempted to create device twice.
	DACERR_INIT,				///< Attempted a call without first initializing the object.
	DACERR_ADDRESS,				///< DAC replied with ADDRESS ERROR.
	DACERR_CHKSUM,				///< DAC replied with BAD CHECKSUM.
	DACERR_COMMAND,				///< DAC replied with COMMAND ERROR.
	DACERR_LIMIT,				///< DAC replied with LIMIT ERROR.
	DACERR_MANUAL,				///< DAC replied with MANUAL MODE.
	DACERR_PARITY,				///< DAC replied with PARITY ERROR.
	DACERR_SYNTAX,				///< DAC replied with SYNTAX ERROR.
	DACERR_VALUE,				///< DAC replied with VALUE ERROR.
	DACERR_WRITEPROT,			///< DAC replied with WRITE PROTECTED.
	DACERR_REPLY,				///< Reply is not one of the known types.
	DACERR_TIMEOUT,				///< DAC failed to reply within the timeout period.
};

// EcEmeraldMMPort and EcSerialPort errors.
enum {
	COMI_ERR_NCOM = 900,		///< Tried to create port with too low or high a port number.
	COMI_ERR_NOCOM,				///< Call without initialized board.
	COMI_ERR_SYSCTL,			///< invalid system control pointer.
	COMI_ERR_BRATE,				///< invalid baude rate requested.
	COMI_ERR_DLENGTH,			///< invalid data length requested.
	COMI_ERR_SLENGTH,			///< invalid stop bit length requested.
	COMI_ERR_RXBUFF,			///< invalid receive buffer length requested.
	COMI_ERR_TXBUFF,			///< invalid transmit buffer length requested.
	COMI_ERR_FTHRESH,			///< invalid FIFO Rx threshold requested.
	COMI_ERR_INTERRUPT,			///< invalid IRQ requested.
	COMI_ERR_INIT,				///< object already initialized.
	COMI_ERR_NOINIT,			///< Object not initialized.
	COMI_ERR_SETISR,			///< Failed to set interrupt service routine.
	COMI_ERR_RXSEMCREATE,		///< Failed to create Rx semaphore.
	COMI_ERR_TXMUTCREATE,		///< Failed to create Tx mutex.
	COMI_ERR_EXITCREATE,		///< Failed to create exit event.
	COM_ERR_SIZE,				///< Size negative or larger than buffer.
	COM_ERR_TIMEOUT,			///< Timed out on buffer mutex.
	COM_ERR_ABANDON,			///< Abandoned mutex satisfied wait.
	COM_ERR_KILLED,				///< Serial process terminated.
	COM_ERR_BUFFER,				///< Buffer errror.
};

// EcGseGateway errors..
enum {
	GGERR_REINIT = 1000,			///< Attempted to start an object before stopping it.
	GGERR_NULLSTREAM,				///< Attempted a read or write on a null stream.
};

// EcGsePacket errors.
enum {
	HGSEPKTERR_TYPE = 1100,				///< Attempted to create a packet of unknown type.
	HGSEPKTERR_NOPKT,					///< The pointer to the packet data is NULL.
	HGSEPKTERR_OPTYPE,					///< Attempted an operation on the wrong type packet.
	HGSEPKTERR_LENGTH,					///< Length didn't match type.
	HGSEPKTERR_SHORT,					///< VerifyBuffer buffer was too short for header to fit.
	HGSEPKTERR_BUFFLENGTH,				///< Packet length didn't match buffer length (for VerifyBuffer).
	HGSEPKTERR_SYNCHERR,				///< VerifyBuffer synch words were incorrect.
	HGSEPKTERR_ZEROLENGTH,				///< Variable length packet had zero data length.
	HGSEPKTERR_CRC,						///< CRC didn't match.
	HGSEPKTERR_INCOMPLETE,				///< Attempted an operation on an incomplete packet.
	HGSEPKTERR_NOTRESET,				///< Attempted to create a packet before it was reset.
	HGSEPKTERR_RESET,					///< Attempted an operation on a packet that was reset.
	HGSEPKTERR_USERPARAM,				///< A user passed an invalid parameter.
	HGSEPKTERR_GPSTYPE,					///< Motorola GPS type was not a known type.
	HGSEPKTERR_TANSTYPE,				///< TANS type was not known.
	HGSEPKTERR_NULLSTREAM,				///< Attempted a read or write on a null stream.
	HGSEPKTERR_ASPRAWIMAGE_INVCAMERA,	///< Invalid camera ID
	HGSEPKTERR_ASPRAWIMAGE_INVLENGTH,	///< Invalid length given for SetImageData
	HGSEPKTERR_ASPRAWIMAGE_INVPXLSTEP,	///< Invalid pixel step given for SetImageData
	HGSEPKTERR_ASPRAWIMAGE_BOUNDS,		///< Pixel operation went out of the bounds of EcAspectImage
};

//	EcMagHandler errors.
enum {
	MAGHND_STRINVALID = 1200,		///< Invalid stream passed in Create routine or used in another routine.
	MAGHND_THREADFAIL,				///< Failed to create receive thread.
};

// EcMksHps920 errors.
enum {
	HPSERR_STRINVALID = 1300,		///< Invalid stream passed in Create routine.
	HPSERR_NOTXSTR,					///< No output stream (not initialized).
	HPSERR_RESPONSE,				///< An unknown response was received.
	HPSERR_NORESPONSE,				///< No response was received from the HPS unit.
	HPSERR_NAK,						///< The HPS unit returned a "NAK" error response.
};

// EcMksHpsPacket errors.
enum {
	MKSHPSERR_INVALIDARG=1400,		///< Invalid argument passed to a call.
	MKSHPSERR_NOTRESET,				///< Attempted operation requiring a reset packet on a non-reset packet.
	MKSHPSERR_INCOMPLETE,			///< Attempted to read data from an incomplete packet.
	MKSHPSERR_NOTACK,				///< Attempted to read data from a non-acknowledgement packet.
	MKSHPSERR_SCANF,				///< Error attempting to read from string.
};

// EcMotGpsHandler errors.
enum {
	MGPSH_ERR_INIT = 1500,			///< Attempted to re-initialize an existing object.
	MGPSH_ERR_REINIT,				///< Attempted operation without initializing.
	MGPSH_ERR_INSTRINVALID,			///< Passed an invalid input stream pointer.
	MGPSH_ERR_OUTSTRINVALID,			///< Passed an invalid output stream pointer.
	MGPSH_ERR_BUFFER,				///< Error waiting on serial buffer.
	MGPSH_ERR_KILLED,				///< Serial process was killed.
	MGPSH_ERR_TIMEOUT,				///< Timed out waiting on serial buffer.
	MGPSH_ERR_TSTARTFAIL,			///< Failed to start serial processing thread.
	MGPSH_ERR_TIMESTARTFAIL,		///< Failed to start time processing thread.
	MGPSH_ERR_TIMEPSETFAIL,			///< Failed to set time processing thread priority.
	MGPSH_ERR_PACKETDLE,			///< Packet DLE error.  Start of new packet before end.
	MGPSH_ERR_PACKETLENGTH,			///< Packet exceeded max allowed length (128 bytes).
	MGPSH_ERR_SPTOBJ,				///< Serial processing thread not started from correct object.
	MGPSH_ERR_SPTRUN,				///< Tried to start serial processing thread more than once.
	MGPSH_ERR_SPTEXIT,				///< Serial processing thread exited.
	MGPSH_ERR_SNDFAIL,				///< Failed to send GPS packet to ground.
};

// EcPacket errors.
enum {
	PKT_NOPKT = 1600,				///< No data packet is available (not initialized).
	PKT_NULLSTR,					///< Passed a NULL stream to a packet operation.
};

// EcShaftEncoder errors..
enum {
	SHEN_ERR_COMINVALID = 1700,			///< Passed invalid EcComHandler pointer.
	SHEN_ERR_UIOINVALID,				///< Passed invalid EcUio48 pointer.
	SHEN_ERR_CREATED,					///< Attempted multiple Creates of this object.
	SHEN_ERR_NOTCREATED,				///< Attempted to read before creating.
};

// EcSocket errors..
enum {
	SOCKERR_TYPEINVALID = 1800,		///< Type was invalid.
	SOCKERR_MUTEXFAIL,				///< Failed to create mutex.
	SOCKERR_MUTEXERR,				///< Mutex wait failed to obtain object.
	SOCKERR_NOTBOUND,				///< Required a bound socket, but this one is not.
	SOCKERR_NOTLISTENING,			///< Required a listening socket, but this one is not.
	SOCKERR_ISBOUND,				///< Attempted to bind or connect a bound socket.
	SOCKERR_ISLISTENING,			///< Attempted a listen on a listening socket.
	SOCKERR_NAMEINVALID,			///< Tried to connect to a NULL host name.
	SOCKERR_ADDRINVALID,			///< Passed a NULL pointer as an address pointer.
	SOCKERR_PSOCKINVALID,			///< Passed a NULL pointer as a socket pointer.
	SOCKERR_PSOCKEXISTS,			///< Tried to accept on an existing socket.
	SOCKERR_LENGTHINVALID,			///< Passed an invalid namelength to a retreival.
	SOCKERR_BUFFINVALID,			///< Passed an invalid buffer to a call.
	SOCKERR_CMDINVALID,				///< Passed an invalid command to ioctl.
	SOCKERR_TMOINVALID,				///< Passed an invalid timeout to select.
	SOCKERR_OPTINVALID,				///< Passed an invalid option to setsockopt.
	SOCKERR_LVLINVALID,				///< Passed an invalid level to setsockopt.
	SOCKERR_DISINVALID,				///< Passed an invalid "how" argument to shutdown.
	SOCKERR_NOHOST,					///< No valid host was found.
	SOCKERR_NORECON,				///< Socket not reconnectable.
    SOCKERR_CREATED,                ///< Attempted to create a socket that was already created.
    SOCKERR_NOTCREATED,             ///< Attempted operation on a socket that wasn't created.
    SOCKERR_CONNECTED,              ///< Socket already connected.
    SOCKERR_NOTCONN,                ///< Socket is not yet connected.
    SOCKERR_TIMEDOUT,               ///< Socket operation timed out.
};

// EcSocketStream errors.
enum {
	SOCKSTR_INSOCK = 1900,			///< Attempted an output operation on an input socket.
	SOCKSTR_OUTSOCK,				///< Attempted an input operation on an output socket.
};

// EcTansHandler errors.
enum {
	GPSH_ERR_INIT = 2000,			///< Attempted to re-initialize an existing object.
	GPSH_ERR_REINIT,				///< Attempted operation without initializing.
	GPSH_ERR_COMINVALID,			///< Passed an invalid Com port pointer.
	GPSH_ERR_TSRVINVALID,			///< Passed an invalid TimeServer pointer.
	GPSH_ERR_BUFFER,				///< Error waiting on serial buffer.
	GPSH_ERR_KILLED,				///< Serial process was killed.
	GPSH_ERR_TIMEOUT,				///< Timed out waiting on serial buffer.
	GPSH_ERR_TSTARTFAIL,			///< Failed to start serial processing thread.
	GPSH_ERR_TIMESTARTFAIL,			///< Failed to start time processing thread.
	GPSH_ERR_TIMEPSETFAIL,			///< Failed to set time processing thread priority.
	GPSH_ERR_PACKETDLE,				///< Packet DLE error.  Start of new packet before end.
	GPSH_ERR_PACKETLENGTH,			///< Packet exceeded max allowed length (128 bytes).
};

// EcTansPacket errors.
enum {
	ADU5ERR_PKTBUILD = 2100,		///< Error building ADU5 Packet
	ADU5ERR_PKTTOOLONG,				///< ADU5 Packet was too long
	ADU5ERR_CHECKSUM,				///< Packet had incorrect checksum
	ADU5ERR_PKTFORMAT,				///< Packet was malformed
	ADU5ERR_MUTEX,					///< ADU5 unable to lock mutex
	ADU5ERR_NOTUPDATED,				///< Attitude information wasn't updated
	ADU5ERR_SEND,					///< ADU5 Handler unable to send
	ADU5ERR_SENDLENGTH,				///< ADU5 Handler was not able to send all data.
};

// SerialStream errors.
enum {
  SERSTRERR_DEVNAMETOOLONG=2200,    ///< Device name was too long.
  SERSTRERR_OPENFAILED,             ///< Failed to open serial device.
  SERSTRERR_POLLERR,                ///< Poll function returned an error.
  SERSTRERR_TIMEDOUT,               ///< Input or output timed out.
  SERSTRERR_DEVICEERR,              ///< Poll indicated a device error.
  SERSTRERR_READERR,                ///< Error reading input from device.
  SERSTRERR_WRITEERR,               ///< Error writing output to device.
  SERSTRERR_BAUDRATE,               ///< Baud rate not recognized.
  SERSTRERR_CHARBITS,              ///< Number of bits per character not recognized.
};

enum {
	CRHERR_STARTED = 2300,					///< Tried to start thread when it is already started.
	CRHERR_PACKETCPU,				///< Packet received with unknown CPU ID.
};

// EcUio48 errors.
enum {
	UIO48_ERR_DEVNUMBOUNDS = 2400,	///< Device number was out of bounds.
	UIO48_ERR_DEVERROR,				///< Device failed to open.
	UIO48_ERR_INTINVALID,			///< Attempted to set an interrupt on an invalid bit
  UIO48_ERR_ADDROVERLAP,    ///< I/O address space overlapped with another Uio48 module.
  UIO48_ERR_IRQOVERLAP,     ///< IRQ overlapped with another Uio48 module.
  UIO48_ERR_IPBTAKEN,       ///< IPB already exists for this Uio48 module.
  UIO48_ERR_NOTINIT,        ///< Object not initialized.
  UIO48_ERR_REINIT,         ///< Attempted to re-initialize object.
  UIO48_ERR_IRQINVALID,     ///< Device created with invalid IRQ number.
  UIO48_ERR_SAVEISR,        ///< Error saving old ISR.
  UIO48_ERR_SETISR,         ///< Error trying to set new ISR.
};

// EcUio48Stream errors.
enum {
	U48SERR_STARTED = 2500,	///< Attempted to create a Uio48Stream after creation.
	U48SERR_NOTCREATED,		  ///< Attempted to use object before creating it.
	U48SERR_UINVALID,			  ///< Passed a null pointer as the pointer to the Uio48 object.
	U48SERR_ISRINSTALL,		  ///< Error installing the UIO48 ISR.
	U48SERR_SEMFAIL,			  ///< Failed to create semphore.
	U48SERR_WAITERR,			  ///< An error was encountered waiting for the semaphore.
	U48SERR_BUFF,					  ///< A buffer error occurred.
	U48SERR_OVERFLOW,			  ///< Input overflowed buffer.
	U48SERR_TIMEDOUT,			  ///< Received timed out waiting for semaphore.
};

// EcAspectControl errors.
enum {
	ACTLERR_NOBUFF = 2600,			///< Called Update with a NULL buffer pointer.
	ACTLERR_INVALIDBUFF,			///< Buffer passed to Update was wrong size.
	ACTLERR_MODEINVALID,			///< Attempted to set parameters for an invalid mode.
	ACTLERR_RESTOREDEF,				///< Attempted to restore default parameters while a pointing mode was active.
	ACTLERR_SETV,					///< Attempted to set a control motor voltage while actively pointing.
	ACTLERR_NODACHND,				///< Attempted to set a motor voltage with a NULL DAC handler pointer.
	ACTLERR_ASPIMERR,				///< ASP image aspect data error was above threshold, so invalid.
	ACTLERR_EHKUPDATE,				///< EHK update did not come in time.
	ACTLERR_MUTEX,            ///< Error locking mutex.
  ACTLERR_GEOPOSSRC,        ///< FDR updated geographic position, but is not selected as geo pos source.
};

// EcControlMode errors.
enum {
	ECMERR_SLEWDONE = 2800,	///< A slew mode completed its target calculations.
	ECMERR_PBOUND,      		///< Proportional bounds exceeded for this mode, cannot control motion.
  ECMERR_DBOUND,          ///< Differential bounds exceeded for this mode.
	ECMERR_SRCFAIL,					///< A critical aspect input source was not available to control motion.
	ECMERR_MUWAIT,					///< Access to the mutex controlling the parameters could not be obtained.
};

// EcPointingMode errors.
enum {
	EPMERR_CREATED = 2900,			///< Attempted to create object more than once.
	EPMERR_NOTCREATED,				///< Attempted to use object before creating it.
	EPMERR_NOTPOINTING,				///< Attempted to call CalcOutput before calling StartPointing.
	EPMERR_GOTOSAFEMODE,			///< A loss of input source requires going into safe mode.
};

// EcPciCard errors.
enum {
	PCICERR_INITFAIL = 3000,		///< Attempt to start WD system failed.
	PCICERR_NOCARD,					///< No cards with given vendor and device ID were found.
	PCICERR_CARDNUM,				///< Card number out of range of available cards.
	PCICERR_CARDINUSE,				///< Failed to register card because it is already in use.
	PCICERR_TOOMANYCARDS,			///< Failed to register card because too many cards already open.
	PCICERR_MUTEXFAIL,				///< Failed to obtain mutex to process operation.
	PCICERR_NOTCREATED,				///< Attempted an operation before successfully calling Create.
	PCICERR_NOTREG,					///< Attempted to unregister a card that was never registered.
};

// EcM4i34PlusFpga errors.
enum {
	M4I34PFERR_NOINT = 3100,		///< Interrupt information missing.
	M4I34PFERR_NOMEM,				///< No active memory space found.
	M4I34PFERR_LASTERR				///< Last error.
};

// EcAltaCamHandler errors.
enum {
	CACHERR_INIT = 3200,	///< Attempted to Create previously created camera.
	CACHERR_NOTINIT,			///< Attempted an operation before creating the camera.
	CACHERR_CAMCREATE,		///< Error creating instance of camera object.
	CACHERR_WAITMUTEX,		///< Wait for mutex failed.
	CACHERR_WAITTIMER,		///< Wait for timer failed.
	CACHERR_TIMEDOUT,			///< Operation timed out.
	CACHERR_NOTCONN,			///< Camera not connected.
	CACHERR_CONN,					///< Camera connected, cannot perform operation on connected camera.
	CACHERR_GETIMFAIL,		///< Failed to read image from camera.
	CACHERR_IDINVALID,		///< Attempted to set negative camera ID.
	CACHERR_IMSIZE,				///< Image size read from connected camera did not match size parameters.
	CACHERR_DEFFILE,			///< Error opening default file.
	CACHERR_CAMCONNECT,		///< Error (exception thrown) attempting to connect (Init) camera.
	CACHERR_NOEXP,				///< Could not detect end of exposure.
	CACHERR_INITDEFAULTS,	///< Camera InitDefaults method failed.
};

// EcHeroDataRecorder errors.
enum {
	HDRERR_NULLSTREAM = 3300,		///< NULL pointer passed as input stream pointer.
	HDRERR_DPSNUM,					///< An invalid DPS number was passed (valid is 0-7).
	HDRERR_DETNUM,					///< An invalid detector number was passed (valid is 0-8).
	HDRERR_THREADFAIL,				///< Failed to start data processing thread.
	HDRERR_FOPENFAIL,				///< Failed to open a save file.
	HDRERR_MUTEXFAIL,				///< Failed to obtain ownership of the data mutex.
};

// CImageObjectList errors.
enum {
	IMOBJLERR_WAITFAIL = 3400,		///< Failed to obtain ownership of the mutex protecting operations.
	IMOBJLERR_INDEX,				///< Invalid index passed to GetObject routine.
};

//	EcPrz32eaMotor errors.
enum {
	P32MOERR_STEPBIT = 3500,		///< Invalid step bit number was passed to the Create routine.
	P32MOERR_CREATED,				///< Attempted to call Create multiple times for the same object.
	P32MOERR_NOTCREATED,			///< Attempted to call movement functions before the object was Created.
	P32MOERR_MOVING,				///< Attempted to call movement function while motor was already running.
	P32MOERR_OPABORT,				///< Operation aborted before completion.
	P32MOERR_WAITFAIL,				///< Wait on synch object failed.
	P32MOERR_THREADFAIL,			///< Failed to create motor thread.
	P32MOERR_LIMIT,						///< Movement was beyond motor limits.
};

//	EcTrackAndFocusCtl errors.
enum {
	TAFCERR_INIT = 3600,			///< Attempt multiple Create calls on the same object.
	TAFCERR_THREADFAIL,				///< Failed to create the tracking thread.
	TAFCERR_NOTINIT,				///< Attempted operations without initializing the object.
	TAFCERR_MUTEXWAIT,				///< Wait for mutex timed out.
	TAFCERR_OPENFAIL,				///< Failed to open parameter file for read or write.
	TAFCERR_MODE,					///< Track/focus mode is not appropriate for attempted operation.
	TAFCERR_DATA,					///< Invalid data passed to a setting routine.
	TAFCERR_CAMNULL,				///< Camera object pointer is NULL.
	TAFCERR_CAMINIT,				///< Failed to initialize the camera.
	TAFCERR_CAMCONN,				///< Attempted an operation before camera connected.
	TAFCERR_FOCINIT,				///< Attempted focus op before motor is power or initialized.
	TAFCERR_CATNOBJ,				///< Number of catalog objects is less than # needed for ID.
	TAFCERR_NOTOFFSET,				///< Attempted to set offset and angle of non-offset camera.
	TAFCERR_FOCUSMODE,				///< Attempted to set an invalid focus mode.
};

// EcFocusCtl errors.
enum {
	FCTLERR_RUNNING = 3700,			///< Attempted to change parameters while focus is running.
	FCTLERR_NOTRUNNING,				///< Attempted GetNextStepPosition call before StartFocus call.
	FCTLERR_MUTEXWAIT,				///< Wait for mutex timed out.
	FCTLERR_STEPSIZE,				///< Stepsize did not cascade correctly, or was too big.
	FCTLERR_NOPOINTS,				///< No focus positions were valid for a given focus loop.
	FCTLERR_NOFFIG,					///< A focus figure was not supplied before moving to next position.
	FCLTERR_NOWEIGHT,				///< No weights were available to calculate position, leading to divide-by-zero error.
	FCTLERR_BACKLASH,				///< Indicates a step that requires backlash removal before exposing next image.
	FCTLERR_ATEND,					///< Indicates this step is the final focus position.
	FCTLERR_NOTUPDATED,			///< Dated was not updated since last read.
};

// CSeaMac4Stream errors.
enum {
	CSM4ERR_INIT = 3800,			///< Attempt multiple Create calls on the same object.
	CSM4ERR_DEVNUM,					///< Called Create with an invalid device number.
	CSM4ERR_OPENFAIL,				///< Failed to create the SeaMAC device.
	CSM4ERR_NOTINIT,				///< Call function before Create.
	CSM4ERR_TIMEDOUT,				///< Waitable call timed out.
};

// Error values returned by EcFilteredData members.
enum {
	EFDERR_INIT = 3900,		///< Object already created.
	EFDERR_NOTINIT,			///< Attempted operations when object was not initialized.
	EFDERR_SIZE,			///< Object size incorrect.
	EFDERR_PERIODINVALID,	///< Sample period was zero or less.
	EFDERR_FREQINVALID,		///< Filter cutoff frequency was zero or less.
	EFDERR_NYQUIST,			///< Filter cutoff frequency not < 1/2 sample frequency.
	EFDERR_TIMEOUT,			///< Data validity has expired.
};

// CSeaMac4Dev errors.
enum {
	CSM4DERR_INIT = 4000,			///< Attempted multiple Create calls on the same object.
	CSM4DERR_DEVNUM,				///< Called Create with an invalid device number.
	CSM4DERR_OPENFAIL,				///< Failed to create the SeaMAC device.
	CSM4DERR_MUTEXFAIL,				///< CreateMutex call failed.
	CSM4DERR_THREADFAIL,			///< CreateThread call failed.
	CSM4DERR_NOTINIT,				///< Call function before Create.
	CSM4DERR_TIMEDOUT,				///< Waitable call timed out.
	CSM4DERR_INVALIDHANDLE,			///< Invalid handle.
};

// EcSensorAutoTrim errors.
enum {
	SATERR_INIT = 4100,			///< Attempted multiple Create calls on the same object without Close.
	SATERR_NOTINIT,				///< Attempt an operation before Create.
	SATERR_NERROR,					///< Create called with invalid number of fit or update points.
	SATERR_UPDATED,				///< Status flag indicating fit parameters were updated.
	SATERR_MODE,					///< Attempted to set an auto-trim mode or source.
	SATERR_MUTEX,					///< Mutex was not obtained to complete requested operation.
	SATERR_NOFIT,					///< Fit not yet valid, so no YFit value returned.
	SATERR_NOMATCH,            ///< Time of asynch trim sensor did not match any in history.
};

//	EcEventCBuff errors.
enum {
	EECBERR_INIT = 4200,			///< Attempted multiple create calls on the same object.
	EECBERR_NOTINIT,				///< Attempted an operation before creating object.
	EECBERR_MUTEX,					///< Mutex wait failed.
	EECBERR_EVENT,					///< Event wait failed.
	EECBEER_NODATA,					///< No data was available to be read.
};

// EcGyroAspHandler errors.
enum {
	EGAHERR_MUTEXWAIT = 4400,		///< Attempted operation failed because mutex wasn't present.
	EGAHERR_FILEOPEN,				///< Failed to open a file.
};

//	CGpsData errors.
enum
{
	GPSDATA_LENGTHERR = 4500,		///< Length of GPS data was too long for structure.
	GPSDATA_DLEERR,					///< Value following DLE was wrong.
	GPSDATA_INCOMPLETE,				///< GPS data packet is incomplete.
	GPSDATA_TYPEERR,				///< GPS type was not valid for the operation attempted.
};

///	AxisAutoTrim errors.
enum {
	AATERR_SENSORERR = 4800,		///< A required sensor was not valid.
};

/// EcCelstialGeometry errors.
enum {
	ECGEOMERR_MUTEXWAIT = 4900,		///< Mutex wait failed (timeout or other error).
	ECGEOMERR_NULLRETURN,			///< Pointer passed for a return value was NULL.
};

/// EcTargetStarCatalog errors.
enum {
	ETSCERR_INDEX = 5000,			///< Index was outside catalog bounds.
	ETSCERR_OFFSETVAL,				///< Offset value is negative.
};

/// EcGsePacketQueue errors.
enum {
	EGPQERR_CREATED = 5100,			///< Attempted to create a queue multiple times.
	EGPQERR_NOTCREATED,				///< Attempted operating on queue before creating it.
	EGPQERR_QFULL,					///< Attempted to enqueue a packet into a full queue.
	EGPQERR_TMOUT,					///< A mutex or event wait timed out.
	EGPQERR_NOSIZE,					///< Attempted to create mutex with a size of zero.
};

///	EcImageInputHandler errors.
enum {
	EIIHERR_CREATED = 5200,			///< Attemped to create object multiple times.
	EIIHERR_NOTCREATED,				///< Attempted to use object before creating it.
	EIIHERR_THREADFAIL,				///< Failed to create thread.
	EIIHERR_MUTEXWAIT,				///< Mutex wait failed.
};

///	EcImagePacketPacer errors.
enum {
	EIPPERR_CREATED = 5300,			///< Attemped to create object multiple times.
	EIPPERR_NOTCREATED,				///< Attempted to use object before creating it.
	EIPPERR_THREADFAIL,				///< Failed to create thread.
	EIPPERR_MUTEXWAIT,				///< Mutex wait failed.
};

/// EcFileStream errors
enum {
	EFSERR_OPENFAIL = 5400,			///< Error opening file stream
	EFSERR_READFAIL,				///< Error writing to file stream
	EFSERR_WRITEFAIL,				///< Error reading from file stream
	EFSERR_READONLY,				///< Attempted to write to a read only file stream
	EFSERR_WRITEONLY,				///< Attempted to read from a write only file stream
	EFSERR_FILECLOSED,				///< Attempted to perform operation on a closed file stream
	EFSERR_RESETERR,				///< Error during stream reset
};

/// CSeaMacStream errors.
enum {
	CSMSERR_MUTEXFAIL = 5500,		///< Wait for mutex failed.
};

/// PidCtl errors.
enum
{
  PIDCTLERR_PERIOD = 5900,  ///< Tried to create object with period=zero.
};

/// AspectSensorDataManager errors.
enum
{
  ASDMERR_MUTEX = 6000,       ///< Wait for mutex failed.
  ASDMERR_CAMID,              ///< Invalid camera ID.
};

/// GeoPosManager errors.
enum
{
  GPMERR_NODATA = 6100,     ///< No valid position data is available.
  GPMERR_MUTEX,             ///< Error locking the mutex.
};

/// SensorTrimManager errors.
enum
{
  STMERR_NOGEOPOS = 6200,   ///< No valid geo position available.
};

/// Differencer errors.
enum
{
  DIFFERR_NOPREV = 6300,    ///< No valid previous value.
};

/// PointingMode and PointingModeManager errors.
enum
{
  PMMERR_NOMODE = 6400,     ///< No control modes valid for this pointing mode.
  PMMERR_SCORE,             ///< Sensor score is not valid for this mode.
};

/// Slew manager errors.
enum
{
	SLEWMANERR_NOTSLEW = 6500, ///< Slew operation attempted before slew is activated.
	SLEWMANERR_PARAM,          ///< Attempted to create object with invalid parameter.
};

/// DiamondMM Errors.
enum
{
	DMMERR_CHANNELBOUNDS = 6600,///< Channel address is out of bounds
	DMMERR_LOWABOVEHIGH,	 	///< Lowest Channel specified is > Highest Channel
	DMMERR_TEMPRBOUNDS, 		///< Temperature address out of bounds
	DMMERR_RELAYBOUNDS,			///< Relay address out of bounds
	DMMERR_ANALOGBOUNDS,		///< Analog Mux address out of bounds
};

// CtlDmmRelay errors.
enum
{
	CDMMRERR_DEVNULL = 6700,	///< CTL_DiamondMM device pointer is NULL.
	CDMMRERR_FILEOPEN,				///< Failed to open file that stores bit state.
	CDMMRERR_FILEWRITE,				///< Failed to write bit state to file.
};

// ControlBase errors.
enum
{
	CTLBASERR_CREATED = 6800,	///< Tried to create a second object in this space.
};

// CtlStatusReporter errors.
enum
{
	CTLSREPERR_DATALENGTH = 6900,		///< Input ADC sample vector was wrong size.
};

/// H3000MagHandler errors.
enum
{
	H3MHERR_NOTUPDATED = 7000,		///< Input from mag not received.
	H3MHERR_CHECKSUM,							///< Input packet checksum failed.
	H3MHERR_LOWALARM,							///< Input flagged with low alarm.
	H3MHERR_LOWWARN,							///< Input flagged with low warning.
	H3MHERR_HIGHWARN,							///< Input flagged with high warning.
	H3MHERR_HIGHALARM,						///< Input flagged with high alarm.
	H3MHERR_TUNING,								///< Mag analog circuit tuning.
	H3MHERR_SEND,									///< Error sending data.
	H3MHERR_SENDLENGTH,						///< Send less than requested length.
	H3MHERR_MUTEX,								///< Error locking mutex.
	H3MHERR_PKTTOOLONG,						///< Packet length exceeded before terminated.
	H3MHERR_PKTBUILD,							///< Packet not yet completed.
	H3MHERR_PKTFORMAT,						///< Packet format incorrect.
};

/// ImageData16 errors.
enum
{
	CID16ERR_WAITFAIL=7100,	// Failed to obtain mutex ownership protecting an operation.
	CID16ERR_NULLDATA,				// The Image data array pointer is NULL, operation will fail.
	CID16ERR_BOUNDFAIL,				// A pixel index was beyond the image bounds.
	CID16ERR_ZEROSIZE,				// Attempted to create an image with one or both sizes <= 0.
	CID16ERR_FOPENFAIL,				// Failed to open read/write file.
	CID16ERR_FHEADER,					// File header (magic word) was invalid.
};

/// TimeHandler errors.
enum {
	TIMEHERR_OUTSTRINVALID = 7200, ///< Null pointer was passed as the output stream
	TIMEHERR_GPSHANDLRINVALID,		///< Null pointer was passed as the GPS handler object
	TIMEHERR_HANDLERRUNNING,		///< Time sync handler is already running.

};

/// DioDevice errors.
enum {
	DIODEV_ERR_PORTINVALID = 7300,			///< Attempted to read or write an invalid port #.
	DIODEV_ERR_BITINVALID,			///< Attempted to read or write an invalid bit #.
};

// EcTansPacket errors.
enum {
	TANSPACKET_DLEERR = 7400,		///< DLE indicated a new packet before an old one ended.
	TANSPACKET_LENERR,				///< Counted bytes exceeded maximum allowed packet length.
	TANSPACKET_OK,					///< Indicates a TANS packet has been completed.
};

/// Event handler errors.
enum {
	DEPERR_INVALIDDET = 7500,		///< Detector number invalid.
	DEPERR_MUTEXWAIT,				///< Mutex wait failed (timeout or other error).
};

//	EcFdrEventProcessor errors.
enum {
	EFEPERR_INIT = 7600,			///< Attempted multiple create calls on the same object.
	EFEPERR_NOTINIT,				///< Attempted an operation before creating object.
	EFEPERR_NOTHREAD,				///< Failed to start event processing thread.
	EFEPERR_THREAD,					///< Attempted to call thread function while running.
	EFEPERR_MUTEX,					///< Failed to gain ownership of data mutex.
	EFEPERR_DETNUM,					///< Invalid detector number.
};

/// EhkHeaterControl errors.
enum
{
  EHKHTRCTLERR_HTRID = 7700,  ///< Heater ID was out of bounds.
  EHKHTRCTLERR_TEMPID,        ///< Mux temperature ID was out of bounds.
};

/// NetworkTimeClient errors.
enum
{
  NTCLIENTERR_INIT = 7800,    ///< Client already exists on this host.
  NTCLIENTERR_INVALID,        ///< No valid data to calculate offset.
  NTCLIENTERR_NOTCLIENT,      ///< Current object is not the valid NT client.
  NTCLIENTERR_MUTEXERR,       ///< Error locking mutex.
  NTCLIENTERR_TIMEDOUT,       ///< Timed out waiting for response from server.
  NTCLIENTERR_RXNOTRUN,       ///< NT client receiver is not running.
  NTCLIENTERR_OUTOFBOUNDS,    ///< Input time difference was too different from mean value.
};

enum
{
	FDRERR_HDLCCREATE = 7900,	///< Error creating HDLC Stream
	FDRERR_DDRCREATE,			///< Error creating Detector Data Recorder
	FDRERR_DDRNOTEXIST,			///< Detector Data Record does not exist
	FDRERR_DDRRUNNING,			///< Error tried to start a DDR thread thats already running
	FDRERR_DDRNOTRUNNING,		///< Error tried to stop a non-existant DDR thread
	FDRERR_DDRJOINERROR,		///< Error when attempting to join with the DDR exiting thread
	FDRERR_DDRINVDETECTOR,		///< Error invalid detector number was given

	FDRERR_SCIOUTCREATE,		///< Error creating SciOutQueueHandler
	FDRERR_SCIOUTNOTEXIST,		///< SciOutQueueHandler does not exist
	FDRERR_SCIOUTRUNNING,		///< Error tried to start a SciOutQueueHandler thread thats already running
	FDRERR_SCIOUTNOTRUNNING,	///< Error tried to stop a non-existant SciOutQueueHandler thread
	FDRERR_SCIOUTJOINERROR,		///< Error when attempting to join with the SciOutQueueHandler exiting thread

	FDRERR_IMGHANDLERNOTEXIST,	///< Image Handler does not exist
};

enum
{
	QERR_MUTEXLOCK = 9000,      ///< Mutex failed to lock.
	QERR_SEMWAIT,               ///< Semaphore wait failed.
	QERR_LISTFULL,              ///< List reached max count.
	QERR_EXITED,                ///< Indicates semaphore is being deleted.
	QERR_NODATA,                ///< No data found in list.
};

enum
{
	LOGERR_FILEOPEN = 9100,		///< Could not open a file for writing
	LOGERR_NOTOPEN,				///< Attempted to write to a invalid file
	LOGERR_WRITE,				///< Was unable to write all data to file
};

enum
{
	IRCVERR_ALREADYCREATED = 9200,	///< Tried to instantiate another Image Receiver
};

#endif //__ERROR_CODES_H__
