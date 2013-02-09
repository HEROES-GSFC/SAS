#include <sys/time.h>

#include "Telemetry.hpp"

#define INDEX_TELEMETRY_TYPE 2
#define INDEX_SOURCE_ID 3
#define INDEX_PAYLOAD_LENGTH 4
#define INDEX_CHECKSUM 6
#define INDEX_NANOSECONDS 8
#define INDEX_SECONDS 12
#define INDEX_PAYLOAD 16

using std::ostream;

class TelemetryPacketSizeException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "TelemetryPacket payload is too large";
  }
} tpSizeException;

class TelemetryPacketQueueEmptyException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "TelemetryPacketQueue has no more telemetry packets";
  }
} tpqEmptyException;

TelemetryPacket::TelemetryPacket(uint8_t typeID, uint8_t sourceID)
{
  //Zeros are payload length and checksum
  *this << typeID << sourceID << (uint16_t)0 << (uint16_t)0;
  //Zeros are nanoseconds and seconds
  *this << (uint32_t)0 << (uint32_t)0;
  setReadIndex(INDEX_PAYLOAD);
}

TelemetryPacket::TelemetryPacket(const uint8_t *ptr, uint16_t num)
  : Packet(ptr, num)
{
  setReadIndex(INDEX_PAYLOAD);
}

void TelemetryPacket::finish()
{
  writePayloadLength();
  writeTime();
  writeChecksum();
}

void TelemetryPacket::writePayloadLength()
{
  if(getLength() > TELEMETRY_PACKET_MAX_SIZE) throw tpSizeException;
  replace(INDEX_PAYLOAD_LENGTH, (uint16_t)(getLength()-INDEX_PAYLOAD));
}

void TelemetryPacket::writeChecksum()
{
  replace(INDEX_CHECKSUM, (uint16_t)0);
  replace(INDEX_CHECKSUM, (uint16_t)checksum());
}

void TelemetryPacket::writeTime()
{
  timeval now;
  gettimeofday(&now, NULL);
  replace(INDEX_NANOSECONDS, (uint32_t)now.tv_usec*1000);
  replace(INDEX_SECONDS, (uint32_t)now.tv_sec);
}

bool TelemetryPacket::valid()
{
  return Packet::valid();
}

uint8_t TelemetryPacket::getTypeID()
{
  uint8_t value;
  this->readAtTo(INDEX_TELEMETRY_TYPE, value);
  return value;
}

uint8_t TelemetryPacket::getSourceID()
{
  uint8_t value;
  this->readAtTo(INDEX_SOURCE_ID, value);
  return value;
}

TelemetryPacketQueue &operator<<(TelemetryPacketQueue &tpq, const TelemetryPacket &tp)
{
  tpq.push_back(tp);
  return tpq;
}

TelemetryPacketQueue &operator<<(TelemetryPacketQueue &tpq, TelemetryPacketQueue &other)
{
  tpq.splice(tpq.end(), other);
  return tpq;
}

TelemetryPacketQueue &operator>>(TelemetryPacketQueue &tpq, TelemetryPacket &tp)
{
  if(tpq.empty()) throw tpqEmptyException;
  tp = tpq.front();
  tpq.pop_front();
  return tpq;
}

ostream &operator<<(ostream &os, TelemetryPacketQueue &tpq)
{
  if(tpq.empty()) throw tpqEmptyException;
  int i = 0;
  for (TelemetryPacketQueue::iterator it=tpq.begin(); it != tpq.end(); ++it) {
    os << ++i << ": "<< *it << std::endl;
  }
  return os;
}
