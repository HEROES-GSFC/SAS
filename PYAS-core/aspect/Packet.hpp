/*

ByteString and Packet

These are base classes used by Command* and Telemetry* classes.  See the
documentation for those classes for examples of usage.

The recommended approach to append data to a ByteString is to use the insertion
operator <<, which can accept the following data types:
  uint8_t, uint16_t, uint32_t, int8_t, int16_t, int32_t, float, double,
  ByteString*
For classes (*), the actual data inserted is customized.  To insert an array,
you will need to use the append_bytes() method.

Once the ByteString is built to satisfaction, you can extract the data.  The
recommended approach is to use the extraction operator >> to a uint8_t array
pointer.  It is your responsibility to allocate the space before calling the
extraction; the necessary size can be retrieved by getLength(), or just use a
large enough destination array.

For convenience when testing, the packet can be inserted into an ostream for
hexadecimal output.

A variety of exceptions, derived from std::exception, can be thrown.

How to catch an exception:
  try {
    ...
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

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
    uint16_t read_index;

    //A hook to allow derived classes to apply finishing touches to the buffer
    //when outputTo() or >> is used
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

    template <class T>
    void readAtTo(uint16_t loc, T& value);

    void readAtTo_bytes(uint16_t loc, void *ptr, uint16_t num);

    void setReadIndex(uint16_t loc);

    template <class T>
    void readNextTo(T& value);

    void readNextTo_bytes(void *ptr, uint16_t num);

    uint16_t remainingBytes() { return length-read_index; };

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

class Packet : public ByteString {
  private:
    virtual void finish() {};

  public:
    Packet();
    Packet(const uint8_t *ptr, uint16_t num);
    virtual bool valid();
};

namespace pkt
{
  //Handy macros for ostream formatting
  std::ostream& byte(std::ostream& os);
  std::ostream& word(std::ostream& os);
  std::ostream& reset(std::ostream& os);
}

#endif
