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
    void Initialize();
    void ConfigureSnap();
    void Snap(cv::Mat &frame);
    void Stop();
    void Disconnect();
    
    void SetExposure(int exposureTime);
    void SetROISize(cv::Size size);
    void SetROISize(int width, int height);
    void SetROIOrigin(cv::Point origin);
    void SetROIOrigin(int x, int y);
    void SetROIHeight(int height);
    void SetROIWidth(int width);
    
    int GetExposure();
    cv::Size GetROISize();
    cv::Point GetROIOrigin();
    int GetROIHeight();
    int GetROIWidth();

    long long int getTemperature( void );

private:
    PvDevice lDevice;
    PvDeviceInfo *lDeviceInfo;
    PvGenParameterArray *lDeviceParams;
    PvStream lStream;
    PvGenParameterArray *lStreamParams;
    PvPipeline lPipeline;
};

