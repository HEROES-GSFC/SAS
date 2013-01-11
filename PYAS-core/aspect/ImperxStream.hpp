#include <PvSampleUtils.h>
#include <PvSystem.h>
#include <PvInterface.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvStream.h>
#include <PvStreamRaw.h>
#include <opencv.hpp>
#include <mutex>

#include <string>

#include <utilities.hpp>

class ImperxStream
{
public:
    ImperxStream();
    int Connect();
    int Connect(const std::string &IP);
    //get/set parameters(name, value);
    void Initialize();
    void Snap(cv::OutputArray _frame);
    void Stop();
    void Disconnect();

private:
    PvDevice lDevice;
    PvDeviceInfo *lDeviceInfo;
    PvGenParameterArray *lDeviceParams;
    PvStream lStream;
    PvPipeline lPipeline;
};

