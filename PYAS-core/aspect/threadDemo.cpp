#define CHORDS 50
#define THRESHOLD 50 

#define FID_WIDTH 5
#define FID_LENGTH 23
#define SOLAR_RADIUS 105
#define FID_ROW_THRESH 5
#define FID_COL_THRESH 0
#define FID_MATCH_THRESH 5

#define NUM_LOCS 20

#include <opencv.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "ImperxStream.hpp"
#include "processing.hpp"
#include "utilities.hpp"

cv::Mat frame;
cv::Point center;
bool enable = 1;
std::mutex enableMutex, frameMutex, centerMutex, fiducialMutex;
Semaphore frameReady, frameProcessed;
cv::Point fiducialLocations[NUM_LOCS];
int numFiducials;

int runtime, exposure, frameRate;

void stream_image()
{    
    cv::Mat localFrame;
    int width, height;
    ImperxStream camera;
    if (camera.Connect() != 0)
    {
	std::cout << "Error connecting to camera!\n";	
	return;
    }
    else
    {
	camera.ConfigureSnap();
	camera.SetROISize(966,966);
	camera.SetROIOrigin(165,0);
	camera.SetExposure(exposure);
	
	width = camera.GetROIWidth();
	height = camera.GetROIHeight();
	localFrame.create(height, width, CV_8UC1);
	if(camera.Initialize() != 0)
	{
	    std::cout << "Error initializing camera!\n";
	    return;
	}
	do
	{
	    enableMutex.lock();
	    if(!enable)
	    {
		enableMutex.unlock();
		camera.Stop();
		camera.Disconnect();
		std::cout << "Stream thread stopped\n";
		return;
	    }
	    enableMutex.unlock();
	    
	    camera.Snap(localFrame);

	    frameMutex.lock();
	    localFrame.copyTo(frame);
	    frameMutex.unlock();

	    frameReady.increment();
	    fine_wait(0,frameRate - exposure,0,0);

	} while (true);
    }
}
/*
void process_image()
{
    cv::Size frameSize;
    cv::Mat localFrame;
    double chordOutput[6];

    cv::Mat kernel;
    cv::Mat subImage;
    int height, width;
    cv::Range rowRange, colRange;
    matchKernel(kernel);

    cv::Point localFiducialLocations[NUM_LOCS];
    int localNumFiducials;
    
    do
    {
	while(true)
	{
	    enableMutex.lock();
	    if(!enable)
	    {
		enableMutex.unlock();
		std::cout << "Chord thread stopped.\n";
		return;
	    }
	    enableMutex.unlock();
	    
	    try
	    {
		frameReady.decrement();
		break;
	    }
	    catch(const char* e)
	    {
		fine_wait(0,frameRate/10,0,0);
	    }
	}

	frameMutex.lock();
	frame.copyTo(localFrame);
	frameMutex.unlock();
	

	frameSize = localFrame.size();
	height = frameSize.height;
	width = frameSize.width;
	chordCenter((const unsigned char*) localFrame.data, height, width, CHORDS, THRESHOLD, chordOutput);
       
	centerMutex.lock();
	center.x = chordOutput[0];
	center.y = chordOutput[1];
	centerMutex.unlock();

	if (chordOutput[0] > 0 && chordOutput[1] > 0 && chordOutput[0] < width && chordOutput[1] < height)
	{
	    rowRange.end = (((int) chordOutput[1]) + SOLAR_RADIUS < height-1) ? (((int) chordOutput[1]) + SOLAR_RADIUS) : (height-1);
	    rowRange.start = (((int) chordOutput[1]) - SOLAR_RADIUS > 0) ? (((int) chordOutput[1]) - SOLAR_RADIUS) : 0;
	    colRange.end = (((int) chordOutput[0]) + SOLAR_RADIUS < width) ? (((int) chordOutput[0]) + SOLAR_RADIUS) : (width-1);
	    colRange.start = (((int) chordOutput[0]) - SOLAR_RADIUS > 0) ? (((int) chordOutput[0]) - SOLAR_RADIUS) : 0;
	    subImage = localFrame(rowRange, colRange);
	    localNumFiducials = matchFindFiducials(subImage, kernel, FID_MATCH_THRESH, localFiducialLocations, NUM_LOCS);
	}
	
	
	fiducialMutex.lock();
	numFiducials = localNumFiducials;
	for (int k = 0; k < localNumFiducials; k++)
	{
	    fiducialLocations[k].x = localFiducialLocations[k].x + colRange.start;
	    fiducialLocations[k].y = localFiducialLocations[k].y + rowRange.start;
	}
	fiducialMutex.unlock();
	
	frameProcessed.increment();
    } while(true);		        
}
*/
void display()
{
    bool validCenter;
    cv::Mat localFrame;
    cv::namedWindow("Current Frame", CV_WINDOW_AUTOSIZE);
    do
    {

	while(true)
	{
	    enableMutex.lock();
	    if(!enable)
	    {
		enableMutex.unlock();
		std::cout << "Display thread stopped.\n";
		return;
	    }
	    enableMutex.unlock();
	    
	    try
	    {
		frameProcessed.decrement();
		break;
	    }
	    catch(const char* e)
	    {
		fine_wait(0,frameRate/10,0,0);
	    }
	}
	frameMutex.lock();
	frame.copyTo(localFrame);
	frameMutex.unlock();
	
	cv::imshow("Current Frame", localFrame);
	cv::waitKey(10);
/*	
	centerMutex.lock();
	validCenter = (center.x != -1 && center.y != -1);
	if (validCenter)
	{
	    std::cout << "Image center at: " << center.x << ", " << center.y << "\n";
	}
	else
	{
	    std::cout << "No center found\n";
	}
	centerMutex.unlock();
	
	fiducialMutex.unlock();
	if (validCenter)
	{
	    std::cout << "Found " << numFiducials << " fiducials\n";
	    if (numFiducials > 0)
	    {
		std::cout << "First Fiducial at: " << fiducialLocations[0].x << ", " << fiducialLocations[0].y << "\n";
	    }
	}

	fiducialMutex.unlock();
*/
	
    } while(true);
}
	
     
int main()
{
    
    std::cout << "Enter runtime (s): ";
    std::cin >> runtime;
    std::cout << "Enter exposure time (us): ";
    std::cin >> exposure;
    std::cout << "Enter frame period in ms (>= 250): ";
    std::cin >> frameRate;
    std::thread stream(stream_image);
    //  std::thread process(process_image);
    std::thread show(display);

    
    fine_wait(runtime,0,0,0);
    
    enableMutex.lock();
    enable = 0;
    enableMutex.unlock();

    stream.join();
//    process.join();
    show.join();

    std::cout << "All threads stopped. Exiting\n";
    return 0;
}
