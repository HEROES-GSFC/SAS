#include "opencv.hpp"
#include "utilities.hpp"
#include "AspectError.hpp"
#include <string>
#include <ctime>

struct HeaderData
{
    timespec captureTime, captureTimeMono;
    int cameraID;
    float cameraTemperature;
    int cpuTemperature;
    int i2c_temperatures[8];
    long frameCount;
    int exposure;
    timespec imageWriteTime;
    int preampGain;
    int analogGain;
    float sunCenter[2];
    float sunCenterError[2];
    int imageMinMax[2];
    float XYinterceptslope[4];
    double CTLsolution[2];
    float screenCenter[2];
    float screenCenterError[2];
    int fiducialCount;
    float fiducialX[10];
    float fiducialY[10];
    int limbCount;
    float limbX[10];
    float limbY[10];
    //float limbXerror[10];
    //float limbYerror[10];
    int fiducialIDX[10];
    int fiducialIDY[10];
    float cpuVoltage[5];
    bool isTracking;
    bool isOutputting;

    AspectCode runResult;

    float latitude;
    float longitude;
    float solarTarget[2];
    double northAngle;
    float clockingAngle;
};

int writePNGImage(cv::InputArray _image, const std::string fileName);
int writeFITSImage(cv::InputArray, HeaderData keys, const std::string fileName);
int readFITSImage(const std::string fileName, cv::OutputArray image);
int readFITSHeader(const std::string fileName, HeaderData &keys);
