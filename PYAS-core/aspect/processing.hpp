#include <opencv.hpp>

int chordCenter(const unsigned char*, int, int, int, int, double*);
double chord(const unsigned char*, int, int, int, int, int, bool);

struct morphParams{
    bool dim;
    int tophatWidth;
    int threshold;
};

int morphPeakFind(cv::Mat, morphParams, int*, int);
int morphFindFiducials(cv::Mat, morphParams, morphParams, int, int*, int);
int matchFindFiducials(cv::InputArray, cv::InputArray, int , cv::Point2f*, int);
void matchKernel(cv::OutputArray);
