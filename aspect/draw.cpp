#include "draw.hpp"

void DrawCross(cv::Mat &image, cv::Point2f point, cv::Scalar color, int length, int thickness, int resolution)
{
    cv::Point2f pt1, pt2;
    int upscale, downscale;
    downscale = resolution;
    upscale = pow(2, downscale);

    length = (length+1)/2;
    pt1.x = point.x-length;
    pt1.y = point.y-length;
    pt2.x = point.x+length;
    pt2.y = point.y+length;
    cv::line(image, pt1*upscale, pt2*upscale, color, thickness, CV_AA, downscale);
    pt1.x = point.x+length;
    pt1.y = point.y-length;
    pt2.x = point.x-length;
    pt2.y = point.y+length;
    cv::line(image, pt1*upscale, pt2*upscale, color, thickness, CV_AA, downscale);
}
