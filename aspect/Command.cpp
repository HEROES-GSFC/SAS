#include <iostream>

#include "Command.hpp"

#define INDEX_TARGET_ID 2
#define INDEX_PAYLOAD_LENGTH 3
#define INDEX_SEQUENCE_NUMBER 4
#define INDEX_CHECKSUM 6
#define INDEX_PAYLOAD 8

using std::ostream;

class CommandUnknownException : public std::exception
{
    virtual const char* what() const throw()
        {
            return "Unknown Command specified";
        }
} cmUnknownException;

class CommandInvalidException : public std::exception
{
    virtual const char* what() const throw()
        {
            return "Command object has an incorrect number of associated bytes";
        }
} cmInvalidException;

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

Command::Command(uint16_t heroes_c, uint16_t sas_c)
{
    if (heroes_c != 0) {
        *this << heroes_c;
        this->setReadIndex(2);
        if(sas_c != 0) {
            *this << sas_c;
            this->setReadIndex(4);
        }
    }
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
    case 0x000A:
        return 8;
    
    default:
        throw cmUnknownException;
    }
}

uint16_t Command::lookup_sas_payload_length(uint16_t sas_cm)
{
    //The least significant 4 bits of a SAS command key
    //  are the number of associated 2-byte variables
    return 2*(sas_cm & 0x000f);
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

ByteString &operator<<(ByteString &bs, Command &cm)
{
    if(cm.getLength() != 2+cm.lookup_payload_length(cm.get_heroes_command(), cm.get_sas_command())) {
        throw cmInvalidException;
    }
    return (bs << (ByteString)cm);
}

CommandPacket::CommandPacket(uint8_t targetID, uint16_t number)
{
    //Zeros are payload length and checksum
    *this << targetID << (uint8_t)0 << number << (uint16_t)0;
    setReadIndex(INDEX_PAYLOAD);
}

CommandPacket::CommandPacket(const uint8_t *ptr, uint16_t num)
    : Packet(ptr, num)
{
    setReadIndex(INDEX_PAYLOAD);
}

CommandPacket::CommandPacket(const void *ptr)
{
    //Assumes that NULL was passed in
}

void CommandPacket::finish()
{
    writePayloadLength();
    writeChecksum();
}

void CommandPacket::writePayloadLength()
{
    if(getLength() > COMMAND_PACKET_MAX_SIZE) throw cpSizeException;
    //should check if payload length is even
    //should also check if payload length is >= 2 bytes
    replace(INDEX_PAYLOAD_LENGTH, (uint8_t)(getLength()-INDEX_PAYLOAD));
}

void CommandPacket::writeChecksum()
{
    replace(INDEX_CHECKSUM, (uint16_t)0);
    replace(INDEX_CHECKSUM, (uint16_t)checksum());
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
    uint8_t buf[COMMAND_PACKET_MAX_SIZE-INDEX_PAYLOAD];
    readNextTo_bytes(buf, num);
    result.append_bytes(buf, num);
    cm = result;
}

uint8_t CommandPacket::getTargetID()
{
    uint8_t value;
    this->readAtTo( INDEX_TARGET_ID, value);
    return value;
}

uint16_t CommandPacket::getSequenceNumber()
{
    uint16_t value;
    this->readAtTo(INDEX_SEQUENCE_NUMBER, value);
    return value;
}

CommandQueue::CommandQueue(CommandPacket &cp)
{
    add_packet(cp);
}

int CommandQueue::add_packet(CommandPacket &cp)
{
    if(!cp.valid()) throw cpInvalidException;

    Command cm(0x10ff, 0xffff);
    int count = 0;

    while(cp.remainingBytes() > 0) {
        cp.readNextCommandTo(cm);
        *this << cm;
        count++;
    }

    return count;
}
