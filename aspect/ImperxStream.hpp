#include <PvSampleUtils.h>
#include <PvSystem.h>
#include <PvInterface.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvStream.h>
#include <PvStreamRaw.h>

#include <string>
#include <opencv.hpp>

#include <stdint.h>

struct CameraSettings
{
    CameraSettings(): exposure(10000),
                      size(1296, 966),
                      offset(0,0),
                      analogGain(400),
                      preampGain(-3),
                      blackLevel(0) {};
    uint16_t exposure;
    cv::Size size;
    cv::Point offset;
    uint16_t analogGain;
    int16_t preampGain;
    int blackLevel;
};

class ImperxStream
{
public:
    ImperxStream();
    ~ImperxStream();
    int Connect();
    int Connect(const std::string &IP);
    //get/set parameters(name, value);
    int Initialize();
    void ConfigureSnap();
    int Snap(cv::Mat &frame, timespec timeout);
    int Snap(cv::Mat &frame, int timeout);
    int Snap(cv::Mat &frame);
    void Stop();
    void Disconnect();
    
    /* Set-functions for camera values
       returns 0 for a successful set,
       returns -1 otherwise
    */
    int SetExposure(int exposureTime);
    int SetROISize(cv::Size size);
    int SetROISize(int width, int height);
    int SetROIOffset(cv::Point offset);
    int SetROIOffset(int x, int y);
    int SetROIOffsetX(int x);
    int SetROIOffsetY(int y);
    int SetROIHeight(int height);
    int SetROIWidth(int width);
    int SetAnalogGain(int gain);
    int SetBlackLevel(int black);
    int SetPreAmpGain(int gain);
    
    int GetExposure();
    cv::Size GetROISize();
    cv::Point GetROIOffset();
    int GetROIHeight();
    int GetROIWidth();
    int GetROIOffsetX();
    int GetROIOffsetY();
    int GetAnalogGain();
    int GetBlackLevel();
    int GetPreAmpGain();

    float getTemperature( void );

private:
    PvSystem lSystem;
    PvDevice lDevice;
    PvDeviceInfo *lDeviceInfo;
    PvGenParameterArray *lDeviceParams;
    PvStream lStream;
    PvGenParameterArray *lStreamParams;
    PvPipeline lPipeline;
};

