#define NUM_THREADS 10
#define SAS_TARGET_ID 0x30
#define SAS_TM_TYPE 0x70
#define SAS_IMAGE_TYPE 0x82
#define SAS_SYNC_WORD 0xEB90
#define SAS_CM_ACK_TYPE 0x01
#define CTL_IP_ADDRESS "192.168.1.2"
#define FDR_IP_ADDRESS "192.168.2.4"
#define CTL_CMD_PORT 2000
#define SAS_CMD_PORT 2001
#define TPCPORT_FOR_IMAGE_DATA 2013
#define UDPPORT_FOR_TM 2002
#define SAVE_LOCATION "/mnt/disk2/"
#define SECONDS_AFTER_SAVE 5

// computer variables
#define EC_INDEX 0x6f0
#define EC_DATA 0x6f1

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
uint32_t tm_frame_sequence_number = 0;

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

struct thread_data{
    int  thread_id;
    uint16_t  var;
};
struct thread_data thread_data_array[NUM_THREADS];

sig_atomic_t volatile g_running = 1;

cv::Mat frame;

Aspect aspect;
Transform solarTransform;

cv::Point2f pixelCenter, screenCenter, error;
CoordList limbs, pixelFiducials, screenFiducials;
IndexList ids;
std::vector<float> mapping;

bool staleFrame;
Flag procReady, saveReady;
int runtime = 10;
int exposure = 10000;
timespec frameRate = {0,100000000L};
int cameraReady = 0;

timespec frameTime;
long int frameCount = 0;

int8_t camera_temperature;
signed char cpu_temperature;

void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        g_running = 0;
    }
}

signed char get_cpu_temperature( void )
{
    signed char start;
    outb(0x40, EC_INDEX );
    start = 0x01 | inb( EC_DATA );
    outb( start, EC_DATA );
    outb(0x26, EC_INDEX );
    return inb( EC_DATA );
}

unsigned long get_cpu_voltage( int index )
{

    switch( index )
    {
    case 0:         // +1.05V
        outb(0x21, EC_INDEX );
        return inb( EC_DATA ) * 2000 / 255;
        break;
    case 1:         // +2.5 V
        outb(0x20, EC_INDEX );
        return inb( EC_DATA ) * 3320 / 255;
        break;
    case 2:         // +3.3 V
        outb(0x22, EC_INDEX );
        return inb( EC_DATA ) * 4380 / 255;
        break;
    case 3:         // +5.0 V
        outb(0x23, EC_INDEX );
        return inb( EC_DATA ) * 6640 / 255;
        break;
    case 4:         // +12.0 V
        outb(0x24, EC_INDEX );
        return inb( EC_DATA ) * 1600 / 255;
        break;
    default:
        return -1;
    }      
}

void kill_all_threads( void ){
    for(int i = 0; i < NUM_THREADS; i++ ){
        stop_message[i] = 1;
    }
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
    int hours, minutes, days;
    int failcount = 0;

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
            clock_gettime(CLOCK_REALTIME, &preExposure);
            minutes = preExposure.tv_sec/60;
            hours = minutes/60;
            days = hours/24;
            std::cout << days << " "
                      << hours - 24*days << ":"
                      << minutes - 60*hours << ":"
                      << preExposure.tv_sec - 60*minutes << "."
                      << preExposure.tv_nsec/1000000 << "."
                      << (preExposure.tv_nsec/1000)%1000 << "."
                      << (preExposure.tv_nsec/1000000) << std::endl;

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
            std::cout << timeElapsed.tv_sec << " " << timeElapsed.tv_nsec << "\n";
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

                    if(!aspect.Run())
                    {
                        aspect.GetPixelCrossings(localLimbs);
                        aspect.GetPixelCenter(localPixelCenter);
                        aspect.GetPixelError(localError);
                        aspect.GetPixelFiducials(localPixelFiducials);
                        aspect.GetFiducialIDs(localIds);
                        aspect.GetScreenFiducials(localScreenFiducials);
                        aspect.GetScreenCenter(localScreenCenter);
                        aspect.GetMapping(localMapping);

                        pthread_mutex_lock(&mutexProcess);

                        limbs = localLimbs;
                        pixelCenter = localPixelCenter;
                        error = localError;
                        pixelFiducials = localPixelFiducials;
                        ids = localIds;
                        screenFiducials = localScreenFiducials;
                        screenCenter = localScreenCenter;
                        mapping = localMapping;

                        pthread_mutex_unlock(&mutexProcess);

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
                    else
                    {
                        //std::cout << "Aspect module failed for this frame." << std::endl;
                    }
                }
                else
                {
                    pthread_mutex_unlock(&mutexImage);
                }
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
            TelemetryPacket tp(0x70, 0x30);
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

void *SaveTemperaturesThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("SaveTemperatures thread #%ld!\n", tid);
 
    char stringtemp[80];
    char obsfilespec[100];    
    FILE *file;
    time_t ltime;
    struct tm *times;

	time(&ltime);	
	times = localtime(&ltime);
	strftime(stringtemp,30,"temp_data_%y%m%d_%H%M%S.dat",times);
	strncpy(obsfilespec,stringtemp,128 - 1);
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
            fprintf(file, "%s, %d, %d\n", current_time, camera_temperature, cpu_temperature);
            printf("%s, %d, %d\n", current_time, camera_temperature, cpu_temperature);
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
                    pthread_mutex_unlock(&mutexImage);

                    char stringtemp[80];
                    char obsfilespec[128];
                    time_t ltime;
                    struct tm *times;

                    //Use clock_gettime instead?
                    time(&ltime);
                    times = localtime(&ltime);
                    strftime(stringtemp,40,"%y%m%d_%H%M%S",times);

                    sprintf(obsfilespec, "%simage_%s_%02d.fits", SAVE_LOCATION, stringtemp, (int)localFrameCount);

                    printf("Saving image %s\n", obsfilespec);
                    writeFITSImage(localFrame, obsfilespec);

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

    CoordList localLimbs, localFiducials;
    std::vector<float> localMapping;
    cv::Point2f localCenter, localError;
    
    while(1)    // run forever
    {
        usleep(250000);
        tm_frame_sequence_number++;
        
        TelemetryPacket tp(SAS_TM_TYPE, SAS_TARGET_ID);
        tp << (uint16_t)SAS_SYNC_WORD;     // SAS-1 syncword
        tp << tm_frame_sequence_number;
        tp << command_sequence_number;
        tp << latest_sas_command_key;

        
        if(pthread_mutex_trylock(&mutexProcess) == 0) 
        {
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

        /*
          tp << (double)localCenter.x;
          tp << (double)localCenter.y;

          for(uint8_t i = 0; i < 20; i++){
          if (i < localFiducials.size()) {
          tp << (float) localFiducials[i].x;
          tp << (float) localFiducials[i].y;
          } else {
          tp << (float)0 << (float)0;
          }
          }

          for(uint8_t j = 0; j < 20; j++) {
          if (j < localLimbs.size()) {
          tp << localLimbs[j].x;
          tp << localLimbs[j].y;
          } else {
          tp << (float)0 << (float)0;
          }
          }
        
          tp << (int) camera_temperature;
        */

        //Housekeeping fields, two of them
        tp << (uint16_t)camera_temperature;
        tp << (uint16_t)0x0;

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
        tp << (uint8_t)255; //max
        tp << (uint8_t)0; //min

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

            // add tm ack packet
            TelemetryPacket ack_tp(SAS_CM_ACK_TYPE, SAS_TARGET_ID);
            ack_tp << command_sequence_number;
            tm_packet_queue << ack_tp;
     
            // update the command count
            printf("command sequence number to %i", command_sequence_number);
            
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
        sleep(1);
        
        if( !cm_packet_queue.empty() ){
            CommandPacket cp(0x01, 100);
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

    sleep(0.2);      // delay a little compared to the SenderThread

    while(1)    // run forever
    {
        sleep(1);
        CommandPacket cp(0x01, 100);
        cp << (uint16_t)0x1100;
        cm_packet_queue << cp;

        if (stop_message[tid] == 1){
            printf("sendCTLCommands thread #%ld exiting\n", tid);
            pthread_exit( NULL );
        }
    }

    /* NEVER REACHED */
    return NULL;
}

void *commandHandlerThread(void *threadargs)
{
    long tid;
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadargs;
    tid = (long)my_data->thread_id;
    uint16_t command_key = my_data->var;

    printf("commandHandler thread #%ld!\n", tid);
    printf("Received data 0x%04x\n", command_key);
    
    switch( command_key ){
        case 0x1210:
        {
        	//send_image_to_ground( localFrame );
        	
        	cv::Mat localFrame;
        	TCPSender tcpSndr(FDR_IP_ADDRESS, (unsigned short) TPCPORT_FOR_IMAGE_DATA);
  			tcpSndr.init_connection();
			if (pthread_mutex_trylock(&mutexImage) == 0)
			{ 
				if( !frame.empty() ){ frame.copyTo(localFrame); }
				for( int i = 0; i < 10; i++){ printf("%d\n", localFrame.at<uint8_t>(i,10));}
				pthread_mutex_unlock(&mutexImage);
			}
			if( !localFrame.empty() ){
				int numXpixels = localFrame.rows;
				int numYpixels = localFrame.cols;	
				TelemetryPacket tp(SAS_IMAGE_TYPE, 0x30);
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
					//if ((i % 100) == 0){ printf("sending %d/%d\n", i, num_packets); }
					//printf("%d\n", i);
					TelemetryPacket tp(0x70, 0x30);
					
					for( int j = 0; j < pixels_per_packet; j++){
						x = k % numXpixels;
                                                y = k / numYpixels;
                                                tp << (uint8_t)localFrame.at<uint8_t>(x, y);
						k++;
					}
					tcpSndr.send_packet( &tp );
					//printf("sending %d bytes\n", tp.getLength());
					count++;
				}
			}
			tcpSndr.close_connection();
		}
		break;
        default:
            printf("Unknown command!\n");
    }

    /* NEVER REACHED */
    return NULL;
}

void start_all_threads( void ){
    int rc;
    long t;
 
    pthread_mutex_init(&mutexImage, NULL);
    pthread_mutex_init(&mutexProcess, NULL);
 
    for(int i = 0; i < NUM_THREADS; i++ ){
        skip[i] = true;
        // reset stop message
        stop_message[i] = 0;
    }
    
    // start all threads
    t = 0L;
    rc = pthread_create(&threads[0],NULL, TelemetryPackagerThread,(void *)t);
    if ((skip[0] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 1L;
    rc = pthread_create(&threads[1],NULL, listenForCommandsThread,(void *)t);
    if ((skip[1] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 2L;
    rc = pthread_create(&threads[2],NULL, sendCTLCommandsThread,(void *)t);
    if ((skip[2] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 3L;
    rc = pthread_create(&threads[3],NULL, TelemetrySenderThread,(void *)t);
    if ((skip[3] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 4L;
    rc = pthread_create(&threads[4],NULL, CommandSenderThread,(void *)t);
    if ((skip[4] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 5L;
    rc = pthread_create(&threads[5],NULL, CameraStreamThread,(void *)t);
    if ((skip[5] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 6L;
    rc = pthread_create(&threads[6],NULL, ImageProcessThread,(void *)t);
    if ((skip[6] = (rc != 0))) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 7L;
    rc = pthread_create(&threads[7],NULL, SaveImageThread,(void *)t);
    if ((skip[7] = (rc != 0))) {
	printf("ERROR; return code from pthread_create() is %d\n", rc);
    }    
    t = 8L;
    rc = pthread_create(&threads[8],NULL, SaveTemperaturesThread,(void *)t);
    if ((skip[8] = (rc != 0))) {
	printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    //Thread #9 is for the commandHandler
}



int main(void)
{  
    // to catch a Ctrl-C and clean up
    signal(SIGINT, &sig_handler);
    
    recvd_command_queue = CommandQueue();
    tm_packet_queue = TelemetryPacketQueue();
    cm_packet_queue = CommandPacketQueue();
    
    /* Create worker threads */
    printf("In main: creating threads\n");

    start_all_threads();

    while(g_running){
        // check if new command have been added to command queue and service them
        if (!recvd_command_queue.empty()){
            printf("size of queue: %zu\n", recvd_command_queue.size());
            Command command;
            command = Command();
            recvd_command_queue >> command;

            latest_sas_command_key = command.get_sas_command();
            printf("sas command key: %X\n", (uint16_t) latest_sas_command_key);
            
            switch( latest_sas_command_key ){
                case 0x1000:     // test, do nothing
                    break;
                case 0x1010:    // kill all worker threads
                    kill_all_threads();
                    break;
                case 0x1020:    // (re)start all worker threads
                    // kill them all just in case
                    kill_all_threads();
                    sleep(1);
                    start_all_threads();
                    break;
                default:
                    long t = 9L;
                    int rc;
                    thread_data_array[t].thread_id = t;
                    thread_data_array[t].var = latest_sas_command_key;
                    //thread_data_array[t].message = messages[t];
                    rc = pthread_create(&threads[t],NULL, commandHandlerThread,(void *) &thread_data_array[t]);
                    if ((skip[9] = (rc != 0))) {
                        printf("ERROR; return code from pthread_create() is %d\n", rc);
                    };
            }
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

