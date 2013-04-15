#include "opencv.hpp"
#include "utilities.hpp"
#include <string>
#include <time.h>

struct HeaderData
{
    timespec captureTime;
    int cameraID;
    int cameraTemperature;
    int cpuTemperature;
    long frameCount;
    int exposure;
    timespex imageWriteTime;
    int preampGain;
    int analogGain;
    float sunCenter[2];
    float sunCenterError[2];
    int imageMinMax[2];
    float XYinterceptslope[4];
    float CTLsolution[2];
    float screenCenter[2];
    float fiducialX[10];
    float fiducialY[10];
    float limbX[10];
    float limbY[10];
    int fiducialIDX[10];
    int fiducialIDY[10];
    float cpuVoltages[6];
};

int writePNGImage(cv::InputArray _image, const std::string fileName);
int writeFITSImage(cv::InputArray, HeaderData keys, const std::string fileName);
int readFITSImage(const std::string fileName, cv::OutputArray image);
