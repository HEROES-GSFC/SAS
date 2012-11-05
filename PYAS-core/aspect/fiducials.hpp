#include <opencv.hpp>
struct morphParams{
	bool dim;
	int tophatWidth;
	int threshold;
};

int morphPeakFind(cv::Mat, morphParams, int*, int);
int morphFindFiducials(cv::Mat, morphParams, morphParams, int, int*, int);
int matchFindFiducials(cv::InputArray, cv::InputArray, int , cv::Point*, int);
void matchKernel(cv::OutputArray);
