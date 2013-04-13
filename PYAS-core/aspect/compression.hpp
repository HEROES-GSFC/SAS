#include "opencv.hpp"
#include "utilities.hpp"
#include <string>

struct HeaderData
{
    timespec captureTime;
    long frameCount;
    int exposure;
    
};

int writePNGImage(cv::InputArray _image, const std::string fileName);
int writeFITSImage(cv::InputArray, HeaderData keys, const std::string fileName);
int readFITSImage(const std::string fileName, cv::OutputArray image);
