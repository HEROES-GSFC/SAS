#define CHORDS 50
#define THRESHOLD 50 

#define FID_WIDTH 5
#define FID_LENGTH 23
#define SOLAR_RADIUS 105
#define FID_ROW_THRESH 5
#define FID_COL_THRESH 0
#define FID_MATCH_THRESH 5

#define NUM_LOCS 20

#define FRAME_PERIOD 500

#include <opencv.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <ImperxStream.hpp>
#include <processing.hpp>

void stream_image(std::mutex* en_mtx, bool& en, std::mutex* frame_mtx, cv::OutputArray _frame, Semaphore* outReady)
{    
    cv::Mat temp;
    int width, height;
    ImperxStream Camera;
    if (Camera.Connect() != 0)
    {
	std::cout << "Error connecting to camera!\n";	
    }
    else
    {
	Camera.ConfigureSnap(width, height);
	temp.create(height, width, CV_8UC1);
	Camera.Initialize();
	do
	{
	    (*en_mtx).lock();
	    if(!en)
	    {
		(*en_mtx).unlock();
		Camera.Stop();
		Camera.Disconnect();
		std::cout << "Stream thread stopped\n";
		return;
	    }
	    (*en_mtx).unlock();
	    
	    Camera.Snap(temp);
	    (*frame_mtx).lock();
	    temp.copyTo(_frame);
	    (*frame_mtx).unlock();
	    (*outReady).increment();
	    fine_wait(0,FRAME_PERIOD,0,0);

	} while (true);
    }
}

void process_image(std::mutex* en_mtx, bool* en, std::mutex* frame_mtx, cv::InputArray _frame, std::mutex* center_mtx, cv::Point* center, Semaphore* inReady, Semaphore* outReady, cv::Point* _locs, int* _fidLocs, std::mutex* fid_mtx)
{
    cv::Size frameSize;
    cv::Mat frame;
    double chordOutput[6];

    cv::Mat kernel;
    cv::Mat subImage;
    int height, width;
    cv::Range rowRange, colRange;
    matchKernel(kernel);

    cv::Point locs[NUM_LOCS];
    int fidLocs;
    
    do
    {
	while(true)
	{
	    (*en_mtx).lock();
	    if(!(*en))
	    {
		(*en_mtx).unlock();
		std::cout << "Chord thread stopped.\n";
		return;
	    }
	    (*en_mtx).unlock();
	    try
	    {
		(*inReady).decrement();
		break;
	    }
	    catch(const char* e)
	    {
		fine_wait(0,10,0,0);
	    }
	}

	(*frame_mtx).lock();
	frame = _frame.getMat();
	(*frame_mtx).unlock();
	

	frameSize = frame.size();
	height = frameSize.height;
	width = frameSize.width;
	chordCenter((const unsigned char*) frame.data, height, width, CHORDS, THRESHOLD, chordOutput);
       
	(*center_mtx).lock();
	(*center).x = chordOutput[0];
	(*center).y = chordOutput[1];
	(*center_mtx).unlock();

	if (chordOutput[0] > 0 && chordOutput[1] > 0 && chordOutput[0] < width && chordOutput[1] < height)
	{
	    rowRange.end = (((int) chordOutput[1]) + SOLAR_RADIUS < height-1) ? (((int) chordOutput[1]) + SOLAR_RADIUS) : (height-1);
	    rowRange.start = (((int) chordOutput[1]) - SOLAR_RADIUS > 0) ? (((int) chordOutput[1]) - SOLAR_RADIUS) : 0;
	    colRange.end = (((int) chordOutput[0]) + SOLAR_RADIUS < width) ? (((int) chordOutput[0]) + SOLAR_RADIUS) : (width-1);
	    colRange.start = (((int) chordOutput[0]) - SOLAR_RADIUS > 0) ? (((int) chordOutput[0]) - SOLAR_RADIUS) : 0;
	    subImage = frame(rowRange, colRange);
	    fidLocs = matchFindFiducials(subImage, kernel, FID_MATCH_THRESH, locs, NUM_LOCS);
	}
	
	
	(*fid_mtx).lock();
	*_fidLocs = fidLocs;
	for (int k = 0; k < fidLocs; k++)
	{
	    _locs[k].x = locs[k].x + colRange.start;
	    _locs[k].y = locs[k].y + rowRange.start;
	}
	(*fid_mtx).unlock();
	

	(*outReady).increment();
    } while(true);		        
}

void display_image(std::mutex* en_mtx, bool* en, std::mutex* frame_mtx, cv::InputArray _frame, std::mutex* center_mtx, cv::Point* center, Semaphore* inReady, cv::Point* locs, int* fidLocs, std::mutex* fid_mtx)
{
    cv::namedWindow("Frame", CV_WINDOW_AUTOSIZE);
    cv::Mat render, frame;
    cv::Mat channels[] = {frame, frame, frame};
    cv::Scalar color(0,0,192);
    cv::Scalar color2(128,0,0);
    cv::Point pt, pt1, pt2;

    do
    {
	while(true)
	{
	    (*en_mtx).lock();
	    if(!(*en))
	    {
		(*en_mtx).unlock();
		cv::destroyWindow("Frame");
		cv::waitKey(1);				     
		std::cout << "Display thread stopped\n";
		return;
	    }
	    (*en_mtx).unlock();	
	    try
	    {
		(*inReady).decrement();
		break;
	    }
	    catch(const char* e)
	    {
		fine_wait(0,10,0,0);
	    }
	}
	(*frame_mtx).lock();
	frame = _frame.getMat();
	(*frame_mtx).unlock();

	for (int q = 0; q < 3; q++)
      	    channels[q] = frame;

	cv::merge(channels, 3, render);

	(*center_mtx).lock();
	pt = *center;
	(*center_mtx).unlock();
	pt1.x = pt.x-5;
	pt1.y = pt.y;
	pt2.x = pt.x+5;
	pt2.y = pt.y;
	cv::line(render, pt1*128, pt2*128, color, 1, CV_AA, 7);
				
	pt1.x = pt.x;
	pt1.y = pt.y-5;
	pt2.x = pt.x;
	pt2.y = pt.y+5;
	cv::line(render, pt1*128, pt2*128, color, 1, CV_AA, 7);
	
	pt1.x = pt.x-15;
	pt1.y = pt.y-15;
	pt2.x = pt.x- 5;
	pt2.y = pt.y- 5;
	cv::line(render, pt1*128, pt2*128, color, 5, CV_AA, 7);
	
	pt1.x = pt.x+ 5;
	pt1.y = pt.y+ 5;
	pt2.x = pt.x+15;
	pt2.y = pt.y+15;
	cv::line(render, pt1*128, pt2*128, color, 5, CV_AA, 7);
		
	pt1.x = pt.x-15;
	pt1.y = pt.y+15;
	pt2.x = pt.x- 5;
	pt2.y = pt.y+ 5;
	cv::line(render, pt1*128, pt2*128, color, 5, CV_AA, 7);
		
	pt1.x = pt.x+ 5;
	pt1.y = pt.y- 5;
	pt2.x = pt.x+15;
	pt2.y = pt.y-15;
	cv::line(render, pt1*128, pt2*128, color, 5, CV_AA, 7);
	(*fid_mtx).lock();
	for(int k = 0; k < *fidLocs; k++)
	{
	    cv::circle(render, cv::Point(locs[k].x, locs[k].y), 10, color2, 2, CV_AA, 0);
	}
	(*fid_mtx).unlock();
	cv::imshow("Frame", render);
	cv::waitKey(10);
    } while(true);
}

int main()
{
    cv::Mat frame;
    cv::Point center;
    bool en = 1;
    std::mutex en_mtx, frame_mtx, center_mtx, fid_mtx;
    Semaphore frameReady, frameProcessed;
    cv::Point locs[NUM_LOCS];
    int fidLocs;

    std::thread stream(stream_image, &en_mtx, en, &frame_mtx, frame, &frameReady);
    std::thread process(process_image, &en_mtx, &en, &frame_mtx, frame, &center_mtx, &center, &frameReady, &frameProcessed, locs, &fidLocs, &fid_mtx);
    std::thread show(display_image, &en_mtx, &en, &frame_mtx, frame, &center_mtx, &center,&frameProcessed, locs, &fidLocs, &fid_mtx);
    
    fine_wait(30,0,0,0);
    
    en_mtx.lock();
    en = 0;
    en_mtx.unlock();

    stream.join();
    process.join();
    show.join();

    std::cout << "All threads stopped. Exiting\n";
    return 0;
}
