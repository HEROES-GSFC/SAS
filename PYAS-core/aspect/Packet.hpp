/*

CommandPacket and TelemetryPacket

For each type of packet, the constructor requires inputs.  For CommandPacket,
the inputs are the target ID and the sequence number.  For TelemetryPacket, the
inputs are the data type and the source ID.

After the packet is created, you can append data to the packet.  The recommended
approach is to use the insertion operator <<, which can accept the following
data types:
  uint8_t, uint16_t, uint32_t, int8_t, int16_t, int32_t, float, double,
  ByteString*
For classes (*), the actual data inserted is customized.  To insert an array,
you will need to use the append_bytes() method.

Once the packet is built to satisfaction, you can extract the data.  The
recommended approach is to use the extraction operator >> to a uint8_t array
pointer.  It is your responsibility to allocate the space before calling the
extraction; the necessary size can be retrieved by getLength(), or just use a
large enough destination array.  By design, the payload length and the checksum
are recorded upon extraction, not at any point before.

For convenience when testing, the packet can be inserted into an ostream for
hexadecimal output.  This call will trigger the recording of the payload length
and checksum.

A variety of exceptions, derived from std::exception, can be thrown.


Examples:

//SAS-to-CTL command acknowledging that it should be providing solutions
//Then print it out to stdout
CommandPacket cp1(0x01, 100);
cp1 << (uint16_t)0x1100;
std::cout << cp1 << std::endl;

//SAS-to-CTL command providing a solution
//Uses the class ByteString to help build the packet
//Then the packet is extracted to an allocated uint8_t array
ByteString solution;
solution << (double)1 << (double)10 << (double)100 << (double)1000;
solution << (uint32_t)5 << (uint16_t)6;
CommandPacket cp2(0x01, 101);
cp2 << (uint16_t)0x1102 << solution;
uint8_t *array = new uint8_t[cp2.getLength()];
cp2 >> array;

//Telemetry packet from SAS acknowledging the receipt of command 200
//Here with handling of exceptions
try
{
  TelemetryPacket tp1(0x01, 0x30);
  tp1 << (uint16_t)200;
  std::cout << tp1 << std::endl;
}
catch (std::exception& e)
{
  std::cerr << e.what() << std::endl;
}

//Telemetry packet from SAS containing an array
uint8_t image[5] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
TelemetryPacket tp2(0x70, 0x30);
tp2 << (uint32_t)0xEFBEADDE;
tp2.append_bytes(image, 5);
std::cout << tp2 << std::endl;


Inheritance tree:

  ByteString
      |
      |
    Packet
      |
      +---------------+
      |               |
CommandPacket  TelemetryPacket

*/

#ifndef _PACKET_HPP_
#define _PACKET_HPP_

#include <iostream>
#include <stdint.h>

#define PACKET_MAX_SIZE 1024
#define PACKET_HEROES_SYNC_WORD (uint16_t)0xc39a

//Useful base class, may wish to break out
class ByteString {
  private:
    uint8_t buffer[PACKET_MAX_SIZE];
    uint16_t length;

    //This is a hook to allow derived classes to apply finishing touches
    //to the buffer when outputTo() or >> is used
    virtual void finish() {};

  public:
    ByteString();
    ByteString(const char *str); //from a null-terminated hexadecimal string

    uint16_t getLength() { return length; }

    template <class T>
    void append(const T& value);

    void append_bytes(const void *ptr, uint16_t num);

    template <class T>
    void replace(uint16_t loc, const T& value);

    void clear();

    uint16_t outputTo(uint8_t dest[]);

    uint16_t checksum();

    //insertion operator <<
    //Overloaded for appending and for stream output
    template <class T>
    friend ByteString& operator<<(ByteString& bs, const T& value);
    friend std::ostream& operator<<(std::ostream& os, ByteString& bs);

    //extraction operator >>
    friend uint16_t operator>>(ByteString& pk, uint8_t dest[]);
};

class ByteStringFullException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "ByteString is full";
  }
};

class ByteStringAccessException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Bad index for accessing ByteString";
  }
};

class Packet : public ByteString {
  private:
    virtual void finish() {};

  public:
    Packet();
};

class CommandPacket : public Packet {
  private:
    uint8_t targetID;
    uint16_t number;

    virtual void finish();
    void writePayloadLength();
    void writeChecksum();

  public:
    CommandPacket(uint8_t i_targetID, uint16_t i_number);
};

class CommandPacketSizeException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "CommandPacket payload is too large";
  }
};

class TelemetryPacket : public Packet {
  private:
    uint8_t typeID;
    uint8_t sourceID;

    virtual void finish();
    void writePayloadLength();
    void writeChecksum();
    void writeTime();

  public:
    TelemetryPacket(uint8_t i_typeID, uint8_t i_sourceID);
};

class TelemetryPacketSizeException : public std::exception
{
  virtual const char* what() const throw()
  {
    return "TelemetryPacket payload is too large";
  }
};

namespace pkt
{
  //Handy macros for ostream formatting
  std::ostream& byte(std::ostream& os);
  std::ostream& word(std::ostream& os);
  std::ostream& reset(std::ostream& os);
}

#endif
