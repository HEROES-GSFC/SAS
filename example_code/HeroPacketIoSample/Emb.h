//	EMB.H
//		Header file to define fundamental types and constants.

#ifndef __EMB_H__
#define __EMB_H__

#define EXIST_MAX_DATA 20

#include <stdint.h>

//	Byte/short/integer union.
union U_ISBYTE
{
	int32_t		ival;			// Integer value.
	int16_t	sval[2];			// Short values.
	uint8_t	cval[4];			// character values.
};

/// The EXIST system identifiers.
enum EXIST_CPU_NUM
{
  CPU_FDR = 0x00,
	CPU_CTL = 0x01,
	CPU_ASP = 0x02,
  CPU_EHK = 0X03,
  CPU_DPS1 = 0x10,
  CPU_DPS2 = 0x11,
  CPU_DPS3 = 0x12,
  CPU_DPS4 = 0x13,
	CPU_DK = 0xd0,
	CPU_SRV = 0x80,
	CPU_GSE = 0x81,
	CPU_ALL = 0xff,
};

// Define the number of detectors.
#define NUM_HERO_CPUS 8
#define NUM_HERO_DPS 4
#define NUM_HERO_DETECTORS 8

// Define the addresses of each computer.
#define FDR_ADDR "192.168.1.1"
#define CTL_ADDR "192.168.1.2"
#define ASP_ADDR "192.168.1.3"
#define EHK_ADDR "192.168.1.4"
#define DPS1_ADDR "192.168.1.6"
#define DPS2_ADDR "192.168.1.7"
#define DPS3_ADDR "192.168.1.8"
#define DPS4_ADDR "192.168.1.9"
#define GSE_ADDR  "192.168.1.100"
#define HK_ADDR FDR_ADDR

// Define the addresses of each computer as binary numbers.
#define FDR_ADDR_NUM	0x0101a8c0
#define CTL_ADDR_NUM	0x0201a8c0
#define ASP_ADDR_NUM	0x0301a8c0
#define EHK_ADDR_NUM	0x0401a8c0
#define DPS1_ADDR_NUM	0x0601a8c0
#define DPS2_ADDR_NUM	0x0701a8c0
#define DPS3_ADDR_NUM	0x0801a8c0
#define DPS4_ADDR_NUM	0x0901a8c0

// Enumerated port address constants.
enum
{
	COMMAND_PORT = 2000,
	GROUND_COMMAND_PORT = 2001,
	DATA_PORT = 2002,
	GROUND_DATA_PORT = 2003,
	SCI_DATA_PORT1 = 2004,
	SCI_DATA_PORT2 = 2005,
	SCI_DATA_PORT3 = 2006,
	SCI_DATA_PORT4 = 2007,
        PROC_DATA_PORT = 2012,
	RAWIM_DATA_PORT = 2013,
        HSK_SERVER_COMMAND_PORT = 3000,
        SCI_SERVER_COMMAND_PORT = 3001
};

//	Define the stow pin DIO address.
#define STOWPIN_DIO_ADDR 'Z'

// Enumerate the class types.
enum
{
	CTYPE_TIME_SERVER = 1,		// Time server class type.
	CTYPE_TIME_CLIENT,			// Time client class type.
	CTYPE_DIO_HANDLER,			// Dio handler class type.
};

/// Enumerated geographic position sources.
enum GEOPOS_SRC
{
	GEOSRC_FDR_GPS = 0,	///< Source of geographic position is FDR GPS.
  GEOSRC_CTL_GPS,     ///< Source of geographic position is CTL GPS.
	GEOSRC_DGPS,			  ///< Source of geographic position is DGPS.
	GEOSRC_MANUAL,			///< Source of geographic position is manual setting from the GSE.
};

// Customize this define for each CPU.
extern uint8_t CPU_NUM;

#endif // #ifndef __EMB_H__
