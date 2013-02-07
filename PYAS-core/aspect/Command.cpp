#include <iostream>

#include "Command.hpp"

using std::ostream;

class CommandUnknownException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Unknown Command specified";
  }
} cmUnknownException;

class CommandPacketInvalidException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "CommandPacket is not valid (syncword or checksum)";
  }
} cpInvalidException;

class CommandPacketSizeException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "CommandPacket payload is too large";
  }
} cpSizeException;

class CommandPacketEndException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "CommandPacket end reached";
  }
} cpEndException;

class CommandQueueEmptyException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "CommandQueue has no more commands";
  }
} cqEmptyException;

Command::Command(const uint8_t *ptr)
{
  uint16_t heroes_cm = *((uint16_t *)ptr);
  uint16_t sas_cm = 0;
  if(heroes_cm == 0x10ff) sas_cm = *((uint16_t *)(ptr+2));

  uint16_t len = lookup_payload_length(heroes_cm, sas_cm);
  this->append_bytes(ptr, 2+len);
}

Command::Command(uint16_t heroes_c, uint16_t sas_c)
{
  if (heroes_c != 0) *this << heroes_c;
  if(sas_c != 0) *this << sas_c;
}

uint16_t Command::lookup_payload_length(uint16_t heroes_cm, uint16_t sas_cm)
{
  switch (heroes_cm) {
    case 0x1000:
    case 0x1001:
      return 0;
    case 0x10ff:
      return 2+lookup_sas_payload_length(sas_cm);

    case 0x1100:
    case 0x1101:
      return 0;
    case 0x1102:
      return 38;
    case 0x1103:
      return 2;
    case 0x1104:
      return 10;
    
    default:
      throw cmUnknownException;
  }
}

uint16_t Command::lookup_sas_payload_length(uint16_t sas_cm)
{
  switch(sas_cm) {
    default:
      return 0;
  }
}

uint16_t Command::get_heroes_command()
{
  uint16_t value;
  this->readAtTo(0, value); //no range checking!
  return value;
}

uint16_t Command::get_sas_command()
{
  if(get_heroes_command() != 0x10ff) return 0;

  uint16_t value;
  this->readAtTo(2, value); //no range checking!
  return value;
}

//This one's a bit weird to have to exist, but just roll with it
ByteString &operator<<(ByteString &bs, const Command &cm)
{
  return (bs << (ByteString)cm);
}

CommandPacket::CommandPacket(uint8_t i_targetID, uint16_t i_number)
  : targetID(i_targetID), number(i_number)
{
  //Zeros are payload length and checksum
  *this << targetID << (uint8_t)0 << number << (uint16_t)0;
  setReadIndex(8);
}

CommandPacket::CommandPacket(const uint8_t *ptr, uint16_t num)
  : Packet(ptr, num)
{
  setReadIndex(8);
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

bool CommandPacket::valid()
{
  return Packet::valid();
}

void CommandPacket::readNextCommandTo(Command &cm)
{
  if(remainingBytes() == 0) throw cpEndException;

  uint16_t heroes_cm, sas_cm = 0;
  readNextTo(heroes_cm);
  if(heroes_cm == 0x10ff) readNextTo(sas_cm);
  Command result(heroes_cm, sas_cm);
  uint16_t num = result.lookup_payload_length(heroes_cm, sas_cm);
  if(heroes_cm == 0x10ff) num -= 2;
  uint8_t buf[254];
  readNextTo_bytes(buf, num);
  result.append_bytes(buf, num);
  cm = result;
}

CommandQueue::CommandQueue(CommandPacket &cp)
{
  add_packet(cp);
}

int CommandQueue::add_packet(CommandPacket &cp)
{
  int count = 0;
  Command cm;

  if(!cp.valid()) throw cpInvalidException;

  while(cp.remainingBytes() > 0) {
    cp.readNextCommandTo(cm);
    *this << cm;
    count++;
  }

  return count;
}

CommandQueue &operator<<(CommandQueue &cq, Command &c)
{
  cq.push_back(c);
  return cq;
}

CommandQueue &operator<<(CommandQueue &cq, CommandQueue &cq2)
{
  cq.splice(cq.end(), cq2);
  return cq;
}

CommandQueue &operator>>(CommandQueue &cq, Command &c)
{
  if(cq.empty()) throw cqEmptyException;
  c = cq.front();
  cq.pop_front();
  return cq;
}

ostream &operator<<(ostream &os, CommandQueue &cq)
{
  if(cq.empty()) throw cqEmptyException;
  int i = 0;
  for (CommandQueue::iterator it=cq.begin(); it != cq.end(); ++it) {
    os << ++i << ": "<< *it << std::endl;
  }
  return os;
}
