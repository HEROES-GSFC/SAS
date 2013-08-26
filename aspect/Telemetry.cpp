#include <sys/time.h>
#include <time.h>

#include <fstream>
#include <iostream>

#include "Telemetry.hpp"

#define INDEX_TELEMETRY_TYPE 2
#define INDEX_SOURCE_ID 3
#define INDEX_PAYLOAD_LENGTH 4
#define INDEX_CHECKSUM 6
#define INDEX_NANOSECONDS 8
#define INDEX_SECONDS 12
#define INDEX_PAYLOAD 16

using std::ostream;

class TelemetryPacketSizeException : public std::exception
{
    virtual const char* what() const throw()
        {
            return "TelemetryPacket payload is too large";
        }
} tpSizeException;

class TelemetryPacketSASException : public std::exception
{
    virtual const char* what() const throw()
        {
            return "TelemetryPacket error manipulating SAS ID";
        }
} tpSASException;

TelemetryPacket::TelemetryPacket(uint8_t typeID, uint8_t sourceID)
{
    //Zeros are payload length and checksum
    *this << typeID << sourceID << (uint16_t)0 << (uint16_t)0;
    //Zeros are nanoseconds and seconds
    *this << (uint32_t)0 << (uint32_t)0;
    setReadIndex(INDEX_PAYLOAD);
}

TelemetryPacket::TelemetryPacket(const uint8_t *ptr, uint16_t num)
    : Packet(ptr, num)
{
    setReadIndex(INDEX_PAYLOAD);
}

TelemetryPacket::TelemetryPacket(const void *ptr)
{
    //Assumes that NULL was passed in
}

void TelemetryPacket::finish()
{
    writePayloadLength();
    writeChecksum();
}

void TelemetryPacket::writePayloadLength()
{
    if(getLength() > TELEMETRY_PACKET_MAX_SIZE) throw tpSizeException;
    replace(INDEX_PAYLOAD_LENGTH, (uint16_t)(getLength()-INDEX_PAYLOAD));
}

void TelemetryPacket::writeChecksum()
{
    replace(INDEX_CHECKSUM, (uint16_t)0);
    replace(INDEX_CHECKSUM, (uint16_t)checksum());
}

bool TelemetryPacket::valid()
{
    return Packet::valid();
}

uint8_t TelemetryPacket::getTypeID()
{
    uint8_t value;
    this->readAtTo(INDEX_TELEMETRY_TYPE, value);
    return value;
}

uint8_t TelemetryPacket::getSourceID()
{
    uint8_t value;
    this->readAtTo(INDEX_SOURCE_ID, value);
    return value;
}

uint16_t TelemetryPacket::getSync()
{
    uint16_t value;
    this->readAtTo(0, value);
    return value;
}

uint32_t TelemetryPacket::getSeconds()
{
    uint32_t value;
    this->readAtTo(INDEX_SECONDS, value);
    return value;
}

uint32_t TelemetryPacket::getNanoseconds()
{
    uint32_t value;
    this->readAtTo(INDEX_NANOSECONDS, value);
    return value;
}

int TelemetryPacket::getSAS()
{
    uint16_t value;
    if(getLength() < INDEX_PAYLOAD+sizeof(value)) throw tpSASException;
    readAtTo(INDEX_PAYLOAD, value);
    setReadIndex(INDEX_PAYLOAD+sizeof(value));
    switch(value) {
    case SAS1_SYNC_WORD:
        return 1;
    case SAS2_SYNC_WORD:
        return 2;
    default:
        throw tpSASException;
    }
    return 0; //never reached
}

void TelemetryPacket::setSAS(int id)
{
    uint16_t value;
    switch(id) {
    case 1:
        value = SAS1_SYNC_WORD;
        break;
    case 2:
        value = SAS2_SYNC_WORD;
        break;
    default:
        throw tpSASException;
    }
    if(getLength() == INDEX_PAYLOAD) {
        append(value);
    } else if(getLength() >= INDEX_PAYLOAD+sizeof(value)) {
        replace(INDEX_PAYLOAD, value);
    } else throw tpSASException;
}

void TelemetryPacket::setTimeAndFinish()
{
    timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    setTimeAndFinish(now);
}

void TelemetryPacket::setTimeAndFinish(const struct timespec &time)
{
    replace(INDEX_NANOSECONDS, (uint32_t)time.tv_nsec);
    replace(INDEX_SECONDS, (uint32_t)time.tv_sec);
    finish();
}

TelemetryPacketQueue::TelemetryPacketQueue() : filter_typeID(false), filter_sourceID(false)
{
}

void TelemetryPacketQueue::filterTypeID(uint8_t typeID)
{
    filter_typeID = true;
    i_typeID = typeID;
}

void TelemetryPacketQueue::filterSourceID(uint8_t sourceID)
{
    filter_sourceID = true;
    i_sourceID = sourceID;
}

void TelemetryPacketQueue::resetFilters()
{
    filter_typeID = false;
    filter_sourceID = false;
}

void TelemetryPacketQueue::add_file(const char* file)
{
    uint32_t ct_sync = 0, ct_length = 0, ct_valid = 0;
    uint32_t ct_typeID = 0, ct_sourceID = 0;
    std::streampos cur;

    bool pass_sourceID, pass_typeID;

    uint8_t buffer[TELEMETRY_PACKET_MAX_SIZE];
    buffer[0] = 0x9a;

    uint16_t length;

    TelemetryPacket tp((uint8_t)0x0, (uint8_t)0x0);

    std::ifstream ifs(file);

    while (ifs.good()) {

        if(ifs.get() == 0x9a) {
            if(ifs.peek() == 0xc3) {
                ct_sync++; // sync word found

                cur = ifs.tellg(); // points one byte into sync word
                ifs.seekg(3, std::ios::cur);
                ifs.read((char *)&length, 2);

                if(length > TELEMETRY_PACKET_MAX_SIZE-16) continue; //invalid payload size
                ct_length++;

                ifs.seekg(cur);

                ifs.read((char *)buffer+1, length+15);

                tp = TelemetryPacket(buffer, length+16);

                if(tp.valid()) {
                    ct_valid++;
                    pass_sourceID = !(filter_sourceID && !(tp.getSourceID() == i_sourceID));
                    pass_typeID = !(filter_typeID && !(tp.getTypeID() == i_typeID));
                    if(pass_sourceID) ct_sourceID++;
                    if(pass_typeID) ct_typeID++;
                    if(pass_sourceID && pass_typeID) *this << tp;
                }

                ifs.seekg(cur);
            }
        }

    }

    std::cout << ct_sync << " sync words found, ";
    std::cout << ct_valid << " packets with valid checksums\n";

    if(filter_sourceID) {
        std::cout << ct_sourceID << " packets with with the filtered source ID\n";
    }
    if(filter_typeID) {
        std::cout << ct_typeID << " packets with with the filtered type ID\n";
    }

}
