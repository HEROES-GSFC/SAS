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

int main(int argc, char* agrv[])
{
  int startTime, duration, framesCapped = 0;
    ImperxStream camera;
    int exposure = 3000;
    cv::Mat frame;
    int width, height;
    Aspect aspect;


#if SAVE == 1
    char number[4] = "000";
    std::string savefile;
				
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

    CoordList fiducials, crossings;
    IndexList IDs;
    std::vector<float> mapping;
    mapping.resize(4);
    std::cout << "fullDemo: Connecting to camera" << std::endl;
    if (camera.Connect() != 0)
    {
	std::cout << "fullDemo: Error connecting to camera!\n";	
	return -1;
    }
    else
    {
      
	std::cout << "fullDemo: Configuring camera" << std::endl;
	camera.ConfigureSnap();
//	camera.SetROISize(960,960);
//	camera.SetROIOffset(165,0);
	camera.SetExposure(exposure);
	
	width = camera.GetROIWidth();
	height = camera.GetROIHeight();
	if ( height == 0 || width == 0)
	{
	    std::cout << "fullDemo: Attempting to allocate frame of size 0\n";
	    return -1;
	}
	
	std::cout << "fullDemo: Allocating frame" << std::endl;
	frame.create(height, width, CV_8UC1);
	
	std::cout << "fullDemo: Initializing camera" << std::endl;
	if(camera.Initialize() != 0)
	{
	    std::cout << "fullDemo: Error initializing camera!\n";
	    return -1;
	}
	std::cout << "CameraStart Done. Running CameraSnap loop\n";
	std::cout << "Run for how many seconds: ";
	std::cin >> duration;
	std::cout << std::endl;
					

	cv::Mat list[] = {frame,frame,frame};
			
	std::cout << "\n\n";
	std::cout.flush();
	startTime = time(NULL);
	while ( time(NULL) < startTime + duration)
	{
	    std::cout << "fullDemo: Snap Frame" << std::endl;
	    camera.Snap(frame);

	    cv::merge(list,3,image);
	    cv::imshow("Solar Solution", image);
	    cv::waitKey(1);

	    std::cout << "fullDemo: Load Frame" << std::endl;
	    aspect.LoadFrame(frame);

	    std::cout << "fullDemo: Run Aspect" << std::endl;
	    aspect.Run();

	    std::cout << "fullDemo: Get Crossings" << std::endl;
	    aspect.GetPixelCrossings(crossings);
	    DrawCross(image, center, centerColor, 20, 1);
	    for (int k = 0; k < crossings.size(); k++)
		DrawCross(image, crossings[k], crossingColor, 10, 1);
	    cv::imshow("Solar Solution", image);
	    cv::waitKey(1);

	    std::cout << "fullDemo: Get Center" << std::endl;
	    aspect.GetPixelCenter(center);
	    std::cout << "fullDemo: Center: " << center.x << " " << center.y << std::endl;
	    DrawCross(image, center, centerColor, 20, 1);
	    cv::imshow("Solar Solution", image);
	    cv::waitKey(1);
	    
	    std::cout << "fullDemo: Get Error" << std::endl;
	    aspect.GetPixelError(error);
	    std::cout << "fullDemo: Error:  " << error.x << " " << error.y << std::endl;
	    
	    std::cout << "fullDemo: Get Fiducials" << std::endl;
	    aspect.GetPixelFiducials(fiducials);
	    for (int k = 0; k < fiducials.size(); k++)
		DrawCross(image, fiducials[k], fiducialColor, 15, 1);
	    cv::imshow("Solar Solution", image);
	    cv::waitKey(1);

	    std::cout << "fullDemo: Get IDs" << std::endl;
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
	    cv::imshow("Solar Solution", image);
	    cv::waitKey(1);

	    /*	    std::cout << "fullDemo: Get Screen Center" << std::endl;
	    aspect.GetScreenCenter(IDCenter);

	    std::cout << "fullDemo: Get Mapping" << std::endl;
	    aspect.GetMapping(mapping);
	    std::cout << "Mapping: " << std::endl;
	    for (int d = 0; d < 2; d++)
	    {
		for (int o = 0; o < 2; o++)
		    std::cout << mapping[2*d+o] << " ";
		std::cout << endl;
		}*/
	    

	    std::cout.flush();

	    std::cout << "Screen center: " << IDCenter << std::endl;
	    imshow("Solar Solution", image);
	    cv::waitKey(10);
						
#if SAVE					
	    sprintf(number, "%03d", framesCapped);
	    
	    savefile = "./frames/frame";
	    savefile += number;
//	    savefile += ".fits";
	    savefile += ".png";
	    writePNGImage(frame, savefile);
	    
//	    writeFITSImage(frame, savefile);
#endif
	    framesCapped++;
	}
	std::cout << "Frame rate was: " << ((float) framesCapped/(duration)) << "\n";
	std::cout << "CameraSnap loop Done. Running CameraStop\n";
	camera.Stop();
	camera.Disconnect();
	return 0;
    }
}
