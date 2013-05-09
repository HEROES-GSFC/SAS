#define MAX_THREADS 30
#define SAVE_LOCATION "/mnt/disk2/" // location for saving full images locally
#define REPORT_FOCUS false
#define LOG_PACKETS true
#define USE_MOCK_PYAS_IMAGE false
#define MOCK_PYAS_IMAGE "/mnt/disk1/130421/image_130421_153000_45361.fits"

//Major settings
#define FRAME_CADENCE 250000 // microseconds

//Frequency settings, do each per this many snaps (be mindful of non-multiples)
#define MOD_PROCESS 1 //process the image
#define MOD_CTL     4 //send the processing results to CTL
#define MOD_SAVE    20 //save the image to a local FITS file

//Sleep settings (seconds)
#define SLEEP_LOG_TEMPERATURE 10 // period for logging temperature locally
#define SLEEP_CAMERA_CONNECT   1 // waits for errors while connecting to camera
#define SLEEP_KILL             2 // waits when killing all threads

//Sleep settings (microseconds)
#define USLEEP_CMD_SEND     5000 // period for popping off the command queue
#define USLEEP_TM_SEND     50000 // period for popping off the telemetry queue
#define USLEEP_TM_GENERIC 950000 // period for adding generic telemetry packets to queue
#define USLEEP_UDP_LISTEN   1000 // safety measure in case UDP listening is changed to non-blocking

#define SAS1_MAC_ADDRESS "00:20:9d:23:26:b9"
#define SAS2_MAC_ADDRESS "00:20:9d:23:5c:9e"

//IP addresses
#define IP_FDR      "192.168.1.1"   // will be 192.168.1.1 in flight
#define IP_CTL      "192.168.1.2"   // will be 192.168.1.2 in flight
#define IP_PYAS     "192.168.4.4"   // PYAS camera (same for both SAS-1 and SAS-2)
#define IP_RAS      "192.168.8.8"   // RAS camera

#define IP_LOOPBACK "127.0.0.1"

//UDP ports, aside from PORT_IMAGE, which is TCP
#define PORT_CMD      2000 // commands, FDR (receive) and CTL (send/receive)
#define PORT_TM       2002 // send telemetry to FDR (except images)
#define PORT_IMAGE    2013 // send images to FDR, TCP port
#define PORT_SAS2     3000 // commands output from SAS2 to CTL are redirected here
#define PORT_SBC_INFO 3456 //
#define PORT_SBC_SHUTDOWN 3789 //

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
#define SKEY_SHUTDOWN            0x00F0

//Setting commands
#define SKEY_SET_TARGET          0x0412
#define SKEY_SET_IMAGESAVEFLAG   0x0421
#define SKEY_SET_PYAS_EXPOSURE   0x0451
#define SKEY_SET_PYAS_ANALOGGAIN 0x0481
#define SKEY_SET_PYAS_PREAMPGAIN 0x0491
#define SKEY_SET_RAS_EXPOSURE    0x0551
#define SKEY_SET_RAS_ANALOGGAIN  0x0581
#define SKEY_SET_RAS_PREAMPGAIN  0x0591

//Getting commands
#define SKEY_REQUEST_PYAS_IMAGE  0x0810
#define SKEY_GET_PYAS_EXPOSURE   0x0850
#define SKEY_GET_PYAS_ANALOGGAIN 0x0860
#define SKEY_GET_PYAS_PREAMPGAIN 0x0870
#define SKEY_GET_DISKSPACE       0x0881
#define SKEY_REQUEST_RAS_IMAGE   0x0910
#define SKEY_GET_RAS_EXPOSURE    0x0950
#define SKEY_GET_RAS_ANALOGGAIN  0x0960
#define SKEY_GET_RAS_PREAMPGAIN  0x0970

#define PASSPHRASE "cS8XU:DpHq;dpCSA>wllge+gc9p2Xkjk;~a2OXahm0hFZDaXJ6C}hJ6cvB-WEp,"

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
pthread_mutex_t mutexHeader[2];  //Used to protect both the frame and header information
pthread_mutex_t mutexImageSave[2];  //Used to make sure that no more than one ImageSaveThread is running

struct Thread_data{
    int thread_id;
    int camera_id;
    uint16_t command_key;
    uint8_t command_num_vars;
    uint16_t command_vars[15];
};
struct Thread_data thread_data[MAX_THREADS];

sig_atomic_t volatile g_running = 1;

int sas_id;

cv::Mat frame[2]; //protected by mutexHeader
HeaderData header[2]; //protected by mutexHeader

Aspect aspect;
AspectCode runResult;
Transform solarTransform;

CameraSettings settings[2];

timespec frameRate = {0,FRAME_CADENCE*1000};
bool cameraReady[2] = {false, false};

long int frameCount[2] = {0, 0};

float camera_temperature[2];
int8_t sbc_temperature;
int8_t i2c_temperatures[8];
float sbc_v105, sbc_v25, sbc_v33, sbc_v50, sbc_v120;

//Function declarations
void sig_handler(int signum);

void start_thread(void *(*start_routine) (void *), const Thread_data *tdata);
void start_all_workers();
void kill_all_threads(); //kills all threads
void kill_all_workers(); //kills all threads except the one that listens for commands

void *CameraThread( void * threadargs, int camera_id);
void *PYASCameraThread( void * threadargs);
void *RASCameraThread( void * threadargs);

void image_process(int camera_id, cv::Mat &argFrame, HeaderData &argHeader);
void image_queue_solution(HeaderData &argHeader);
void *ImageSaveThread(void *threadargs);

void *TelemetrySenderThread(void *threadargs);
void *TelemetryPackagerThread(void *threadargs);

void *CommandSenderThread( void *threadargs );

void *CommandListenerThread(void *threadargs);
void cmd_process_heroes_command(uint16_t heroes_command);
void cmd_process_sas_command(uint16_t sas_command, Command &command);
void *CommandHandlerThread(void *threadargs);
void queue_cmd_proc_ack_tmpacket( uint16_t error_code );
uint16_t cmd_send_image_to_ground( int camera_id );

void *ForwardCommandsFromSAS2Thread( void *threadargs );
void *SBCInfoThread(void *threadargs);
void *SaveTemperaturesThread(void *threadargs);

void identifySAS();
uint16_t get_disk_usage( uint16_t disk );
void send_shutdown();

template <class T>
bool set_if_different(T& variable, T value); //returns true if the value is different

void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        g_running = 0;
    }
}

template <class T>
bool set_if_different(T& variable, T value)
{
    if(variable != value) {
        variable = value;
        return true;
    } else return false;
}

void kill_all_workers()
{
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

void kill_all_threads()
{
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
        std::cerr << "Error identifying computer, defaulting to SAS-1\n";
        sas_id = 1;
        return;
    }

    fgets(buff, sizeof(buff), in);

    if(strstr(buff, SAS1_MAC_ADDRESS) != NULL) {
        std::cerr << "SAS-1 identified\n";
        sas_id = 1;
    } else if(strstr(buff, SAS2_MAC_ADDRESS) != NULL) {
        std::cerr << "SAS-2 identified\n";
        sas_id = 2;
    } else {
        std::cerr << "Unknown computer, defaulting to SAS-1\n";
        sas_id = 1;
    }

    pclose(in);
}

void *PYASCameraThread( void *threadargs)
{
    return CameraThread(threadargs, 0);
}

void *RASCameraThread( void *threadargs)
{
    return CameraThread(threadargs, 1);
}

void *CameraThread( void * threadargs, int camera_id)
{
    // camera_id refers to 0 PYAS, 1 is RAS (if valid)
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("%sCamera thread #%ld!\n", (camera_id == 1 ? "RAS" : "PYAS"), tid);

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
    HeaderData localHeader;
    timespec localCaptureTime, preExposure, postExposure, timeElapsed, timeToWait;
    int width, height;
    int failcount = 0;

    uint16_t localExposure = settings[camera_id].exposure;
    int16_t localPreampGain = settings[camera_id].preampGain;
    uint16_t localAnalogGain = settings[camera_id].analogGain;

    cameraReady[camera_id] = false;
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
                std::cerr << "Error connecting to camera!\n";
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
                    std::cerr << "Error initializing camera!\n";
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
            if(isOutputting && isTracking && acknowledgedCTL && (frameCount[camera_id] % MOD_CTL == 0)) {
                ctl_sequence_number++;
                CommandPacket cp(TARGET_ID_CTL, ctl_sequence_number);
                cp << (uint16_t)HKEY_SAS_TIMESTAMP;
                cp << (uint16_t)0x0001;             // Camera ID (=1 for SAS, irrespective which SAS is providing solutions) 
                cp << (double)(preExposure.tv_sec + (double)preExposure.tv_nsec/1e9);  // timestamp 
                cm_packet_queue << cp;
            }

            if(!camera.Snap(localFrame, frameRate))
            {
                if((camera_id == 0) && USE_MOCK_PYAS_IMAGE) {
                    readFITSImage(MOCK_PYAS_IMAGE, localFrame);
                }
                frameCount[camera_id]++;
                failcount = 0;

                camera_temperature[camera_id] = camera.getTemperature();

                // save data into the fits_header
                memset(&localHeader, 0, sizeof(HeaderData));

                localHeader.cameraID = sas_id+4*camera_id;
                localHeader.captureTime = localCaptureTime;
                localHeader.captureTimeMono = preExposure;
                localHeader.frameCount = frameCount[camera_id];
                localHeader.exposure = localExposure;
                localHeader.preampGain = localPreampGain;
                localHeader.analogGain = localAnalogGain;

                localHeader.cameraTemperature = camera_temperature[camera_id];
                localHeader.cpuTemperature = sbc_temperature;

                localHeader.cpuVoltage[0] = sbc_v105;
                localHeader.cpuVoltage[1] = sbc_v25;
                localHeader.cpuVoltage[2] = sbc_v33;
                localHeader.cpuVoltage[3] = sbc_v50;
                localHeader.cpuVoltage[4] = sbc_v120;

                if(frameCount[camera_id] % MOD_PROCESS == 0) {
                    image_process(camera_id, localFrame, localHeader);
                }

                pthread_mutex_lock(mutexHeader+camera_id);
                localFrame.copyTo(frame[camera_id]);
                header[camera_id] = localHeader;
                pthread_mutex_unlock(mutexHeader+camera_id);

                if(frameCount[camera_id] % MOD_CTL == 0) {
                    if(camera_id == 0) image_queue_solution(localHeader);
                }

                if(frameCount[camera_id] % MOD_SAVE == 0) {
                    if(pthread_mutex_trylock(mutexImageSave+camera_id) == 0) {
                        Thread_data tdata;
                        tdata.camera_id = camera_id;
                        start_thread(ImageSaveThread, &tdata);
                    } else {
                        printf("Already saving a %s image\n", (camera_id == 0 ? "PYAS" : "RAS"));
                    }
                }
            }
            else
            {
                failcount++;
                std::cerr << "Frame failure count = " << failcount << std::endl;
                if (failcount >= 10)
                {
                    camera.Stop();
                    camera.Disconnect();
                    cameraReady[camera_id] = false;
                    failcount = 0;
                    std::cerr << "*********************** RESETTING CAMERA ***********************************" << std::endl;
                    continue;
                }
            }

            //Make any changes to camera settings that happened since last exposure.
            if(set_if_different(localExposure, settings[camera_id].exposure)) camera.SetExposure(localExposure);
            if(set_if_different(localPreampGain, settings[camera_id].preampGain)) camera.SetPreAmpGain(localPreampGain);
            if(set_if_different(localAnalogGain, settings[camera_id].analogGain)) camera.SetAnalogGain(localAnalogGain);

            //Record time that camera exposure finished. As little as possible should happen between this call and the call to nanosleep()
            clock_gettime(CLOCK_MONOTONIC, &postExposure);

            //Determine time spent on camera exposure
            timeElapsed = TimespecDiff(preExposure, postExposure);

            //Calculate the time to wait for next exposure
            timeToWait.tv_sec = frameRate.tv_sec - timeElapsed.tv_sec;
            timeToWait.tv_nsec = frameRate.tv_nsec - timeElapsed.tv_nsec;
            //std::cout << timeElapsed.tv_nsec << " " << timeToWait.tv_nsec << "\n";

            //Wait till next exposure time
            nanosleep(&timeToWait, NULL);
        }
    }
}

void image_process(int camera_id, cv::Mat &argFrame, HeaderData &argHeader)
{
    CoordList localLimbs, localPixelFiducials, localScreenFiducials;
    IndexList localIds;
    uint8_t localMin, localMax;
    std::vector<float> localMapping;
    cv::Point2f localPixelCenter, localScreenCenter, localError;
    Pair localOffset;
    
    if((camera_id == 0) && !argFrame.empty())
    {
        aspect.LoadFrame(argFrame);

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

        switch(GeneralizeError(runResult))
        {
            case NO_ERROR:
                solarTransform.set_conversion(Pair(localMapping[0],localMapping[2]),Pair(localMapping[1],localMapping[3]));
                localOffset = solarTransform.calculateOffset(Pair(localPixelCenter.x,localPixelCenter.y));

                argHeader.CTLsolution[0] = localOffset.x();
                argHeader.CTLsolution[1] = localOffset.y();

                argHeader.screenCenter[0] = localScreenCenter.x;
                argHeader.screenCenter[1] = localScreenCenter.y;

                if(localMapping.size() == 4){
                    argHeader.XYinterceptslope[0] = localMapping[0];
                    argHeader.XYinterceptslope[1] = localMapping[2];
                    argHeader.XYinterceptslope[2] = localMapping[1];
                    argHeader.XYinterceptslope[3] = localMapping[3];
                }

            case MAPPING_ERROR:
                argHeader.fiducialCount = localIds.size();
                for(uint8_t j = 0; j < 10; j++) {
                    if (j < localIds.size()) {
                        argHeader.fiducialIDX[j] = localIds[j].x;
                        argHeader.fiducialIDY[j] = localIds[j].y;
                    } else {
                        argHeader.fiducialIDX[j] = 0;
                        argHeader.fiducialIDY[j] = 0;
                    }
                }

            case ID_ERROR:
                argHeader.fiducialCount = localPixelFiducials.size();
                for(uint8_t j = 0; j < 10; j++) {
                    if (j < localPixelFiducials.size()){
                        argHeader.fiducialX[j] = localPixelFiducials[j].x;
                        argHeader.fiducialY[j] = localPixelFiducials[j].y;
                    } else {
                        argHeader.fiducialX[j] = 0;
                        argHeader.fiducialY[j] = 0;
                    }
                }

            case FIDUCIAL_ERROR:
                argHeader.sunCenter[0] = localPixelCenter.x;
                argHeader.sunCenter[1] = localPixelCenter.y;

                argHeader.screenCenterError[0] = localError.x;
                argHeader.screenCenterError[1] = localError.y;

            case CENTER_ERROR:
                argHeader.limbCount = localLimbs.size();
                for(uint8_t j = 0; j < 10; j++) {
                    if (j < localLimbs.size()) {
                        argHeader.limbX[j] = localLimbs[j].x;
                        argHeader.limbY[j] = localLimbs[j].y;
                    } else {
                        argHeader.limbX[j] = 0;
                        argHeader.limbY[j] = 0;
                    }
                }

            case LIMB_ERROR:
            case RANGE_ERROR:
                argHeader.imageMinMax[0] = localMin;
                argHeader.imageMinMax[1] = localMax;
                break;

            default:
                break;
        }

        argHeader.isTracking = isTracking;

    }
    else if((camera_id == 1) && !argFrame.empty()) {
        double min, max;
        cv::minMaxLoc(frame[1], &min, &max, NULL, NULL);
        argHeader.imageMinMax[0] = (uint8_t)min;
        argHeader.imageMinMax[1] = (uint8_t)max;
    }
    else
    {
        //std::cout << "Frame empty!" << std::endl;
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

void *ImageSaveThread(void *threadargs)
{
    timespec preSave, postSave, elapsedSave;

    clock_gettime(CLOCK_MONOTONIC, &preSave);

    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    struct Thread_data *my_data;
    my_data = (struct Thread_data *) threadargs;

    int camera_id = my_data->camera_id;

    cv::Mat localFrame;
    HeaderData localHeader;

    pthread_mutex_lock(mutexHeader+camera_id);
    frame[camera_id].copyTo(localFrame);
    localHeader = header[camera_id];
    pthread_mutex_unlock(mutexHeader+camera_id);

    if(!localFrame.empty())
    {
        char stringtemp[80];
        char obsfilespec[128];
        time_t ltime;
        struct tm *times;

        //Use clock_gettime instead?
        time(&ltime);
        times = localtime(&ltime);
        strftime(stringtemp,40,"%y%m%d_%H%M%S",times);

        sprintf(obsfilespec, "%s%s_%s_%06d.fits", SAVE_LOCATION, (camera_id == 1 ? "ras" : "pyas"), stringtemp, (int)localHeader.frameCount);

        printf("Saving image %s: exposure %d us, analog gain %d, preamp gain %d\n", obsfilespec, localHeader.exposure, localHeader.analogGain, localHeader.preampGain);
        writeFITSImage(localFrame, localHeader, obsfilespec);
    }
    else
    {
    }

    //This thread should only ever be started if the lock was set
    pthread_mutex_unlock(mutexImageSave+camera_id);

    clock_gettime(CLOCK_MONOTONIC, &postSave);
    elapsedSave = TimespecDiff(preSave, postSave);
    //std::cout << "Saving took: " << elapsedSave.tv_sec << " " << elapsedSave.tv_nsec << std::endl;

    started[tid] = false;
    return NULL;
}

void *TelemetryPackagerThread(void *threadargs)
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("TelemetryPackager thread #%ld!\n", tid);

    CoordList localLimbs, localFiducials;
    std::vector<float> localMapping;
    cv::Point2f localCenter, localError;
    Pair localOffset;
    HeaderData localHeaders[2];

    while(1)    // run forever
    {
        usleep(USLEEP_TM_GENERIC);
        tm_frame_sequence_number++;

        TelemetryPacket tp(TM_SAS_GENERIC, SOURCE_ID_SAS);
        tp.setSAS(sas_id);
        tp << tm_frame_sequence_number;
        tp << command_sequence_number;
        tp << latest_sas_command_key;

        if(pthread_mutex_trylock(&mutexHeader[0]) == 0) {
            localHeaders[0] = header[0];
            pthread_mutex_unlock(&mutexHeader[0]);
        }
        if(pthread_mutex_trylock(&mutexHeader[1]) == 0) {
            localHeaders[1] = header[1];
            pthread_mutex_unlock(&mutexHeader[1]);
        }

        int camera_id = tm_frame_sequence_number % sas_id;

/*
        std::cout << "Telemetry packet with Sun center (pixels): " << Pair(localHeaders[0].sunCenter[0], localHeaders[0].sunCenter[1]);
        std::cout << ", mapping is";
        for(uint8_t l = 0; l < 4; l++) std::cout << " " << localHeaders[0].XYinterceptslope[l];
        std::cout << std::endl;

        std::cout << "Offset: " << Pair(localHeaders[0].CTLsolution[0], localHeaders[0].CTLsolution[1]) << std::endl;
*/

        //Housekeeping fields, two of them
        tp << Float2B(localHeaders[camera_id].cameraTemperature);
        tp << (uint16_t)localHeaders[0].cpuTemperature;

        //Sun center and error
        tp << Pair3B(localHeaders[0].sunCenter[0], localHeaders[0].sunCenter[1]);
        tp << Pair3B(localHeaders[0].sunCenterError[0], localHeaders[0].sunCenterError[1]);

        //Predicted Sun center and error
        tp << Pair3B(0, 0);
        tp << Pair3B(0, 0);

        //Number of limb crossings
        tp << (uint16_t)localHeaders[0].limbCount;

        //Limb crossings (currently 8)
        for(uint8_t j = 0; j < 8; j++) {
            uint8_t jp = (localHeaders[0].limbCount > 0 ? (j+tm_frame_sequence_number) % localHeaders[0].limbCount : 0);
            tp << Pair3B(localHeaders[0].limbX[jp], localHeaders[0].limbY[jp]);
        }

        //Number of fiducials
        tp << (uint16_t)localHeaders[0].fiducialCount;

        //Fiduicals (currently 6)
        for(uint8_t j = 0; j < 6; j++) {
            uint8_t jp = (localHeaders[0].fiducialCount > 0 ? (j+tm_frame_sequence_number) % localHeaders[0].fiducialCount : 0);
            tp << Pair3B(localHeaders[0].fiducialX[jp], localHeaders[0].fiducialY[jp]);
        }

        //Pixel to screen conversion
        tp << localHeaders[0].XYinterceptslope[0]; //X intercept
        tp << localHeaders[0].XYinterceptslope[2]; //X slope
        tp << localHeaders[0].XYinterceptslope[1]; //Y intercept
        tp << localHeaders[0].XYinterceptslope[3]; //Y slope

        //Image max and min
        tp << (uint8_t) localHeaders[camera_id].imageMinMax[1]; //max
        tp << (uint8_t) localHeaders[camera_id].imageMinMax[0]; //min

        //Tacking on the offset numbers intended for CTL
        tp << Pair(localHeaders[0].CTLsolution[0], localHeaders[0].CTLsolution[1]);

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

void *CommandListenerThread(void *threadargs)
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

void image_queue_solution(HeaderData &argHeader)
{
    if (isOutputting) {
        ctl_sequence_number++;
        CommandPacket cp(TARGET_ID_CTL, ctl_sequence_number);

        if (isTracking) {
            if (!acknowledgedCTL) {
                cp << (uint16_t)HKEY_SAS_TRACKING_IS_ON;
                acknowledgedCTL = true;
            } else {
                cp << (uint16_t)HKEY_SAS_SOLUTION;
                cp << (double)argHeader.CTLsolution[0]; // azimuth offset
                cp << (double)argHeader.CTLsolution[1]; // elevation offset
                cp << (double)0; // roll offset
                cp << (double)0.003; // error
                cp << (uint32_t)argHeader.captureTime.tv_sec; //seconds
                cp << (uint16_t)(argHeader.captureTime.tv_nsec/1000000); //milliseconds
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
    camera_id = camera_id % sas_id;
    uint16_t error_code = 0;
    cv::Mat localFrame;
    HeaderData localHeader;

    TCPSender tcpSndr(IP_FDR, (unsigned short) PORT_IMAGE);
    int ret = tcpSndr.init_connection();
    if (ret > 0){
        if (pthread_mutex_trylock(mutexHeader+camera_id) == 0){
            if( !frame[camera_id].empty() ){
                frame[camera_id].copyTo(localFrame);
                localHeader = header[camera_id];
            }
            pthread_mutex_unlock(mutexHeader+camera_id);
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
            uint32_t temp = localHeader.exposure;
            im_packet_queue << ImageTagPacket(camera, &temp, TLONG, "EXPOSURE", "Exposure time in msec");
            im_packet_queue << ImageTagPacket(camera, (camera_id == 0 ? (sas_id == 1 ? "PYAS-F" : "PYAS-R") : "RAS"), TSTRING, "INSTRUME", "Name of instrument");
            im_packet_queue << ImageTagPacket(camera, (sas_id == 1 ? "HEROES/SAS-1" : "HEROES/SAS-2"), TSTRING, "ORIGIN", "Location where file was made");

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
        
void *CommandHandlerThread(void *threadargs)
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
        case SKEY_REQUEST_PYAS_IMAGE:
            {
                error_code = cmd_send_image_to_ground( 0 );
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_REQUEST_RAS_IMAGE:
            {
                error_code = cmd_send_image_to_ground( 1 );
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_SET_IMAGESAVEFLAG:
            {
                if(my_data->command_num_vars == 1) isSavingImages = (my_data->command_vars[0] > 0);
                if( isSavingImages == my_data->command_vars[0] ) error_code = 0;
                if( isSavingImages == true ){ std::cout << "Image saving is now turned on" << std::endl; }
                if( isSavingImages == false ){ std::cout << "Image saving is now turned off" << std::endl; }
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_SET_PYAS_EXPOSURE:    // set exposure time
            {
                if(my_data->command_num_vars == 1) settings[0].exposure = my_data->command_vars[0];
                if( settings[0].exposure == my_data->command_vars[0] ) error_code = 0;
                std::cout << "Current exposure time is: " << settings[0].exposure << std::endl;
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_SET_PYAS_PREAMPGAIN:    // set preamp gain
            {
                if( my_data->command_num_vars == 1) settings[0].preampGain = (int16_t)my_data->command_vars[0];
                if( settings[0].preampGain == (int16_t)my_data->command_vars[0] ) error_code = 0;
                std::cout << "Current preamp gain is: " << settings[0].preampGain << std::endl;
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_SET_PYAS_ANALOGGAIN:    // set analog gain
            {
                if( my_data->command_num_vars == 1) settings[0].analogGain = my_data->command_vars[0];
                if( settings[0].analogGain == my_data->command_vars[0] ) error_code = 0;
                std::cout << "Current analog gain is: " << settings[0].analogGain << std::endl;
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_SET_RAS_EXPOSURE:    // set exposure time
            {
                if(my_data->command_num_vars == 1) settings[1].exposure = my_data->command_vars[0];
                if( settings[1].exposure == my_data->command_vars[0] ) error_code = 0;
                std::cout << "Current exposure time is: " << settings[1].exposure << std::endl;
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_SET_RAS_PREAMPGAIN:    // set preamp gain
            {
                if( my_data->command_num_vars == 1) settings[1].preampGain = (int16_t)my_data->command_vars[0];
                if( settings[1].preampGain == (int16_t)my_data->command_vars[0] ) error_code = 0;
                std::cout << "Current preamp gain is: " << settings[1].preampGain << std::endl;
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case SKEY_SET_RAS_ANALOGGAIN:    // set analog gain
            {
                if( my_data->command_num_vars == 1) settings[1].analogGain = my_data->command_vars[0];
                if( settings[1].analogGain == my_data->command_vars[0] ) error_code = 0;
                std::cout << "Current analog gain is: " << settings[1].analogGain << std::endl;
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
        case SKEY_GET_PYAS_EXPOSURE:
            {
                queue_cmd_proc_ack_tmpacket( (uint16_t)settings[0].exposure );
            }
            break;
        case SKEY_GET_PYAS_ANALOGGAIN:
            {
                queue_cmd_proc_ack_tmpacket( (uint16_t)settings[0].analogGain );
            }
            break;
        case SKEY_GET_PYAS_PREAMPGAIN:
            {
                queue_cmd_proc_ack_tmpacket( (int16_t)settings[0].preampGain );
            }
            break;
        case SKEY_GET_RAS_EXPOSURE:
            {
                queue_cmd_proc_ack_tmpacket( (uint16_t)settings[1].exposure );
            }
            break;
        case SKEY_GET_RAS_ANALOGGAIN:
            {
                queue_cmd_proc_ack_tmpacket( (uint16_t)settings[1].analogGain );
            }
            break;
        case SKEY_GET_RAS_PREAMPGAIN:
            {
                queue_cmd_proc_ack_tmpacket( (int16_t)settings[1].preampGain );
            }
            break;
        case SKEY_GET_DISKSPACE:
            {
                if( my_data->command_num_vars == 1) {
                    uint16_t disk = (uint16_t)my_data->command_vars[0];
                    error_code = get_disk_usage( disk );
                }
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
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

uint16_t get_disk_usage( uint16_t disk )
{
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

    float fraction_used = 1-(double)vfs.f_bavail/vfs.f_blocks;
    return( (uint16_t)(100*fraction_used) );
}

void send_shutdown()
{
    UDPSender out(IP_LOOPBACK, PORT_SBC_SHUTDOWN);
    Packet pkt((const uint8_t *)PASSPHRASE, strlen(PASSPHRASE));
    out.send(&pkt);
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

                    start_thread(CommandListenerThread, NULL);
                    start_all_workers();
                    queue_cmd_proc_ack_tmpacket( 0 );
                }
                break;
            case SKEY_SHUTDOWN:
                {
                    kill_all_threads();
                    queue_cmd_proc_ack_tmpacket( 0 );
                    sleep(2);
                    send_shutdown();
                }
            default:
                {
                    start_thread(CommandHandlerThread, &tdata);
                }
        } //switch
    } else printf("Not the intended SAS for this command\n");
}

void start_all_workers()
{
    start_thread(TelemetryPackagerThread, NULL);
    start_thread(TelemetrySenderThread, NULL);
    start_thread(CommandSenderThread, NULL);
    start_thread(PYASCameraThread, NULL);
    start_thread(SaveTemperaturesThread, NULL);
    start_thread(SBCInfoThread, NULL);
    switch (sas_id) {
        case 1:
            start_thread(ForwardCommandsFromSAS2Thread, NULL);
            break;
        case 2:
            start_thread(RASCameraThread, NULL);
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

    pthread_mutex_init(mutexHeader, NULL);
    pthread_mutex_init(mutexHeader+1, NULL);
    pthread_mutex_init(mutexImageSave, NULL);
    pthread_mutex_init(mutexImageSave+1, NULL);

    /* Create worker threads */
    printf("In main: creating threads\n");

    for(int i = 0; i < MAX_THREADS; i++ ){
        started[0] = false;
    }

    // start the listen for commands thread right away
    start_thread(CommandListenerThread, NULL);
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
    pthread_mutex_destroy(mutexHeader);
    pthread_mutex_destroy(mutexHeader+1);
    pthread_mutex_destroy(mutexImageSave);
    pthread_mutex_destroy(mutexImageSave+1);
    pthread_exit(NULL);

    return 0;
}

