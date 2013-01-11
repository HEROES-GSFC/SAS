#include <PvSampleUtils.h>
#include <PvSystem.h>
#include <PvInterface.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvStream.h>
#include <PvStreamRaw.h>

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
    void Start(char &frame, Semaphore &frame_semaphore, Flag &stream_flag);
    void Stop();
    void Disconnect();

private:
    PvDevice lDevice;
    PvDeviceInfo *lDeviceInfo;
    PvGenParameterArray *lDeviceParams;
    PvStream lStream;
    PvPipeline lPipeline;
};

