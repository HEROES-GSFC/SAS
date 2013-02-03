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
#include <opencv.hpp>

class ImperxStream
{
public:
	ImperxStream();
	int Connect();
	int Connect(const std::string &IP);
	//get/set parameters(name, value);
	void Initialize();
	void ConfigureSnap(int &width, int &height);
	void Stream(unsigned char *frame, Semaphore &frame_semaphore, Flag &stream_flag);
	void Snap(cv::Mat &frame);
	void Stop();
	void Disconnect();
	long long int getTemperature( void );

private:
	PvDevice lDevice;
	PvDeviceInfo *lDeviceInfo;
	PvGenParameterArray *lDeviceParams;
	PvStream lStream;
	PvGenParameterArray *lStreamParams;
	PvPipeline lPipeline;
};

