#include <iostream>
#include <iomanip>
#include <string>

#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/time.h>

#include "Packet.hpp"
#include "lib_crc/lib_crc.h"

using std::ostream;

ByteStringFullException bsFullException;
ByteStringAccessException bsAccessException;
CommandPacketSizeException cpSizeException;
TelemetryPacketSizeException tpSizeException;

ByteString::ByteString()
{
  length = 0;
}

ByteString::ByteString(const char *str)
{
  length = 0;
  std::string sstr(str);
  for (uint16_t i=0;i<(uint16_t)sstr.length()/2;i++) {
    *this << (uint8_t)strtol(sstr.substr(i*2, 2).c_str(), NULL, 16);
  }
}

template <class T>
void ByteString::append(T value)
{
  append(&value, sizeof(value));
}

void ByteString::append(const void *ptr, uint16_t num)
{
  if(length+num > PACKET_MAX_SIZE) throw bsFullException;
  memcpy(buffer+length, ptr, num);
  length += num;
}

void ByteString::appendBS(ByteString& bs)
{
  append(bs.buffer, bs.length);
}

template <class T>
void ByteString::replace(uint16_t loc, T value)
{
  if(loc+sizeof(value) > length) throw bsAccessException;
  memcpy(buffer+loc, &value, sizeof(value));
}

void ByteString::clear()
{
  length = 0;
}

uint16_t ByteString::outputTo(uint8_t dest[])
{
  finish();
  memcpy(dest, buffer, length);
  return length;
}

ByteString& operator<<(ByteString& bs, uint8_t value) { bs.append(value); return bs; }
ByteString& operator<<(ByteString& bs, uint16_t value) { bs.append(value); return bs; }
ByteString& operator<<(ByteString& bs, uint32_t value) { bs.append(value); return bs; }
ByteString& operator<<(ByteString& bs, double value) { bs.append(value); return bs; }
ByteString& operator<<(ByteString& bs, ByteString& other) { bs.appendBS(other); return bs; }

ostream& operator<<(ostream& os, ByteString& bs)
{
  bs.finish();
  for (uint16_t i=0;i<bs.length;i++) {
    os << pkt::byte << (int)bs.buffer[i]; //hex output does not work on uint8_t
  }
  return os << pkt::reset;
}

uint16_t operator>>(ByteString& bs, uint8_t dest[]) { return bs.outputTo(dest); }

Packet::Packet()
{
  *this << PACKET_HEROES_SYNC_WORD;
}

uint16_t ByteString::checksum()
{
  unsigned short value = 0xffff;
  for(uint16_t i=0;i<length;i++) value = update_crc_16(value, (char)buffer[i]);
  //Flip the byte order for the checksum for writing as a word
  return ((value & 0xff) << 8) | (value >> 8);
}

CommandPacket::CommandPacket(uint8_t i_targetID, uint16_t i_number)
  : targetID(i_targetID), number(i_number)
{
  //Zeros are payload length and checksum
  *this << targetID << (uint8_t)0 << number << (uint16_t)0;
}

void CommandPacket::finish()
{
  writePayloadLength();
  writeChecksum();
}

void CommandPacket::writePayloadLength()
{
  if(getLength()-8 > 254) throw cpSizeException;
  //should check if payload length is even
  //should also check if payload length is >= 2 bytes
  replace((uint16_t)3, (uint8_t)(getLength()-8));
}

void CommandPacket::writeChecksum()
{
  replace(6, (uint16_t)0);
  replace(6, (uint16_t)checksum());
}

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

namespace pkt
{
  ostream& byte(ostream& os) { return os << std::hex << std::setw(2) << std::setfill('0'); }
  ostream& word(ostream& os) { return os << std::hex << std::setw(4) << std::setfill('0'); }
  ostream& reset(ostream& os) { return os << std::resetiosflags(std::ios_base::hex) << std::setfill(' '); }
}
