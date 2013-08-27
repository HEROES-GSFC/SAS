/*

  ByteString, Packet, and ByteStringQueue

  These are base classes used by Command* and Telemetry* classes.  See the
  documentation for those classes for examples of usage.  Private variables
  should not added when inheriting from ByteString to keep casting safe.  For
  example, *Queue classes only contain ByteString entries, but can be cast to the
  "proper" type upon extraction.

  The recommended approach to append data to a ByteString is to use the insertion
  operator <<, which can accept the following data types:
      uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t,
      float, double, ByteString*
  For classes (*), the actual data inserted is customized.  To insert an array,
  you will need to use the append_bytes() method.

  Once the ByteString is built to satisfaction, you can extract the data using
  the extraction operator >>.  If you extract to a uint8_t array pointer, the
  entire buffer is extracted.  Otherwise, only enough bytes are read out to
  fill the data type, and a read pointer is advanced.  Alternatively, you can use
  outputTo() and readNextTo().

  It is your responsibility to allocate the space before calling the extraction.
  The necessary size can be retrieved by getLength(), or just use a large enough
  destination array.

  For convenience when testing, the ByteString can be inserted into an ostream for
  hexadecimal output.

  The ByteStringQueue class protects the insertion and extraction operators with
  mutex locking and unlocking.  If the queue is locked when attempting an
  operation, the operation will be blocked for up to 250 ms.  If the timeout is
  reach, an exception will be thrown.

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
#include <list>
#include <stdint.h>
#include <pthread.h>

#define PACKET_MAX_SIZE 1024
#define PACKET_HEROES_SYNC_WORD (uint16_t)0xc39a

//Useful base class, may wish to break out
class ByteString {
private:
    uint8_t buffer[PACKET_MAX_SIZE];
    uint16_t length;
    uint16_t read_index;

protected:
    //A hook to allow derived classes to apply finishing touches to the buffer
    //when outputTo() or >> is used
    virtual void finish() {};

public:
    ByteString();
    ByteString(const char *str); //from a null-terminated hexadecimal string

    uint16_t getLength() { return length; }

    //Appending to the end
    void append_bytes(const void *ptr, uint16_t num);
    template <class T>
    void append(const T& value);

    //Replacing at a specific location
    template <class T>
    void replace(uint16_t loc, const T& value);

    //Reading at a specific location
    void readAtTo_bytes(uint16_t loc, void *ptr, uint16_t num);
    template <class T>
    void readAtTo(uint16_t loc, T& value);

    //Sequential reading using a read pointer
    uint16_t getReadIndex();
    void setReadIndex(uint16_t loc);
    void readNextTo_bytes(void *ptr, uint16_t num);
    template <class T>
    void readNextTo(T& value);
    uint16_t remainingBytes() { return length-read_index; };

    //Output the entire buffer to an array
    uint16_t outputTo(uint8_t dest[]);

    uint16_t checksum();

    void clear();

    //insertion operator <<
    //Overloaded for appending and for stream output
    template <class T>
    friend ByteString& operator<<(ByteString& bs, const T& value);
    friend std::ostream& operator<<(std::ostream& os, ByteString& bs);

    //extraction operator >>
    //If T is a uint8_t pointer, then calls outputTo()
    //Otherwise, calls readNextTo()
    template<class T>
    friend ByteString& operator>>(ByteString& bs, T& dest);
};

class Packet : public ByteString {
protected:
    virtual void finish() {};

public:
    Packet();
    Packet(const uint8_t *ptr, uint16_t num);

    //Checks for a valid checksum
    virtual bool valid();
};

class ByteStringQueue : public std::list<ByteString> {
private:
    pthread_mutex_t flag;

public:
    ByteStringQueue();
    ~ByteStringQueue();

    //Mutex-based locking of the queue
    int lock();
    int unlock();

    //insertion operator <<
    //If a queue is inserted, the source queue will be emptied
    friend ByteStringQueue &operator<<(ByteStringQueue &bq, const ByteString &bs);
    friend ByteStringQueue &operator<<(ByteStringQueue &bq, ByteStringQueue &other);

    //insertion operator << for ostream
    friend std::ostream &operator<<(std::ostream &os, ByteStringQueue &bq);

    //extraction operator >>
    //Removes the entry from the queue
    friend ByteStringQueue &operator>>(ByteStringQueue &bq, ByteString &bs);
};

namespace pkt
{
    //Handy macros for ostream formatting
    std::ostream& byte(std::ostream& os);
    std::ostream& word(std::ostream& os);
    std::ostream& reset(std::ostream& os);
}

//Unlike Linux, OS X does not have a clock_gettime() function in time.h
//Here is a wrapper for gettimeofday() in sys/time.h to allow compilation
#if __DARWIN_UNIX03

#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1

int clock_gettime(int clk_id, struct timespec *tp);

#endif

#endif
