#include <sys/time.h>
#include <string.h>
#include <math.h>

//#include <fstream>
#include <iostream>

#include "Image.hpp"
#include "types.hpp" //for bitread and bitwrite

#define SAS_TARGET_ID 0x30
#define IMAGE_DATA 0x82
#define IMAGE_TAG 0x83

#define INDEX_NANOSECONDS 8
#define INDEX_SECONDS 12
#define INDEX_DATA_OFFSET_FIELD 16
#define INDEX_IMAGE_FORMAT_FIELD 20
#define INDEX_IMAGE_DATA 24

using std::ostream;

class ImagePacketInvalidException : public std::exception
{
    virtual const char* what() const throw()
        {
            return "Invalid ImagePacket";
        }
} ipInvalidException;

class ImageTagTypeException : public std::exception
{
    virtual const char* what() const throw()
        {
            return "Unknown tag type";
        }
} itTypeException;

class ImageQueueEmptyException : public std::exception
{
    virtual const char* what() const throw()
        {
            return "Empty ImagePacketQueue";
        }
} iqEmptyException;

uint8_t sizeofTag(uint8_t type)
{
  switch(type) {
    case TSTRING:
        return 16;
    case TUINT:
    case TINT:
        return 4;
    case TBYTE:
    case TSBYTE:
    case TLOGICAL:
    case TUSHORT:
    case TSHORT:
    case TULONG:
    case TLONG:
    case TFLOAT:
    case TLONGLONG:
    case TDOUBLE:
    case TCOMPLEX:
    case TDBLCOMPLEX:
        return type/10;
    default:
        throw itTypeException;
  }
}

ImagePacket::ImagePacket(uint8_t typeID, uint8_t sourceID)
    : TelemetryPacket(typeID, sourceID)
{
}

ImagePacket::ImagePacket(const void *ptr)
    : TelemetryPacket(ptr)
{
    //Assumes that NULL was passed in
}

ImageSectionPacket::ImageSectionPacket(uint8_t camera,
                                       uint16_t xpixels, uint16_t ypixels,
                                       uint32_t offset, bool last)
    : ImagePacket(IMAGE_DATA, SAS_TARGET_ID)
{
    uint32_t data_offset = 0, image_format = 0;

    //The bit positions are all subtracted from 31 because
    //  In bitread/bitwrite, bit #0 is the least-significant bit
    //  In HEROES, bit #0 is the most-significant bit
    bitwrite(&data_offset, 32-0-24, 24, offset);
    bitwrite(&data_offset, 32-31-1, 1, (last ? 1 : 0));
    *this << data_offset;

    bitwrite(&image_format, 32-0-12, 12, xpixels);
    bitwrite(&image_format, 32-12-12, 12, ypixels);
    bitwrite(&image_format, 32-24-4, 4, camera);
    bitwrite(&image_format, 32-28-3, 3, 3); //3 is for 8 bits/pixel
    bitwrite(&image_format, 32-31-1, 1, 0); //0 is for non-central-quadrant image
    *this << image_format;
}

ImageSectionPacket::ImageSectionPacket(const void *ptr)
    : ImagePacket(ptr)
{
    //Assumes that NULL was passed in
}

uint8_t ImageSectionPacket::getCamera()
{
    uint32_t image_format;
    this->readAtTo(INDEX_IMAGE_FORMAT_FIELD, image_format);
    return (uint8_t)bitread(&image_format, 32-24-4, 4);
}

uint16_t ImageSectionPacket::getXPixels()
{
    uint32_t image_format;
    this->readAtTo(INDEX_IMAGE_FORMAT_FIELD, image_format);
    return (uint16_t)bitread(&image_format, 32-0-12, 12);
}

uint16_t ImageSectionPacket::getYPixels()
{
    uint32_t image_format;
    this->readAtTo(INDEX_IMAGE_FORMAT_FIELD, image_format);
    return (uint16_t)bitread(&image_format, 32-12-12, 12);
}

uint32_t ImageSectionPacket::getOffset()
{
    uint32_t data_offset;
    this->readAtTo(INDEX_DATA_OFFSET_FIELD, data_offset);
    return (uint32_t)bitread(&data_offset, 32-0-24, 24);
}

bool ImageSectionPacket::last()
{
    uint32_t data_offset;
    this->readAtTo(INDEX_DATA_OFFSET_FIELD, data_offset);
    return (bool)bitread(&data_offset, 32-31-1, 1);
}

ImageTagPacket::ImageTagPacket(uint8_t camera, const void *data, uint8_t type,
                               const char *name, const char *comment)
    : ImagePacket(IMAGE_TAG, SAS_TARGET_ID)
{
    uint64_t tag_data[2] = { 0, 0 }; //initialize 16 bytes
    memcpy(&tag_data, data, sizeofTag(type));

    *this << tag_data[0] << tag_data[1] << type << camera;

    uint8_t tag_name[8], tag_comment[32]; //not initialized
    strncpy((char *)tag_name, name, 8);
    strncpy((char *)tag_comment, comment, 32);

    this->append_bytes(tag_name, 8);
    this->append_bytes(tag_comment, 32);
}

ImageTagPacket::ImageTagPacket(const void *ptr)
    : ImagePacket(ptr)
{
    //Assumes that NULL was passed in
}

ImagePacketQueue::ImagePacketQueue()
{
}

void ImagePacketQueue::add_array(uint8_t camera,
                                uint16_t xpixels, uint16_t ypixels,
                                const uint8_t *array)
{
    ImageSectionPacket isp(NULL);
    uint32_t offset = 0;
    uint16_t nsections = ceil(((float)xpixels*ypixels) / SECTION_MAX_PIXELS);
    uint16_t last_length = xpixels*ypixels - (nsections-1)*SECTION_MAX_PIXELS;
    bool last = false;

    timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    for (uint16_t i=0; i<nsections; i++) {
        last = (i == nsections-1);
        offset = i*SECTION_MAX_PIXELS;
        isp = ImageSectionPacket(camera, xpixels, ypixels,
                                 offset, last);
        isp.append_bytes(array+offset,
                         (last ? last_length : SECTION_MAX_PIXELS));
        isp.setTimeAndFinish(now);
        *this << isp;
    }
}

void ImagePacketQueue::reassembleTo(uint8_t &camera,
                                    uint16_t &xpixels, uint16_t &ypixels,
                                    std::vector<uint8_t> &output)
{
    ImageSectionPacket isp(NULL);

    //Look for the first ImageSectionPacket
    do {
        *this >> isp;
        if (!isp.valid()) throw ipInvalidException;
    } while (isp.getTypeID() != IMAGE_DATA);

    camera = isp.getCamera();
    xpixels = isp.getXPixels();
    ypixels = isp.getYPixels();

    output.clear();
    output.resize(xpixels*ypixels);

    while(!isp.last()) {
        if (isp.getTypeID() != IMAGE_DATA) break;
        isp.readAtTo_bytes(INDEX_IMAGE_DATA, &output[isp.getOffset()],
                           isp.getLength()-INDEX_IMAGE_DATA);
        *this >> isp;
        if (!isp.valid()) throw ipInvalidException;
    }

    isp.readAtTo_bytes(INDEX_IMAGE_DATA, &output[isp.getOffset()],
                       isp.getLength()-INDEX_IMAGE_DATA);
}

void ImagePacketQueue::synchronize()
{
    timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    ImagePacket im(NULL);

    lock();

    if(empty()) throw iqEmptyException;
    for (ImagePacketQueue::iterator it=begin(); it != end(); ++it) {
        ((ImagePacket *)&(*it))->setTimeAndFinish(now);
    }

    unlock();
}

void ImagePacketQueue::add_FITS(const char* file)
{
}
