#include <iostream>

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
