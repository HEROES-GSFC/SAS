#include "opencv.hpp"
#include "utilities.hpp"
#include <string>

struct HeaderData
{
    timespec captureTime;
    int cameraID;
    int cameraTemperature;
    long frameCount;
    int exposure;
    timespex imageWriteTime;
    int preampGain;
    int analogGain;
    float sunCenter[2];
    int imageMinMax[2];
    float XYinterceptslope[4];
    float CTLsolution[2];
    float screenCenter[2];
    float fiducialCenters[2];
    float limbs[2];
};

int writePNGImage(cv::InputArray _image, const std::string fileName);
int writeFITSImage(cv::InputArray, HeaderData keys, const std::string fileName);
int readFITSImage(const std::string fileName, cv::OutputArray image);
