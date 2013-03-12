#define NUM_THREADS 9
#define SAS_TARGET_ID 0x30
#define SAS_TM_TYPE 0x70
#define SAS_IMAGE_TYPE 0x82
#define SAS_SYNC_WORD 0xEB90
#define SAS_CM_ACK_TYPE 0x01

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

#include "UDPSender.hpp"
#include "UDPReceiver.hpp"
#include "Command.hpp"
#include "Telemetry.hpp"

#include <opencv.hpp>
#include <iostream>
#include <string>
#include "ImperxStream.hpp"
#include "processing.hpp"
#include "compression.hpp"
#include "utilities.hpp"

// global declarations
uint16_t command_sequence_number = 0;
uint16_t latest_sas_command_key = 0x0000;
uint32_t tm_frame_sequence_number = 0;

char ip[] = "192.168.2.4";

CommandQueue recvd_command_queue;
TelemetryPacketQueue tm_packet_queue;
CommandPacketQueue cm_packet_queue;

// related to threads
unsigned int stop_message[NUM_THREADS];
pthread_t threads[NUM_THREADS];
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
cv::Point2f center, error;
CoordList limbs, fiducials, ids;

Flag procReady, saveReady;
int runtime = 10;
int exposure = 10000;
int frameRate = 250;
int cameraReady = 0;

timespec frameTime;
long int frameCount = 0;

long long camera_temperature;
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
    printf("Hello World! It's me, thread #%ld!\n", tid);

    ImperxStream camera;

    cv::Mat localFrame;
    int width, height;
    while(1)
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
	    camera.SetROISize(960,960);
	    camera.SetROIOffset(165,0);
	    camera.SetExposure(exposure);
	
	    width = camera.GetROIWidth();
	    height = camera.GetROIHeight();
	    localFrame.create(height, width, CV_8UC1);
	    if(camera.Initialize() != 0)
	    {
		std::cout << "Error initializing camera!\n";
		sleep(1);
		continue;
	    }
	    cameraReady = 1;
	    frameCount = 0;
	    break;
	}
    }	
    while(1)
    {

        if (stop_message[tid] == 1)
	{
            printf("thread #%ld exiting\n", tid);
            camera.Stop();
            camera.Disconnect();
            pthread_exit( NULL );
        }
	camera.Snap(localFrame);
	procReady.raise();
	saveReady.raise();

        //printf("CameraStreamThread: trying to lock\n");
        pthread_mutex_lock(&mutexImage);
	clock_gettime(CLOCK_REALTIME, &frameTime);
        //printf("CameraStreamThread: got lock, copying over\n");
	localFrame.copyTo(frame);
        //printf("%d\n", frame.at<uint8_t>(0,0));
	frameCount++;
        pthread_mutex_unlock(&mutexImage);

	//printf("camera temp is %lld\n", camera.getTemperature());
	camera_temperature = camera.getTemperature();
	
	
	procReady.raise();
	saveReady.raise();
	fine_wait(0,frameRate - exposure,0,0);
    }
}

void *ImageProcessThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);

    Aspect aspect;
    
    while(1)
    {
	if (stop_message[tid] == 1)
	{
            printf("thread #%ld exiting\n", tid);
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
		    usleep(1000*frameRate/10);
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
		    
                    pthread_mutex_lock(&mutexProcess);

                    aspect.GetPixelCrossings(limbs);
		    aspect.GetPixelCenter(center);
		    aspect.GetPixelError(error);
		    aspect.GetPixelFiducials(fiducials);
		    aspect.GetFiducialIDs(ids);

        std::cout << ids.size() << " fiducials found:";
        for(int i = 0; i < 20; i++){
            if (i < ids.size()) {
                std::cout << " (" << fiducials[i].x << "," << fiducials[i].y << ")";
            }
        }
        std::cout << std::endl;

        for(int i = 0; i < 20; i++){
            if (i < ids.size()) {
                std::cout << " (" << ids[i].x << "," << ids[i].y << ")";
            }
        }
        std::cout << std::endl;

                    pthread_mutex_unlock(&mutexProcess);
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
    printf("Hello World! It's me, thread #%ld!\n", tid);

    TelemetrySender telSender(ip, (unsigned short) 5002);

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
            printf("thread #%ld exiting\n", tid);
            pthread_exit( NULL );
        }
    }
}

void *SaveTemperaturesThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);
 
    char stringtemp[80];
    char obsfilespec[100];    
    FILE *file;
    time_t ltime;
    struct tm *times;

	time(&ltime);	
	times = localtime(&ltime);
	strftime(stringtemp,25,"temp_data_%y%m%d_%H%M%S.dat",times);
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
                printf("thread #%ld exiting\n", tid);
                fclose(file);
                pthread_exit( NULL );
            }
            sleep(1);
            time(&ltime);
            times = localtime(&ltime);
            strftime(current_time,25,"%y/%m/%d %H:%M:%S",times);
            fprintf(file, "%s, %lld, %lld\n", current_time, camera_temperature, cpu_temperature);
            printf("%s, %lld, %lld\n", current_time, camera_temperature, cpu_temperature);
        }
    }
}

void *SaveImageThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);
    
    cv::Mat localFrame;
    long int localFrameCount;
    char number[6] = "00000";
    std::string fitsfile;

    while(1)
    {
        if (stop_message[tid] == 1)
        {
            printf("thread #%ld exiting\n", tid);
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
		        usleep(1000*frameRate/10);
		    }
	    }
    
        printf("SaveImageThread: trying to lock\n");
        if (pthread_mutex_trylock(&mutexImage) == 0)
	    {
            //printf("ImageProcessThread: got lock\n");
            if(!frame.empty())
		    {
                char stringtemp[80];
                char obsfilespec[100];    
                FILE *file;
                time_t ltime;
                struct tm *times;

                time(&ltime);	
                times = localtime(&ltime);
                strftime(stringtemp,25,"image_%y%m%d_%H%M%S.fits",times);
                strncpy(obsfilespec,stringtemp,128 - 1);
                obsfilespec[128 - 1] = '\0';

                frame.copyTo(localFrame);
                localFrameCount = frameCount;
                pthread_mutex_unlock(&mutexImage); 
                fitsfile = "./test/frame";
                sprintf(number, "%05d", (int) localFrameCount);
                fitsfile += number;
                fitsfile += ".fit";
                std::cout << fitsfile << "\n";
                writeFITSImage(frame, obsfilespec);
                printf("Saving image %s\n", obsfilespec);
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
    printf("Hello World! It's me, thread #%ld!\n", tid);
    
    sleep(1);      // delay a little compared to the TelemetrySenderThread

    CoordList localLimbs, localFiducials;
    cv::Point2f localCenter, localError;
    
    while(1)    // run forever
    {
        usleep(250000);
        tm_frame_sequence_number++;
        
        //Telemetry packet from SAS containing an array
        TelemetryPacket tp(SAS_TM_TYPE, SAS_TARGET_ID);
        tp << (uint16_t)SAS_SYNC_WORD;     // SAS-1 syncword
        tp << tm_frame_sequence_number;
        tp << command_sequence_number;
        tp << latest_sas_command_key;

        //printf("cpu temp is %3d\n", get_cpu_temperature());
        //for(int i = 0; i < 5; i++){
        //    printf("voltage is %d V\n", get_cpu_voltage(i));
        //}
        
        if(pthread_mutex_trylock(&mutexProcess) == 0) 
	{
	    localLimbs = limbs;
	    localCenter = center;
	    localError = error;
	    localFiducials = fiducials;

	    pthread_mutex_unlock(&mutexProcess);
        }

        tp << (double)localCenter.x;
        tp << (double)localCenter.y;

        for(int i = 0; i < 20; i++){
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
        
        //add telemetry packet to the queue
        tm_packet_queue << tp;
                
        if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
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
    printf("Hello World! It's me, thread #%ld!\n", tid);
    CommandReceiver comReceiver( (unsigned short) 5001);
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
            printf("thread #%ld exiting\n", tid);
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
    printf("Hello World! It's me, thread #%ld!\n", tid);

    CommandSender comSender( ip, (unsigned short) 5000);

    while(1)    // run forever
    {
        sleep(1);
        
        if( !cm_packet_queue.empty() ){
            CommandPacket cp(0x01, 100);
            cm_packet_queue >> cp;
            comSender.send( &cp );
            //std::cout << "CommandSender:" << cp << std::endl;
        }

        if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
            comSender.close_connection();
            pthread_exit( NULL );
        }
    }
      
}

void *sendCTLCommandsThread( void *threadid )
{
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);

    sleep(0.2);      // delay a little compared to the SenderThread

    while(1)    // run forever
    {
        sleep(1);
        CommandPacket cp(0x01, 100);
        cp << (uint16_t)0x1100;
        cm_packet_queue << cp;
        	    
        if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
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

    printf("Hello World! It's me, thread #%ld!\n", tid);
    printf("Received data 0x%04x\n", command_key);
    
    switch( latest_sas_command_key ){
        default:
            printf("Unknown command!\n");
    }
    
}

void start_all_threads( void ){
    int rc;
    long t;
 
    pthread_mutex_init(&mutexImage, NULL);
    pthread_mutex_init(&mutexProcess, NULL);
 
    // reset stop message
    for(int i = 0; i < NUM_THREADS; i++ ){
        stop_message[i] = 0;
    }
    
    // start all threads
    t = 0L;
    //rc = pthread_create(&threads[0],NULL, TelemetryPackagerThread,(void *)t);
    //if (rc){
	//printf("ERROR; return code from pthread_create() is %d\n", rc);
    //}
    t = 1L;
    rc = pthread_create(&threads[1],NULL, listenForCommandsThread,(void *)t);
    if (rc){
	printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 2L;
    rc = pthread_create(&threads[2],NULL, sendCTLCommandsThread,(void *)t);
    if (rc){
	printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 3L;
    rc = pthread_create(&threads[3],NULL, TelemetrySenderThread,(void *)t);
    if (rc){
	printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 4L;
    rc = pthread_create(&threads[4],NULL, CommandSenderThread,(void *)t);
    if (rc){
	printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 5L;
    rc = pthread_create(&threads[5],NULL, CameraStreamThread,(void *)t);
    if (rc){
	printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 6L;
    rc = pthread_create(&threads[6],NULL, SaveImageThread,(void *)t);
    if (rc){
	printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 7L;
    rc = pthread_create(&threads[7],NULL, SaveTemperaturesThread,(void *)t);
    if (rc){
	printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    
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
                case 0x0100:     // test, do nothing
                    break;
                case 0x0101:    // kill all worker threads
                    kill_all_threads();
                    break;
                    case 0x0102:    // (re)start all worker threads
                    // kill them all just in case
                    kill_all_threads();
                    sleep(1);
                    start_all_threads();
                    break;
                default:
                    long t = 8L;
                    int rc;
                    thread_data_array[t].thread_id = t;
                    thread_data_array[t].var = latest_sas_command_key;
                    //thread_data_array[t].message = messages[t];
                    rc = pthread_create(&threads[t],NULL, commandHandlerThread,(void *) &thread_data_array[t]);
                    if (rc){
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
        printf("Quitting thread %i, quitting status is %i\n", i, pthread_cancel(threads[i]));
    }
    pthread_mutex_destroy(&mutexImage);
    pthread_mutex_destroy(&mutexProcess);
    pthread_exit(NULL);
    
    return 0;
}

