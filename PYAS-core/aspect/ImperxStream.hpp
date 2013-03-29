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
    
    int GetExposure();
    cv::Size GetROISize();
    cv::Point GetROIOffset();
    int GetROIHeight();
    int GetROIWidth();
    int GetROIOffsetX();
    int GetROIOffsetY();

    int8_t getTemperature( void );

private:
    PvDevice lDevice;
    PvDeviceInfo *lDeviceInfo;
    PvGenParameterArray *lDeviceParams;
    PvStream lStream;
    PvGenParameterArray *lStreamParams;
    PvPipeline lPipeline;
};

