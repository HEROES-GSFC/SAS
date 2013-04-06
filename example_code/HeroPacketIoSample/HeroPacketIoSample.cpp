/**
 *  \file HeroPacketIoSample.cpp
 *  \brief  Demonstrates reading/writing of HeroGsePacket data.
 *  \date June 29, 2012.
 *  \author Kurt Dietz
 */

#include "stdafx.h"
#include "HeroGsePacket.h"
#include "HeroCommandPacket.h"
#include "FileStream.h"
#include "ErrorCodes.h"

// Forward declarations.
int writeTelemetryPacket();
int writeCommandPacket();
int readTelemetryPackets();
int readCommandPackets();

extern uint8_t CPU_NUM = CPU_GSE;

/**
 *  Windows compiler console application main routine.
 */
int _tmain(int argc, _TCHAR* argv[])
{
  int rval;
  // Write a telemetry packet to a file.
  if ( (rval = writeTelemetryPacket()) )
    return rval;

  // Write a command packet to a file.
  if ( (rval = writeCommandPacket()) )
    return rval;

  // Read telemetry packets from a file.
  if ( (rval = readTelemetryPackets()) )
    return rval;

  // Read telemetry packets from a file.
  if ( (rval = readCommandPackets()) )
    return rval;

  return 0;
}

/**
 *  Writes telemetry packets to a file.
 */
int writeTelemetryPacket()
{
  // Create and open the file stream.
  EcFileStream fStream;
  fStream.Open( "telem.out", false, true );

  // Create an HK command reply telemetry packet.
  HeroGsePacket packet(GSEPTYPE_HKCMDREPLY);

  // Set the data into the command packet.
  packet.SetCmdReply( 0xaa55 );

  // Now send the packet.
  int rval = packet.SendPacket( &fStream );

  // Close the file.
  fStream.Close();

  return rval;
}

/**
 *  Writes command packets to a file.
 */
int writeCommandPacket()
{
  // Create and open the file stream.
  EcFileStream fStream;
  fStream.Open( "cmd.out", false, true );

  // Create an command packet, ready it for output, and set the target ID.
  HeroCommandPacket packet;
  packet.ResetOutputPacket();
  packet.SetTargetID( CPU_ASP );

  // Set a command for the night-time aspect system to start tracking stars.
  packet.SetAspStartTrackingCmd( 0 );

  // Send the packet.
  packet.SendOutputPacket( &fStream );

  // Close the file.
  fStream.Close();

  return 0;
}

/**
 *  Reads telemetry packets from a file.
 */
int readTelemetryPackets()
{
  // Define the maximum packet size (for read-in buffer).
  const int size = sizeof(HeroGsePacket::Header) + GSEPKT_MAX_DATA_SIZE;

  // Create the read-in buffer.
  char buff[size];

  // Create the packet.
  HeroGsePacket packet;

  // Create and open the file stream.
  EcFileStream fStream;
  fStream.Open( "telem.in", true, false );

  // Set the length.
  int err, errcount=0, length = size;

	// Read in one buffer of data from the input file stream.
	while( (err = fStream.Receive( buff, &length )) == 0 && length > 0 )
	{
		// Loop through buffer until the start reaches the end of the buffer.
		int start = 0;
		while ( start < length )
		{
      // Process the input.  This function returns true when a valid packet
      //  is found in the input.
			if ( packet.ProcessInput( buff, &start, length, &errcount ) )
			{
        // Packet is valid!!
        // TBD:  Use the packet in some way.

        // Reset the packet so that new packet data can be accumulated.
        packet.Reset();
	    }
    }

    // Reset the length for the next read (not necessary for files, but
    //  can become necessary for serial streams and UDP streams).
    length = start;
  }

  // Close the stream.
  fStream.Close();

  return err;
}

/**
 *  Reads command packets from a file.
 */
int readCommandPackets()
{
  // Define the maximum packet size (for read-in buffer).
  const int size = sizeof(sizeof( EcCommandPacket::Header ) + CMDPKT_MAX_SIZE);

  // Create the read-in buffer.
  char buff[size];

  // Create the packet.
  HeroCommandPacket packet;

  // Create and open the file stream.
  EcFileStream fStream;
  fStream.Open( "cmd.in", true, false );

  // Set the length.
  int err, errcount=0, length = size;

	// Read in one buffer of data from the input file stream.
	while( (err = fStream.Receive( buff, &length )) == 0 && length > 0 )
	{
		// Loop through buffer until the start reaches the end of the buffer.
		int start = 0;
		while ( start < length )
		{
      // Process the input.  This function returns true when a valid packet
      //  is found in the input.
			if ( packet.ProcessInput( buff, &start, length, &errcount ) )
			{
        // Packet is valid!!
        // TBD:  Use the packet in some way.

        // Reset the packet so that new packet data can be accumulated.
        packet.Reset();
	    }
    }

    // Reset the length for the next read (not necessary for files, but
    //  can become necessary for serial streams and UDP streams).
    length = start;
  }

  // Close the stream.
  fStream.Close();

  return err;
}
