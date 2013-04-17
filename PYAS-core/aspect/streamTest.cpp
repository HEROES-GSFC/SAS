#define MAX_THREADS 20
#define SAVE_LOCATION "/mnt/disk2/" // location for saving full images locally
#define REPORT_FOCUS false

//Default camera settings
#define CAMERA_EXPOSURE 15000 // microseconds, was 4500 microseconds in first Sun test
#define CAMERA_ANALOGGAIN 400 // camera defaults to 143, but we are changing it
#define CAMERA_PREAMPGAIN -3 // camera defaults to +6, but we are changing it
#define CAMERA_XSIZE 1296 // full frame is 1296
#define CAMERA_YSIZE 966 //full frame is 966
#define CAMERA_XOFFSET 0
#define CAMERA_YOFFSET 0

//Sleep settings (seconds)
#define SLEEP_SOLUTION         1 // period for providing solutions to CTL
#define SLEEP_SAVE             5 // period for saving full images locally
#define SLEEP_LOG_TEMPERATURE 10 // period for logging temperature locally
#define SLEEP_CAMERA_CONNECT   1 // waits for errors while connecting to camera
#define SLEEP_KILL             2 // waits when killing all threads

//Sleep settings (microseconds)
#define USLEEP_CMD_SEND     5000 // period for popping off the command queue
#define USLEEP_TM_SEND     50000 // period for popping off the telemetry queue
#define USLEEP_TM_GENERIC 250000 // period for adding generic telemetry packets to queue

#define SAS1_MAC_ADDRESS "00:20:9d:23:26:b9"
#define SAS2_MAC_ADDRESS "00:20:9d:23:5c:9e"

//IP addresses
#define IP_FDR      "192.168.2.4"   // will be 192.168.1.1 in flight
#define IP_CTL      "192.168.2.4"   // will be 192.168.1.2 in flight
#define IP_SAS1     "192.168.2.221" // will be 192.168.1.32 in flight
#define IP_SAS2     "192.168.16.16" // not yet implemented
#define IP_PYAS     "192.168.4.4"   // not yet implemented
#define IP_RAS      "192.168.8.8"   // not yet implemented

#define IP_LOOPBACK "127.0.0.1"   // should not change

//UDP ports, aside from PORT_IMAGE, which is TCP
#define PORT_CMD      2000 // commands, FDR (receive) and CTL (send/receive)
#define PORT_TM       2002 // send telemetry to FDR (except images)
#define PORT_IMAGE    2013 // send images to FDR, TCP port
#define PORT_SBC_INFO 3456 //

//HEROES target ID for commands, source ID for telemetry
#define TARGET_ID_CTL 0x01
#define SOURCE_ID_SAS 0x30

//HEROES telemetry types
#define TM_ACK_RECEIPT 0x01
#define TM_ACK_PROCESS 0xE1
#define TM_SAS_GENERIC 0x70
#define TM_SAS_IMAGE   0x82
#define TM_SAS_TAG     0x83

//HEROES commands, CTL/FDR to SAS
#define HKEY_CTL_START_TRACKING  0x1000
#define HKEY_CTL_STOP_TRACKING   0x1001
#define HKEY_FDR_SAS_CMD         0x10FF

//HEROES commands, SAS to CTL
#define HKEY_SAS_TRACKING_IS_ON  0x1100
#define HKEY_SAS_TRACKING_IS_OFF 0x1101
#define HKEY_SAS_SOLUTION        0x1102
#define HKEY_SAS_ERROR           0x1103
#define HKEY_SAS_TIMESTAMP       0x1104

//Operation commands
#define SKEY_OP_DUMMY            0x0000
#define SKEY_KILL_WORKERS        0x0010
#define SKEY_RESTART_THREADS     0x0020
#define SKEY_START_OUTPUTTING    0x0030
#define SKEY_STOP_OUTPUTTING     0x0040

//Setting commands
#define SKEY_SET_TARGET          0x0112
#define SKEY_SET_EXPOSURE        0x0151
#define SKEY_SET_ANALOGGAIN      0x0181
#define SKEY_SET_PREAMPGAIN      0x0191

//Getting commands
#define SKEY_REQUEST_IMAGE       0x0210

#include <cstring>
#include <stdio.h>      /* for printf() and fprintf() */
#include <pthread.h>    /* for multithreading */
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for sleep()  */
#include <signal.h>     /* for signal() */
#include <math.h>       /* for testing only, remove when done */
#include <ctime>        /* time_t, struct tm, time, gmtime */
#include <opencv.hpp>
#include <iostream>
#include <string>

#include "ImperxStream.hpp"
#include "processing.hpp"
#include "compression.hpp"
#include "utilities.hpp"

// global declarations
uint16_t command_sequence_number = 0;
uint16_t latest_heroes_command_key = 0x0000;
uint16_t latest_sas_command_key = 0x0000;
uint16_t latest_sas_command_vars[15];
uint32_t tm_frame_sequence_number = 0;
uint16_t solution_sequence_number = 0;

bool isTracking = false; // does CTL want solutions?
bool isOutputting = false; // is this SAS supposed to be outputting solutions?
bool acknowledgedCTL = true; // have we acknowledged the last command from CTL?

// related to threads
unsigned int stop_message[MAX_THREADS];
pthread_t threads[MAX_THREADS];
bool started[MAX_THREADS];
int tid_listen = 0;
pthread_attr_t attr;
pthread_mutex_t mutexImage;
pthread_mutex_t mutexProcess;

struct Thread_data{
    int  thread_id;
    uint16_t command_key;
    uint8_t command_num_vars;
    uint16_t command_vars[15];
};
struct Thread_data thread_data[MAX_THREADS];

sig_atomic_t volatile g_running = 1;

int sas_id;

cv::Mat frame;

Aspect aspect;
AspectCode runResult;

uint8_t frameMin, frameMax;
cv::Point2f pixelCenter, screenCenter, error;
CoordList limbs, pixelFiducials, screenFiducials;
IndexList ids;
std::vector<float> mapping;

HeaderData keys;

bool staleFrame;
Flag procReady, saveReady;
int runtime = 10;
uint16_t exposure = CAMERA_EXPOSURE;
uint16_t analogGain = CAMERA_ANALOGGAIN;
int16_t preampGain = CAMERA_PREAMPGAIN;

timespec frameRate = {0,100000000L};
int cameraReady = 0;


timespec captureTimeNTP, captureTimeFixed;
long int frameCount = 0;

float camera_temperature;
int8_t sbc_temperature;
float sbc_v105, sbc_v25, sbc_v33, sbc_v50, sbc_v120;

//Function declarations
void sig_handler(int signum);
void kill_all_threads( void ); //kills all threads
void kill_all_workers( void ); //kills all threads except the one that listens for commands
void identifySAS();
void *CameraStreamThread( void * threadargs);
void *ImageProcessThread(void *threadargs);
void *SaveImageThread(void *threadargs);
void start_all_workers( void );
void start_thread(void *(*start_routine) (void *), const Thread_data *tdata);

void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        g_running = 0;
    }
}

void kill_all_workers( void ){
    for(int i = 0; i < MAX_THREADS; i++ ){
        if ((i != tid_listen) && started[i]) {
            stop_message[i] = 1;
        }
    }
    sleep(SLEEP_KILL);
    for(int i = 0; i < MAX_THREADS; i++ ){
        if ((i != tid_listen) && started[i]) {
            printf("Quitting thread %i, quitting status is %i\n", i, pthread_cancel(threads[i]));
            started[i] = false;
        }
    }
}

void kill_all_threads( void){
    if (started[tid_listen]) {
        stop_message[tid_listen] = 1;
        kill_all_workers();
        printf("Quitting thread %i, quitting status is %i\n", tid_listen, pthread_cancel(threads[tid_listen]));
        started[tid_listen] = false;
    }
}

void identifySAS()
{
    FILE *in;
    char buff[128];

    if(!(in = popen("ifconfig sbc | grep ether", "r"))) {
        std::cout << "Error identifying computer, defaulting to SAS-1\n";
        sas_id = 1;
        return;
    }

    fgets(buff, sizeof(buff), in);

    if(strstr(buff, SAS1_MAC_ADDRESS) != NULL) {
        std::cout << "SAS-1 identified\n";
        sas_id = 1;
    } else if(strstr(buff, SAS2_MAC_ADDRESS) != NULL) {
        std::cout << "SAS-2 identified\n";
        sas_id = 2;
    } else {
        std::cout << "Unknown computer, defaulting to SAS-1\n";
        sas_id = 1;
    }

    pclose(in);
}

void *CameraStreamThread( void * threadargs)
{    
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("CameraStream thread #%ld!\n", tid);

    ImperxStream camera;

    cv::Mat localFrame;
    timespec localCaptureTime, preExposure, postExposure, timeElapsed, timeToWait;
    int width, height;
    int failcount = 0;

    uint16_t localExposure = exposure;
    int16_t localPreampGain = preampGain;
    uint16_t localAnalogGain = analogGain;

    cameraReady = 0;
    staleFrame = true;
    while(1)
    {
        if (stop_message[tid] == 1)
        {
            printf("CameraStream thread #%ld exiting\n", tid);
            camera.Stop();
            camera.Disconnect();
            started[tid] = false;
            pthread_exit( NULL );
        }
        else if (cameraReady == false)
        {
            if (camera.Connect() != 0)
            {
                std::cout << "Error connecting to camera!\n";
                sleep(SLEEP_CAMERA_CONNECT);
                continue;
            }
            else
            {
                camera.ConfigureSnap();

                camera.SetROISize(CAMERA_XSIZE,CAMERA_YSIZE);
                camera.SetROIOffset(CAMERA_XOFFSET,CAMERA_YOFFSET);
                camera.SetExposure(localExposure);
                camera.SetAnalogGain(localAnalogGain);
                camera.SetPreAmpGain(localPreampGain);

                width = camera.GetROIWidth();
                height = camera.GetROIHeight();
                localFrame.create(height, width, CV_8UC1);
                if(camera.Initialize() != 0)
                {
                    std::cout << "Error initializing camera!\n";
                    //may need disconnect here
                    sleep(SLEEP_CAMERA_CONNECT);
                    continue;
                }
                cameraReady = 1;
                frameCount = 0;
            }
        }
        else
        {
            //Record start time of this exposure. Nothing should happen before recording preExposure.
            //For either of these times to be accurate, as little as possible should happen between them and ImpxerStream::Snap
            //Not sure if this is the best way to handle this.
            clock_gettime(CLOCK_MONOTONIC, &preExposure);
            clock_gettime(CLOCK_REALTIME, &localCaptureTime);

            //Request an image from camera
            if(!camera.Snap(localFrame,frameRate))
            {
                failcount = 0;
                procReady.raise();
                saveReady.raise();

                //printf("CameraStreamThread: trying to lock\n");
                pthread_mutex_lock(&mutexImage);
                //printf("CameraStreamThread: got lock, copying over\n");
                localFrame.copyTo(frame);
                captureTimeNTP = localCaptureTime;
                captureTimeFixed = preExposure;
                //printf("%d\n", frame.at<uint8_t>(0,0));
                frameCount++;
                pthread_mutex_unlock(&mutexImage);
                staleFrame = false;

                //printf("camera temp is %lld\n", camera.getTemperature());
                camera_temperature = camera.getTemperature();
            }
            else
            {
                failcount++;
                std::cout << "Frame failure count = " << failcount << std::endl;
                if (failcount >= 10)
                {
                    camera.Stop();
                    camera.Disconnect();
                    cameraReady = false;
                    staleFrame = true;
                    std::cout << "*********************** RESETTING CAMERA ***********************************" << std::endl;
                    continue;
                }
            }

            //Make any changes to camera settings that happened since last exposure.
            if (localExposure != exposure) {
                localExposure = exposure;
                camera.SetExposure(localExposure);
            }
            
            if (localPreampGain != preampGain) {
                localPreampGain = preampGain;
                camera.SetPreAmpGain(localPreampGain);
            }
            
            if (localAnalogGain != analogGain) {
                localAnalogGain = analogGain;
                camera.SetAnalogGain(analogGain);
            }

            //Record time that camera exposure finished. As little as possible should happen between this call and the call to nanosleep()
            clock_gettime(CLOCK_MONOTONIC, &postExposure);

            //Determine time spent on camera exposure
            timeElapsed = TimespecDiff(preExposure, postExposure);

            //Calculate the time to wait for next exposure
            timeToWait.tv_sec = frameRate.tv_sec - timeElapsed.tv_sec;
            timeToWait.tv_nsec = frameRate.tv_nsec - timeElapsed.tv_nsec;
//            std::cout << timeElapsed.tv_sec << " " << timeElapsed.tv_nsec << "\n";

            //Wait till next exposure time
            nanosleep(&timeToWait, NULL);
        }
    }
}

void *ImageProcessThread(void *threadargs)
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("ImageProcess thread #%ld!\n", tid);

    CoordList localLimbs, localPixelFiducials, localScreenFiducials;
    IndexList localIds;
    uint8_t localMin, localMax;
    std::vector<float> localMapping;
    cv::Point2f localPixelCenter, localScreenCenter, localError;
    timespec waittime;

    waittime.tv_sec = frameRate.tv_sec/10;
    waittime.tv_nsec = frameRate.tv_nsec/10;
    
    while(1)
    {
        if (stop_message[tid] == 1)
        {
            printf("ImageProcess thread #%ld exiting\n", tid);
            started[tid] = false;
            pthread_exit( NULL );
        }
        
        if (cameraReady)
        {
            while(1)
            {
                if(procReady.check())
                {
                    procReady.lower();
                    break;
                }
                else
                {
                    nanosleep(&waittime, NULL);
                }
            }
    
            //printf("ImageProcessThread: trying to lock\n");
            if (pthread_mutex_trylock(&mutexImage) == 0)
            {
                //printf("ImageProcessThread: got lock\n");
                if(!frame.empty())
                {
                    aspect.LoadFrame(frame);

                    pthread_mutex_unlock(&mutexImage);
                    
                    runResult = aspect.Run();
                    
                    switch(GeneralizeError(runResult))
                    {
                        case NO_ERROR:
                            aspect.GetScreenFiducials(localScreenFiducials);
                            aspect.GetScreenCenter(localScreenCenter);
                            aspect.GetMapping(localMapping);

                        case MAPPING_ERROR:
                            aspect.GetFiducialIDs(localIds);

                        case ID_ERROR:
                            aspect.GetPixelFiducials(localPixelFiducials);

                        case FIDUCIAL_ERROR:
                            aspect.GetPixelCenter(localPixelCenter);
                            aspect.GetPixelError(localError);

                        case CENTER_ERROR:
                            aspect.GetPixelCrossings(localLimbs);
                            if (REPORT_FOCUS) aspect.ReportFocus();

                        case LIMB_ERROR:
                        case RANGE_ERROR:
                            aspect.GetPixelMinMax(localMin, localMax);
                            break;
                        default:
                            std::cout << "Nothing worked\n";
                    }

                    pthread_mutex_lock(&mutexProcess);
                    switch(GeneralizeError(runResult))
                    {
                        case NO_ERROR:
                            screenFiducials = localScreenFiducials;
                            screenCenter = localScreenCenter;
                            mapping = localMapping;
                        case MAPPING_ERROR:
                            ids = localIds;

                        case ID_ERROR:
                            pixelFiducials = localPixelFiducials;

                        case FIDUCIAL_ERROR:
                            pixelCenter = localPixelCenter;  
                            error = localError;

                        case CENTER_ERROR:
                            limbs = localLimbs;

                        case LIMB_ERROR:
                        case RANGE_ERROR:
                            frameMin = localMin;
                            frameMax = localMax;
                            break;
                        default:
                            break;
                    }
                    pthread_mutex_unlock(&mutexProcess);
                }
                else
                {
                    //std::cout << "Frame empty!" << std::endl;
                }

                /*
                  std::cout << ids.size() << " fiducials found:";
                  for(uint8_t i = 0; i < ids.size() && i < 20; i++) std::cout << pixelFiducials[i];
                  std::cout << std::endl;

                  for(uint8_t i = 0; i < ids.size() && i < 20; i++) std::cout << ids[i];
                  std::cout << std::endl;

                  for(uint8_t i = 0; i < ids.size() && i < 20; i++) std::cout << screenFiducials[i];
                  std::cout << std::endl;

                  std::cout << "Sun center (pixels): " << pixelCenter << ", Sun center (screen): " << screenCenter << std::endl;
                */
            }
        }
    }
}

void *SaveImageThread(void *threadargs)
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("SaveImage thread #%ld!\n", tid);

    cv::Mat localFrame;
    long int localFrameCount;
    std::string fitsfile;
    timespec waittime = {1,0};
    //timespec thetimenow;
    while(1)
    {
        if (stop_message[tid] == 1)
        {
            printf("SaveImage thread #%ld exiting\n", tid);
            started[tid] = false;
            pthread_exit( NULL );
        }
        if (cameraReady)
        {
            while(1)
            {
                if(saveReady.check())
                {
                    saveReady.lower();
                    break;
                }
                else
                {
                    nanosleep(&waittime, NULL);
                }
            }

            //printf("SaveImageThread: trying to lock\n");
            if (pthread_mutex_trylock(&mutexImage) == 0)
            {
                //printf("ImageProcessThread: got lock\n");
                if(!frame.empty())
                {
                    localFrameCount = frameCount;
                    frame.copyTo(localFrame);
                    keys.captureTimeFixed = captureTimeFixed;
                    keys.captureTimeNTP = captureTimeNTP;
                    keys.frameCount = frameCount;
                    pthread_mutex_unlock(&mutexImage);

                    keys.exposureTime = exposure;

                    char stringtemp[80];
                    char obsfilespec[128];
                    time_t ltime;
                    struct tm *times;

                    //Use clock_gettime instead?
                    time(&ltime);
                    times = localtime(&ltime);
                    strftime(stringtemp,40,"%y%m%d_%H%M%S",times);

                    sprintf(obsfilespec, "%simage_%s_%02d.fits", SAVE_LOCATION, stringtemp, (int)localFrameCount);

                    printf("Saving image %s: exposure %d us, analog gain %d, preamp gain %d\n", obsfilespec, exposure, analogGain, preampGain);
                    writeFITSImage(localFrame, keys, obsfilespec);

                    sleep(SLEEP_SAVE);
                }
                else
                {
                    pthread_mutex_unlock(&mutexImage);
                }
            }
        }
    }
}

void start_thread(void *(*routine) (void *), const Thread_data *tdata)
{
    int i = 0;
    while (started[i] == true) {
        i++;
        if (i == MAX_THREADS) return; //should probably thrown an exception
    }

    //Copy the thread data to a global to prevent deallocation
    if (tdata != NULL) memcpy(&thread_data[i], tdata, sizeof(Thread_data));
    thread_data[i].thread_id = i;

    stop_message[i] = 0;

    int rc = pthread_create(&threads[i], NULL, routine, &thread_data[i]);
    if (rc != 0) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    } else started[i] = true;

    return;
}

void start_all_workers( void ){
    start_thread(CameraStreamThread, NULL);
    start_thread(ImageProcessThread, NULL);
    start_thread(SaveImageThread, NULL);
}

int main(void)
{  
    // to catch a Ctrl-C and clean up
    signal(SIGINT, &sig_handler);

    identifySAS();
    if (sas_id == 1) isOutputting = true;

    pthread_mutex_init(&mutexImage, NULL);
    pthread_mutex_init(&mutexProcess, NULL);

    /* Create worker threads */
    printf("In main: creating threads\n");

    for(int i = 0; i < MAX_THREADS; i++ ){
        started[0] = false;
    }

    // start the listen for commands thread right away
    start_all_workers();

    while(g_running);

    /* Last thing that main() should do */
    printf("Quitting and cleaning up.\n");
    /* wait for threads to finish */
    kill_all_threads();
    pthread_mutex_destroy(&mutexImage);
    pthread_mutex_destroy(&mutexProcess);
    pthread_exit(NULL);

    return 0;
}

