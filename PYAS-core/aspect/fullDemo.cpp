#define CHORDS 50
#define THRESHOLD 50

#define FID_WIDTH 5
#define FID_LENGTH 23
#define SOLAR_RADIUS 105
#define FID_ROW_THRESH 5
#define FID_COL_THRESH 0
#define FID_MATCH_THRESH 7

#define DEBUG 0
#define DISPLAY 0
#define SAVE 1
#define RATE 0
#define FIDTYPE 1
#include <string.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <fstream>

#include "ImperxStream.hpp"
#include "processing.hpp"
#include "compression.hpp"
 
int main(int argc, char* agrv[])
{
    int startTime, endTime, framesCapped = 0;
    float duration = 0;
    double center[6];
    cv::Scalar color(0,0,192);
    cv::Scalar color2(128,0,0);
    cv::Point2d pt;
    cv::Point2d pt1,pt2;
    
#if SAVE == 1
    char number[4] = "000";
    std::string savefile;
#endif
    ImperxStream camera;
    int exposure = 15000;
    cv::Mat frame;
    int width, height;
    Aspect aspect;
    if (camera.Connect() != 0)
    {
	std::cout << "Error connecting to camera!\n";	
	return -1;
    }
    else
    {
	camera.ConfigureSnap();
	camera.SetROISize(960,960);
	camera.SetROIOffset(165,0);
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
	cv::Mat frame(height, width, CV_8UC1);
	
	cv::Mat subImage;
	cv::Range rowRange, colRange;
					
	cv::Mat image; //contains RGB version of image
	cv::Point2f center;
	CoordList fiducials;

					
#if DISPLAY
	cv::Mat list[] = {frame,frame,frame};
	cv::namedWindow("Solar Solution", CV_WINDOW_AUTOSIZE);
#endif
					
	startTime = time(NULL);
	while ( time(NULL) < startTime + duration)
	{
	    camera.Snap(frame);
	    aspect.LoadFrame(frame);
	    aspect.GetPixelCenter(center);
	    aspect.GetPixelFiducials(fiducials);

#if DISPLAY
	    imshow("Solar Solution", frame);
	    cv::waitKey(10);
#endif
						
#if SAVE					
	    sprintf(number, "%d", framesCapped);
												
	    savefile = "./frames/frame";
	    savefile += number;
	    savefile += ".fits";
	
	    writeFITSImage(frame, savefile);
#endif
	    framesCapped++;
	}
	endTime = time(NULL);
	std::cout << "Frame rate was: " << ((float) framesCapped/(endTime-startTime)) << "\n";
	std::cout << "CameraSnap loop Done. Running CameraStop\n";
	camera.Stop();
	camera.Disconnect();
	return 0;
    }
}
