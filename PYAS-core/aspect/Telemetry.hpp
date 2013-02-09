/*

TelemetryPacket and TelemetryPacketQueue
  derived from Packet and std::list<TelemetryPacket>

//Telemetry packet from SAS containing an array
uint8_t image[5] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
TelemetryPacket tp2(0x70, 0x30);
tp2 << (uint32_t)0xEFBEADDE;
tp2.append_bytes(image, 5);
std::cout << tp2 << std::endl;

//Parsing telemetry packets from a static file
TelemetryPacketQueue tpq;
tpq.filterSourceID(0x70);
tpq.add_file("sample.dat");
TelemetryPacket tp;
if(!tpq.empty()) tpq >> tp;

*/

#ifndef _TELEMETRY_HPP_
#define _TELEMETRY_HPP_

#include <list>
#include <iostream>

#include "Packet.hpp"

#define TELEMETRY_PACKET_MAX_SIZE 1024

class TelemetryPacket : public Packet {
  private:
    virtual void finish();
    void writePayloadLength();
    void writeChecksum();
    void writeTime();

  public:
    //Use this constructor when assembling a telemetry packet for sending
    TelemetryPacket(uint8_t typeID, uint8_t sourceID);

    //Use this constructor when handling a received telemetry packet
    TelemetryPacket(const uint8_t *ptr, uint16_t num);

    //Checks for the HEROES sync word and a valid checksum
    virtual bool valid();

    uint8_t getTypeID();
    uint8_t getSourceID();
};

class TelemetryPacketQueue : public std::list<TelemetryPacket> {
  private:
    uint8_t i_typeID;
    uint8_t i_sourceID;
    bool filter_typeID;
    bool filter_sourceID;

  public:
    TelemetryPacketQueue();

    //Adds the telemetry packets from a static file
    void add_file(const char *file);

    void filterTypeID(uint8_t typeID);
    void filterSourceID(uint8_t typeID);
    void resetFilters();

  //insertion operator <<
  //Overloaded to add TelemetryPacket objects
  //  or TelemetryPacket objects from a TelemetryPacketQueue
  //In the latter case, the source TelemetryPacketQueue is emptied
  friend TelemetryPacketQueue &operator<<(TelemetryPacketQueue &tpq, const TelemetryPacket &tp);
  friend TelemetryPacketQueue &operator<<(TelemetryPacketQueue &tpq, TelemetryPacketQueue &other);

  //insertion operator << for ostream output
  friend std::ostream &operator<<(std::ostream &os, TelemetryPacketQueue &tpq);

  //extraction operator >> for popping off the next TelemetryPacket object
  friend TelemetryPacketQueue &operator>>(TelemetryPacketQueue &tpq, TelemetryPacket &tp);
};

#endif
