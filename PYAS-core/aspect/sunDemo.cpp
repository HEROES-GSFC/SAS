#define MAX_THREADS 30
#define SAVE_LOCATION "/mnt/disk2/" // location for saving full images locally
#define REPORT_FOCUS false
#define LOG_PACKETS true

//Major settings
#define FRAME_CADENCE 250000 // microseconds

//Default camera settings
#define CAMERA_EXPOSURE 15000 // microseconds
#define CAMERA_ANALOGGAIN 400 // camera defaults to 143, but we are changing it
#define CAMERA_PREAMPGAIN -3 // camera defaults to +6, but we are changing it
#define CAMERA_XSIZE 1296 // full frame is 1296
#define CAMERA_YSIZE 966 //full frame is 966
#define CAMERA_XOFFSET 0
#define CAMERA_YOFFSET 0

//Sleep settings (seconds)
#define SLEEP_SOLUTION         1 // period for providing solutions to CTL
#define SLEEP_SAVE             0 // period for saving full images locally (set USLEEP_SAVE to 0 if used)
#define SLEEP_LOG_TEMPERATURE 10 // period for logging temperature locally
#define SLEEP_CAMERA_CONNECT   1 // waits for errors while connecting to camera
#define SLEEP_KILL             2 // waits when killing all threads

//Sleep settings (microseconds)
#define USLEEP_SAVE       250000 // period for saving full images locally (set SLEEP_SAVE to 0 if used)
#define USLEEP_CMD_SEND     5000 // period for popping off the command queue
#define USLEEP_TM_SEND     50000 // period for popping off the telemetry queue
#define USLEEP_TM_GENERIC 250000 // period for adding generic telemetry packets to queue
#define USLEEP_UDP_LISTEN   1000 // safety measure in case UDP listening is changed to non-blocking

#define SAS1_MAC_ADDRESS "00:20:9d:23:26:b9"
#define SAS2_MAC_ADDRESS "00:20:9d:23:5c:9e"

//IP addresses
#define IP_FDR      "192.168.2.4"   // will be 192.168.1.1 in flight
#define IP_CTL      "192.168.2.4"   // will be 192.168.1.2 in flight
#define IP_PYAS     "192.168.4.4"   // not yet implemented
#define IP_RAS      "192.168.8.8"   // not yet implemented

#define IP_LOOPBACK "127.0.0.1"   // should not change

//UDP ports, aside from PORT_IMAGE, which is TCP
#define PORT_CMD      2000 // commands, FDR (receive) and CTL (send/receive)
#define PORT_TM       2002 // send telemetry to FDR (except images)
#define PORT_IMAGE    2013 // send images to FDR, TCP port
#define PORT_SAS2     3000 // commands output from SAS2 to CTL are redirected here
#define PORT_SBC_INFO 3456 //

//HEROES target ID for commands, source ID for telemetry
#define TARGET_ID_CTL 0x01
#define TARGET_ID_SAS 0x30
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
#define SKEY_SET_IMAGESAVETOGGLE 0x0121
#define SKEY_SET_EXPOSURE        0x0151
#define SKEY_SET_ANALOGGAIN      0x0181
#define SKEY_SET_PREAMPGAIN      0x0191

//Getting commands
#define SKEY_REQUEST_IMAGE       0x0210
#define SKEY_GET_EXPOSURE        0x0250
#define SKEY_GET_ANALOGGAIN      0x0260
#define SKEY_GET_PREAMPGAIN      0x0270
#define SKEY_GET_DISKSPACE       0x0281

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
#include <sys/statvfs.h> /* for statvfs (get_disk_usage) */
#include <fstream>

#include "UDPSender.hpp"
#include "UDPReceiver.hpp"
#include "Command.hpp"
#include "Telemetry.hpp"
#include "Image.hpp"
#include "Transform.hpp"
#include "types.hpp"
#include "TCPSender.hpp"
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
uint16_t ctl_sequence_number = 0;

bool isTracking = false; // does CTL want solutions?
bool isOutputting = false; // is this SAS supposed to be outputting solutions?
bool acknowledgedCTL = true; // have we acknowledged the last command from CTL?
bool isSavingImages = true;  // is the SAS saving images?

CommandQueue recvd_command_queue;
TelemetryPacketQueue tm_packet_queue;
CommandPacketQueue cm_packet_queue;
ImagePacketQueue im_packet_queue;

// related to threads
unsigned int stop_message[MAX_THREADS];
pthread_t threads[MAX_THREADS];
bool started[MAX_THREADS];
int tid_listen = 0;
pthread_attr_t attr;
pthread_mutex_t mutexImage[2];
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

cv::Mat frame[2];

Aspect aspect;
AspectCode runResult;
Transform solarTransform;

uint8_t frameMin[2], frameMax[2];
cv::Point2f pixelCenter, screenCenter, error;
CoordList limbs, pixelFiducials, screenFiducials;
IndexList ids;
std::vector<float> mapping;
Pair offset;

HeaderData fits_keys[2];

bool staleFrame[2];
Flag procReady[2], saveReady[2];
int runtime = 10;

uint16_t exposure = CAMERA_EXPOSURE;
uint16_t analogGain = CAMERA_ANALOGGAIN;
int16_t preampGain = CAMERA_PREAMPGAIN;

timespec frameRate = {0,FRAME_CADENCE*1000};
bool cameraReady[2] = {false, false};

long int frameCount[2] = {0, 0};

float camera_temperature[2];
int8_t sbc_temperature;
int8_t i2c_temperatures[8];
float sbc_v105, sbc_v25, sbc_v33, sbc_v50, sbc_v120;

//Function declarations
void sig_handler(int signum);
void kill_all_threads( void ); //kills all threads
void kill_all_workers( void ); //kills all threads except the one that listens for commands
void identifySAS();

void *CameraStreamThread( void * threadargs, int camera_id);
void *PYASCameraStreamThread( void * threadargs);
void *RASCameraStreamThread( void * threadargs);

void *ImageProcessThread(void *threadargs, int camera_id);
void *PYASImageProcessThread(void *threadargs);
void *RASImageProcessThread(void *threadargs);

void *SaveImageThread(void *threadargs, int camera_id);
void *PYASSaveImageThread(void *threadargs);
void *RASSaveImageThread(void *threadargs);

void *TelemetrySenderThread(void *threadargs);
void *SBCInfoThread(void *threadargs);
void *SaveTemperaturesThread(void *threadargs);
void *TelemetryPackagerThread(void *threadargs);
void *listenForCommandsThread(void *threadargs);
void *CommandSenderThread( void *threadargs );
void *CommandPackagerThread( void *threadargs );
void *ForwardCommandsFromSAS2Thread( void *threadargs );
void queue_cmd_proc_ack_tmpacket( uint16_t error_code );
uint16_t cmd_send_image_to_ground( int camera_id );
void *commandHandlerThread(void *threadargs);
void cmd_process_heroes_command(uint16_t heroes_command);
void cmd_process_sas_command(uint16_t sas_command, Command &command);
void start_all_workers( void );
void start_thread(void *(*start_routine) (void *), const Thread_data *tdata);
uint16_t get_disk_usage( uint16_t disk );

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

void *PYASCameraStreamThread( void *threadargs)
{
    return CameraStreamThread(threadargs, 0);
}

void *RASCameraStreamThread( void *threadargs)
{
    return CameraStreamThread(threadargs, 1);
}

void *CameraStreamThread( void * threadargs, int camera_id)
{
    // camera_id refers to 0 PYAS, 1 is RAS (if valid)
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("%sCameraStream thread #%ld!\n", (camera_id == 1 ? "RAS" : "PYAS"), tid);

    char ip[50];

    switch (camera_id) {
        case 0: //PYAS
           strcpy(ip, IP_PYAS);
           break;
        case 1:
            strcpy(ip, IP_RAS);
            break;
        default:
            std::cerr << "Invalid camera specified!\n";
            stop_message[tid] = 1;
    }

    ImperxStream camera;

    cv::Mat localFrame;
    timespec localCaptureTime, preExposure, postExposure, timeElapsed, timeToWait;
    int width, height;
    int failcount = 0;

    uint16_t localExposure = exposure;
    int16_t localPreampGain = preampGain;
    uint16_t localAnalogGain = analogGain;

    cameraReady[camera_id] = false;
    staleFrame[camera_id] = true;
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
        else if (cameraReady[camera_id] == false)
        {
            if (camera.Connect(ip) != 0)
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
                cameraReady[camera_id] = true;
                frameCount[camera_id] = 0;
            }
        }
        else
        {
            // Record time of frame capture
            clock_gettime(CLOCK_MONOTONIC, &preExposure);
            clock_gettime(CLOCK_REALTIME, &localCaptureTime);

            // Need to send timestamp of the next SAS solution *before* the exposure is taken
            // Conceptually this would be part of CommandPackagerThread, but the timing requirement is strict
            if(isOutputting && isTracking && acknowledgedCTL) {
                ctl_sequence_number++;
                CommandPacket cp(TARGET_ID_CTL, ctl_sequence_number);
                cp << (uint16_t)HKEY_SAS_TIMESTAMP;
                cp << (uint16_t)0x0001;             // Camera ID (=1 for SAS, irrespective which SAS is providing solutions) 
                cp << (double)(preExposure.tv_sec + (double)preExposure.tv_nsec/1e9);  // timestamp 
                cm_packet_queue << cp;
            }

            if(!camera.Snap(localFrame, frameRate))
            {
                failcount = 0;
                procReady[camera_id].raise();
                saveReady[camera_id].raise();

                //printf("CameraStreamThread: trying to lock\n");
                pthread_mutex_lock(mutexImage+camera_id);
                //printf("CameraStreamThread: got lock, copying over\n");
                localFrame.copyTo(frame[camera_id]);
                //printf("%d\n", frame.at<uint8_t>(0,0));
                frameCount[camera_id]++;
                pthread_mutex_unlock(mutexImage+camera_id);
                staleFrame[camera_id] = false;

                //printf("camera temp is %lld\n", camera.getTemperature());
                camera_temperature[camera_id] = camera.getTemperature();
                
                // save data into the fits_header
                fits_keys[camera_id].captureTime = localCaptureTime;
                fits_keys[camera_id].captureTimeMono = preExposure;
                fits_keys[camera_id].frameCount = frameCount[camera_id];
                fits_keys[camera_id].exposure = exposure;
                fits_keys[camera_id].preampGain = preampGain;
                fits_keys[camera_id].analogGain = analogGain;
                fits_keys[camera_id].cameraTemperature = camera_temperature[camera_id];

            }
            else
            {
                failcount++;
                std::cout << "Frame failure count = " << failcount << std::endl;
                if (failcount >= 10)
                {
                    camera.Stop();
                    camera.Disconnect();
                    cameraReady[camera_id] = false;
                    staleFrame[camera_id] = true;
                    failcount = 0;
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

void *PYASImageProcessThread( void *threadargs)
{
    return ImageProcessThread(threadargs, 0);
}

void *RASImageProcessThread( void *threadargs)
{
    return ImageProcessThread(threadargs, 1);
}

void *ImageProcessThread(void *threadargs, int camera_id)
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("%sImageProcess thread #%ld!\n", (camera_id == 1 ? "RAS" : "PYAS"), tid);

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
            printf("%sImageProcess thread #%ld exiting\n", (camera_id == 1 ? "RAS" : "PYAS"), tid);
            started[tid] = false;
            pthread_exit( NULL );
        }
        
        if (cameraReady[0])
        {
            while(1)
            {
                if(procReady[camera_id].check())
                {
                    procReady[camera_id].lower();
                    break;
                }
                else
                {
                    nanosleep(&waittime, NULL);
                }
            }
    
            //printf("ImageProcessThread: trying to lock\n");
            if (pthread_mutex_trylock(mutexImage+camera_id) == 0)
            {
                //printf("ImageProcessThread: got lock\n");
                if((camera_id == 0) && !frame[camera_id].empty())
                {
                    aspect.LoadFrame(frame[0]);

                    pthread_mutex_unlock(mutexImage+camera_id);
                    
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

                    //printf("Aspect result: %s\n", GetMessage(runResult));

                    pthread_mutex_lock(&mutexProcess);
                    switch(GeneralizeError(runResult))
                    {
                        case NO_ERROR:
                            solarTransform.set_conversion(Pair(localMapping[0],localMapping[2]),Pair(localMapping[1],localMapping[3]));
                            offset = solarTransform.calculateOffset(Pair(localPixelCenter.x,localPixelCenter.y));

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
                            frameMin[0] = localMin;
                            frameMax[0] = localMax;
                            break;
                        default:
                            break;
                    }
                    
                    fits_keys[0].sunCenter[0] = pixelCenter.x;
                    fits_keys[0].sunCenter[1] = pixelCenter.y;
                  
                    fits_keys[0].CTLsolution[0] = offset.x();
                    fits_keys[0].CTLsolution[1] = offset.y();

                    fits_keys[0].screenCenter[0] = screenCenter.x; 
                    fits_keys[0].screenCenter[1] = screenCenter.y;
                    fits_keys[0].screenCenterError[0] = error.x;
                    fits_keys[0].screenCenterError[1] = error.y;
                    fits_keys[0].imageMinMax[0] = frameMin[0];
                    fits_keys[0].imageMinMax[1] = frameMax[1];

                    if(mapping.size() == 4){
                        fits_keys[0].XYinterceptslope[0] = mapping[0];
                        fits_keys[0].XYinterceptslope[1] = mapping[2];
                        fits_keys[0].XYinterceptslope[2] = mapping[1];
                        fits_keys[0].XYinterceptslope[3] = mapping[3];
                    }
                    fits_keys[0].isTracking = isTracking;
                    
                    for(uint8_t j = 0; j < 8; j++) {
                        if (j < limbs.size()) {
                                fits_keys[0].limbX[j] = limbs[j].x,
                                fits_keys[0].limbY[j] = limbs[j].y;
                            } else {
                                fits_keys[0].limbX[j] = 0,
                                fits_keys[0].limbY[j] = 0;
                            }
                    }
                    for(uint8_t j = 0; j < 8; j++) {
                        if (j < ids.size()) {
                            fits_keys[0].fiducialIDX[j] = ids[j].x,
                            fits_keys[0].fiducialIDY[j] = ids[j].y;
                        } else {
                            fits_keys[0].fiducialIDX[j] = 0,
                            fits_keys[0].fiducialIDY[j] = 0;
                        }
                        if (j < pixelFiducials.size()){
                            fits_keys[0].fiducialX[j] = pixelFiducials[j].x;
                            fits_keys[0].fiducialY[j] = pixelFiducials[j].y;
                        } else {
                            fits_keys[0].fiducialX[j] = 0;
                            fits_keys[0].fiducialY[j] = 0;
                        }
                    }
                    pthread_mutex_unlock(&mutexProcess);
                }
                else if((camera_id == 1) && !frame[camera_id].empty()) {
                    double min, max;
                    cv::minMaxLoc(frame[1], &min, &max, NULL, NULL);
                    fits_keys[0].imageMinMax[0] = (uint8_t)min;
                    fits_keys[0].imageMinMax[1] = (uint8_t)max;

                    pthread_mutex_unlock(mutexImage+camera_id);
                }
                else
                {
                    pthread_mutex_unlock(mutexImage+camera_id);
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

void *TelemetrySenderThread(void *threadargs)
{    
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("TelemetrySender thread #%ld!\n", tid);

    char stringtemp[80];
    char filename[128];
    time_t ltime;
    struct tm *times;
    std::ofstream log; 

    if (LOG_PACKETS) {
        time(&ltime);
        times = localtime(&ltime);
        strftime(stringtemp,40,"%y%m%d_%H%M%S",times);
        sprintf(filename, "%slog_tm_%s.bin", SAVE_LOCATION, stringtemp);
        filename[128 - 1] = '\0';
        printf("Creating telemetry log file %s \n",filename);
        log.open(filename, std::ofstream::binary);
    }

    TelemetrySender telSender(IP_FDR, (unsigned short) PORT_TM);

    while(1)    // run forever
    {
        usleep(USLEEP_TM_SEND);

        if( !tm_packet_queue.empty() ){
            TelemetryPacket tp(NULL);
            tm_packet_queue >> tp;
            telSender.send( &tp );
            //std::cout << "TelemetrySender:" << tp << std::endl;
            if (LOG_PACKETS) {
                uint8_t length = tp.getLength();
                uint8_t *payload = new uint8_t[length];
                tp.outputTo(payload);
                log.write((char *)payload, length);
                delete payload;
                log.flush();
            }
        }

        if (stop_message[tid] == 1){
            printf("TelemetrySender thread #%ld exiting\n", tid);
            started[tid] = false;
            pthread_exit( NULL );
        }
    }
}

void *SBCInfoThread(void *threadargs)
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("SBCInfo thread #%ld!\n", tid);

    UDPReceiver receiver(PORT_SBC_INFO);
    receiver.init_connection();

    uint16_t packet_length;
    uint8_t *array;

    while(1)
    {
        if (stop_message[tid] == 1)
        {
            printf("SBCInfo thread #%ld exiting\n", tid);
            started[tid] = false;
            pthread_exit( NULL );
        }

        //This call will block forever if the service is not running
        usleep(USLEEP_UDP_LISTEN);
        packet_length = receiver.listen();
        array = new uint8_t[packet_length];
        receiver.get_packet(array);

        Packet packet( array, packet_length );
        packet >> sbc_temperature >> sbc_v105 >> sbc_v25 >> sbc_v33 >> sbc_v50 >> sbc_v120;
        for (int i=0; i<8; i++) packet >> i2c_temperatures[i];
        delete array;
    }
}

void *SaveTemperaturesThread(void *threadargs)
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("SaveTemperatures thread #%ld!\n", tid);

    char stringtemp[80];
    char obsfilespec[128];
    FILE *file;
    time_t ltime;
    struct tm *times;

    time(&ltime);
    times = localtime(&ltime);
    strftime(stringtemp,40,"%y%m%d_%H%M%S",times);
    sprintf(obsfilespec, "%stemp_data_%s.dat", SAVE_LOCATION, stringtemp);
    obsfilespec[128 - 1] = '\0';
    printf("Creating file %s \n",obsfilespec);

    int count = 0;

    if((file = fopen(obsfilespec, "w")) == NULL){
        printf("Cannot open file\n");
        started[tid] = false;
        pthread_exit( NULL );
    } else {
        fprintf(file, "time, camera temp, cpu temp, i2c temp x8\n");
        sleep(SLEEP_LOG_TEMPERATURE);
        while(1)
        {
            char current_time[25];
            if (stop_message[tid] == 1)
            {
                printf("SaveTemperatures thread #%ld exiting\n", tid);
                fclose(file);
                started[tid] = false;
                pthread_exit( NULL );
            }
            sleep(SLEEP_LOG_TEMPERATURE);

            time(&ltime);
            times = localtime(&ltime);
            strftime(current_time,25,"%y/%m/%d %H:%M:%S",times);
            fprintf(file, "%s, %f, %d", current_time, camera_temperature[count % sas_id], sbc_temperature);
            for (int i=0; i<8; i++) fprintf(file, ", %d", i2c_temperatures[i]);
            fprintf(file, "\n");
            printf("%s, %f, %d", current_time, camera_temperature[count % sas_id], sbc_temperature);
            for (int i=0; i<8; i++) printf(", %d", i2c_temperatures[i]);
            printf("\n");
            count++;
        }
    }
}

void *PYASSaveImageThread( void *threadargs)
{
    return SaveImageThread(threadargs, 0);
}

void *RASSaveImageThread( void *threadargs)
{
    return SaveImageThread(threadargs, 1);
}

void *SaveImageThread(void *threadargs, int camera_id)
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("%sSaveImage thread #%ld!\n", (camera_id == 1 ? "RAS" : "PYAS"), tid);

    cv::Mat localFrame;
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
        if (cameraReady[camera_id])
        {
            while(1)
            {
                if(saveReady[camera_id].check() && isSavingImages)
                {
                    saveReady[camera_id].lower();
                    break;
                }
                else
                {
                    nanosleep(&waittime, NULL);
                }
            }

            //printf("SaveImageThread: trying to lock\n");
            if (pthread_mutex_trylock(mutexImage+camera_id) == 0)
            {
                //printf("ImageProcessThread: got lock\n");
                if(!frame[camera_id].empty())
                {
                    frame[camera_id].copyTo(localFrame);
                    fits_keys[camera_id].cpuTemperature = sbc_temperature;
                    fits_keys[camera_id].cameraID = sas_id+4*camera_id;

                    fits_keys[camera_id].cpuVoltage[0] = sbc_v105;
                    fits_keys[camera_id].cpuVoltage[1] = sbc_v25;
                    fits_keys[camera_id].cpuVoltage[2] = sbc_v33;
                    fits_keys[camera_id].cpuVoltage[3] = sbc_v50;
                    fits_keys[camera_id].cpuVoltage[4] = sbc_v120;
                    
                    pthread_mutex_unlock(mutexImage+camera_id);

                    char stringtemp[80];
                    char obsfilespec[128];
                    time_t ltime;
                    struct tm *times;

                    //Use clock_gettime instead?
                    time(&ltime);
                    times = localtime(&ltime);
                    strftime(stringtemp,40,"%y%m%d_%H%M%S",times);

                    sprintf(obsfilespec, "%s%s_%s_%06d.fits", SAVE_LOCATION, (camera_id == 1 ? "ras" : "pyas"), stringtemp, (int)fits_keys[camera_id].frameCount);

                    printf("Saving image %s: exposure %d us, analog gain %d, preamp gain %d\n", obsfilespec, exposure, analogGain, preampGain);
                    writeFITSImage(localFrame, fits_keys[camera_id], obsfilespec);

                    sleep(SLEEP_SAVE);
                    usleep(USLEEP_SAVE);
                }
                else
                {
                    pthread_mutex_unlock(mutexImage+camera_id);
                }
            }
        }
    }
}

void *TelemetryPackagerThread(void *threadargs)
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("TelemetryPackager thread #%ld!\n", tid);

    unsigned char localMin, localMax;
    CoordList localLimbs, localFiducials;
    std::vector<float> localMapping;
    cv::Point2f localCenter, localError;
    Pair localOffset;

    while(1)    // run forever
    {
        usleep(USLEEP_TM_GENERIC);
        tm_frame_sequence_number++;

        TelemetryPacket tp(TM_SAS_GENERIC, SOURCE_ID_SAS);
        tp.setSAS(sas_id);
        tp << tm_frame_sequence_number;
        tp << command_sequence_number;
        tp << latest_sas_command_key;

        if(pthread_mutex_trylock(&mutexProcess) == 0)
        {
            localMin = frameMin[tm_frame_sequence_number % sas_id];
            localMax = frameMax[tm_frame_sequence_number % sas_id];
            localLimbs = limbs;
            localCenter = pixelCenter;
            localError = error;
            localFiducials = pixelFiducials;
            localMapping = mapping;
            localOffset = offset;

            std::cout << "Telemetry packet with Sun center (pixels): " << localCenter;
            if(localMapping.size() == 4) {
                std::cout << ", mapping is";
                for(uint8_t l = 0; l < 4; l++) std::cout << " " << localMapping[l];
            }
            std::cout << std::endl;

            std::cout << "Offset: " << localOffset << std::endl;

            pthread_mutex_unlock(&mutexProcess);
        } else {
            std::cout << "Using stale information for telemetry packet" << std::endl;
        }

        //Housekeeping fields, two of them
        tp << Float2B(camera_temperature[tm_frame_sequence_number % sas_id]);
        tp << (uint16_t)sbc_temperature;

        //Sun center and error
        tp << Pair3B(localCenter.x, localCenter.y);
        tp << Pair3B(localError.x, localError.y);

        //Predicted Sun center and error
        tp << Pair3B(0, 0);
        tp << Pair3B(0, 0);

        //Number of limb crossings
        tp << (uint16_t)localLimbs.size();

        //Limb crossings (currently 8)
        for(uint8_t j = 0; j < 8; j++) {
            if (j < localLimbs.size()) {
                uint8_t jp = (j+tm_frame_sequence_number) % localLimbs.size();
                tp << Pair3B(localLimbs[jp].x, localLimbs[jp].y);
            } else {
                tp << Pair3B(0, 0);
            }
        }

        //Number of fiducials
        tp << (uint16_t)localFiducials.size();

        //Fiduicals (currently 6)
        for(uint8_t k = 0; k < 6; k++) {
            if (k < localFiducials.size()) {
                uint8_t kp = (k+tm_frame_sequence_number) % localFiducials.size();
                tp << Pair3B(localFiducials[kp].x, localFiducials[kp].y);
            } else {
                tp << Pair3B(0, 0);
            }
        }

        //Pixel to screen conversion
        if(localMapping.size() == 4) {
            tp << localMapping[0]; //X intercept
            tp << localMapping[1]; //X slope
            tp << localMapping[2]; //Y intercept
            tp << localMapping[3]; //Y slope
        } else {
            tp << (float)-3000; //X intercept
            tp << (float)6; //X slope
            tp << (float)3000; //Y intercept
            tp << (float)-6; //Y slope
        }

        //Image max and min
        tp << (uint8_t) localMax; //max
        tp << (uint8_t) localMin; //min

        //Tacking on the offset numbers intended for CTL
        tp << localOffset;

        //Tacking on I2C temperatures
        for (int i=0; i<8; i++) tp << i2c_temperatures[i];

        //add telemetry packet to the queue
        tm_packet_queue << tp;
            
        if (stop_message[tid] == 1){
            printf("TelemetryPackager thread #%ld exiting\n", tid);
            started[tid] = false;
            pthread_exit( NULL );
        }
    }

    /* NEVER REACHED */
    return NULL;
}

void *listenForCommandsThread(void *threadargs)
{  
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("listenForCommands thread #%ld!\n", tid);

    tid_listen = tid;

    CommandReceiver comReceiver( (unsigned short) PORT_CMD);
    comReceiver.init_connection();

    while(1)    // run forever
    {
        unsigned int packet_length;

        usleep(USLEEP_UDP_LISTEN);
        packet_length = comReceiver.listen( );
        printf("listenForCommandsThread: %i\n", packet_length);
        uint8_t *packet;
        packet = new uint8_t[packet_length];
        comReceiver.get_packet( packet );

        CommandPacket command_packet( packet, packet_length );

        if (command_packet.valid()){
            printf("listenForCommandsThread: good command packet\n");

            command_sequence_number = command_packet.getSequenceNumber();

            if (sas_id == 1) {
                // add command ack packet
                TelemetryPacket ack_tp(TM_ACK_RECEIPT, SOURCE_ID_SAS);
                ack_tp << command_sequence_number;
                tm_packet_queue << ack_tp;
            }

            // update the command count
            printf("command sequence number to %i\n", command_sequence_number);

            if (command_packet.getTargetID() == TARGET_ID_SAS) {
                try { recvd_command_queue.add_packet(command_packet); }
                catch (std::exception& e) {
                    std::cerr << e.what() << std::endl;
                }
            }

        } else {
            printf("listenForCommandsThread: bad command packet\n");
        }

        delete packet;

        if (stop_message[tid] == 1){
            printf("listenForCommands thread #%ld exiting\n", tid);
            comReceiver.close_connection();
            started[tid] = false;
            pthread_exit( NULL );
        }
    }

    /* NEVER REACHED */
    return NULL;
}

void *ForwardCommandsFromSAS2Thread(void *threadargs)
{  
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("ForwardCommandsFromSAS2 thread #%ld!\n", tid);

    tid_listen = tid;

    CommandReceiver comReceiver( (unsigned short) PORT_SAS2);
    comReceiver.init_connection();

    CommandSender comForwarder(IP_CTL, PORT_CMD);

    while(1)    // run forever
    {
        unsigned int packet_length;

        usleep(USLEEP_UDP_LISTEN);
        packet_length = comReceiver.listen( );
        printf("ForwardCommandsFromSAS2Thread: %i\n", packet_length);
        uint8_t *packet;
        packet = new uint8_t[packet_length];
        comReceiver.get_packet( packet );

        CommandPacket command_packet( packet, packet_length );

        if (command_packet.valid()){
            if (isOutputting) {
                //SAS-1 is outputting, so discard SAS-2 output
                printf("ForwardCommandsFromSAS2Thread: blocking SAS-2 output\n");
            } else {
                //SAS-1 is not outputting, so forward up SAS-2 output
                comForwarder.send(&command_packet);
            }
        }
        delete packet;

        if (stop_message[tid] == 1){
            printf("ForwardCommandsFromSAS2 thread #%ld exiting\n", tid);
            comReceiver.close_connection();
            started[tid] = false;
            pthread_exit( NULL );
        }
    }

    /* NEVER REACHED */
    return NULL;
}

void *CommandSenderThread( void *threadargs )
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("CommandSender thread #%ld!\n", tid);

    char stringtemp[80];
    char filename[128];
    time_t ltime;
    struct tm *times;
    std::ofstream log; 

    if (LOG_PACKETS) {
        time(&ltime);
        times = localtime(&ltime);
        strftime(stringtemp,40,"%y%m%d_%H%M%S",times);
        sprintf(filename, "%slog_cm_%s.bin", SAVE_LOCATION, stringtemp);
        filename[128 - 1] = '\0';
        printf("Creating command log file %s \n",filename);
        log.open(filename, std::ofstream::binary);
    }

    CommandSender comSender(IP_CTL, PORT_CMD);

    while(1)    // run forever
    {
        usleep(USLEEP_CMD_SEND);
    
        if( !cm_packet_queue.empty() ){
            CommandPacket cp(NULL);
            cm_packet_queue >> cp;
            comSender.send( &cp );
            //std::cout << "CommandSender: " << cp << std::endl;
            if (LOG_PACKETS) {
                uint8_t length = cp.getLength();
                uint8_t *payload = new uint8_t[length];
                cp.outputTo(payload);
                log.write((char *)payload, length);
                delete payload;
                log.flush();
            }
        }

        if (stop_message[tid] == 1){
            printf("CommandSender thread #%ld exiting\n", tid);
            started[tid] = false;
            pthread_exit( NULL );
        }
    }
}

void *CommandPackagerThread( void *threadargs )
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("CommandPackager thread #%ld!\n", tid);

    while(1)    // run forever
    {
        sleep(SLEEP_SOLUTION);

        if (isOutputting) {
            ctl_sequence_number++;
            CommandPacket cp(TARGET_ID_CTL, ctl_sequence_number);

            if (isTracking) {
                if (!acknowledgedCTL) {
                    cp << (uint16_t)HKEY_SAS_TRACKING_IS_ON;
                    acknowledgedCTL = true;
                } else {
                    if(pthread_mutex_trylock(&mutexProcess) == 0)
                    {
                        cp << (uint16_t)HKEY_SAS_SOLUTION;
                        cp << (double)fits_keys[0].CTLsolution[0]; // azimuth offset
                        cp << (double)fits_keys[0].CTLsolution[1]; // elevation offset
                        cp << (double)0; // roll offset
                        cp << (double)0.003; // error
                        cp << (uint32_t)fits_keys[0].captureTime.tv_sec; //seconds
                        cp << (uint16_t)(fits_keys[0].captureTime.tv_nsec/1000000); //milliseconds

                        pthread_mutex_unlock(&mutexProcess);
                    } else {
                        std::cout << "Could not send a new solution packet\n";
                    }
                }
            } else { // isTracking is false
                if (!acknowledgedCTL) {
                    cp << (uint16_t)HKEY_SAS_TRACKING_IS_OFF;
                    acknowledgedCTL = true;
                }
            } // isTracking

            //Add packet to the queue if any commands have been inserted to the packet
            if(cp.remainingBytes() > 0) {
                cm_packet_queue << cp;
            }
        } // isOutputting

        if (stop_message[tid] == 1){
            printf("CommandPackager thread #%ld exiting\n", tid);
            started[tid] = false;
            pthread_exit( NULL );
        }
    }

    /* NEVER REACHED */
    return NULL;
}

void queue_cmd_proc_ack_tmpacket( uint16_t error_code )
{
    TelemetryPacket ack_tp(TM_ACK_PROCESS, SOURCE_ID_SAS);
    ack_tp << command_sequence_number;
    ack_tp << latest_sas_command_key;
    ack_tp << error_code;
    tm_packet_queue << ack_tp;
}

uint16_t cmd_send_image_to_ground( int camera_id )
{
    // camera_id refers to 0 PYAS, 1 is RAS (if valid)
    uint16_t error_code = 0;
    cv::Mat localFrame;
    HeaderData localKeys;

    TCPSender tcpSndr(IP_FDR, (unsigned short) PORT_IMAGE);
    int ret = tcpSndr.init_connection();
    if (ret > 0){
        if (pthread_mutex_trylock(mutexImage+camera_id) == 0){
            if( !frame[camera_id].empty() ){
                frame[camera_id].copyTo(localFrame);
                localKeys = fits_keys[camera_id];
            }
            pthread_mutex_unlock(mutexImage+camera_id);
        }
        if( !localFrame.empty() ){
            //1 for SAS-1/PYAS, 2 for SAS-2/PYAS, 6 for SAS-2/RAS
            uint8_t camera = sas_id+4*camera_id;

            uint16_t numXpixels = localFrame.cols;
            uint16_t numYpixels = localFrame.rows;
            uint32_t totalpixels = numXpixels*numYpixels;

            //Copy localFrame to a C array
            uint8_t *array = new uint8_t[totalpixels];

            uint16_t rows = (localFrame.isContinuous() ? 1 : localFrame.rows);
            uint32_t cols = totalpixels/rows;

            for (int j = 0; j < rows; j++) {
                memcpy(array+j*cols, localFrame.ptr<uint8_t>(j), cols);
            }

            im_packet_queue.add_array(camera, numXpixels, numYpixels, array);

            delete array;

            //Add FITS header tags
            uint32_t temp = localKeys.exposure;
            im_packet_queue << ImageTagPacket(camera, &temp, TLONG, "EXPOSURE", "Exposure time (msec)");

            //Make sure to synchronize all the timestamps
            im_packet_queue.synchronize();

            std::cout << "Sending " << im_packet_queue.size() << " packets\n";

            ImagePacket im(NULL);
            while(!im_packet_queue.empty()) {
                im_packet_queue >> im;
                tcpSndr.send_packet( &im );
            }

        }
        tcpSndr.close_connection();
        error_code = 1;
    } else { error_code = 2; }
    return error_code;
}
        
void *commandHandlerThread(void *threadargs)
{
    // command error code definition
    // error_code   description
    // 0x0000       command implemented successfully
    // 0x0001       command not implemented
    // 0xffff       unknown command
    // 
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    struct Thread_data *my_data;
    uint16_t error_code = 0x0001;
    my_data = (struct Thread_data *) threadargs;

    switch( my_data->command_key & 0x0FFF)
    {
        case SKEY_REQUEST_IMAGE:
            {
                error_code = cmd_send_image_to_ground( 0 );
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_SET_EXPOSURE:    // set exposure time
            {
                if(my_data->command_num_vars == 1) exposure = my_data->command_vars[0];
                if( exposure == my_data->command_vars[0] ) error_code = 0;
                std::cout << "Requested exposure time is: " << exposure << std::endl;
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_SET_IMAGESAVETOGGLE:
            {
                if(my_data->command_num_vars == 1) isSavingImages = (my_data->command_vars[0] > 0);
                if( isSavingImages == my_data->command_vars[0] ) error_code = 0;
                if( isSavingImages == true ){ std::cout << "Image saving is now turned on" << std::endl; }
                if( isSavingImages == false ){ std::cout << "Image saving is now turned off" << std::endl; }
                queue_cmd_proc_ack_tmpacket( error_code );
            }
        case SKEY_SET_PREAMPGAIN:    // set preamp gain
            {
                if( my_data->command_num_vars == 1) preampGain = (int16_t)my_data->command_vars[0];
                if( preampGain == (int16_t)my_data->command_vars[0] ) error_code = 0;
                std::cout << "Requested preamp gain is: " << preampGain << std::endl;
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_SET_ANALOGGAIN:    // set analog gain
            {
                if( my_data->command_num_vars == 1) analogGain = my_data->command_vars[0];
                if( analogGain == my_data->command_vars[0] ) error_code = 0;
                std::cout << "Requested analog gain is: " << analogGain << std::endl;
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_SET_TARGET:    // set new solar target
            solarTransform.set_solar_target(Pair((int16_t)my_data->command_vars[0], (int16_t)my_data->command_vars[1]));
            break;
        case SKEY_START_OUTPUTTING:
            {
                isOutputting = true;
            }
            break;
        case SKEY_STOP_OUTPUTTING:
            {
                isOutputting = false;
            }
            break;
        case SKEY_GET_EXPOSURE:
            {
                queue_cmd_proc_ack_tmpacket( (uint16_t)exposure );
            }
        case SKEY_GET_ANALOGGAIN:
            {
                queue_cmd_proc_ack_tmpacket( (uint16_t)analogGain );
            }
        case SKEY_GET_PREAMPGAIN:
            {
                queue_cmd_proc_ack_tmpacket( (int16_t)preampGain );
            }
        case SKEY_GET_DISKSPACE:
            {
                if( my_data->command_num_vars == 1) {
                    uint16_t disk = (uint16_t)my_data->command_vars[0];
                    error_code = get_disk_usage( disk );
                }
                queue_cmd_proc_ack_tmpacket( error_code );
            }
        default:
            {
                error_code = 0xffff;            // unknown command!
                queue_cmd_proc_ack_tmpacket( error_code );
            }
    }

    started[tid] = false;
    return NULL;
}

void cmd_process_heroes_command(uint16_t heroes_command)
{
    if ((heroes_command & 0xFF00) == 0x1000) {
        switch(heroes_command) {
            case HKEY_CTL_START_TRACKING: // start tracking
                isTracking = true;
                acknowledgedCTL = false;
                // need to send 0x1100 command packet
                break;
            case HKEY_CTL_STOP_TRACKING: // stop tracking
                isTracking = false;
                acknowledgedCTL = false;
                // need to send 0x1101 command packet
                break;
            case HKEY_FDR_SAS_CMD: // SAS command, so do nothing here
                break;
            default:
                printf("Unknown HEROES command\n");
        }
    } else printf("Not a HEROES-to-SAS command\n");
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

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    int rc = pthread_create(&threads[i], &attr, routine, &thread_data[i]);
    if (rc != 0) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    } else started[i] = true;

    pthread_attr_destroy(&attr);

    return;
}

uint16_t get_disk_usage( uint16_t disk ){
    struct statvfs vfs;
    switch (disk) {
        case 1:
            statvfs("/mnt/disk1/", &vfs);
            break;
        case 2:
            statvfs("/mnt/disk2/", &vfs);
            break;
        default:
            return 0;
    }

    unsigned long total = vfs.f_blocks * vfs.f_frsize / 1024;
    unsigned long available = vfs.f_bavail * vfs.f_frsize / 1024;
    unsigned long free = vfs.f_bfree * vfs.f_frsize / 1024;
    unsigned long used = total - free;

    uintmax_t u100 = used * 100;
    uintmax_t nonroot_total = used + available;
    uint16_t percent = u100 / nonroot_total + (u100 % nonroot_total != 0);
    return( percent );
}

void cmd_process_sas_command(uint16_t sas_command, Command &command)
{
    Thread_data tdata;

    if ((sas_command & (sas_id << 12)) != 0) {
        tdata.command_key = sas_command;
        tdata.command_num_vars = sas_command & 0x000F;

        for(int i = 0; i < tdata.command_num_vars; i++){
            try {
              command >> tdata.command_vars[i];
            } catch (std::exception& e) {
               std::cerr << e.what() << std::endl;
            }
        }

        switch( sas_command & 0x0FFF){
            case SKEY_OP_DUMMY:     // test, do nothing
                queue_cmd_proc_ack_tmpacket( 0 );
                break;
            case SKEY_KILL_WORKERS:    // kill all worker threads
                {
                    kill_all_workers();
                    queue_cmd_proc_ack_tmpacket( 0 );
                }
                break;
            case SKEY_RESTART_THREADS:    // (re)start all worker threads
                {
                    kill_all_threads();

                    start_thread(listenForCommandsThread, NULL);
                    start_all_workers();
                    queue_cmd_proc_ack_tmpacket( 0 );
                }
                break;
            default:
                {
                    start_thread(commandHandlerThread, &tdata);
                }
        } //switch
    } else printf("Not the intended SAS for this command\n");
}

void start_all_workers( void )
{
    start_thread(TelemetryPackagerThread, NULL);
    start_thread(CommandPackagerThread, NULL);
    start_thread(TelemetrySenderThread, NULL);
    start_thread(CommandSenderThread, NULL);
    start_thread(PYASCameraStreamThread, NULL);
    start_thread(PYASImageProcessThread, NULL);
    start_thread(PYASSaveImageThread, NULL);
    start_thread(SaveTemperaturesThread, NULL);
    start_thread(SBCInfoThread, NULL);
    switch (sas_id) {
        case 1:
            start_thread(ForwardCommandsFromSAS2Thread, NULL);
            break;
        case 2:
            start_thread(RASCameraStreamThread, NULL);
            start_thread(RASImageProcessThread, NULL);
            start_thread(RASSaveImageThread, NULL);
            break;
        default:
            break;
    }
}

int main(void)
{  
    // to catch a Ctrl-C and clean up
    signal(SIGINT, &sig_handler);

    identifySAS();
    if (sas_id == 1) isOutputting = true;

    pthread_mutex_init(mutexImage, NULL);
    pthread_mutex_init(mutexImage+1, NULL);
    pthread_mutex_init(&mutexProcess, NULL);

    /* Create worker threads */
    printf("In main: creating threads\n");

    for(int i = 0; i < MAX_THREADS; i++ ){
        started[0] = false;
    }

    // start the listen for commands thread right away
    start_thread(listenForCommandsThread, NULL);
    start_all_workers();

    while(g_running){
        // check if new command have been added to command queue and service them
        if (!recvd_command_queue.empty()){
            printf("size of queue: %zu\n", recvd_command_queue.size());
            Command command;
            command = Command();
            recvd_command_queue >> command;

            latest_heroes_command_key = command.get_heroes_command();
            latest_sas_command_key = command.get_sas_command();
            printf("Received command key 0x%x/0x%x\n", latest_heroes_command_key, latest_sas_command_key);

            cmd_process_heroes_command(latest_heroes_command_key);
            if(latest_heroes_command_key == HKEY_FDR_SAS_CMD) {
                cmd_process_sas_command(latest_sas_command_key, command);
            }
        }
    }

    /* Last thing that main() should do */
    printf("Quitting and cleaning up.\n");
    /* wait for threads to finish */
    kill_all_threads();
    pthread_mutex_destroy(mutexImage);
    pthread_mutex_destroy(mutexImage+1);
    pthread_mutex_destroy(&mutexProcess);
    pthread_exit(NULL);

    return 0;
}

