#define _x64
#define _LINUX
#define PVDECL

#include <PvApi.h>

#define CHORDS 30
#define THRESHOLD 75

#define FID_WIDTH 5
#define FID_LENGTH 23
#define SOLAR_RADIUS 105
#define FID_ROW_THRESH 5
#define FID_COL_THRESH 0

#include <string.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <fstream>

#include <camera.hpp>
#include <chord.hpp>
#include <fiducials.hpp>

#include <opencv.hpp>
#include <highgui/highgui.hpp>


void FrameCentroid(const unsigned char* Frame, int width, int height)
{
	  
    double centm, centn, total, pixel;
    double Cx, Cy;
    centm = 0; centn = 0; total = 0;
    std::ofstream myfile;
    myfile.open ("example.txt");
    for (int m = 0; m < height; m++)
    {
	for (int n = 0; n < width; n++)
	{
            //pixel = (double) ((((int) Frame[width*m + n])+256)% 256);
        pixel = (double) Frame[width*m + n];
	    centm += m*pixel;
	    centn += n*pixel;
	    total += pixel;
	    myfile << pixel << ',';
	}
	myfile << '\n';
    }
    myfile.close();
    Cx = (double) centn/total;
    Cy = (double) centm/total;
    std::cout << "Centroid " << Cx << ", " << Cy << "\n";
}
 
int main(int argc, char* agrv[])
{
    tPvErr errCode;
    int startTime, endTime, framesCapped = 0;
    float duration = 0;
    tPvUint32 width, height;
    double center[6];
    cv::Scalar color(0,0,192);
    cv::Scalar color2(128,0,0);
    cv::Point2d pt;
    cv::Point2d pt1,pt2;
    int numLocs = 20;
    int locs[2*numLocs];
    double* temp;
    int fidLocs;
    //std::vector<int> param;
    //param.push_back(0);	
    if((errCode = PvInitialize()) != ePvErrSuccess)
    {
        std::cout << "PvInitialize err: " << errCode << "\n";
    }
    else
    {
		tCamera Camera;
		memset(&Camera,0,sizeof(tCamera));
		WaitForCamera();


		if(CameraGet(&Camera))
		{
			std::cout << "CameraGet Done. Running CameraSetup\n";
			if(CameraSetup(&Camera, width, height))
			{
				std::cout << "CameraSetup Done. Running CameraStart\n";
				if(CameraStart(&Camera))
				{
					std::cout << "CameraStart Done. Running CameraSnap loop\n";
					std::cout << "Run for how many seconds: ";
					std::cin >> duration;
					cv::Mat frame(height, width, CV_8UC1, Camera.Frame.ImageBuffer, cv::Mat::AUTO_STEP);
					
					cv::Mat subImage;
					cv::Range rowRange, colRange;
					
					cv::Mat image; //contains RGB version of image
										
					morphParams rowParams;
				    rowParams.dim = 0;
					rowParams.tophatWidth = FID_LENGTH;
					rowParams.threshold = FID_ROW_THRESH;
        
				    morphParams colParams;
					colParams.dim = 1;
					colParams.tophatWidth = FID_WIDTH;
					colParams.threshold = FID_COL_THRESH;

					cv::namedWindow("Solar Solution", CV_WINDOW_AUTOSIZE);
					startTime = time(NULL);
					while ( time(NULL) < startTime + duration)
					//while(framesCapped < 1)
					{
						CameraSnap(&Camera);
						chordCenter((const unsigned char*) Camera.Frame.ImageBuffer, height, width, 
												CHORDS, THRESHOLD, center);
						std::cout << "Chord Center: " << center[0] << "+/-"<< center[4] << " (" << center[2] << "), " 
								  << center[1] << "+/-" << center[5] << " (" << center[3] << ")\n";
						if (center[0] > 0 && center[1] > 0 &&
							center[0] < width && center[1] < height)
						{
							
							rowRange.end = (((int) center[1]) + SOLAR_RADIUS < height-1) ? (((int) center[1]) + SOLAR_RADIUS) : (height-1);
							rowRange.start = (((int) center[1]) - SOLAR_RADIUS > 0) ? (((int) center[1]) - SOLAR_RADIUS) : 0;
							colRange.end = (((int) center[0]) + SOLAR_RADIUS < width) ? (((int) center[0]) + SOLAR_RADIUS) : (width-1);
							colRange.start = (((int) center[0]) - SOLAR_RADIUS > 0) ? (((int) center[0]) - SOLAR_RADIUS) : 0;
							subImage = frame(rowRange, colRange);
						
							fidLocs = morphFindFiducials(subImage, rowParams, colParams, FID_LENGTH, locs, numLocs);
/*							std::cout << "Found " << fidLocs << " fiducials\n";
							cv::Mat frame2 = frame.clone();
							cv::Mat list[] = {frame,frame2,frame};
							for (int k = 0; k < fidLocs; k++)
							{
								cv::circle(frame2, cv::Point(locs[k]+colRange.start, locs[numLocs + k]+rowRange.start), 10, color2, 2, CV_AA, 0);
								cv::merge(list,3,image);
							}	
						}
						else
						{
							fidLocs = morphFindFiducials(frame, rowParams, colParams, FID_LENGTH, locs, numLocs);
							std::cout << "Found " << fidLocs << " fiducials\n";
							for (int k = 0; k < fidLocs; k++)
							{
								cv::circle(image, cv::Point(locs[k], locs[numLocs + k]), 10, color, 5, CV_AA, 0);
								std::cout <<  locs[numLocs + k] << ", " << locs[k] << "\n";
							}
*/						}
								
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
						//imwrite("MeasureRadius.png", image, param);
						framesCapped++;
					}
					endTime = time(NULL);
					std::cout << "Frame rate was: " << ((float) framesCapped/(endTime-startTime)) << "\n";
					std::cout << "CameraSnap loop Done. Running CameraStop\n";
					CameraStop(&Camera);
				}
				std::cout << "CameraStop Done. Running CameraUnsetup\n";
				CameraUnsetup(&Camera);
			}
		}
		std::cout << "CameraUnsetup Done.\n";
		PvUnInitialize();
	}
	return 0;
}
