#include <sys/time.h>

#include "Telemetry.hpp"

class TelemetryPacketSizeException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "TelemetryPacket payload is too large";
  }
} tpSizeException;

TelemetryPacket::TelemetryPacket(uint8_t i_typeID, uint8_t i_sourceID)
  : typeID(i_typeID), sourceID(i_sourceID)
{
  //Zeros are payload length and checksum
  *this << typeID << sourceID << (uint16_t)0 << (uint16_t)0;
  //Zeros are microseconds and seconds
  *this << (uint32_t)0 << (uint32_t)0;
}

void TelemetryPacket::finish()
{
  writePayloadLength();
  writeTime();
  writeChecksum();
}

void TelemetryPacket::writePayloadLength()
{
  if(getLength()-16 > 1008) throw tpSizeException;
  replace(4, (uint16_t)(getLength()-16));
}

void TelemetryPacket::writeChecksum()
{
  replace(6, (uint16_t)0);
  replace(6, (uint16_t)checksum());
}

void TelemetryPacket::writeTime()
{
  timeval now;
  gettimeofday(&now, NULL);
  replace(8, (uint32_t)now.tv_usec);
  replace(12, (uint32_t)now.tv_sec);
}
