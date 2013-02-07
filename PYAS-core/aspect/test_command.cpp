#include <iostream>

#include "Command.hpp"

int main()
{
  ByteString solution;
  solution << (double)1 << (double)10 << (double)100 << (double)1000;
  solution << (uint32_t)5 << (uint16_t)6;
  Command cm1(0x1100);
  Command cm2(0x1102);
  cm2 << solution;
  std::cout << cm2.remainingBytes() << std::endl;
  Command cm3(0x10ff, 0x1234);
  Command cm4(0x10ff, 0x1234);
  cm4 << 0xABCD;

  CommandPacket cp2(0x01, 101);
  cp2 << cm1 << cm2 << cm3;

  try {
    cp2 << cm4;
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  uint16_t length = cp2.getLength();
  uint8_t *array = new uint8_t[length];
  try
  {
  cp2 >> array;
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  std::cout << cp2 << std::endl;

  CommandPacket k(array, length);

  std::cout << k.valid() << std::endl;
  k.replace(6, 0x1234);
  std::cout << k.valid() << std::endl;

  CommandQueue cq3(cp2);

  std::cout << cq3;

  try {
    cq3.add_packet(k);
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }


  return 0;
}
