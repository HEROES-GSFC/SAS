/*

Pair, Pair3B

These classes are for pairs of doubles.  Pair is not particularly more useful
than other alternatives, but Pair3B is critical for writing reduced-precision
pixel location floats to ByteStrings.  Two floats or doubles are stored in only
3 bytes by rounding to the nearest third of a pixel.  Pair3B has defined
behavior only for floats between -35 and 1330; outside of that, all bets are
off (but no exception is thrown).

For now, the actual values are kept private, and can be read by using x() and
y().  The only way to set the values after the construction is using the
ByteString extraction operator.

To write a Pair3B:
  Pair3B a(123.45,56.78);
  ByteString bs;
  bs << a;

To write a Pair as a Pair3B:
  Pair p(135.79,246.80);
  bs << Pair3B(p);

To read a Pair3B:
  Pair3B b;
  bs >> b;

*/


#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include <iostream>
#include <stdint.h>

#include "Packet.hpp"

class Pair;
class Pair3B;

class Pair {
  private:
    double i_x, i_y;

  public:
    Pair(double x = 0, double y = 0);
    Pair(const Pair3B& p3);

    double x() const { return i_x; };
    double y() const { return i_y; };

    friend ByteString& operator<<(ByteString& bs, const Pair& p);
    friend std::ostream& operator<<(std::ostream& os, const Pair& p);

    friend ByteString& operator>>(ByteString& bs, Pair& p);
};

class Pair3B {
  private:
    uint16_t i_a, i_b;

    void initialize(double x, double y);

  public:
    Pair3B(double x = 0, double y = 0);
    Pair3B(const Pair& p);

    double x() const;
    double y() const;

    friend ByteString& operator<<(ByteString& bs, const Pair3B& p3);
    friend std::ostream& operator<<(std::ostream& os, const Pair3B& p3);

    friend ByteString& operator>>(ByteString& bs, Pair3B& p3);
};

#endif
