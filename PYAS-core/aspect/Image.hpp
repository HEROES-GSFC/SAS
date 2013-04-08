/*

  ImageSectionPacket, ImageTagPacket, and ImagePacketQueue
  derived from TelemetryPacket and ByteStringQueue

*/

#ifndef _IMAGE_HPP_
#define _IMAGE_HPP_

#include <vector>

#include <sys/time.h>

#include "Telemetry.hpp"

#define SECTION_MAX_PIXELS 100

#define TBYTE 11
#define TSBYTE 12
#define TLOGICAL 14
#define TSTRING 16
#define TUSHORT 20
#define TSHORT 21
#define TUINT 30
#define TINT 31
#define TULONG 40
#define TLONG 41
#define TFLOAT 42
#define TLONGLONG 81
#define TDOUBLE 82
#define TCOMPLEX 83
#define TDBLCOMPLEX 163

class ImagePacket : public TelemetryPacket {
protected:
    virtual void finish() { TelemetryPacket::finish(); };
    virtual void writeTime() { /* disables automatic timestamp */ };

public:
    ImagePacket(uint8_t typeID, uint8_t sourceID);

    //Use this constructor when needing to have an empty image packet
    //Pass in NULL
    //This packet is non-functional!  Be sure not to use without reassignment!
    ImagePacket(const void *ptr);

    void setTimeAndFinish(const timeval &time);
};

class ImageSectionPacket : public ImagePacket {
public:
    ImageSectionPacket(uint8_t camera, uint16_t xpixels, uint16_t ypixels,
                       uint32_t offset, bool last);

    //Use this constructor when needing to have an empty image section packet
    //Pass in NULL
    //This packet is non-functional!  Be sure not to use without reassignment!
    ImageSectionPacket(const void *ptr);

    uint8_t getCamera();
    uint16_t getXPixels();
    uint16_t getYPixels();
    uint32_t getOffset();

    bool last();
};

class ImageTagPacket : public ImagePacket {
public:
    ImageTagPacket(uint8_t camera, const void *data, uint8_t type,
                   const char *name, const char *comment);

    //Use this constructor when needing to have an empty image tag packet
    //Pass in NULL
    //This packet is non-functional!  Be sure not to use without reassignment!
    ImageTagPacket(const void *ptr);
};

class ImagePacketQueue : public TelemetryPacketQueue {

public:
    ImagePacketQueue();

    void add_array(uint8_t camera, uint16_t xpixels, uint16_t ypixels,
                   const uint8_t *array);
    void reassembleTo(std::vector<uint8_t> &output);

    //Not yet implemented
    void add_FITS(const char *file);
};

#endif
