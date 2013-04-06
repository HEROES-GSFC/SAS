#define NUM_THREADS 11
#define SAS_TARGET_ID 0x30
#define SAS_TM_TYPE 0x70
#define SAS_IMAGE_TYPE 0x82
#define SAS_CM_ACK_TYPE 0x01
#define SAS_CM_PROC_ACK_TYPE 0xe1
#define CTL_IP_ADDRESS "192.168.1.2"
#define FDR_IP_ADDRESS "192.168.2.4"
#define CTL_CMD_PORT 2000
#define SAS_CMD_PORT 2001
#define TPCPORT_FOR_IMAGE_DATA 2013
#define UDPPORT_FOR_TM 2002
#define SAVE_LOCATION "/mnt/disk2/"
#define SECONDS_AFTER_SAVE 5

#define START_CTL_CMD_KEY 0x0030
#define STOP_CTL_CMD_KEY 0x0040

#define SAS1_MAC_ADDRESS "00:20:9d:23:26:b9"
#define SAS2_MAC_ADDRESS "00:20:9d:23:5c:9e"

#include <cstring>
#include <stdio.h>      /* for printf() and fprintf() */
#include <pthread.h>    /* for multithreading */
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for sleep()  */
#include <signal.h>     /* for signal() */
#include <math.h>       /* for testing only, remove when done */
#include <sys/io.h>     /* for outb, computer parameters */
#include <ctime>        /* time_t, struct tm, time, gmtime */
#include <opencv.hpp>
#include <iostream>
#include <string>

#include "UDPSender.hpp"
#include "UDPReceiver.hpp"
#include "Command.hpp"
#include "Telemetry.hpp"
#include "Transform.hpp"
#include "types.hpp"
#include "TCPSender.hpp"
#include "ImperxStream.hpp"
#include "processing.hpp"
#include "compression.hpp"
#include "utilities.hpp"
//#include "commandHandler.hpp"

// global declarations
uint16_t command_sequence_number = 0;
uint16_t latest_sas_command_key = 0x0000;
uint16_t latest_sas_command_vars[15];
uint32_t tm_frame_sequence_number = 0;

bool provide_CTL_solutions = 0;

CommandQueue recvd_command_queue;
TelemetryPacketQueue tm_packet_queue;
CommandPacketQueue cm_packet_queue;

// related to threads
unsigned int stop_message[NUM_THREADS];
pthread_t threads[NUM_THREADS];
bool skip[NUM_THREADS];
pthread_attr_t attr;
pthread_mutex_t mutexImage;
pthread_mutex_t mutexProcess;

struct Thread_data{
    int  thread_id;
    uint16_t command_key;
    uint8_t command_num_vars;
    uint16_t command_vars[15];
};
struct Thread_data thread_data;

sig_atomic_t volatile g_running = 1;

int sas_id;

cv::Mat frame;

Aspect aspect;
AspectCode runResult;
Transform solarTransform;

uint8_t frameMin, frameMax;
cv::Point2f pixelCenter, screenCenter, error;
CoordList limbs, pixelFiducials, screenFiducials;
IndexList ids;
std::vector<float> mapping;

bool staleFrame;
Flag procReady, saveReady;
int runtime = 10;
uint16_t exposure = 4500;
timespec frameRate = {0,100000000L};
int cameraReady = 0;

timespec frameTime;
long int frameCount = 0;

float camera_temperature;
int8_t sbc_temperature;
float sbc_v105, sbc_v25, sbc_v33, sbc_v50, sbc_v120;

void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        g_running = 0;
    }
}

void kill_all_threads( void ){
    for(int i = 1; i < NUM_THREADS; i++ ){
        stop_message[i] = 1;
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

void *CameraStreamThread( void * threadid)
{    
    long tid;
    tid = (long)threadid;
    printf("CameraStream thread #%ld!\n", tid);

    ImperxStream camera;

    cv::Mat localFrame;
    timespec preExposure, postExposure, timeElapsed, duration;
    int width, height;
    int failcount = 0;

    uint16_t localExposure;
    localExposure = exposure;

    cameraReady = 0;
    staleFrame = true;
    while(1)
    {
        if (stop_message[tid] == 1)
        {
            printf("CameraStream thread #%ld exiting\n", tid);
            camera.Stop();
            camera.Disconnect();
            pthread_exit( NULL );
        }
        else if (cameraReady == false)
        {
            if (camera.Connect() != 0)
            {
                std::cout << "Error connecting to camera!\n";
                sleep(1);
                continue;
            }
            else
            {
                camera.ConfigureSnap();
                //camera.SetROISize(960,960);
                //camera.SetROIOffset(165,0);
                camera.SetExposure(exposure);

                width = camera.GetROIWidth();
                height = camera.GetROIHeight();
                localFrame.create(height, width, CV_8UC1);
                if(camera.Initialize() != 0)
                {
                    std::cout << "Error initializing camera!\n";
                    //may need disconnect here
                    sleep(1);
                    continue;
                }
                cameraReady = 1;
                frameCount = 0;
            }
        }
        else
        {
            if (localExposure != exposure) {
                localExposure = exposure;
                camera.SetExposure(localExposure);
            }

            clock_gettime(CLOCK_REALTIME, &preExposure);

            if(!camera.Snap(localFrame))
            {
                failcount = 0;
                procReady.raise();
                saveReady.raise();

                //printf("CameraStreamThread: trying to lock\n");
                pthread_mutex_lock(&mutexImage);
                //printf("CameraStreamThread: got lock, copying over\n");
                localFrame.copyTo(frame);
                frameTime = preExposure;
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
            clock_gettime(CLOCK_REALTIME, &postExposure);
            timeElapsed = TimespecDiff(preExposure, postExposure);
            duration.tv_sec = frameRate.tv_sec - timeElapsed.tv_sec;
            duration.tv_nsec = frameRate.tv_nsec - timeElapsed.tv_nsec;
//            std::cout << timeElapsed.tv_sec << " " << timeElapsed.tv_nsec << "\n";
            nanosleep(&duration, NULL);
        }
    }
}

void *ImageProcessThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
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

void *TelemetrySenderThread(void *threadid)
{    
    long tid;
    tid = (long)threadid;
    printf("TelemetrySender thread #%ld!\n", tid);

    TelemetrySender telSender(FDR_IP_ADDRESS, (unsigned short) UDPPORT_FOR_TM);

    while(1)    // run forever
    {
        usleep(50000);

        if( !tm_packet_queue.empty() ){
            TelemetryPacket tp(NULL);
            tm_packet_queue >> tp;
            telSender.send( &tp );
            //std::cout << "TelemetrySender:" << tp << std::endl;
        }

        if (stop_message[tid] == 1){
            printf("TelemetrySender thread #%ld exiting\n", tid);
            pthread_exit( NULL );
        }
    }
}

void *SBCInfoThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("SBCInfo thread #%ld!\n", tid);

    UDPReceiver receiver(3456);
    receiver.init_connection();

    uint16_t packet_length;
    uint8_t *array;

    while(1)
    {
        if (stop_message[tid] == 1)
        {
            printf("SBCInfo thread #%ld exiting\n", tid);
            pthread_exit( NULL );
        }

        //This call will block forever if the service is not running
        packet_length = receiver.listen();
        array = new uint8_t[packet_length];
        receiver.get_packet(array);

        Packet packet( array, packet_length );
        packet >> sbc_temperature >> sbc_v105 >> sbc_v25 >> sbc_v33 >> sbc_v50 >> sbc_v120;
    }
}

void *SaveTemperaturesThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
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

    if((file = fopen(obsfilespec, "w")) == NULL){
        printf("Cannot open file\n");
        pthread_exit( NULL );
    } else {
        fprintf(file, "time, camera temp, cpu temp\n");
        sleep(10);
        while(1)
        {
            char current_time[25];
            if (stop_message[tid] == 1)
            {
                printf("SaveTemperatures thread #%ld exiting\n", tid);
                fclose(file);
                pthread_exit( NULL );
            }
            sleep(5);

            time(&ltime);
            times = localtime(&ltime);
            strftime(current_time,25,"%y/%m/%d %H:%M:%S",times);
            fprintf(file, "%s, %f, %d\n", current_time, camera_temperature, sbc_temperature);
            printf("%s, %f, %d\n", current_time, camera_temperature, sbc_temperature);
        }
    }
}

void *SaveImageThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("SaveImage thread #%ld!\n", tid);

    cv::Mat localFrame;
    long int localFrameCount;
    std::string fitsfile;
    timespec waittime = {1,0};
    HeaderData keys;
    //timespec thetimenow;
    while(1)
    {
        if (stop_message[tid] == 1)
        {
            printf("SaveImage thread #%ld exiting\n", tid);
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
                    keys.captureTime = frameTime;
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

                    printf("Saving image %s with exposure %d microseconds\n", obsfilespec, exposure);
                    writeFITSImage(localFrame, keys, obsfilespec);

                    sleep(SECONDS_AFTER_SAVE);
                }
                else
                {
                    pthread_mutex_unlock(&mutexImage);
                }
            }
        }
    }
}

void *TelemetryPackagerThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("TelemetryPackager thread #%ld!\n", tid);

    sleep(1);      // delay a little compared to the TelemetrySenderThread

    unsigned char localMin, localMax;
    CoordList localLimbs, localFiducials;
    std::vector<float> localMapping;
    cv::Point2f localCenter, localError;

    while(1)    // run forever
    {
        usleep(250000);
        tm_frame_sequence_number++;

        TelemetryPacket tp(SAS_TM_TYPE, SAS_TARGET_ID);
        tp.setSAS(sas_id);
        tp << tm_frame_sequence_number;
        tp << command_sequence_number;
        tp << latest_sas_command_key;

        if(pthread_mutex_trylock(&mutexProcess) == 0) 
        {
            localMin = frameMin;
            localMax = frameMax;
            localLimbs = limbs;
            localCenter = pixelCenter;
            localError = error;
            localFiducials = pixelFiducials;
            localMapping = mapping;

            std::cout << "Telemetry packet with Sun center (pixels): " << localCenter;
            if(localMapping.size() == 4) {
                std::cout << ", mapping is";
                for(uint8_t l = 0; l < 4; l++) std::cout << " " << localMapping[l];
                solarTransform.set_conversion(Pair(localMapping[0],localMapping[2]),Pair(localMapping[1],localMapping[3]));
            }
            std::cout << std::endl;

            std::cout << "Offset: " << solarTransform.calculateOffset(Pair(localCenter.x,localCenter.y)) << std::endl;

            pthread_mutex_unlock(&mutexProcess);
        } else {
            std::cout << "Using stale information for telemetry packet" << std::endl;
        }

        //Housekeeping fields, two of them
        tp << Float2B(camera_temperature);
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
                tp << Pair3B(localLimbs[j].x, localLimbs[j].y);
            } else {
                tp << Pair3B(0, 0);
            }
        }

        //Number of fiducials
        tp << (uint16_t)localFiducials.size();

        //Fiduicals (currently 6)
        for(uint8_t k = 0; k < 6; k++) {
            if (k < localFiducials.size()) {
                tp << Pair3B(localFiducials[k].x, localFiducials[k].y);
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
        tp << solarTransform.calculateOffset(Pair(localCenter.x,localCenter.y));

        //add telemetry packet to the queue
        tm_packet_queue << tp;
            
        if (stop_message[tid] == 1){
            printf("TelemetryPackager thread #%ld exiting\n", tid);
            pthread_exit( NULL );
        }
    }

    /* NEVER REACHED */
    return NULL;
}

void *listenForCommandsThread(void *threadid)
{  
    long tid;
    tid = (long)threadid;
    printf("listenForCommands thread #%ld!\n", tid);
    CommandReceiver comReceiver( (unsigned short) SAS_CMD_PORT);
    comReceiver.init_connection();
    
    while(1)    // run forever
    {
        unsigned int packet_length;
    
        packet_length = comReceiver.listen( );
        printf("listenForCommandsThread: %i\n", packet_length);
        uint8_t *packet;
        packet = new uint8_t[packet_length];
        comReceiver.get_packet( packet );
    
        CommandPacket command_packet( packet, packet_length );

        if (command_packet.valid()){
            printf("listenForCommandsThread: good command packet\n");

            command_sequence_number = command_packet.getSequenceNumber();

            // add command ack packet
            TelemetryPacket ack_tp(SAS_CM_ACK_TYPE, SAS_TARGET_ID);
            ack_tp << command_sequence_number;
            tm_packet_queue << ack_tp;

            // update the command count
            printf("command sequence number to %i\n", command_sequence_number);

            try { recvd_command_queue.add_packet(command_packet); }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
            }

        } else {
            printf("listenForCommandsThread: bad command packet\n");
        }

        if (stop_message[tid] == 1){
            printf("listenForCommands thread #%ld exiting\n", tid);
            comReceiver.close_connection();
            pthread_exit( NULL );
        }
    }

    /* NEVER REACHED */
    return NULL;
}

void *CommandSenderThread( void *threadid )
{
    long tid;
    tid = (long)threadid;
    printf("CommandSender thread #%ld!\n", tid);

    CommandSender comSender( CTL_IP_ADDRESS, CTL_CMD_PORT);

    while(1)    // run forever
    {
        usleep(5000);
    
        if( !cm_packet_queue.empty() ){
            CommandPacket cp(NULL);
            cm_packet_queue >> cp;
            comSender.send( &cp );
        }

        if (stop_message[tid] == 1){
            printf("CommandSender thread #%ld exiting\n", tid);
            comSender.close_connection();
            pthread_exit( NULL );
        }
    }
  
}

void *sendCTLCommandsThread( void *threadid )
{
    long tid;
    tid = (long)threadid;
    printf("sendCTLCommands thread #%ld!\n", tid);

    while(1)    // run forever
    {
        usleep(2500);
        if (provide_CTL_solutions) {
            sleep(1);
            CommandPacket cp(0x01, 100);
            cp << (uint16_t)0x1100;
            cm_packet_queue << cp;
        }

        if (stop_message[tid] == 1){
            printf("sendCTLCommands thread #%ld exiting\n", tid);
            pthread_exit( NULL );
        }
    }

    /* NEVER REACHED */
    return NULL;
}

void queue_cmd_proc_ack_tmpacket( uint16_t error_code )
{
    TelemetryPacket ack_tp(SAS_CM_PROC_ACK_TYPE, SAS_TARGET_ID);
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
    TCPSender tcpSndr(FDR_IP_ADDRESS, (unsigned short) TPCPORT_FOR_IMAGE_DATA);
    int ret = tcpSndr.init_connection();
    if (ret > 0){
        if (pthread_mutex_trylock(&mutexImage) == 0){
            if( !frame.empty() ){ frame.copyTo(localFrame); }
            for( int i = 0; i < 10; i++){ printf("%d\n", localFrame.at<uint8_t>(i,10));}
            pthread_mutex_unlock(&mutexImage);
        }
        if( !localFrame.empty() ){
            int numXpixels = localFrame.cols;
            int numYpixels = localFrame.rows;
            TelemetryPacket tp(SAS_IMAGE_TYPE, SAS_TARGET_ID);
            printf("sending %dx%d image\n", numXpixels, numYpixels);
            int pixels_per_packet = 100;
            int num_packets = numXpixels * numYpixels / pixels_per_packet;
            tp << (uint16_t)numXpixels;
            tp << (uint16_t)numYpixels;
            tcpSndr.send_packet( &tp );
            long k = 0;
            long int count = 0;
 
            printf("sending %d packets\n", num_packets);

            int x, y;
            for( int i = 0; i < num_packets; i++ ){
                TelemetryPacket tp(SAS_TM_TYPE, SAS_TARGET_ID);

                for( int j = 0; j < pixels_per_packet; j++){
                    x = k % numXpixels;
                    y = k / numXpixels;
                    tp << (uint8_t)localFrame.at<uint8_t>(y, x);
                    k++;
                }
                tcpSndr.send_packet( &tp );
                count++;
            }
        }
        tcpSndr.close_connection();
        error_code = 1;
    } else { error_code = 2; }
    return error_code;
}
        
void *commandHandlerThread(void *threadargs)
{
    struct Thread_data *my_data;
    uint16_t error_code = 0;
    my_data = (struct Thread_data *) threadargs;
    //long tid = (long)my_data->thread_id;

    switch( my_data->command_key & 0x0FFF)
    {
        case 0x0210:
            {
                error_code = cmd_send_image_to_ground( 0 );
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case 0x0151:    // set exposure time
            {
                if( (my_data->command_vars[0] > 0) && (my_data->command_num_vars == 1)) exposure = my_data->command_vars[0];
                std::cout << "Requested exposure time is: " << exposure << std::endl;
                queue_cmd_proc_ack_tmpacket( error_code );
            }
            break;
        case 0x0112:    // set new solar target
            solarTransform.set_solar_target(Pair((int16_t)my_data->command_vars[0], (int16_t)my_data->command_vars[1]));
            break;
        case START_CTL_CMD_KEY:
            {
                provide_CTL_solutions = 1;
            }
            break;
        case STOP_CTL_CMD_KEY:
            {
                provide_CTL_solutions = 0;
            }
            break;
        default:
            {
                error_code = 0xffff;            // unknown command!
                queue_cmd_proc_ack_tmpacket( error_code );
            }
    }

    return NULL;
}

void start_all_threads( void ){
    int rc;
    long t;
 
    for(int i = 1; i < NUM_THREADS; i++ ){
        skip[i] = true;
        // reset stop message
        stop_message[i] = 0;
    }

    // start all threads
    t = 1L;
    rc = pthread_create(&threads[t],NULL, TelemetryPackagerThread,(void *)t);
    if ((skip[t] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 2L;
    rc = pthread_create(&threads[t],NULL, sendCTLCommandsThread,(void *)t);
    if ((skip[t] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 3L;
    rc = pthread_create(&threads[t],NULL, TelemetrySenderThread,(void *)t);
    if ((skip[t] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 4L;
    rc = pthread_create(&threads[t],NULL, CommandSenderThread,(void *)t);
    if ((skip[t] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 5L;
    rc = pthread_create(&threads[t],NULL, CameraStreamThread,(void *)t);
    if ((skip[t] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 6L;
    rc = pthread_create(&threads[t],NULL, ImageProcessThread,(void *)t);
    if ((skip[t] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 7L;
    rc = pthread_create(&threads[t],NULL, SaveImageThread,(void *)t);
    if ((skip[t] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 8L;
    rc = pthread_create(&threads[t],NULL, SaveTemperaturesThread,(void *)t);
    if ((skip[t] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 9L;
    rc = pthread_create(&threads[t],NULL, SBCInfoThread,(void *)t);
    if ((skip[t] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    //Thread #10 is for the commandHandler
}

int main(void)
{  
    // to catch a Ctrl-C and clean up
    signal(SIGINT, &sig_handler);

    identifySAS();

    pthread_mutex_init(&mutexImage, NULL);
    pthread_mutex_init(&mutexProcess, NULL);

    /* Create worker threads */
    printf("In main: creating threads\n");

    // start the listen for commands thread right away
    long t = 0L;
    int rc = 0;
    rc = pthread_create(&threads[t],NULL, listenForCommandsThread,(void *)t);
    if ((skip[t] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    stop_message[0] = 0;
    start_all_threads();

    while(g_running){
        // check if new command have been added to command queue and service them
        if (!recvd_command_queue.empty()){
            printf("size of queue: %zu\n", recvd_command_queue.size());
            Command command;
            command = Command();
            recvd_command_queue >> command;

            latest_sas_command_key = command.get_sas_command();
            printf("Received command key 0x%x\n", latest_sas_command_key);

            if ((latest_sas_command_key & (sas_id << 12)) != 0) { 
                thread_data.command_key = latest_sas_command_key;
                thread_data.command_num_vars = latest_sas_command_key & 0x000F;

                for(int i = 0; i < thread_data.command_num_vars; i++){
                    try {
                      command >> thread_data.command_vars[i];
                    } catch (std::exception& e) {
                       std::cerr << e.what() << std::endl;
                    }
                }

                switch( latest_sas_command_key & 0x0FFF){
                    case 0x0000:     // test, do nothing
                        queue_cmd_proc_ack_tmpacket( 1 );
                        break;
                    case 0x0010:    // kill all worker threads
                        {
                            kill_all_threads();
                            queue_cmd_proc_ack_tmpacket( 1 );
                        }
                        break;
                    case 0x0020:    // (re)start all worker threads
                        {
                            kill_all_threads();
                            stop_message[0] = 1;    // also kill command listening thread
                            sleep(1);
                            t = 0L;
                            rc = pthread_create(&threads[t], NULL, listenForCommandsThread,(void *)t);
                            start_all_threads();
                            queue_cmd_proc_ack_tmpacket( 1 );
                        }
                        break;
                    default:
                        {
                            long t = 10L;
                            int rc;
                            thread_data.thread_id = t;
                            rc = pthread_create(&threads[t],NULL, commandHandlerThread,(void *) &thread_data);
                            if ((skip[t] = (rc != 0))) {
                                printf("ERROR; return code from pthread_create() is %d\n", rc);
                            };
                        }
                } //switch
            } else printf("Not intended recipient of this command\n");
        }
    }

    /* Last thing that main() should do */
    printf("Quitting and cleaning up.\n");
    //    kill_all_threads();
    /* wait for threads to finish */
    kill_all_threads();
    sleep(2);
    for(int i = 0; i < NUM_THREADS; i++ ){
        if (!skip[i]) {
            printf("Quitting thread %i, quitting status is %i\n", i, pthread_cancel(threads[i]));
        }
    }
    pthread_mutex_destroy(&mutexImage);
    pthread_mutex_destroy(&mutexProcess);
    pthread_exit(NULL);

    return 0;
}

