#include "processing.hpp"
#include "utilities.hpp"
#include "compression.hpp"

void DrawCross(cv::Mat &image, cv::Point2f point, cv::Scalar color, int length, int thickness)
{
    cv::Point2f pt1, pt2;
    length = (length+1)/2;
    pt1.x = point.x-length;
    pt1.y = point.y-length;
    pt2.x = point.x+length;
    pt2.y = point.y+length;
    cv::line(image, pt1*128, pt2*128, color, thickness, CV_AA, 7);
    pt1.x = point.x+length;
    pt1.y = point.y-length;
    pt2.x = point.x-length;
    pt2.y = point.y+length;
    cv::line(image, pt1*128, pt2*128, color, thickness, CV_AA, 7);
}

int main(int argc, char* argv[])
{

    if (argc != 2)
    {
	std::cout << "Correct whatever is: kill <yourself>\n";
	return -1;
    }
    cv::namedWindow("Do it.", CV_WINDOW_AUTOSIZE);
    cv::Mat frame = cv::imread(argv[1],0);
    cv::Mat image;
    cv::Point2f center;

    cv::Scalar crossingColor(0,128,0);
    cv::Scalar centerColor(0,0,192);
    cv::Scalar fiducialColor(128,0,0);

    CoordList crossings, fiducials;
    
    cv::imshow("Do it.",frame);
    cv::waitKey();
    Aspect thingy;
    thingy.LoadFrame(frame);
    cv::Mat list[] = {frame,frame,frame};
    double start = GetSystemTime();
    thingy.GetPixelCrossings(crossings);
    thingy.GetPixelCenter(center);
    thingy.GetPixelFiducials(fiducials);

    // writeFITSImage(frame, "./Stuff.fits");
    double end = GetSystemTime();
    
    std::cout << "Estimated Balls: " << 1/(.03 + end-start) << "\n";

    cv::merge(list,3,image);
    DrawCross(image, center, centerColor, 20, 1);
    for (int k = 0; k < crossings.size(); k++)
	DrawCross(image, crossings[k], crossingColor, 10, 1);
    
    std::cout << "Fiducials\n";
    for (int k = 0; k < fiducials.size(); k++)
    {
	std::cout << fiducials[k].x << " " << fiducials[k].y << "\n";
	DrawCross(image, fiducials[k], fiducialColor, 15, 1);
    }

    cv::imshow("Do it.", image);
    cv::waitKey();

    return 0;
}


