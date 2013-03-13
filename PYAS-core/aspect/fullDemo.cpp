#define DISPLAY 1
#define SAVE 1
#define DEBUG 1
#include <string.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <fstream>

#include "ImperxStream.hpp"
#include "processing.hpp"
//#include "compression.hpp"
 
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

int main(int argc, char* agrv[])
{
    int startTime, endTime, duration, framesCapped = 0;
    ImperxStream camera;
    int exposure = 15000;
    cv::Mat frame;
    int width, height;
    Aspect aspect;


#if SAVE == 1
    char number[4] = "000";
    std::vector<int> pngstuff;
    std::string savefile;
    pngstuff.push_back(CV_IMWRITE_PNG_COMPRESSION);
    pngstuff.push_back(0);
				
#endif
    

#if DISPLAY == 1
    cv::Mat image; //contains RGB version of image
    std::string label;
    cv::Scalar color(0,0,192);
    cv::Scalar color2(128,0,0);
    cv::Scalar crossingColor(0,128,0);
    cv::Scalar centerColor(0,0,192);
    cv::Scalar fiducialColor(128,0,0);
    cv::Scalar textColor(0,0,0);
#endif

    cv::Point2f center, error, IDCenter;

    CoordList fiducials, crossings, IDs;
    if (camera.Connect() != 0)
    {
	std::cout << "Error connecting to camera!\n";	
	return -1;
    }
    else
    {
	camera.ConfigureSnap();
//	camera.SetROISize(960,960);
//	camera.SetROIOffset(165,0);
	camera.SetExposure(exposure);
	
	width = camera.GetROIWidth();
	height = camera.GetROIHeight();
	if ( height == 0 || width == 0)
	{
	    std::cout << "Attempting to allocate frame of size 0\n";
	    return -1;
	}
	
	frame.create(height, width, CV_8UC1);
	
	if(camera.Initialize() != 0)
	{
	    std::cout << "Error initializing camera!\n";
	    return -1;
	}
	std::cout << "CameraStart Done. Running CameraSnap loop\n";
	std::cout << "Run for how many seconds: ";
	std::cin >> duration;
					


					
#if DISPLAY
	cv::Mat list[] = {frame,frame,frame};
	cv::namedWindow("Solar Solution", CV_WINDOW_AUTOSIZE);
#endif
			
	std::cout << "\n\n";
	std::cout.flush();
	startTime = time(NULL);
	while ( time(NULL) < startTime + duration)
	{
	    camera.Snap(frame);
	    aspect.LoadFrame(frame);
	    aspect.GetPixelCrossings(crossings);
	    aspect.GetPixelCenter(center);
	    aspect.GetPixelError(error);
	    aspect.GetPixelFiducials(fiducials);
	    aspect.GetFiducialIDs(IDs);
	    aspect.GetScreenCenter(IDCenter);
	    
	    std::cout << "\x1b[A\x1b[A\r";
#if DEBUG
	    std::cout << "Center: " << center.x << " " << center.y << "\n";
	    std::cout << "Error:  " << error.x << " " << error.y << "\n";
	    std::cout.flush();
#endif

#if DISPLAY
	    cv::merge(list,3,image);
	    DrawCross(image, center, centerColor, 20, 1);
	    for (int k = 0; k < crossings.size(); k++)
		DrawCross(image, crossings[k], crossingColor, 10, 1);
	    
	    for (int k = 0; k < fiducials.size(); k++)
		DrawCross(image, fiducials[k], fiducialColor, 15, 1);
	    std::cout << "Fiducials\n";
	    for (int k = 0; k < fiducials.size(); k++)
	    {
		label = "";
		sprintf(number, "%d", (int) IDs[k].x);
		label += number;
		label += ",";
		sprintf(number, "%d", (int) IDs[k].y);
		label += number;

//	std::cout << fiducials[k].x << "," << fiducials[k].y << "\n";
		DrawCross(image, fiducials[k], fiducialColor, 15, 1);
		cv::putText(image, label, fiducials[k], cv::FONT_HERSHEY_SIMPLEX, .5, textColor);
	    }
	    std::cout << IDCenter.x << "," << IDCenter.y << "\n";
	    imshow("Solar Solution", image);
	    cv::waitKey(10);
#endif
						
#if SAVE					
	    sprintf(number, "%03d", framesCapped);
	    
	    savefile = "./frames/frame";
	    savefile += number;
//	    savefile += ".fits";
	    savefile += ".png";
	    cv::imwrite(savefile, frame, pngstuff);
	    
//	    writeFITSImage(frame, savefile);
#endif
	    framesCapped++;
	}
	endTime = time(NULL);
	std::cout << "Frame rate was: " << ((float) framesCapped/(duration)) << "\n";
	std::cout << "CameraSnap loop Done. Running CameraStop\n";
	camera.Stop();
	camera.Disconnect();
	return 0;
    }
}
