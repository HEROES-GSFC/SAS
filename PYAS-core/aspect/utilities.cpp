#include "utilities.hpp"

Semaphore::Semaphore()
{
    pthread_mutex_init(&mutex, NULL);
    count = 0;
}

Semaphore::~Semaphore()
{
    pthread_mutex_destroy(&mutex);
}

void Semaphore::increment()
{
    pthread_mutex_lock(&mutex);
    count++;
    pthread_mutex_unlock(&mutex);
}
void Semaphore::decrement()
{
    pthread_mutex_lock(&mutex);;
    if (count == 0)
    {
	pthread_mutex_unlock(&mutex);
	throw "Counter empty";
    }
    count--;
    pthread_mutex_unlock(&mutex);
}
    
Flag::Flag()
{
    pthread_mutex_init(&mutex, NULL);
    value = 0;
}

Flag::~Flag()
{
    pthread_mutex_destroy(&mutex);
}

void Flag::raise()
{
    pthread_mutex_lock(&mutex);
    value = 1;
    pthread_mutex_unlock(&mutex);
}
void Flag::lower()
{
    pthread_mutex_lock(&mutex);
    value = 0;
    pthread_mutex_unlock(&mutex);
}

bool Flag::check()
{
    bool temp;
    pthread_mutex_lock(&mutex);
    temp = value;
    pthread_mutex_unlock(&mutex);
    return temp;
}

timespec TimespecDiff(timespec start, timespec end)
{
	timespec diff;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		diff.tv_sec = end.tv_sec-start.tv_sec-1;
		diff.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		diff.tv_sec = end.tv_sec-start.tv_sec;
		diff.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return diff;
}

const std::string nanoString(long tv_nsec)
{
    char number[4] = "000";
    std::string output;
    int msec, usec, nsec;
    msec = (int) tv_nsec/1000000;
    usec = (int) (tv_nsec/1000)%1000;
    nsec = (int) (tv_nsec-1000*(1000*msec+usec));
    output = "";
    sprintf(number, ".%03d", msec);
    output += number;
    sprintf(number, ".%03d", usec);
    output += number; 
    sprintf(number, ".%03d", nsec);
    output += number;
    return output;
}

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
