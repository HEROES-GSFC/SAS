#include "opencv.hpp"
#include <string>

int writePNGImage(cv::InputArray _image, const std::string fileName);
int writeFITSImage(cv::InputArray, const uint16_t exposure, const std::string fileName);
