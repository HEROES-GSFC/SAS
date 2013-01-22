#define CHORDS 30
#define THRESHOLD 75

#define FID_WIDTH 5
#define FID_LENGTH 23
#define SOLAR_RADIUS 105
#define FID_ROW_THRESH 5
#define FID_COL_THRESH 0
#define FID_MATCH_THRESH 5

#define DEBUG 1
#define DISPLAY 1
#define SAVE 1
#define RATE 0
#define FIDTYPE 1
#include <string.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <fstream>

#include <ImperxStream.hpp>
#include <processing.hpp>
 
int main(int argc, char* agrv[])
{
    int startTime, endTime, framesCapped = 0;
    float duration = 0;
    double center[6];
    cv::Scalar color(0,0,192);
    cv::Scalar color2(128,0,0);
    cv::Point2d pt;
    cv::Point2d pt1,pt2;
    int numLocs = 20;
#if FIDTYPE == 0
    int locs[2*numLocs];
#else
    cv::Point locs[numLocs];
#endif
    double* temp;
    int fidLocs;
    
#if SAVE == 1
    char number[4] = "000";
    std::string savefile;
    std::vector<int> pngstuff;
    pngstuff.push_back(CV_IMWRITE_PNG_COMPRESSION);
    pngstuff.push_back(RATE);
#endif
    ImperxStream camera;
    int height, width;
	camera.Connect();
	camera.ConfigureSnap(width, height);
	camera.Initialize();
    std::cout << "CameraStart Done. Running CameraSnap loop\n";
    std::cout << "Run for how many seconds: ";
    std::cin >> duration;
    cv::Mat frame(height, width, CV_8UC1);
	
    cv::Mat subImage;
    cv::Range rowRange, colRange;
					
    cv::Mat image; //contains RGB version of image
#if FIDTYPE == 0					
    morphParams rowParams;
    rowParams.dim = 0;
    rowParams.tophatWidth = FID_LENGTH;
    rowParams.threshold = FID_ROW_THRESH;
        
    morphParams colParams;
    colParams.dim = 1;
    colParams.tophatWidth = FID_WIDTH;
    colParams.threshold = FID_COL_THRESH;
#else
    cv::Mat kernel;
    matchKernel(kernel);
#endif
					
#if DISPLAY
    cv::Mat list[] = {frame,frame,frame};
    cv::namedWindow("Solar Solution", CV_WINDOW_AUTOSIZE);
#endif
					
    startTime = time(NULL);
    while ( time(NULL) < startTime + duration)
	//while(framesCapped < 1)
    {
		camera.Snap(frame);

	chordCenter(frame.data, height, width, 
		    CHORDS, THRESHOLD, center);
						
#if DEBUG
	std::cout << "Chord Center: " << center[0] << "+/-"<< center[4] << " (" << center[2] << "), " 
		  << center[1] << "+/-" << center[5] << " (" << center[3] << ")\n";
#endif
						
	if (center[0] > 0 && center[1] > 0 &&
	    center[0] < width && center[1] < height)
	{
							
	    rowRange.end = (((int) center[1]) + SOLAR_RADIUS < height-1) ? (((int) center[1]) + SOLAR_RADIUS) : (height-1);
	    rowRange.start = (((int) center[1]) - SOLAR_RADIUS > 0) ? (((int) center[1]) - SOLAR_RADIUS) : 0;
	    colRange.end = (((int) center[0]) + SOLAR_RADIUS < width) ? (((int) center[0]) + SOLAR_RADIUS) : (width-1);
	    colRange.start = (((int) center[0]) - SOLAR_RADIUS > 0) ? (((int) center[0]) - SOLAR_RADIUS) : 0;
	    subImage = frame(rowRange, colRange);
							
#if FIDTYPE == 0
	    fidLocs = morphFindFiducials(subImage, rowParams, colParams, FID_LENGTH, locs, numLocs);
#else
	    fidLocs = matchFindFiducials(subImage, kernel, FID_MATCH_THRESH, locs, numLocs);
#endif
							
#if DEBUG
	    std::cout << "Found " << fidLocs << " fiducials\n";
#endif
							
#if DISPLAY
	    cv::merge(list,3,image);
	    for (int k = 0; k < fidLocs; k++)
	    {
#if FIDTYPE == 0
		cv::circle(image, cv::Point(locs[k]+colRange.start, locs[numLocs + k]+rowRange.start), 10, color2, 2, CV_AA, 0);
#else
		cv::circle(image, cv::Point(locs[k].x+colRange.start, locs[k].y+rowRange.start), 10, color2, 2, CV_AA, 0);	
#endif
	    }
#endif	
	}
	else
	{
#if DEBUG
	    std::cout << "No center found. Skipping frame\n";
#endif
	    cv::merge(list,3,image);
	    imshow("Solar Solution", image );
	    cv::waitKey(10);
	    continue;   
	}
	
#if DISPLAY		
	pt.x = center[0]; pt.y = center[1];
	//cv::circle(image, pt, 1, color, 1, CV_AA, 0);
	//cv::circle(image, pt, 10, color, 1, CV_AA, 0);
	
	//Symbol is sub-pixel rendered to 1/128 of a pixel
	pt1.x = pt.x-5;
	pt1.y = pt.y;
	pt2.x = pt.x+5;
	pt2.y = pt.y;
	cv::line(image, pt1*128, pt2*128, color, 1, CV_AA, 7);
				
	pt1.x = pt.x;
	pt1.y = pt.y-5;
	pt2.x = pt.x;
	pt2.y = pt.y+5;
	cv::line(image, pt1*128, pt2*128, color, 1, CV_AA, 7);
	
	pt1.x = pt.x-15;
	pt1.y = pt.y-15;
	pt2.x = pt.x- 5;
	pt2.y = pt.y- 5;
	cv::line(image, pt1*128, pt2*128, color, 5, CV_AA, 7);
	
	pt1.x = pt.x+ 5;
	pt1.y = pt.y+ 5;
	pt2.x = pt.x+15;
	pt2.y = pt.y+15;
	cv::line(image, pt1*128, pt2*128, color, 5, CV_AA, 7);
		
	pt1.x = pt.x-15;
	pt1.y = pt.y+15;
	pt2.x = pt.x- 5;
	pt2.y = pt.y+ 5;
	cv::line(image, pt1*128, pt2*128, color, 5, CV_AA, 7);
		
	pt1.x = pt.x+ 5;
	pt1.y = pt.y- 5;
	pt2.x = pt.x+15;
	pt2.y = pt.y-15;
	cv::line(image, pt1*128, pt2*128, color, 5, CV_AA, 7);

	//1-sigma error ellipse
	if (center[2] > 1 && center[3] > 1) 
	{
	    cv::Size axes(center[4],center[5]);
	    cv::ellipse(image, pt*128, axes*128, 0, 0, 360, color2, 1, CV_AA, 7);
	}

	imshow("Solar Solution", image);
	cv::waitKey(10);
#endif
						
#if SAVE					
	sprintf(number, "%d", framesCapped);
												
	savefile = "./frames/frame";
	savefile += number;
	savefile += "_rate";
	sprintf(number, "%d", RATE);
	savefile += number;
	savefile += ".png";
	
	cv::imwrite(savefile, frame, pngstuff);
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
