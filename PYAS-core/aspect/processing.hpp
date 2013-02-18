#include <opencv.hpp>
#include <vector>

class CoordList : public std::vector<cv::Point2f>
{
  public:
    void add(float x, float y) { this->push_back(cv::Point2f(x, y)); }
};

int chordCenter(const unsigned char*, int, int, int, int, double*, CoordList&);
double chord(const unsigned char*, int, int, int, int, int, bool, CoordList&);

struct morphParams{
    bool dim;
    int tophatWidth;
    int threshold;
};

int morphPeakFind(cv::Mat, morphParams, int*, int);
int morphFindFiducials(cv::Mat, morphParams, morphParams, int, int*, int);
int matchFindFiducials(cv::InputArray, cv::InputArray, int , cv::Point2f*, int);
void matchKernel(cv::OutputArray);
