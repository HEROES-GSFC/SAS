#define SAVE_IMAGES false
#define MAX_THREADS 30
#define REPORT_FOCUS false
#define LOG_PACKETS true
#define USE_MOCK_PYAS_IMAGE false
#define MOCK_PYAS_IMAGE "/mnt/disk1/mock.fits"

//Save locations for FITS files, alternates between the two locations
#define SAVE_LOCATION1 "/mnt/disk1/"
#define SAVE_LOCATION2 "/mnt/disk2/"

//Calibrated parameters
#define CLOCKING_ANGLE_PYASF -32.425 //model is -33.26
#define CENTER_X_PYASF    124.68 //mils
#define CENTER_Y_PYASF    -74.64 //mils
#define TWIST_PYASF 180.0 //needs to be ~180
#define CLOCKING_ANGLE_PYASR -52.175 //model is -53.26
#define CENTER_X_PYASR -105.59 //mils
#define CENTER_Y_PYASR   -48.64 //mils
#define TWIST_PYASR 0.0 //needs to be ~0

//Major settings
#define FRAME_CADENCE 250000 // microseconds

//Frequency settings, do each of these per this many snaps
//It's best if MOD_CTL and MOD_SAVE are multiples of MOD_PROCESS
#define MOD_PROCESS 1 //process the image
#define MOD_CTL     4 //send the processing results to CTL
#define MOD_SAVE    1 //save the image to a local FITS file

//Sleep settings (seconds)
#define SLEEP_LOG_TEMPERATURE 10 // period for logging temperature locally
#define SLEEP_CAMERA_CONNECT   1 // waits for errors while connecting to camera
#define SLEEP_KILL             2 // waits when killing all threads

//Relay off
#define RELAY_OFF false
#define RELAY_ON true
#define NUM_RELAYS 16

//Sleep settings (microseconds)
#define USLEEP_CMD_SEND     5000 // period for popping off the command queue
#define USLEEP_TM_SEND     50000 // period for popping off the telemetry queue
#define USLEEP_TM_GENERIC 950000 // period for adding generic telemetry packets to queue
#define USLEEP_UDP_LISTEN   1000 // safety measure in case UDP listening is changed to non-blocking
#define USLEEP_MAIN         5000 // period for checking for new commands

#define SAS1_MAC_ADDRESS "00:20:9d:23:26:b9"
#define SAS2_MAC_ADDRESS "00:20:9d:23:5c:9e"

//IP addresses
#define IP_FDR      "192.168.1.1"   // will be 192.168.1.1 in flight
#define IP_CTL      "192.168.1.2"   // will be 192.168.1.2 in flight
#define IP_PYAS     "192.168.4.4"   // PYAS camera (same for both SAS-1 and SAS-2)
#define IP_RAS      "192.168.8.8"   // RAS camera

#define IP_LOOPBACK "127.0.0.1"

#define MAX_CLOCK_OFFSET_UMS 1000   // maximum acceptable clock offset in microseconds 

//UDP ports, aside from PORT_IMAGE, which is TCP
#define PORT_CMD      2000 // commands, FDR (receive) and CTL (send/receive)
#define PORT_TM       2002 // send telemetry to FDR (except images)
#define PORT_IMAGE    2013 // send images to FDR, TCP port
#define PORT_SAS2     3000 // commands output from SAS2 to CTL are redirected here
#define PORT_SBC_INFO 3456 // incoming port to retrieve temperature data
#define PORT_SBC_SHUTDOWN 3789 // outgoing port to signal a shutdown
#define PORT_RELAY_CONTROL 4567 // outgoing port to control relays

//HEROES target ID for commands, source ID for telemetry
#define TARGET_ID_ALL 0xFF
#define TARGET_ID_CTL 0x01
#define TARGET_ID_SAS 0x30
#define SOURCE_ID_SAS TARGET_ID_SAS

//HEROES telemetry types
#define TM_ACK_RECEIPT 0x01
#define TM_IDLE 0x03
#define TM_ACK_PROCESS 0xE1
#define TM_SAS_GENERIC 0x70
#define TM_SAS_IMAGE   0x82
#define TM_SAS_TAG     0x83

//HEROES commands, CTL/FDR to SAS
#define HKEY_CTL_START_TRACKING  0x1000
#define HKEY_CTL_STOP_TRACKING   0x1001
#define HKEY_FDR_SAS_CMD         0x10FF
#define HKEY_FDR_GPS_INFO        0x000A

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
#define SKEY_QUIT_RUNTIME        0x0050
#define SKEY_SUPPRESS_TELEMETRY  0x0071
#define SKEY_SHUTDOWN            0x00F0
#define SKEY_CTL_TEST_CMD        0x0081
#define SKEY_REQUEST_PYAS_IMAGE  0x0210
#define SKEY_REQUEST_RAS_IMAGE   0x0220

//Operations commands for controlling relays
#define SKEY_TURN_RELAY_ON       0x0101
#define SKEY_TURN_RELAY_OFF      0x0111
#define SKEY_TURN_ON_ALL_RELAYS  0x0120
#define SKEY_TURN_OFF_ALL_RELAYS 0x0130
#define SKEY_DEFAULT_RELAYS      0x0140
#define SKEY_PYAS_TSTAT_BYPASS   0x0160
#define SKEY_PYAS_TSTAT_ENABLE   0x0170
#define SKEY_RAS_TSTAT_BYPASS    0x0180
#define SKEY_RAS_TSTAT_ENABLE    0x0190
#define SKEY_CAN_HEATERS_OFF     0x01A0
#define SKEY_CAN_HEATERS_LOW     0x01B0
#define SKEY_CAN_HEATERS_MEDIUM  0x01C0
#define SKEY_CAN_HEATERS_HIGH    0x01D0

//Setting commands
#define SKEY_SET_TARGET          0x0412
#define SKEY_SET_IMAGESAVEFLAG   0x0421
#define SKEY_SET_GPSFLAG         0x0431
#define SKEY_SET_PYAS_SAVEFLAG   0x0441
#define SKEY_SET_PYAS_EXPOSURE   0x0451
#define SKEY_SET_PYAS_ANALOGGAIN 0x0481
#define SKEY_SET_PYAS_PREAMPGAIN 0x0491
#define SKEY_SET_RAS_SAVEFLAG    0x0541
#define SKEY_SET_RAS_EXPOSURE    0x0551
#define SKEY_SET_RAS_ANALOGGAIN  0x0581
#define SKEY_SET_RAS_PREAMPGAIN  0x0591
#define SKEY_SET_CLOCKING        0x0621
#define SKEY_SET_LAT_LON         0x06A2
#define SKEY_SET_ASPECT_INT      0x0712
#define SKEY_SET_ASPECT_FLOAT    0x0722
#define SKEY_SET_CAMERA_TWIST    0x0731

//Getting commands
#define SKEY_GET_PYAS_EXPOSURE   0x0850
#define SKEY_GET_PYAS_ANALOGGAIN 0x0860
#define SKEY_GET_PYAS_PREAMPGAIN 0x0870
#define SKEY_GET_DISKSPACE       0x0881
#define SKEY_GET_RAS_EXPOSURE    0x0950
#define SKEY_GET_RAS_ANALOGGAIN  0x0960
#define SKEY_GET_RAS_PREAMPGAIN  0x0970
#define SKEY_GET_TARGET_X        0x0A10
#define SKEY_GET_TARGET_Y        0x0A20
#define SKEY_GET_ASPECT_INT      0x0B11
#define SKEY_GET_ASPECT_FLOAT    0x0B21
#define SKEY_GET_CAMERA_TWIST    0x0B30
#define SKEY_GET_CLOCKING        0x0C20
#define SKEY_GET_LATITUDE        0x0CA0
#define SKEY_GET_LONGITUDE       0x0CB0
#define SKEY_GET_NTP_OFFSET_US   0x0D10

#define PASSPHRASE_SBC_SHUTDOWN "cS8XU:DpHq;dpCSA>wllge+gc9p2Xkjk;~a2OXahm0hFZDaXJ6C}hJ6cvB-WEp,"
#define PASSPHRASE_RELAY_CONTROL "tAzh0Sh?$:dGo4t8j$8ceh^,d;2#ob}j_VEHXtWrI_AL*5C3l/edTMoO2Q8FY&K"

#include <cstring>
#include <stdio.h>      /* for printf() and fprintf() */
#include <pthread.h>    /* for multithreading */
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for sleep()  */
#include <signal.h>     /* for signal() */
#include <math.h>
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
uint16_t command_sequence_number = -1;      // last SAS command packet number
uint16_t latest_heroes_command_key = 0xFFFF;   // last HEROES command
uint16_t latest_sas_command_key = 0xFFFF;   // last SAS command
uint16_t ctl_sequence_number = 0;           // global so that 0x1104 packets share the same counter as the other 0x110? packets
uint16_t tm_frames_to_suppress = 0;
uint8_t aspect_error_code = 0;

bool isTracking = false;                    // does CTL want solutions?
bool isOutputting = false;                  // is this SAS supposed to be outputting solutions?
bool acknowledgedCTL = true;                // have we acknowledged the last command from CTL?
bool isSavingImages[2] = {SAVE_IMAGES, SAVE_IMAGES}; // is the PYAS/RAS saving images?
bool isClockSynced = false;
bool isAcceptingGPS = true;                 // are we accepting GPS updates from FDR?

bool receivedGoodGPS = false; // have we received a good GPS packet?

CommandQueue recvd_command_queue;
TelemetryPacketQueue tm_packet_queue;
CommandPacketQueue cm_packet_queue;

// related to threads
bool stop_message[MAX_THREADS];
pthread_t threads[MAX_THREADS];
bool started[MAX_THREADS];
int tid_listen = -1; //Stores the ID for the CommandListener thread
pthread_mutex_t mutexStartThread; //Keeps new threads from being started simultaneously
pthread_mutex_t mutexHeader[2];  //Used to protect both the frame and header information
pthread_mutex_t mutexSensors; //Used to protect sensor data

//Used to make sure that there are no more than 3 saving threads per camera
Semaphore semaphoreSave[2] = {Semaphore(3), Semaphore(3)};

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
Transform solarTransform(FORT_SUMNER, FLIGHT); //see Transform.hpp for options

CameraSettings settings[2]; //not protected!

struct Sensors {
    float camera_temperature[2];
    int8_t sbc_temperature;
    int8_t i2c_temperatures[8];
    float sbc_v105, sbc_v25, sbc_v33, sbc_v50, sbc_v120;
};
struct Sensors sensors; //not protected well!
float ntp_drift;
float ntp_offset_ms;
float ntp_stability;

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
bool check_solution(HeaderData &argHeader);
void *ImageSaveThread(void *threadargs);

void *TelemetrySenderThread(void *threadargs);
void *TelemetryPackagerThread(void *threadargs);

void *CommandSenderThread( void *threadargs );

void *CommandListenerThread(void *threadargs);
void cmd_process_heroes_command(uint16_t heroes_command);
void cmd_process_sas_command(Command &command);
void cmd_process_gps_info(Command &command);
void *CommandHandlerThread(void *threadargs);
void queue_cmd_proc_ack_tmpacket( uint16_t error_code );
uint16_t cmd_send_image_to_ground( int camera_id );

uint16_t cmd_send_test_ctl_solution( int type );

void *ForwardCommandsFromSAS2Thread( void *threadargs );
void *SBCInfoThread(void *threadargs);
void *SaveTemperaturesThread(void *threadargs);

void identifySAS();
uint16_t get_disk_usage( uint16_t disk );
void send_shutdown();
void send_relay_control(uint8_t relay_number, bool on_if_true);

template <class T>
bool set_if_different(T& variable, T value); //returns true if the value is different

void sig_handler(int signum)
{
    if ((signum == SIGINT) || (signum == SIGTERM))
    {
        if (signum == SIGINT) std::cerr << "Keyboard interrupt received\n";
        if (signum == SIGTERM) std::cerr << "Termination signal received\n";
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
            stop_message[i] = true;
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
        stop_message[tid_listen] = true;
        kill_all_workers();
        if (started[tid_listen]) {
            printf("Quitting thread %i, quitting status is %i\n", tid_listen, pthread_cancel(threads[tid_listen]));
            started[tid_listen] = false;
        }
    }
}

void identifySAS()
{
    FILE *in;
    char buff[128];

    if(!(in = popen("ip link show sbc | grep ether", "r"))) {
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
            stop_message[tid] = true;
    }

    timespec frameRate = {0,FRAME_CADENCE*1000};
    bool cameraReady[2] = {false, false};
    long int frameCount[2] = {0, 0};

    ImperxStream camera;

    cv::Mat localFrame, mockFrame;
    HeaderData localHeader;
    timespec localCaptureTime, preExposure, postExposure, timeElapsed, timeToWait;
    int width, height;
    int failcount = 0;

    uint16_t localExposure = settings[camera_id].exposure;
    int16_t localPreampGain = settings[camera_id].preampGain;
    uint16_t localAnalogGain = settings[camera_id].analogGain;

    if((camera_id == 0) && USE_MOCK_PYAS_IMAGE) {
        std::cerr << "Loading mock image...";
        if(readFITSImage(MOCK_PYAS_IMAGE, mockFrame) == 0) {
            std::cerr << "success\n";
        } else std::cerr << "failure\n";
    }

    cameraReady[camera_id] = false;
    while(!stop_message[tid])
    {
        if (!cameraReady[camera_id])
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

                camera.SetROISize(settings[camera_id].size.width,settings[camera_id].size.height);
                camera.SetROIOffset(settings[camera_id].offset.x,settings[camera_id].offset.y);
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
            if((camera_id == 0) && isOutputting && isTracking && acknowledgedCTL && ((frameCount[camera_id]+1) % MOD_CTL == 0)) {
                ctl_sequence_number++;
                CommandPacket cp(TARGET_ID_CTL, ctl_sequence_number);
                cp << (uint16_t)HKEY_SAS_TIMESTAMP;
                cp << (uint16_t)0x0001;             // Camera ID (=1 for SAS, irrespective which SAS is providing solutions) 
                cp << (double)(localCaptureTime.tv_sec + (double)localCaptureTime.tv_nsec/1e9);  // timestamp 
                cm_packet_queue << cp;
            }

            if(!camera.Snap(localFrame, frameRate))
            {
                if((camera_id == 0) && USE_MOCK_PYAS_IMAGE) {
                    if (!mockFrame.empty()) mockFrame.copyTo(localFrame);
                }
                frameCount[camera_id]++;
                failcount = 0;

                sensors.camera_temperature[camera_id] = camera.getTemperature();

                // save data into the fits_header
                memset(&localHeader, 0, sizeof(HeaderData));

                localHeader.cameraID = sas_id+4*camera_id;
                localHeader.captureTime = localCaptureTime;
                localHeader.captureTimeMono = preExposure;
                localHeader.frameCount = frameCount[camera_id];
                localHeader.exposure = localExposure;
                localHeader.preampGain = localPreampGain;
                localHeader.analogGain = localAnalogGain;

                localHeader.cameraTemperature = sensors.camera_temperature[camera_id];
                
                if(pthread_mutex_trylock(&mutexSensors) == 0) {

                    localHeader.cpuTemperature = sensors.sbc_temperature;
                    
                    localHeader.cpuVoltage[0] = sensors.sbc_v105;
                    localHeader.cpuVoltage[1] = sensors.sbc_v25;
                    localHeader.cpuVoltage[2] = sensors.sbc_v33;
                    localHeader.cpuVoltage[3] = sensors.sbc_v50;
                    localHeader.cpuVoltage[4] = sensors.sbc_v120;
                    
                    for (int i=0; i<8; i++) localHeader.i2c_temperatures[i] = sensors.i2c_temperatures[i];
                    
                    pthread_mutex_unlock(&mutexSensors);
                }
                if(frameCount[camera_id] % MOD_PROCESS == 0) {
                    image_process(camera_id, localFrame, localHeader);
                }

                pthread_mutex_lock(&mutexHeader[camera_id]);
                localFrame.copyTo(frame[camera_id]);
                header[camera_id] = localHeader;
                pthread_mutex_unlock(&mutexHeader[camera_id]);

                if(frameCount[camera_id] % MOD_CTL == 0) {
                    if(camera_id == 0) image_queue_solution(localHeader);
                }

                if(isSavingImages[camera_id] && (frameCount[camera_id] % MOD_SAVE == 0)) {
                    try {
                        semaphoreSave[camera_id].increment();
                        Thread_data tdata;
                        tdata.camera_id = camera_id;
                        start_thread(ImageSaveThread, &tdata);
                    } catch (std::exception& e) {
                        printf("Already saving too many %s images\n", (camera_id == 0 ? "PYAS" : "RAS"));
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
            //std::cout << camera_id << " " << timeElapsed.tv_nsec << " " << timeToWait.tv_nsec << "\n";

            //Wait till next exposure time
            nanosleep(&timeToWait, NULL);
        }
    }

    printf("CameraStream thread #%ld exiting\n", tid);
    camera.Stop();
    camera.Disconnect();
    started[tid] = false;
    pthread_exit( NULL );
}

void image_process(int camera_id, cv::Mat &argFrame, HeaderData &argHeader)
{
    AspectCode runResult;

    CoordList localLimbs, localPixelFiducials, localScreenFiducials;
    IndexList localIds;
    uint8_t localMin, localMax;
    std::vector<float> localMapping;
    cv::Point2f localPixelCenter, localScreenCenter, localError;
    Pair localOffset;
    
    if((camera_id == 0) && !argFrame.empty())
    {
        aspect.LoadFrame(argFrame);

        argHeader.runResult = runResult = aspect.Run();

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

        argHeader.clockingAngle = solarTransform.get_clocking();

        Pair localLatLon = solarTransform.get_lat_lon();
        argHeader.latitude = localLatLon.x();
        argHeader.longitude = localLatLon.y();

        Pair localSolarTarget = solarTransform.get_solar_target();
        argHeader.solarTarget[0] = localSolarTarget.x();
        argHeader.solarTarget[1] = localSolarTarget.y();

        switch(GeneralizeError(runResult))
        {
            case NO_ERROR:
                solarTransform.set_conversion(Pair(localMapping[0],localMapping[2]),Pair(localMapping[1],localMapping[3]));
                localOffset = solarTransform.calculateOffset(Pair(localPixelCenter.x,localPixelCenter.y), argHeader.captureTime);
                argHeader.northAngle = solarTransform.getOrientation();

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
                    if (j < argHeader.fiducialCount) {
                        uint8_t jp = (j+argHeader.frameCount) % argHeader.fiducialCount;
                        argHeader.fiducialIDX[j] = localIds[jp].x;
                        argHeader.fiducialIDY[j] = localIds[jp].y;
                    } else {
                        argHeader.fiducialIDX[j] = 0;
                        argHeader.fiducialIDY[j] = 0;
                    }
                }

            case ID_ERROR:
                argHeader.fiducialCount = localPixelFiducials.size();
                for(uint8_t j = 0; j < 10; j++) {
                    if (j < argHeader.fiducialCount){
                        uint8_t jp = (j+argHeader.frameCount) % argHeader.fiducialCount;
                        argHeader.fiducialX[j] = localPixelFiducials[jp].x;
                        argHeader.fiducialY[j] = localPixelFiducials[jp].y;
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
                    if (j < argHeader.limbCount) {
                        uint8_t jp = ((int)(j/2)+argHeader.frameCount+(j % 2)*(int)(argHeader.limbCount/2)) % argHeader.limbCount;
                        argHeader.limbX[j] = localLimbs[jp].x;
                        argHeader.limbY[j] = localLimbs[jp].y;
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

        if (GeneralizeError(runResult) != NO_ERROR) {
            argHeader.northAngle = solarTransform.calculateOrientation(argHeader.captureTime);
        }

        argHeader.isTracking = isTracking;
        argHeader.isOutputting = isOutputting;
    }
    else if((camera_id == 1) && !argFrame.empty()) {
        calcMinMax(frame[1], localMin, localMax);
        argHeader.imageMinMax[0] = localMin;
        argHeader.imageMinMax[1] = localMax;
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

    char timestamp[14];
    char filename[128];
    std::ofstream log;

    if (LOG_PACKETS) {
        writeCurrentUT(timestamp);
        sprintf(filename, "%slog_tm_%s.bin", SAVE_LOCATION1, timestamp);
        filename[128 - 1] = '\0';
        printf("Creating telemetry log file %s \n",filename);
        log.open(filename, std::ofstream::binary);
        if (!log.is_open()) {
            sprintf(filename, "%slog_tm_%s.bin", SAVE_LOCATION2, timestamp);
            filename[128 - 1] = '\0';
            printf("Creating telemetry log file %s \n",filename);
            log.open(filename, std::ofstream::binary);
        }
    }

    TelemetrySender telSender(IP_FDR, (unsigned short) PORT_TM);

    while(!stop_message[tid])
    {
        usleep(USLEEP_TM_SEND);

        if( !tm_packet_queue.empty() ){
            TelemetryPacket tp(NULL);
            tm_packet_queue >> tp;
            telSender.send( &tp );
            //std::cout << "TelemetrySender:" << tp << std::endl;
            if (LOG_PACKETS && log.is_open()) {
                uint16_t length = tp.getLength();
                uint8_t *payload = new uint8_t[length];
                tp.outputTo(payload);
                log.write((char *)payload, length);
                delete payload;
                //log.flush();
            }
        }
    }

    printf("TelemetrySender thread #%ld exiting\n", tid);
    if (LOG_PACKETS && log.is_open()) log.close();
    started[tid] = false;
    pthread_exit( NULL );
}

void *SBCInfoThread(void *threadargs)
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("SBCInfo thread #%ld!\n", tid);

    UDPReceiver receiver(PORT_SBC_INFO);
    receiver.init_connection();

    uint16_t packet_length;
    uint8_t *array;

    while(!stop_message[tid])
    {
        //This call will block forever if the service is not running
        usleep(USLEEP_UDP_LISTEN);
        packet_length = receiver.listen();
        array = new uint8_t[packet_length];
        receiver.get_packet(array);

        Packet packet( array, packet_length );
        packet >> sensors.sbc_temperature >> sensors.sbc_v105 >> sensors.sbc_v25 >> sensors.sbc_v33 >> sensors.sbc_v50 >> sensors.sbc_v120;
        for (int i=0; i<8; i++) packet >> sensors.i2c_temperatures[i];
        packet >> ntp_drift;
        packet >> ntp_offset_ms;
        packet >> ntp_stability;
        if (fabs(ntp_offset_ms * 1000) < MAX_CLOCK_OFFSET_UMS){ isClockSynced = true; } else { isClockSynced = false; }
        delete array;
    }

    printf("SBCInfo thread #%ld exiting\n", tid);
    started[tid] = false;
    pthread_exit( NULL );
}

void *SaveTemperaturesThread(void *threadargs)
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("SaveTemperatures thread #%ld!\n", tid);

    char timestamp[14];
    char filename[128];
    FILE *file;

    writeCurrentUT(timestamp);
    sprintf(filename, "%stemp_data_%s.dat", SAVE_LOCATION1, timestamp);
    filename[128 - 1] = '\0';
    printf("Creating file %s \n",filename);

    int count = 0;

    if((file = fopen(filename, "w")) == NULL){
        printf("Cannot open file\n");
        stop_message[tid] = true;
    } else {
        fprintf(file, "time, camera temp, cpu temp, i2c temp x8\n");
        sleep(SLEEP_LOG_TEMPERATURE);
    }

    while(!stop_message[tid])
    {
        sleep(SLEEP_LOG_TEMPERATURE);

        writeCurrentUT(timestamp);
        fprintf(file, "%s, %f, %d", timestamp, sensors.camera_temperature[count % sas_id], sensors.sbc_temperature);
        for (int i=0; i<8; i++) fprintf(file, ", %d", sensors.i2c_temperatures[i]);
        fprintf(file, "\n");
        printf("%s, %f, %d", timestamp, sensors.camera_temperature[count % sas_id], sensors.sbc_temperature);
        for (int i=0; i<8; i++) printf(", %d", sensors.i2c_temperatures[i]);
        printf("\n");
        count++;
    }

    printf("SaveTemperatures thread #%ld exiting\n", tid);
    if (file != NULL) fclose(file);
    started[tid] = false;
    pthread_exit( NULL );
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

    pthread_mutex_lock(&mutexHeader[camera_id]);
    frame[camera_id].copyTo(localFrame);
    localHeader = header[camera_id];
    pthread_mutex_unlock(&mutexHeader[camera_id]);

    if(!localFrame.empty())
    {
        char timestamp[14];
        char filename[128];
        struct tm *capturetime;

        capturetime = gmtime(&localHeader.captureTime.tv_sec);
        strftime(timestamp,14,"%y%m%d_%H%M%S",capturetime);

        sprintf(filename, "%s%s_%s_%03d_%06d.fits",
                ((localHeader.frameCount / MOD_SAVE) % 2 == 0 ? SAVE_LOCATION1 : SAVE_LOCATION2),
                (camera_id == 1 ? "ras" : (sas_id == 1 ? "pyasf" : "pyasr")),
                timestamp, (int)(localHeader.captureTime.tv_nsec/1000000l),
                (int)localHeader.frameCount);

        printf("Saving image %s: exposure %d us, analog gain %d, preamp gain %d\n", filename, localHeader.exposure, localHeader.analogGain, localHeader.preampGain);
        writeFITSImage(localFrame, localHeader, filename);
    }
    else
    {
    }

    //This thread should only ever be started if the semaphore was incremented
    semaphoreSave[camera_id].decrement();

    clock_gettime(CLOCK_MONOTONIC, &postSave);
    elapsedSave = TimespecDiff(preSave, postSave);
    //std::cout << "Saving took: " << elapsedSave.tv_sec << " " << elapsedSave.tv_nsec << std::endl;

    started[tid] = false;
    pthread_exit(NULL);
}

void *TelemetryPackagerThread(void *threadargs)
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("TelemetryPackager thread #%ld!\n", tid);

    uint32_t tm_frame_sequence_number = 0;

    HeaderData localHeaders[2];
    Sensors localSensors;

    float housekeeping1[7], housekeeping2[7];
    for (int j = 0; j < 7; j++) housekeeping1[j] = housekeeping2[j] = 0;

    while(!stop_message[tid])
    {
        usleep(USLEEP_TM_GENERIC);
        tm_frame_sequence_number++;

        TelemetryPacket tp(TM_SAS_GENERIC, SOURCE_ID_SAS);
        tp.setSAS(sas_id);
        tp << (uint32_t)tm_frame_sequence_number;

        uint8_t status_bitfield = 0;
        bitwrite(&status_bitfield, 7, 1, localHeaders[0].isTracking);
        bitwrite(&status_bitfield, 6, 1, receivedGoodGPS);
        receivedGoodGPS = false;
        bitwrite(&status_bitfield, 5, 1, localHeaders[0].isOutputting);
        bitwrite(&status_bitfield, 0, 5, localHeaders[0].runResult);
        tp << (uint8_t)status_bitfield;

        tp << (uint16_t)latest_sas_command_key;

        if(pthread_mutex_trylock(&mutexHeader[0]) == 0) {
            localHeaders[0] = header[0];
            pthread_mutex_unlock(&mutexHeader[0]);
        }
        if(pthread_mutex_trylock(&mutexHeader[1]) == 0) {
            localHeaders[1] = header[1];
            pthread_mutex_unlock(&mutexHeader[1]);
        }
        if(pthread_mutex_trylock(&mutexSensors) == 0) {
            localSensors = sensors;
            pthread_mutex_unlock(&mutexSensors);
        }

        //Housekeeping fields, two of them
        //All temperatures and voltages will be 8-frame averages
        //Especially on startup, these averages can be peculiar
        housekeeping1[0] += localSensors.sbc_temperature;
        housekeeping1[1] += localSensors.i2c_temperatures[0];
        housekeeping1[2] += localSensors.i2c_temperatures[1];
        housekeeping1[3] += localSensors.i2c_temperatures[2];
        housekeeping1[4] += localSensors.i2c_temperatures[3];
        housekeeping1[5] += localSensors.i2c_temperatures[4];
        housekeeping1[6] += localSensors.i2c_temperatures[5];

        housekeeping2[0] += localHeaders[0 % sas_id].cameraTemperature;
        housekeeping2[1] += localHeaders[1 % sas_id].cameraTemperature;
        housekeeping2[2] += localSensors.sbc_v105;
        housekeeping2[3] += localSensors.sbc_v25;
        housekeeping2[4] += localSensors.sbc_v33;
        housekeeping2[5] += localSensors.sbc_v50;
        housekeeping2[6] += localSensors.sbc_v120;

        //Scale factors to make good use of Float2B range
        //  Temperatures multiplied by 10
        //  Voltages multiplied by 500
        int which = tm_frame_sequence_number % 8;
        switch (which){
            case 0:
            case 1:
                tp << Float2B(housekeeping1[which]/8*10);
                tp << Float2B(housekeeping2[which]/8*10);
                housekeeping1[which] = housekeeping2[which] = 0;
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                tp << Float2B(housekeeping1[which]/8*10);
                tp << Float2B(housekeeping2[which]/8*500);
                housekeeping1[which] = housekeeping2[which] = 0;
                break;
            case 7:
                tp << (uint16_t)isClockSynced;
                tp << (uint16_t)(isSavingImages[0]+2*isSavingImages[1]);
                break;
            default:
                tp << (uint16_t)0xFFFF;
                tp << (uint16_t)0xFFFF;
        }

/*
        std::cout << "Telemetry packet with Sun center (pixels): " << Pair(localHeaders[0].sunCenter[0], localHeaders[0].sunCenter[1]);
        std::cout << ", mapping is";
        for(uint8_t l = 0; l < 4; l++) std::cout << " " << localHeaders[0].XYinterceptslope[l];
        std::cout << std::endl;

        std::cout << "Offset: " << Pair(localHeaders[0].CTLsolution[0], localHeaders[0].CTLsolution[1]) << std::endl;
*/

        //Sun center and error
        tp << Pair3B(localHeaders[0].sunCenter[0], localHeaders[0].sunCenter[1]);
        tp << Pair3B(localHeaders[0].sunCenterError[0], localHeaders[0].sunCenterError[1]);

        //Limb crossings (currently 8)
        for(uint8_t j = 0; j < 8; j++) {
            tp << Pair3B(localHeaders[0].limbX[j], localHeaders[0].limbY[j]);
        }

        // Number of fiducials
        tp << (uint8_t)localHeaders[0].fiducialCount;
        // Number of limb crossings
        tp << (uint8_t)localHeaders[0].limbCount;

        //Fiduicals (currently 6)
        for(uint8_t j = 0; j < 6; j++) {
            tp << Pair3B(localHeaders[0].fiducialX[j], localHeaders[0].fiducialY[j]);
        }

        //Pixel to screen conversion
        tp << localHeaders[0].XYinterceptslope[0]; //X intercept
        tp << localHeaders[0].XYinterceptslope[2]; //X slope
        tp << localHeaders[0].XYinterceptslope[1]; //Y intercept
        tp << localHeaders[0].XYinterceptslope[3]; //Y slope

        //Image max and min
        int camera_id = tm_frame_sequence_number % sas_id;
        tp << (uint8_t) localHeaders[camera_id].imageMinMax[1]; //max
        //tp << (uint8_t) localHeaders[camera_id].imageMinMax[0]; //min

        //Tacking on the offset numbers intended for CTL as floats
        tp << (float)(localHeaders[0].CTLsolution[0]);
        tp << (float)(localHeaders[0].CTLsolution[1]);

        //Fiduical IDs (currently 6)
        //7 is added to the ID number (Which ranges from -7 to 7)
        for(uint8_t j = 0; j < 6; j++) {
            uint8_t temp = 0;
            int idx = localHeaders[0].fiducialIDX[j]+7;
            int idy = localHeaders[0].fiducialIDY[j]+7;
            bitwrite(&temp, 0, 4, (uint8_t)(idx >= 0 ? idx : 15));
            bitwrite(&temp, 4, 4, (uint8_t)(idy >= 0 ? idy : 15));
            tp << (uint8_t)temp;
        }

        if (localHeaders[0].captureTime.tv_sec != 0) {
            tp.setTimeAndFinish(localHeaders[0].captureTime);
        } else {
            tp.setTimeAndFinish();
        }

        //add telemetry packet to the queue if not being suppressed
        if (tm_frames_to_suppress > 0) tm_frames_to_suppress--;
        else tm_packet_queue << tp;
    }

    printf("TelemetryPackager thread #%ld exiting\n", tid);
    started[tid] = false;
    pthread_exit( NULL );
}

void *CommandListenerThread(void *threadargs)
{  
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("CommandListener thread #%ld!\n", tid);

    tid_listen = tid;

    CommandReceiver comReceiver( (unsigned short) PORT_CMD);
    comReceiver.init_connection();

    while(!stop_message[tid])
    {
        unsigned int packet_length;

        usleep(USLEEP_UDP_LISTEN);
        packet_length = comReceiver.listen( );
        printf("CommandListenerThread: %i bytes, ", packet_length);
        uint8_t *packet;
        packet = new uint8_t[packet_length];
        comReceiver.get_packet( packet );

        CommandPacket command_packet( packet, packet_length );

        if (command_packet.valid()){
            printf("valid checksum, ");

            command_sequence_number = command_packet.getSequenceNumber();

            if (sas_id == 1) {
                // add command ack packet
                TelemetryPacket ack_tp(TM_ACK_RECEIPT, SOURCE_ID_SAS);
                ack_tp << command_sequence_number;
                ack_tp.setTimeAndFinish();
                tm_packet_queue << ack_tp;
            }

            // update the command count
            printf("command sequence number %i", command_sequence_number);

            if ((command_packet.getTargetID() == TARGET_ID_SAS) ||
                (command_packet.getTargetID() == TARGET_ID_ALL)) {
                try { recvd_command_queue.add_packet(command_packet); }
                catch (std::exception& e) {
                    std::cerr << e.what() << std::endl;
                }
            }

        } else {
            printf("INVALID checksum");
        }
        printf("\n");

        delete packet;
    }

    printf("CommandListener thread #%ld exiting\n", tid);
    comReceiver.close_connection();
    started[tid] = false;
    pthread_exit( NULL );
}

void *ForwardCommandsFromSAS2Thread(void *threadargs)
{  
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("ForwardCommandsFromSAS2 thread #%ld!\n", tid);

    CommandReceiver comReceiver( (unsigned short) PORT_SAS2);
    comReceiver.init_connection();

    CommandSender comForwarder(IP_CTL, PORT_CMD);

    while(!stop_message[tid])
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
    }

    printf("ForwardCommandsFromSAS2 thread #%ld exiting\n", tid);
    comReceiver.close_connection();
    started[tid] = false;
    pthread_exit( NULL );
}

void *CommandSenderThread( void *threadargs )
{
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    printf("CommandSender thread #%ld!\n", tid);

    char timestamp[14];
    char filename[128];
    std::ofstream log;

    if (LOG_PACKETS) {
        writeCurrentUT(timestamp);
        sprintf(filename, "%slog_cm_%s.bin", SAVE_LOCATION1, timestamp);
        filename[128 - 1] = '\0';
        printf("Creating command log file %s \n",filename);
        log.open(filename, std::ofstream::binary);
        if (!log.is_open()) {
            sprintf(filename, "%slog_cm_%s.bin", SAVE_LOCATION2, timestamp);
            filename[128 - 1] = '\0';
            printf("Creating command log file %s \n",filename);
            log.open(filename, std::ofstream::binary);
        }
    }

    CommandSender comSender(IP_CTL, PORT_CMD);

    while(!stop_message[tid])
    {
        usleep(USLEEP_CMD_SEND);
    
        if( !cm_packet_queue.empty() ){
            CommandPacket cp(NULL);
            cm_packet_queue >> cp;
            comSender.send( &cp );
            //std::cout << "CommandSender: " << cp << std::endl;
            if (LOG_PACKETS && log.is_open()) {
                uint8_t length = cp.getLength();
                uint8_t *payload = new uint8_t[length];
                cp.outputTo(payload);
                log.write((char *)payload, length);
                delete payload;
                //log.flush();
            }
        }
    }

    printf("CommandSender thread #%ld exiting\n", tid);
    if (LOG_PACKETS && log.is_open()) log.close();
    started[tid] = false;
    pthread_exit( NULL );
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
            } else if (check_solution(argHeader)) {
                cp << (uint16_t)HKEY_SAS_SOLUTION;
                cp << (double)argHeader.CTLsolution[0]; // azimuth offset
                cp << (double)argHeader.CTLsolution[1]; // elevation offset
                cp << (double)0; // roll offset
                cp << (double)0.003; // error
                cp << (uint32_t)argHeader.captureTime.tv_sec; //seconds
                cp << (uint16_t)(argHeader.captureTime.tv_nsec/1e6+0.5); //milliseconds, rounded
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

bool check_solution(HeaderData &argHeader)
{
    //Do some basic checking to see whether a valid solution was found
    if ((fabs(argHeader.XYinterceptslope[2]) < 5.5) ||
        (fabs(argHeader.XYinterceptslope[2]) > 6)) return false;
    if ((fabs(argHeader.XYinterceptslope[3]) < 5.5) ||
        (fabs(argHeader.XYinterceptslope[3]) > 6)) return false;

    return true;
}

void queue_cmd_proc_ack_tmpacket( uint16_t error_code )
{
    TelemetryPacket ack_tp(TM_ACK_PROCESS, SOURCE_ID_SAS);
    ack_tp << command_sequence_number;
    if (latest_heroes_command_key == HKEY_FDR_SAS_CMD) {
        ack_tp << latest_sas_command_key;
    } else {
        ack_tp << latest_heroes_command_key;
    }
    ack_tp << error_code;
    ack_tp.setTimeAndFinish();
    tm_packet_queue << ack_tp;
}

uint16_t cmd_send_image_to_ground( int camera_id )
{
    // camera_id refers to 0 PYAS, 1 is RAS (if valid)
    camera_id = camera_id % sas_id;
    uint16_t error_code = 0;
    cv::Mat localFrame;
    HeaderData localHeader;

    char timestamp[14];
    char filename[128];
    std::ofstream log;

    if (LOG_PACKETS) {
        writeCurrentUT(timestamp);
        sprintf(filename, "%slog_sc_%s.bin", SAVE_LOCATION1, timestamp);
        filename[128 - 1] = '\0';
        printf("Creating science log file %s \n",filename);
        log.open(filename, std::ofstream::binary);
        if (!log.is_open()) {
            sprintf(filename, "%slog_sc_%s.bin", SAVE_LOCATION2, timestamp);
            filename[128 - 1] = '\0';
            printf("Creating science log file %s \n",filename);
            log.open(filename, std::ofstream::binary);
        }
    }

    TCPSender tcpSndr(IP_FDR, (unsigned short) PORT_IMAGE);
    int ret = tcpSndr.init_connection();
    if (ret > 0){
        if (pthread_mutex_trylock(&mutexHeader[camera_id]) == 0){
            if( !frame[camera_id].empty() ){
                frame[camera_id].copyTo(localFrame);
                localHeader = header[camera_id];
            }
            pthread_mutex_unlock(&mutexHeader[camera_id]);
        }
        if( !localFrame.empty() ){
            ImagePacketQueue im_packet_queue;

            bool tlogical;
            int tint;
            long tlong;
            float tfloat;
            double tdouble;

            //First add FITS header tags
            im_packet_queue << ImageTagPacket(localHeader.cameraID, "HEROES/SAS", TSTRING, "TELESCOP", "Name of source telescope package");

            switch(localHeader.cameraID) {
                case 1:
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, "PYAS-F", TSTRING, "INSTRUME", "Name of instrument");
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, "HEROES/SAS-1", TSTRING, "ORIGIN", "Location where file was made");
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tlong = 6300), TLONG, "WAVELNTH", "Wavelength of observation (ang)");
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, "630 nm", TSTRING, "WAVE_STR", "Wavelength of observation");
                    break;
                case 2:
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, "PYAS-R", TSTRING, "INSTRUME", "Name of instrument");
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, "HEROES/SAS-2", TSTRING, "ORIGIN", "Location where file was made");
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tlong = 6300), TLONG, "WAVELNTH", "Wavelength of observation (ang)");
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, "630 nm", TSTRING, "WAVE_STR", "Wavelength of observation");
                    break;
                case 6:
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, "RAS", TSTRING, "INSTRUME", "Name of instrument");
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, "HEROES/SAS-2", TSTRING, "ORIGIN", "Location where file was made");
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tlong = 6300), TLONG, "WAVELNTH", "Wavelength of observation (ang)");
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, "600 nm", TSTRING, "WAVE_STR", "Wavelength of observation");
                    break;
            }

            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tlong = 8), TLONG, "BITPIX", "Bit depth of image");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, "angstrom", TSTRING, "WAVEUNIT", "Units of WAVELNTH");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, "DN", TSTRING, "PIXLUNIT", "Pixel units");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, "LIGHT", TSTRING, "IMG_TYPE", "Image type");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tdouble = 6.96e8), TDOUBLE, "RSUN_REF", "");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, "LIGHT", TSTRING, "CTLMODE", "Image type");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tint = 0), TINT, "LVL_NUM", "Level of data");

            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tdouble = 0), TDOUBLE, "RSUN_OBS", "");

            im_packet_queue << ImageTagPacket(localHeader.cameraID, "HPLN-TAN", TSTRING, "CTYPE1", "Coordinate axis system");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, "HPLN-TAN", TSTRING, "CTYPE2", "Coordinate axis system");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, "arcsec", TSTRING, "CUNIT1", "Coordinate Units");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, "arcsec", TSTRING, "CUNIT2", "Coordinate Units");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tdouble = 0.0), TDOUBLE, "CRVAL1", "Reference pixel coordinate");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tdouble = 0.0), TDOUBLE, "CRVAL2", "Reference pixel coordinate");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tdouble = localHeader.XYinterceptslope[2] * 1.72), TDOUBLE, "CDELT1", "Plate scale");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tdouble = localHeader.XYinterceptslope[3] * 1.72), TDOUBLE, "CDELT2", "Plate scale");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tdouble = localHeader.sunCenter[0]+1), TDOUBLE, "CRPIX1", "Reference pixel");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tdouble = localHeader.sunCenter[1]+1), TDOUBLE, "CRPIX2", "Reference pixel");

            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.exposure/1e6), TFLOAT, "EXPTIME", "Exposure time in seconds");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, asctime(gmtime(&(localHeader.captureTime).tv_sec)), TSTRING, "DATE_OBS", "Date of observation (UTC)");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.cameraTemperature), TFLOAT, "TEMPCCD", "Temperature of camera (deg C)");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tint = localHeader.cpuTemperature), TINT, "TEMPCPU", "Temperature of cpu (deg C)");

            im_packet_queue << ImageTagPacket(localHeader.cameraID, "N/A", TSTRING, "FILENAME", "Name of the data file");

            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tlong = localHeader.captureTime.tv_sec), TLONG, "RT_SEC", "Realtime clock, seconds");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tlong = localHeader.captureTime.tv_nsec), TLONG, "RT_NSEC", "Realtime clock, nanoseconds");

            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tlong = localHeader.captureTimeMono.tv_sec), TLONG, "MON_SEC", "Monotonic clock, seconds");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tlong = localHeader.captureTimeMono.tv_nsec), TLONG, "MON_NSEC", "Monotonic clock, nanoseconds");

            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tint = localHeader.exposure), TINT, "EXPOSURE", "Exposure time in usec");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tint = localHeader.preampGain), TINT, "GAIN_PRE", "Preamp gain of CCD");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tint = localHeader.analogGain), TINT, "GAIN_ANA", "Analog gain of CCD");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tlong = localHeader.frameCount), TLONG, "FRAMENUM", "Frame number");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.imageMinMax[0]), TFLOAT, "DATAMIN", "Minimum value of data");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.imageMinMax[1]), TFLOAT, "DATAMAX", "Maximum value of data");

            if((localHeader.cameraID == 1) || (localHeader.cameraID == 2)) {
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tlogical = localHeader.isTracking), TLOGICAL, "F_TRACK", "Is SAS currently tracking?");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tlogical = localHeader.isOutputting), TLOGICAL, "F_OUTPUT", "Is this SAS outputting to CTL?");

                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.latitude), TFLOAT, "GPS_LAT", "GPS latitude (degrees)");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.longitude), TFLOAT, "GPS_LAT", "GPS longitude (degrees)");

                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.solarTarget[0]), TFLOAT, "TARGET_X", "Intended solar target in HPC (arcsec)");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.solarTarget[1]), TFLOAT, "TARGET_Y", "Intended solar target in HPC (arcsec)");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.clockingAngle), TFLOAT, "CLOCKANG", "CCW angle from screen +Y to vertical");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tdouble = localHeader.northAngle), TDOUBLE, "NORTHANG", "CW angle from zenith to solar north");

                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.sunCenter[0]), TFLOAT, "SUNCENT1", "Calculated Sun center in x-pixel");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.sunCenter[1]), TFLOAT, "SUNCENT2", "Calculated Sun center in y-pixel");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tdouble = localHeader.CTLsolution[0]), TDOUBLE, "CTL_AZIM", "Azimuth offset for CTL (deg)");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tdouble = localHeader.CTLsolution[1]), TDOUBLE, "CTL_ELEV", "Elevation offset for CTL (deg)");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.XYinterceptslope[0]), TFLOAT, "INTRCPT1", "");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.XYinterceptslope[1]), TFLOAT, "INTRCPT2", "");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.XYinterceptslope[2]), TFLOAT, "SLOPE1", "");
                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.XYinterceptslope[3]), TFLOAT, "SLOPE2", "");

                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tint = localHeader.fiducialCount), TINT, "FID_NUM", "Number of fiducials");

                for (int j = 0; j < 10; j++) {
                    char tag[9];
                    sprintf(tag, "FID%1d_-", j);
                    tag[5] = 'X';
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.fiducialX[j]), TFLOAT, tag, "");
                    tag[5] = 'Y';
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.fiducialY[j]), TFLOAT, tag, "");
                }

                for (int j = 0; j < 10; j++) {
                    char tag[9];
                    sprintf(tag, "FID%1dID_-", j);
                    tag[7] = 'X';
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tint = localHeader.fiducialIDX[j]), TINT, tag, "");
                    tag[7] = 'Y';
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tint = localHeader.fiducialIDY[j]), TINT, tag, "");
                }

                im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tint = localHeader.limbCount), TINT, "LIMB_NUM", "Number of limbs");

                for (int j = 0; j < 10; j++) {
                    char tag[9];
                    sprintf(tag, "LIMB%1d_-", j);
                    tag[6] = 'X';
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.limbX[j]), TFLOAT, tag, "");
                    tag[6] = 'Y';
                    im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.limbY[j]), TFLOAT, tag, "");
                }
            }

            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.cpuVoltage[0]), TFLOAT, "SBC_V105", "");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.cpuVoltage[1]), TFLOAT, "SBC_V25", "");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.cpuVoltage[2]), TFLOAT, "SBC_V33", "");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.cpuVoltage[3]), TFLOAT, "SBC_V50", "");
            im_packet_queue << ImageTagPacket(localHeader.cameraID, &(tfloat = localHeader.cpuVoltage[4]), TFLOAT, "SBC_V120", "");

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

            im_packet_queue.add_array(localHeader.cameraID, numXpixels, numYpixels, array);

            delete array;

            //Make sure to synchronize all the timestamps
            im_packet_queue.synchronize();

            std::cout << "Sending " << im_packet_queue.size() << " packets\n";

            ImagePacket im(NULL);
            while(!im_packet_queue.empty()) {
                im_packet_queue >> im;
                tcpSndr.send_packet( &im );
                if (LOG_PACKETS && log.is_open()) {
                    uint16_t length = im.getLength();
                    uint8_t *payload = new uint8_t[length];
                    im.outputTo(payload);
                    log.write((char *)payload, length);
                    delete payload;
                    //log.flush();
                }
            }

            //This additional packet should flush the receiving buffer
            TelemetryPacket idle(TM_IDLE, SOURCE_ID_SAS);
            uint8_t fill[256];
            memset(fill, 0xAA, 256);
            idle.append_bytes(fill, 256);
            tcpSndr.send_packet(&idle);
        }
        tcpSndr.close_connection();
        error_code = 1;
    } else { error_code = 2; }

    if (LOG_PACKETS && log.is_open()) log.close();
    return error_code;
}
        
void *CommandHandlerThread(void *threadargs)
{
    // command error code definition
    // error_code   description
    // 0x0000       command implemented successfully
    // 0x0001       command not implemented
    // 0xFFFF       unknown command
    // 
    long tid = (long)((struct Thread_data *)threadargs)->thread_id;
    struct Thread_data *my_data;
    uint16_t error_code = 0x0001;
    my_data = (struct Thread_data *) threadargs;

    switch( my_data->command_key & 0x0FFF)
    {
        //Operations commands
        case SKEY_START_OUTPUTTING:
            isOutputting = true;
            error_code = 0;
            break;
        case SKEY_STOP_OUTPUTTING:
            isOutputting = false;
            error_code = 0;
            break;
        case SKEY_SUPPRESS_TELEMETRY:
            tm_frames_to_suppress = (my_data->command_vars[0] <= 300 ? my_data->command_vars[0] : 300);
            error_code = 0;
            break;
        case SKEY_TURN_OFF_ALL_RELAYS:
            for (int i = 0; i < NUM_RELAYS-1; i++) {
                send_relay_control(i, RELAY_OFF);
            }
            error_code = 0;
            break;
        case SKEY_TURN_ON_ALL_RELAYS:
            for (int i = 0; i < NUM_RELAYS-1; i++) {
                send_relay_control(i, RELAY_ON);
            }
            error_code = 0;
            break;
        case SKEY_DEFAULT_RELAYS:
            send_relay_control(0, RELAY_ON);
            send_relay_control(1, RELAY_ON);
            for (int i = 2; i < NUM_RELAYS-1; i++) {
                send_relay_control(i, RELAY_OFF);
            }
            error_code = 0;
            break;
        case SKEY_TURN_RELAY_ON:
            send_relay_control(my_data->command_vars[0], RELAY_ON);
            error_code = 0;
            break;
        case SKEY_TURN_RELAY_OFF:
            send_relay_control(my_data->command_vars[0], RELAY_OFF);
            error_code = 0;
            break;
        case SKEY_PYAS_TSTAT_BYPASS:
            send_relay_control(8, RELAY_ON);
            error_code = 0;
            break;
        case SKEY_PYAS_TSTAT_ENABLE:
            send_relay_control(8, RELAY_OFF);
            error_code = 0;
            break;
        case SKEY_RAS_TSTAT_BYPASS:
            send_relay_control(11, RELAY_ON);
            error_code = 0;
            break;
        case SKEY_RAS_TSTAT_ENABLE:
            send_relay_control(11, RELAY_OFF);
            error_code = 0;
            break;
        case SKEY_CAN_HEATERS_OFF:
            send_relay_control(9, RELAY_OFF);
            send_relay_control(10, RELAY_OFF);
            error_code = 0;
            break;
        case SKEY_CAN_HEATERS_LOW:
            send_relay_control(9, RELAY_OFF);
            send_relay_control(10, RELAY_ON);
            error_code = 0;
            break;
        case SKEY_CAN_HEATERS_MEDIUM:
            send_relay_control(9, RELAY_ON);
            send_relay_control(10, RELAY_OFF);
            error_code = 0;
            break;
        case SKEY_CAN_HEATERS_HIGH:
            send_relay_control(9, RELAY_ON);
            send_relay_control(10, RELAY_ON);
            error_code = 0;
            break;
        case SKEY_REQUEST_PYAS_IMAGE:
            error_code = cmd_send_image_to_ground( 0 ); // 0 for PYAS
            break;
        case SKEY_REQUEST_RAS_IMAGE:
            error_code = cmd_send_image_to_ground( 1 ); // 1 for RAS
            break;

        //Setting commands
        case SKEY_SET_IMAGESAVEFLAG:
            isSavingImages[0] = isSavingImages[1] = (my_data->command_vars[0] > 0);
            if( isSavingImages[0] == my_data->command_vars[0] ) error_code = 0;
            std::cout << "PYAS/RAS image saving is now turned " << ( isSavingImages[0] ? "on\n" : "off\n");
            break;
        case SKEY_SET_GPSFLAG:
            isAcceptingGPS = (my_data->command_vars[0] > 0);
            if( isAcceptingGPS == my_data->command_vars[0] ) error_code = 0;
            std::cout << "GPS updating is now turned " << ( isAcceptingGPS ? "on\n" : "off\n");
            break;
        case SKEY_SET_PYAS_SAVEFLAG:
            isSavingImages[0] = (my_data->command_vars[0] > 0);
            if( isSavingImages[0] == my_data->command_vars[0] ) error_code = 0;
            std::cout << "PYAS image saving is now turned " << ( isSavingImages[0] ? "on\n" : "off\n");
            break;
        case SKEY_SET_PYAS_EXPOSURE:    // set exposure time
            settings[0].exposure = my_data->command_vars[0];
            if( settings[0].exposure == my_data->command_vars[0] ) error_code = 0;
            break;
        case SKEY_SET_PYAS_PREAMPGAIN:    // set preamp gain
            settings[0].preampGain = (int16_t)my_data->command_vars[0];
            if( settings[0].preampGain == (int16_t)my_data->command_vars[0] ) error_code = 0;
            break;
        case SKEY_CTL_TEST_CMD:
             error_code = cmd_send_test_ctl_solution( my_data->command_vars[0] );
             break;
        case SKEY_SET_PYAS_ANALOGGAIN:    // set analog gain
            settings[0].analogGain = my_data->command_vars[0];
            if( settings[0].analogGain == my_data->command_vars[0] ) error_code = 0;
            break;
        case SKEY_SET_RAS_SAVEFLAG:
            isSavingImages[1] = (my_data->command_vars[0] > 0);
            if( isSavingImages[1] == my_data->command_vars[0] ) error_code = 0;
            std::cout << "RAS image saving is now turned " << ( isSavingImages[1] ? "on\n" : "off\n");
            break;
        case SKEY_SET_RAS_EXPOSURE:    // set exposure time
            settings[1].exposure = my_data->command_vars[0];
            if( settings[1].exposure == my_data->command_vars[0] ) error_code = 0;
            break;
        case SKEY_SET_RAS_PREAMPGAIN:    // set preamp gain
            settings[1].preampGain = (int16_t)my_data->command_vars[0];
            if( settings[1].preampGain == (int16_t)my_data->command_vars[0] ) error_code = 0;
            break;
        case SKEY_SET_RAS_ANALOGGAIN:    // set analog gain
            settings[1].analogGain = my_data->command_vars[0];
            if( settings[1].analogGain == my_data->command_vars[0] ) error_code = 0;
            break;
        case SKEY_SET_TARGET:    // set new solar target
            solarTransform.set_solar_target(Pair((int16_t)my_data->command_vars[0], (int16_t)my_data->command_vars[1]));
            error_code = 0;
            break;
        case SKEY_SET_CLOCKING:    // set clocking
            solarTransform.set_clocking(Float2B(my_data->command_vars[0]).value());
            error_code = 0;
            break;
        case SKEY_SET_LAT_LON: //will get overriden if GPS updating is enabled
            solarTransform.set_lat_lon(Pair(Float2B(my_data->command_vars[0]).value(), Float2B(my_data->command_vars[1]).value()));
            error_code = 0;
            break;
        case SKEY_SET_ASPECT_INT:
            aspect.SetInteger((AspectInt)my_data->command_vars[0], (int16_t)my_data->command_vars[1]);
            error_code = 0;
            break;
        case SKEY_SET_ASPECT_FLOAT:
            aspect.SetFloat((AspectFloat)my_data->command_vars[0], Float2B(my_data->command_vars[1]).value());
            error_code = 0;
            break;
        case SKEY_SET_CAMERA_TWIST:
            aspect.SetFloat(FIDUCIAL_TWIST, Float2B(my_data->command_vars[0]).value());
            error_code = 0;
            break;

        //Getting commands
        case SKEY_GET_PYAS_EXPOSURE:
            error_code = (uint16_t)settings[0].exposure;
            break;
        case SKEY_GET_PYAS_ANALOGGAIN:
            error_code = (uint16_t)settings[0].analogGain;
            break;
        case SKEY_GET_PYAS_PREAMPGAIN:
            error_code = (int16_t)settings[0].preampGain;
            break;
        case SKEY_GET_RAS_EXPOSURE:
            error_code = (uint16_t)settings[1].exposure;
            break;
        case SKEY_GET_RAS_ANALOGGAIN:
            error_code = (uint16_t)settings[1].analogGain;
            break;
        case SKEY_GET_RAS_PREAMPGAIN:
            error_code = (int16_t)settings[1].preampGain;
            break;
        case SKEY_GET_DISKSPACE:
            error_code = (uint16_t)get_disk_usage((uint16_t)my_data->command_vars[0]);
            break;
        case SKEY_GET_TARGET_X:
            error_code = (int16_t)solarTransform.get_solar_target().x();
            break;
        case SKEY_GET_TARGET_Y:
            error_code = (int16_t)solarTransform.get_solar_target().y();
            break;
        case SKEY_GET_ASPECT_INT:
            error_code = (int16_t)aspect.GetInteger((AspectInt)my_data->command_vars[0]);
            break;
        case SKEY_GET_ASPECT_FLOAT:
            error_code = (uint16_t)Float2B(aspect.GetFloat((AspectFloat)my_data->command_vars[0])).code();
            break;
        case SKEY_GET_CAMERA_TWIST:
            error_code = (uint16_t)Float2B(aspect.GetFloat(FIDUCIAL_TWIST)).code();
            break;
        case SKEY_GET_CLOCKING:
            error_code = (uint16_t)Float2B(solarTransform.get_clocking()).code();
            break;
        case SKEY_GET_LATITUDE:
            error_code = (uint16_t)Float2B((float)solarTransform.get_lat_lon().x()).code();
            break;
        case SKEY_GET_LONGITUDE:
            error_code = (uint16_t)Float2B((float)solarTransform.get_lat_lon().y()).code();
            break;
        case SKEY_GET_NTP_OFFSET_US:
            error_code = (int16_t)(ntp_offset_ms*1000);
            break;

        default:
            error_code = 0xFFFF;            // unknown command!
    }

    queue_cmd_proc_ack_tmpacket( error_code );

    started[tid] = false;
    pthread_exit(NULL);
}

void cmd_process_heroes_command(uint16_t heroes_command)
{
    if ((heroes_command & 0xFF00) == 0x1000) {
        switch(heroes_command) {
            case HKEY_CTL_START_TRACKING: // start tracking
                isTracking = true;
                acknowledgedCTL = false;
                queue_cmd_proc_ack_tmpacket(0);
                // need to send 0x1100 command packet
                break;
            case HKEY_CTL_STOP_TRACKING: // stop tracking
                isTracking = false;
                acknowledgedCTL = false;
                queue_cmd_proc_ack_tmpacket(0);
                // need to send 0x1101 command packet
                break;
            default:
                printf("Unknown HEROES command\n");
        }
    } else printf("Not a CTL-to-SAS command\n");
}

void start_thread(void *(*routine) (void *), const Thread_data *tdata)
{
    pthread_mutex_lock(&mutexStartThread);
    int i = 0;
    while (started[i] == true) {
        i++;
        if (i == MAX_THREADS) return; //should probably thrown an exception
    }

    //Copy the thread data to a global to prevent deallocation
    if (tdata != NULL) memcpy(&thread_data[i], tdata, sizeof(Thread_data));
    thread_data[i].thread_id = i;

    stop_message[i] = false;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    int rc = pthread_create(&threads[i], &attr, routine, &thread_data[i]);
    if (rc != 0) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    } else started[i] = true;

    pthread_attr_destroy(&attr);

    pthread_mutex_unlock(&mutexStartThread);

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
    Packet pkt((const uint8_t *)PASSPHRASE_SBC_SHUTDOWN, strlen(PASSPHRASE_SBC_SHUTDOWN));
    out.send(&pkt);
}

void send_relay_control(uint8_t relay_number, bool on_if_true)
{
    UDPSender out(IP_LOOPBACK, PORT_RELAY_CONTROL);
    Packet pkt((const uint8_t *)PASSPHRASE_RELAY_CONTROL, strlen(PASSPHRASE_RELAY_CONTROL));
    pkt << relay_number << (uint8_t)on_if_true;
    out.send(&pkt);
}

uint16_t cmd_send_test_ctl_solution( int type )
{
    uint16_t error_code = 0;
    float num_solutions_to_send = 120;

    int num_test_solutions = 8;
    int test_solution_azimuth[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
    int test_solution_elevation[] = { 0, 0, 1, -1, 1, 1, -1, -1 };

    for( int i = 0; i < num_solutions_to_send; i++ ){
        timespec localSolutionTime;
        clock_gettime(CLOCK_REALTIME, &localSolutionTime);
        // first send time of next solution
        ctl_sequence_number++;
        CommandPacket cp(TARGET_ID_CTL, ctl_sequence_number);
        cp << (uint16_t)HKEY_SAS_TIMESTAMP;
        cp << (uint16_t)0x0001; // Camera ID (=1 for SAS, irrespective which SAS is providing solutions) 
        cp << (double)(localSolutionTime.tv_sec + (double)localSolutionTime.tv_nsec/1e9);  // timestamp 
        cm_packet_queue << cp;

        ctl_sequence_number++;
        CommandPacket cp2(TARGET_ID_CTL, ctl_sequence_number);

        cp2 << (uint16_t)HKEY_SAS_SOLUTION;
        if (type < num_test_solutions) {
        //cp2 << (double)test_solution_azimuth[type] * (num_solutions_to_send-(float)i-1)/num_solutions_to_send * 0.5; // azimuth offset
        //cp2 << (double)test_solution_elevation[type] * (num_solutions_to_send-(float)i-1)/num_solutions_to_send * 0.5; // elevation offset
        cp2 << (double)test_solution_azimuth[type] * 0.5; // azimuth offset
        cp2 << (double)test_solution_elevation[type] * 0.5; // elevation offset
        } else {
            cp2 << (double)0; // azimuth offset
            cp2 << (double)0; // elevation offset
        }
        cp2 << (double)0; // roll offset
        cp2 << (double)0.003; // error
        cp2 << (uint32_t)localSolutionTime.tv_sec; //seconds
        cp2 << (uint16_t)(localSolutionTime.tv_nsec/1e6+0.5); //milliseconds, rounded
        cm_packet_queue << cp2;
        sleep(1);
    }
    // no way to check if this worked so just always send 1
    error_code = 1;
    return error_code;
}

void cmd_process_gps_info(Command &command)
{
    if (command.get_heroes_command() != HKEY_FDR_GPS_INFO) return;

    static float new_lat, new_lon;
    static float old_lat = 0, old_lon = 0;

    //Initialize the starting location to the same as the Transform setting
    if ((old_lat == 0) && (old_lon == 0)) {
        Pair start = solarTransform.get_lat_lon();
        old_lat = start.x();
        old_lon = start.y();
    }

    command >> new_lat >> new_lon;

    //These packets shouldn't be sent to us anymore, but just in case...
    if ((new_lat == 0) && (new_lon == 0)) {
        std::cerr << "Bad GPS information packet!\n";
        return;
    }

    receivedGoodGPS = true;

    if (!isAcceptingGPS) return;

    //Update the location if it has changed
    //Broad range of acceptable changes (in case software resets):
    //  +/- 5 degrees in latitude
    //  +/- 15 degrees in longitude
    if (((new_lat != old_lat) || (new_lon != old_lon)) &&
        (fabs(new_lat-old_lat) < 5.) && (fabs(new_lon-old_lon) < 15.)) {
        printf("GPS updated from (%f, %f) to (%f, %f)\n", old_lat, old_lon, new_lat, new_lon);
        solarTransform.set_lat_lon(Pair(new_lat, new_lon));
        old_lat = new_lat;
        old_lon = new_lon;
    }
}

void cmd_process_sas_command(Command &command)
{
    uint16_t sas_command = command.get_sas_command();

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
                }
                break;
            case SKEY_RESTART_THREADS:    // (re)start all worker threads
                {
                    kill_all_threads();

                    start_thread(CommandListenerThread, NULL);
                    start_all_workers();
                }
                break;
            case SKEY_QUIT_RUNTIME:
                {
                    g_running = 0;
                }
                break;
            case SKEY_SHUTDOWN:
                {
                    kill_all_threads();
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
    // to catch a Ctrl-C or termination signal and clean up
    signal(SIGINT, &sig_handler);
    signal(SIGTERM, &sig_handler);

    identifySAS();
    switch (sas_id) {
        case 1:
            isOutputting = true;
            aspect.SetFloat(FIDUCIAL_TWIST, TWIST_PYASF);
            solarTransform.set_clocking(CLOCKING_ANGLE_PYASF);
            solarTransform.set_calibrated_center(Pair(CENTER_X_PYASF, CENTER_Y_PYASF));
            break;
        case 2:
            isOutputting = false;
            aspect.SetFloat(FIDUCIAL_TWIST, TWIST_PYASR);
            solarTransform.set_clocking(CLOCKING_ANGLE_PYASR);
            solarTransform.set_calibrated_center(Pair(CENTER_X_PYASR, CENTER_Y_PYASR));
            break;
    }

    pthread_mutex_init(&mutexStartThread, NULL);
    pthread_mutex_init(&mutexHeader[0], NULL);
    pthread_mutex_init(&mutexHeader[1], NULL);
    pthread_mutex_init(&mutexSensors, NULL);

    /* Create worker threads */
    printf("In main: creating threads\n");

    for(int i = 0; i < MAX_THREADS; i++ ){
        started[0] = false;
    }

    // start the listen for commands thread right away
    start_thread(CommandListenerThread, NULL);
    start_all_workers();

    while(g_running){
        usleep(USLEEP_MAIN);

        // check if new command have been added to command queue and service them
        if (!recvd_command_queue.empty()){
            //printf("size of queue: %zu\n", recvd_command_queue.size());
            Command command;
            command = Command();
            recvd_command_queue >> command;

            latest_heroes_command_key = command.get_heroes_command();

            switch(latest_heroes_command_key) {
                case HKEY_FDR_SAS_CMD:
                    latest_sas_command_key = command.get_sas_command();
                    printf("Received command key 0x%04X/0x%04X\n", latest_heroes_command_key, latest_sas_command_key);
                    cmd_process_sas_command(command);
                    break;
                case HKEY_FDR_GPS_INFO:
                    cmd_process_gps_info(command);
                    break;
                default:
                    printf("Received command key 0x%04X\n", latest_heroes_command_key);
                    cmd_process_heroes_command(latest_heroes_command_key);
            }
        }
    }

    /* Last thing that main() should do */
    printf("Quitting and cleaning up.\n");
    /* wait for threads to finish */
    kill_all_threads();
    pthread_mutex_destroy(&mutexStartThread);
    pthread_mutex_destroy(&mutexHeader[0]);
    pthread_mutex_destroy(&mutexHeader[1]);
    pthread_mutex_destroy(&mutexSensors);
    pthread_exit(NULL);

    return 0;
}

