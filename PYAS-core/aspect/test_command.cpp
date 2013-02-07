#include <iostream>

#include "Command.hpp"

int main()
{
  Command a(0x1000), b(0x1001), c(0x10FF), d;
  Command e("EEEE"), f("FFFFFF");
  CommandQueue cq, cq2;

  std::cout << "Tada!\n";

  cq.push_back("ABCD");
  cq.push_back(a);
  cq.push_back(b);
  cq.push_back(c);

  cq2 << e << f;

  cq << a << cq2;

  std::cout << cq;

  while (!cq.empty()) {
    cq >> d;
    std::cout << d << std::endl;
  }

  std::cout << std::endl;

  ByteString aa("01234567890ABCDEF");
  Packet pp;

  pp << aa;

  std::cout << pp << std::endl;
  std::cout << pp.valid() << std::endl;

  CommandPacket cc(0x01, 100);
  std::cout << cc.valid() << std::endl;
  std::cout << cc << std::endl;
  std::cout << cc.valid() << std::endl;

  ByteString solution;
  solution << (double)1 << (double)10 << (double)100 << (double)1000;
  solution << (uint32_t)5 << (uint16_t)6;
  CommandPacket cp2(0x01, 101);
  cp2 << Command(0x1102) << solution;
  cp2 << Command(0x1100);
  cp2 << Command(0x1104) << (uint16_t)1234 << (double)3.1415926;
  cp2 << Command(0x1102) << solution;

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
