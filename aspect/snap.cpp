#include <iostream>
#include <signal.h>
#include <unistd.h>     /* for sleep()  */

#include "ImperxStream.hpp"
#include "compression.hpp"

#define FRAME_CADENCE 250000 // microseconds
#define SLEEP_CAMERA_CONNECT   1 // waits for errors while connecting to camera

uint16_t localExposure = 15000;
int16_t localPreampGain = -3;
uint16_t localAnalogGain = 400;

sig_atomic_t volatile g_running = 1;

void sig_handler(int signum)
{
    if ((signum == SIGINT) || (signum == SIGTERM))
    {
        if (signum == SIGINT) std::cerr << "Keyboard interrupt received\n";
        if (signum == SIGTERM) std::cerr << "Termination signal received\n";
        g_running = 0;
    }
}

void ImageSave()
{
    cv::Mat localFrame;
    HeaderData localHeader;
    memset(&localHeader, 0, sizeof(HeaderData));

    localHeader.exposure = localExposure;
    localHeader.preampGain = localPreampGain;
    localHeader.analogGain = localAnalogGain;

    if(!localFrame.empty())
    {
        char timestamp[14];
        char filename[128];
        timespec now;
        clock_gettime(CLOCK_REALTIME, &now);
        struct tm *capturetime;
        capturetime = gmtime(&now.tv_sec);
        strftime(timestamp,14,"%y%m%d_%H%M%S",capturetime);

        sprintf(filename, "image_%s_%03d.fits", timestamp, (int)(now.tv_nsec/1000000l));

        printf("Saving image %s: exposure %d us, analog gain %d, preamp gain %d\n", filename, localHeader.exposure, localHeader.analogGain, localHeader.preampGain);
        writeFITSImage(localFrame, localHeader, filename);
    }
    else
    {
        std::cerr << "Empty image!\n";
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2) {
        std::cout << "Calling sequeunce: snap <camera IP>\n";
        return 0;
    }

    // to catch a Ctrl-C or termination signal and clean up
    signal(SIGINT, &sig_handler);
    signal(SIGTERM, &sig_handler);

    timespec frameRate = {0,FRAME_CADENCE*1000};
    bool cameraReady;
    ImperxStream camera;

    cv::Mat localFrame;
    int width, height;

    cameraReady = false;
    while(g_running)
    {
        if (!cameraReady)
        {
            if (camera.Connect(argv[1]) != 0)
            {
                std::cerr << "Error connecting to camera!\n";
                sleep(SLEEP_CAMERA_CONNECT);
                continue;
            }
            else
            {
                camera.ConfigureSnap();

                camera.SetExposure(localExposure);
                camera.SetAnalogGain(localAnalogGain);
                camera.SetPreAmpGain(localPreampGain);

                width = camera.GetROIWidth();
                height = camera.GetROIHeight();
                localFrame.create(height, width, CV_8UC1);
                if(camera.Initialize() != 0)
                {
                    std::cerr << "Error initializing camera!\n";
                    //may need disconnect here
                    sleep(SLEEP_CAMERA_CONNECT);
                    continue;
                }
                cameraReady = true;
            }
        }
        else
        {
            if(!camera.Snap(localFrame, frameRate))
            {
                ImageSave();
            }
            else
            {
                std::cerr << "Image not taken\n";
            }
        }

        std::cout << "Press return to snap\n";
        std::cin.get();
    }

    camera.Stop();
    camera.Disconnect();

    return 0;
}
