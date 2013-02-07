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

class ByteStringFullException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "ByteString is full";
  }
} bsFullException;

class ByteStringAccessException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Bad index for accessing ByteString";
  }
} bsAccessException;

ByteString::ByteString() : length(0), read_index(0)
{
}

ByteString::ByteString(const char *str) : length(0), read_index(0)
{
  std::string sstr(str);
  for (uint16_t i=0;i<(uint16_t)sstr.length()/2;i++) {
    *this << (uint8_t)strtol(sstr.substr(i*2, 2).c_str(), NULL, 16);
  }
}

template <class T>
void ByteString::append(const T& value)
{
  append_bytes(&value, sizeof(value));
}

template <>
void ByteString::append<ByteString>(const ByteString& bs)
{
  append_bytes(bs.buffer, bs.length);
}

void ByteString::append_bytes(const void *ptr, uint16_t num)
{
  if(length+num > PACKET_MAX_SIZE) throw bsFullException;
  memcpy(buffer+length, ptr, num);
  length += num;
}

template <class T>
void ByteString::replace(uint16_t loc, const T& value)
{
  if(loc+sizeof(value) > length) throw bsAccessException;
  memcpy(buffer+loc, &value, sizeof(value));
}

template <class T>
void ByteString::readAtTo(uint16_t loc, T& value)
{
  readAtTo_bytes(loc, &value, sizeof(value));
}

void ByteString::readAtTo_bytes(uint16_t loc, void *ptr, uint16_t num)
{
  if(loc+num > length) throw bsAccessException;
  memcpy(ptr, buffer+loc, num);
}

void ByteString::setReadIndex(uint16_t loc)
{
  read_index = loc;
}

template <class T>
void ByteString::readNextTo(T& value)
{
  readNextTo_bytes(&value, sizeof(value));
}

void ByteString::readNextTo_bytes(void *ptr, uint16_t num)
{
  readAtTo_bytes(read_index, ptr, num);
  read_index += num;
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

template <class T>
ByteString& operator<<(ByteString& bs, const T& value) { bs.append(value); return bs; }

ostream& operator<<(ostream& os, ByteString& bs)
{
  bs.finish();
  for (uint16_t i=0;i<bs.length;i++) {
    os << pkt::byte << (int)bs.buffer[i]; //hex output does not work on uint8_t
  }
  return os << pkt::reset;
}

uint16_t operator>>(ByteString& bs, uint8_t dest[]) { return bs.outputTo(dest); }

uint16_t ByteString::checksum()
{
  unsigned short value = 0xffff;
  for(uint16_t i=0;i<length;i++) value = update_crc_16(value, (char)buffer[i]);
  //Flip the byte order for the checksum for writing as a word
  return ((value & 0xff) << 8) | (value >> 8);
}

Packet::Packet()
{
  *this << PACKET_HEROES_SYNC_WORD;
}

Packet::Packet(const uint8_t *ptr, uint16_t num)
{
  this->append_bytes(ptr, num);
}

bool Packet::valid()
{
  uint16_t syncword;
  this->readAtTo(0, syncword);
  bool syncword_valid = (syncword == PACKET_HEROES_SYNC_WORD);

  uint16_t alleged_checksum;
  //All packets should have the checksum at bytes 6 and 7
  this->readAtTo(6, alleged_checksum);
  this->replace(6, (uint16_t)0);
  bool checksum_valid = (this->checksum() == alleged_checksum);
  this->replace(6, alleged_checksum);

  return syncword_valid && checksum_valid;
}

namespace pkt
{
ostream& byte(ostream& os) { return os << std::hex << std::setw(2) << std::setfill('0'); }
ostream& word(ostream& os) { return os << std::hex << std::setw(4) << std::setfill('0'); }
ostream& reset(ostream& os) { return os << std::resetiosflags(std::ios_base::hex) << std::setfill(' '); }

//Ensures the compiler instantiates the necessary template functions
//Does not need to be actually run
void _template_loader()
{
  ByteString dummy;
  dummy << (uint8_t)0 << (uint16_t)0 << (uint32_t)0;
  dummy << (int8_t)0 << (int16_t)0 << (int32_t)0;
  dummy << (float)0 << (double)0;
  dummy << dummy;

  double temp;

  dummy.readNextTo(*(uint8_t *)(&temp));
  dummy.readNextTo(*(uint16_t *)(&temp));
  dummy.readNextTo(*(uint32_t *)(&temp));
  dummy.readNextTo(*(int8_t *)(&temp));
  dummy.readNextTo(*(int16_t *)(&temp));
  dummy.readNextTo(*(int32_t *)(&temp));
  dummy.readNextTo(*(float *)(&temp));
  dummy.readNextTo(*(double *)(&temp));

  dummy.replace(0, (uint8_t)0);
  dummy.replace(0, (uint16_t)0);
  dummy.replace(0, (uint32_t)0);
  dummy.replace(0, (int8_t)0);
  dummy.replace(0, (int16_t)0);
  dummy.replace(0, (int32_t)0);
  dummy.replace(0, (float)0);
  dummy.replace(0, (double)0);
}

}
