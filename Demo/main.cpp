#define _x64
#define _LINUX
#define PVDECL

#include <string.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <fstream>

#include <PvApi.h>
#include <chord.hpp>

#include <opencv.hpp>
#include <highgui/highgui.hpp>

typedef struct
{
    unsigned long UID;
    tPvHandle Handle;
    tPvFrame Frame;
    tPvUint32 Counter;

} tCamera;

// Sleeeep?
void Sleep(unsigned int time)
{
    struct timespec t,r;
    t.tv_sec = time / 1000;
    t.tv_nsec = (time % 1000) * 1000000;

    while(nanosleep(&t,&r)==-1)
        t = r;
}

// Wait for a camera to be plugged in
void WaitForCamera()
{
    std::cout << "Waiting for a camera ";
    while(PvCameraCount() == 0)
    {
	std::cout << ".";
        Sleep(250);
    }
    std::cout << "\n";
}

// Get UID of camera
bool CameraGet(tCamera* Camera)
{
    tPvUint32 count, connected;
    tPvCameraInfoEx list;
    
    count = PvCameraListEx(&list,1,&connected, sizeof(tPvCameraInfoEx));
    if(count == 1)
    {
	Camera->UID = list.UniqueId;
	std::cout << "CameraGet got camera " << Camera->UID << "\n";
	return true;
    }
    else
    {
	std::cout << "CameraGet failed to get a camera\n";
	return false;
    }
}

// Open camera, display setup info
bool CameraSetup(tCamera* Camera, tPvUint32 &width, tPvUint32 &height)
{
    tPvErr errCode;
    unsigned long FrameSize = 0;
    unsigned long enum_val_size = 20;
    char pixel_val[enum_val_size];
    char exp_mode[enum_val_size];
    tPvUint32 exp_time;

    // Open Camera
    if ((errCode = PvCameraOpen(Camera->UID, ePvAccessMaster, &(Camera->Handle))) != ePvErrSuccess)
    {
	std::cout << "Error Opening Camera: " << errCode << "\n";
	return false;
    }
    
    std::cout << "Exposure time (us): ";
    std::cin >> exp_time;

    // Use Monochrome 8 pixel format
    if ((PvAttrEnumSet(Camera->Handle, "PixelFormat", "Mono8") != ePvErrSuccess ) ||
	(PvAttrEnumSet(Camera->Handle, "ExposureMode", "Manual") != ePvErrSuccess) ||
	(PvAttrUint32Set(Camera->Handle, "ExposureValue", exp_time) != ePvErrSuccess))
    {
	std::cout << "Failed to change camera settings\n";
    }
    // Extract Frame Info
    if ((PvAttrEnumGet(Camera->Handle, "PixelFormat", pixel_val, enum_val_size, &enum_val_size) != ePvErrSuccess) ||
	(PvAttrUint32Get(Camera->Handle, "Width", &width) != ePvErrSuccess) ||
	(PvAttrUint32Get(Camera->Handle, "Height", &height) != ePvErrSuccess) ||
	(PvAttrEnumGet(Camera->Handle, "ExposureMode", exp_mode, enum_val_size, &enum_val_size) != ePvErrSuccess) ||
	(PvAttrUint32Get(Camera->Handle, "ExposureValue", &exp_time) != ePvErrSuccess))

    {
	std::cout << "Error Reading Pixel Stuff\n";
	return false;
    }
    else
    {
	std::cout << "Pixel format: " << pixel_val << "\n";
	std::cout << "Frame Width: " << width << "\n";
	std::cout << "Frame Height: " << height << "\n";
	std::cout << "Exposure Mode: " << exp_mode << "\n";
	std::cout << "Exposure Time: " << exp_time << "\n";
    }

    // Calculate frame buffer size
    if((errCode = PvAttrUint32Get(Camera->Handle,"TotalBytesPerFrame",&FrameSize)) != ePvErrSuccess)
    {
	std::cout << "CameraSetup: Get TotalBytesPerFrame err: " << errCode << "\n";
	return false;
    }
    else
    {
	std::cout << "Frame Size: " << FrameSize << "\n";
    }

    // Allocate Image Buffer
    Camera->Frame.ImageBuffer = new char[FrameSize];
    if(!Camera->Frame.ImageBuffer)
    {
	std::cout << "CameraSetup: Failed to allocate buffers.\n";
	return false;
    }
    Camera->Frame.ImageBufferSize = FrameSize;

    return true;
}
bool CameraStart(tCamera* Camera)
{
    tPvErr errCode; 
    if((errCode = PvCaptureAdjustPacketSize(Camera->Handle,8228)) != ePvErrSuccess)
    {
	std::cout << "CameraStart: PvCaptureAdjustPacketSize err:" << errCode << "\n";
	return false;
    }

    // start driver capture stream 
    if((errCode = PvCaptureStart(Camera->Handle)) != ePvErrSuccess)
    {
	std::cout << "CameraStart: PvCaptureStart err: " << errCode << "\n";
	return false;
    }
	
    // queue frame
    if((errCode = PvCaptureQueueFrame(Camera->Handle,&(Camera->Frame),NULL)) != ePvErrSuccess)
    {
	std::cout << "CameraStart: PvCaptureQueueFrame err: " << errCode << "\n";
	// stop driver capture stream
	PvCaptureEnd(Camera->Handle);
	return false;
    }
		
    // set the camera in continuous mode, and start camera receiving triggers
    if((PvAttrEnumSet(Camera->Handle,"FrameStartTriggerMode","Software") != ePvErrSuccess) ||
       (PvAttrEnumSet(Camera->Handle,"AcquisitionMode","Continuous") != ePvErrSuccess) ||
       (PvCommandRun(Camera->Handle,"AcquisitionStart") != ePvErrSuccess))
    {		
	std::cout << "CameraStart: failed to set camera attributes\n";
	// clear queued frame
	PvCaptureQueueClear(Camera->Handle);
	// stop driver capture stream
	PvCaptureEnd(Camera->Handle);
	return false;
    }	

    return true;
}

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

double* CameraSnap(tCamera* Camera)
{
    tPvErr errCode;
    std::cout << "Triggering camera: ";  
    double *center;
    if((errCode = PvCommandRun(Camera->Handle,"FrameStartTriggerSoftware")) != ePvErrSuccess)
    {
	std::cout << "CameraSnap: Failed to trigger\n";
	return 0;
    }

    //wait for frame to return from camera to host. short timeout here to show ~time for return.
    while(PvCaptureWaitForFrameDone(Camera->Handle,&(Camera->Frame),25) == ePvErrTimeout)
	{
	//std::cout << "Waiting for frame to return to host...\n";
	}
    //check returned Frame.Status
    if(Camera->Frame.Status == ePvErrSuccess)
    {	
       	//FrameCentroid((const unsigned char*) Camera->Frame.ImageBuffer, Camera->Frame.Width, Camera->Frame.Height);
	center = chordCenter((const unsigned char*) Camera->Frame.ImageBuffer, 
				Camera->Frame.Height, Camera->Frame.Width, 7, 128);
	if (center[0] > 0 && center[1] > 0)
		std::cout << "Chord Center: " << center[0] << " " << center[1] << "\n";
        }
    else
    {
	if (Camera->Frame.Status == ePvErrDataMissing)
	    std::cout << "Dropped packets. Possible improper network card settings:\nSee GigE Installation Guide.";
	else
	    std::cout << "Frame.Status error: " << Camera->Frame.Status << "\n";
    }

    //requeue frame	
    if((errCode = PvCaptureQueueFrame(Camera->Handle,&(Camera->Frame),NULL)) != ePvErrSuccess)
    {
	std::cout << "CameraSnap: PvCaptureQueueFrame err: " <<  errCode << "\n";;
	return 0;
    }
    return center;
    }

// stop streaming
void CameraStop(tCamera* Camera)
{
    tPvErr errCode;
	
	//stop camera receiving triggers
	if ((errCode = PvCommandRun(Camera->Handle,"AcquisitionStop")) != ePvErrSuccess)
		std::cout << "AcquisitionStop command err: " <<  errCode << "\n";
	else
		std::cout << "Camera stopped.\n";

    //clear queued frames. will block until all frames dequeued
	if ((errCode = PvCaptureQueueClear(Camera->Handle)) != ePvErrSuccess)
		std::cout << "PvCaptureQueueClear err: " <<  errCode << "\n";
	else
		std::cout << "Queue cleared.\n";  

	//stop driver stream
	if ((errCode = PvCaptureEnd(Camera->Handle)) != ePvErrSuccess)
	    std::cout << "PvCaptureEnd err: " << errCode << "\n";
	else
		std::cout << "Driver stream stopped.\n";
}

// close camera, free memory.
void CameraUnsetup(tCamera* Camera)
{
    tPvErr errCode;
	
    if((errCode = PvCameraClose(Camera->Handle)) != ePvErrSuccess)
    {
	std::cout << "CameraUnSetup error: " <<  errCode << "\n";
    }
    else
    {
	std::cout << "Camera closed.\n";
    }
	
    // free image buffer
    delete [] (char*)Camera->Frame.ImageBuffer;
}

    
int main(int argc, char* agrv[])
{
    tPvErr errCode;
    int startTime, endTime, framesCapped = 0;
    float duration = 0;
    tPvUint32 width, height;
    double *center;
    cv::Scalar color = 0;
    cv::Point2d pt;
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
		    startTime = time(NULL);
		    cv::Mat frame(Camera.Frame.Height, Camera.Frame.Width, CV_8UC1, Camera.Frame.ImageBuffer, cv::Mat::AUTO_STEP);
		    cv::Mat image;
		    cv::namedWindow("Solar Solution", CV_WINDOW_AUTOSIZE);
		    while ( time(NULL) < startTime + duration)
		    //while(framesCapped < 1)
		    {
			center = CameraSnap(&Camera);
			
			image = frame.clone();
			pt.x = center[0]; pt.y = center[1];
			cv::circle(image, pt, 1, color, 1, CV_AA, 0);
			cv::circle(image, pt, 10, color, 1, CV_AA, 0);
			imshow("Solar Solution", image);
			cv::waitKey(10);
			//imwrite("MeasureRadius.png", image, param);
			delete [] center;
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
