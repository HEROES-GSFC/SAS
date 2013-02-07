/*

//Telemetry packet from SAS containing an array
uint8_t image[5] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
TelemetryPacket tp2(0x70, 0x30);
tp2 << (uint32_t)0xEFBEADDE;
tp2.append_bytes(image, 5);
std::cout << tp2 << std::endl;

*/

#ifndef _TELEMETRY_HPP_
#define _TELEMETRY_HPP_

#include "Packet.hpp"

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

#endif
