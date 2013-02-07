/*

Command, CommandPacket, and CommandQueue
  derived from ByteString, Packet, and std::list<Command>

To create a command:
  Command cm1(0x1100);
  Command cm2(0x1102);
  ByteString solution;
  solution << (double)1 << (double)10 << (double)100 << (double)1000;
  solution << (uint32_t)5 << (uint16_t)6;
  cm2 << solution;
  Command cm3(0x10ff, 0x1234);

To create a command packet:
  CommandPacket cp1(0x01, 100); //target ID and sequence number
  cp << cm1 << cm2 << cm3;

To get a uint8_t array from the command packet:
  uint16_t length = cp1.getLength();
  uint8_t *array = new uint8_t[length];
  cp1 >> array; //or cp1.outputTo(array);

To get a uint8_t array into a command packet:
  CommandPacket cp2(array, length);

To get a queue of commands from a command packet:
  CommandQueue cq;
  if(cp2.valid()) cq.add_packet(cp2);

To extract the next command from a queue of commands:
  Command c;
  if(!cq.empty()) cq >> c;

To interpret the command:
  c.get_heroes_command();
  c.get_sas_command(); //returns 0 if the HEROES command is not 0x10ff

Read the first uint16_t from an individual command's payload (after the keys):
  uint16_t x;
  c.readNextTo(x);


Notes:
- Make sure to build exactly complete commands
- Currently, all SAS commands have a zero-length payload
- A proper checksum and payload length are written to the HEROES header of a
  packet when either outputted to an array or to an ostream

*/

#ifndef _COMMAND_HPP_
#define _COMMAND_HPP_

#include <list>
#include <iostream>

#include "Packet.hpp"

class Command : public ByteString {
  private:

  public:
    Command(uint16_t heroes_c = 0, uint16_t sas_c = 0);

    //Unsupported constructors
    Command(const char *str) : ByteString(str) {};
    Command(const uint8_t *ptr);

    //Retrieve the command keys
    uint16_t get_heroes_command();
    uint16_t get_sas_command();

    uint16_t lookup_payload_length(uint16_t heroes_cm, uint16_t sas_cm = 0);
    uint16_t lookup_sas_payload_length(uint16_t sas_cm);

  //insertion operator << for adding a Command object to a CommandPacket object
  friend ByteString &operator<<(ByteString &bs, const Command &cm);
};

class CommandPacket : public Packet {
  private:
    uint8_t targetID;
    uint16_t number;

    virtual void finish();
    void writePayloadLength();
    void writeChecksum();

  public:
    //Use this constuctor when assembling a command packet for sending
    CommandPacket(uint8_t i_targetID, uint16_t i_number);

    //Use this constructor when handling a received command packet
    CommandPacket(const uint8_t *ptr, uint16_t num);

    //Checks for the HEROES sync word and a valid checksum
    virtual bool valid();

    void readNextCommandTo(Command &cm);
};

class CommandQueue : public std::list<Command> {

  public:
    CommandQueue() {};
    CommandQueue(CommandPacket &cp);

    //Returns the number of commands added
    int add_packet(CommandPacket &cp);

  //insertion operator <<
  //Overloaded to add Command objects or Command objects from a CommandQueue
  //In the latter case, the source CommandQueue is emptied
  friend CommandQueue &operator<<(CommandQueue &cq, Command &c);
  friend CommandQueue &operator<<(CommandQueue &cq, CommandQueue &cq2);

  //insertion operator << for ostream output
  friend std::ostream &operator<<(std::ostream &os, CommandQueue &cq);

  //extraction operator >> for popping off the next Command object
  friend CommandQueue &operator>>(CommandQueue &cq, Command &c);
};

#endif
