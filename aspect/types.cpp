#include <iostream>
#include <algorithm>

#include <string.h>
#include <math.h>

#include "types.hpp"

#define PAIR3B_OFFSET 105 //shifts the range of the Pair3B "float", should be >= 1
#define FLOAT2B_OFFSET 8192 //shifts the range of the Float2B "float", should be >= 1

using std::ostream;

Pair::Pair(double x, double y) : i_x(x), i_y(y) {}
Pair::Pair(const Pair3B& p3) : i_x(p3.x()), i_y(p3.y()) {}

Pair operator+(const Pair& a, const Pair& b)
{
    return Pair(a.x()+b.x(), a.y()+b.y());
}

Pair operator-(const Pair& a, const Pair& b)
{
    return Pair(a.x()-b.x(), a.y()-b.y());
}

Pair operator*(const Pair& a, const Pair& b)
{
    return Pair(a.x()*b.x(), a.y()*b.y());
}

ByteString& operator<<(ByteString& bs, const Pair& p)
{
    return bs << p.x() << p.y();
}

ostream& operator<<(ostream& os, const Pair& p)
{
    return os << "(" << p.x() << ", " << p.y() << ")";
}

ByteString& operator>>(ByteString& bs, Pair& p)
{
    return bs >> p.i_x >> p.i_y;
}

Pair3B::Pair3B(double x, double y)
{
    initialize(x, y);
}

Pair3B::Pair3B(const Pair& p)
{
    initialize(p.x(), p.y());
}

void Pair3B::initialize(double x, double y)
{
    //Needs bounds checking!
    i_a = (uint16_t)floor((x*3+PAIR3B_OFFSET)+0.5);
    i_b = (uint16_t)floor((y*3+PAIR3B_OFFSET)+0.5);
}

double Pair3B::x() const { return ((double)i_a-PAIR3B_OFFSET)/3; }
double Pair3B::y() const { return ((double)i_b-PAIR3B_OFFSET)/3; }

ByteString& operator<<(ByteString& bs, const Pair3B& p3)
{
    uint8_t buffer[3];
    buffer[0] = (uint8_t)((p3.i_a & 0x0ff0) >> 4);
    buffer[1] = (uint8_t)(((p3.i_a & 0x000f) << 4) | ((p3.i_b & 0x0f00) >> 8));
    buffer[2] = (uint8_t)(p3.i_b & 0x00ff);
    bs.append_bytes(buffer, 3);
    return bs;
}

ostream& operator<<(ostream& os, const Pair3B& p3)
{
    return os << "(" << p3.x() << ", " << p3.y() << ")";
}

ByteString& operator>>(ByteString& bs, Pair3B& p3)
{
    uint8_t buffer[3];
    bs.readNextTo_bytes(buffer, 3);
    p3.i_a = (buffer[0] << 4) | ((buffer[1] & 0xf0) >> 4);
    p3.i_b = ((buffer[1] & 0x0f) << 8) | buffer[2];
    return bs;
}

Float2B::Float2B(const float value)
{
    //Needs bounds checking!
    i_value = (uint16_t)floor((value*8+FLOAT2B_OFFSET)+0.5);
}

Float2B::Float2B(const uint16_t value) : i_value(value) {}

float Float2B::value() const { return ((float)i_value-FLOAT2B_OFFSET)/8; }
uint16_t Float2B::code() const { return i_value; }

ByteString& operator<<(ByteString& bs, const Float2B& f2)
{
    return bs << f2.i_value;
}

ostream& operator<<(ostream& os, const Float2B& f2)
{
    return os << f2.value();
}

ByteString& operator>>(ByteString& bs, Float2B& f2)
{
    return bs >> f2.i_value;
}

uint64_t bitread(const void *ptr, uint16_t bit_location, uint8_t nbits)
{
    uint8_t *buf = (uint8_t *)ptr;
    if (nbits > 64) nbits = 64;

    uint16_t start_byte = bit_location/8;
    uint16_t end_byte = ceil((bit_location+nbits)/8.)-1;
    uint8_t start_bit = bit_location % 8;
    uint8_t nbytes = end_byte-start_byte+1;

    uint64_t result = 0;

    memcpy(&result, buf+start_byte, std::min(nbytes,(uint8_t)8));
    result = result >> start_bit;
    //If the field spilled over into a ninth bit, grab that portion too
    if (nbytes > 8) result |= buf[start_byte+8] << (64-start_bit);

    //Mask out the bits we didn't ask for
    result &= (1 << nbits) - 1;

    return result;
}

void bitwrite(void *ptr, uint16_t bit_location, uint8_t nbits, uint64_t input)
{
    uint8_t *buf = (uint8_t *)ptr;
    if (nbits > 64) nbits = 64;

    uint16_t start_byte = bit_location/8;
    uint16_t end_byte = ceil((bit_location+nbits)/8.)-1;
    uint8_t start_bit = bit_location % 8;
    uint8_t remainder_bits = (bit_location+nbits) % 8;
    uint8_t nbytes = end_byte-start_byte+1;

    uint64_t result = (input & ((1 << nbits) - 1)) << start_bit;

    uint8_t buf2[9];
    memcpy(buf2, &result, std::min(nbytes,(uint8_t)8));
    if (nbytes > 8) {
        buf2[8] = (input & ((1 << nbits) - 1)) >> (bit_location+nbits)/8;
    }

    buf2[0] |= buf[start_byte] & ((1 << start_bit) - 1);
    buf2[end_byte-start_byte] |= buf[end_byte] & ~((1 << remainder_bits) - 1);

    memcpy(buf+start_byte, buf2, nbytes);
}
