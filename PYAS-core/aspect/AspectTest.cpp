#include "processing.hpp"
#include "utilities.hpp"
#include "compression.hpp"
#include <cstring>

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
    cv::Mat frame;
    cv::Mat image;
    cv::Point2f center,error, IDCenter;

    cv::Scalar crossingColor(0,128,0);
    cv::Scalar centerColor(0,0,192);
    cv::Scalar fiducialColor(128,0,0);
    cv::Scalar textColor(0,0,0);

    CoordList crossings, fiducials;
    IndexList IDs;
    std::vector<float> mapping;
    
    std::string label, file;
    char number[4] = "000";
    
    Aspect aspect;
    for (int iterations = 0; iterations < 100; iterations++)
    {
	for (int f = 0; f < 143; f++)
	{
	    file = argv[1];
	    file += "/frame";
	    sprintf(number, "%03d", f);
	    file += number;
	    file += ".png";
	    frame = cv::imread(file,0);

	    aspect.LoadFrame(frame);
	
	    cv::Mat list[] = {frame, frame, frame};
	    cv::merge(list,3,image);

	    //std::cout << "AspectTest: Load Frame" << std::endl;
	    aspect.LoadFrame(frame);

	    //std::cout << "AspectTest: Run Aspect" << std::endl;
	    if(!aspect.Run())
	    {
		//std::cout << "AspectTest: Get Crossings" << std::endl;
		aspect.GetPixelCrossings(crossings);
		for (int k = 0; k < crossings.size(); k++)
		    DrawCross(image, crossings[k], crossingColor, 10, 1);

		//std::cout << "AspectTest: Get Center" << std::endl;
		aspect.GetPixelCenter(center);
		std::cout << "AspectTest: Pixel Center: " << center.x << " " << center.y << std::endl;
		DrawCross(image, center, centerColor, 20, 1);
	    
		//std::cout << "AspectTest: Get Error" << std::endl;
		aspect.GetPixelError(error);
		//std::cout << "AspectTest: Error:  " << error.x << " " << error.y << std::endl;
	    
		//std::cout << "AspectTest: Get Fiducials" << std::endl;
		aspect.GetPixelFiducials(fiducials);
		for (int k = 0; k < fiducials.size(); k++)
		    DrawCross(image, fiducials[k], fiducialColor, 15, 1);
	
		//std::cout << "AspectTest: Get IDs" << std::endl;
		aspect.GetFiducialIDs(IDs);
		for (int k = 0; k < IDs.size(); k++)
		{
		    label = "";
		    sprintf(number, "%d", (int) IDs[k].x);
		    label += number;
		    label += ",";
		    sprintf(number, "%d", (int) IDs[k].y);
		    label += number;
		    DrawCross(image, fiducials[k], fiducialColor, 15, 1);
		    cv::putText(image, label, fiducials[k], cv::FONT_HERSHEY_SIMPLEX, .5, textColor);
		}
	
		//std::cout << "AspectTest: Getting screen center" << std::endl;
		aspect.GetScreenCenter(IDCenter);
		std::cout << "AspectTest: Screen Center:  " << IDCenter << std::endl;

		aspect.GetScreenFiducials(fiducials);
		std::cout << "AspectTest: Screen fiducials: " << std::endl;
		for (int k = 0; k < fiducials.size(); k++)
		    std::cout << fiducials[k] << std::endl;
	
	    }
	    else
	    {
		std::cout << "AspectTest: Failure in Aspect::Run" << std::endl;
	    }
	    cv::imshow("Do it.", image);
	    cv::waitKey(0);

	}
    }
    return 0;
}


