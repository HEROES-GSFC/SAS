#define NUM_THREADS 7
#define SAS_TARGET_ID 0x30
#define SAS_TM_TYPE 0x70
#define SAS_IMAGE_TYPE 0x82
#define SAS_SYNC_WORD 0xEB90
#define SAS_CM_ACK_TYPE 0x01

// computer variables
#define EC_INDEX 0x6f0
#define EC_DATA 0x6f1

// image variables
#define CHORDS 50
#define THRESHOLD 50 

#define FID_WIDTH 5
#define FID_LENGTH 23
#define SOLAR_RADIUS 105
#define FID_ROW_THRESH 5
#define FID_COL_THRESH 0
#define FID_MATCH_THRESH 5

#define NUM_LOCS 20


#include <stdio.h>      /* for printf() and fprintf() */
#include <pthread.h>    /* for multithreading */
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for sleep()  */
#include <signal.h>     /* for signal() */
#include <math.h>       /* for testing only, remove when done */
#include <sys/io.h>     /* for outb, computer parameters */

#include "UDPSender.hpp"
#include "UDPReceiver.hpp"
#include "Command.hpp"
#include "Telemetry.hpp"

#include <opencv.hpp>
#include <iostream>
#include <string>
#include "ImperxStream.hpp"
#include "processing.hpp"

// global declarations
uint16_t command_sequence_number = 0;
uint16_t latest_sas_command_key = 0x0000;
uint32_t tm_frame_sequence_number = 0;

// loopback IP, just talking to myself
char ip[] = "192.168.2.4";

CommandQueue recvd_command_queue;
TelemetryPacketQueue tm_packet_queue;
CommandPacketQueue cm_packet_queue;

// related to threads
unsigned int stop_message[NUM_THREADS];
pthread_t threads[NUM_THREADS];
pthread_attr_t attr;
pthread_mutex_t mutexImage;

sig_atomic_t volatile g_running = 1;

cv::Mat frame;
cv::Point fiducialLocations[NUM_LOCS];
ImperxStream camera;

int numFiducials;
Semaphore frameReady, frameProcessed;
int runtime = 10;
int exposure = 10000;
int frameRate = 250;
int cameraReady = 0;

double chordOutput[6];

void sig_handler(int signum)
{
  if (signum == SIGINT){
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

    switch( index ){
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

void *CameraStreamThread( void * threadid)
{    
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);

    cv::Mat localFrame;
    int width, height;
    if (camera.Connect() != 0){
	    std::cout << "Error connecting to camera!\n";	
    }
    else{
        // width and height are passed out while exposure is passed in!
        // CRAZY!
        camera.ConfigureSnap(width, height, exposure);
        localFrame.create(height, width, CV_8UC1);
        camera.Initialize();
        cameraReady = 1;
	}
	
	while(1){

        if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
            camera.Stop();
            camera.Disconnect();
            pthread_exit( NULL );
        }
            
	    camera.Snap(localFrame);

        //printf("CameraStreamThread: trying to lock\n");
        pthread_mutex_lock(&mutexImage);
        //printf("CameraStreamThread: got lock, copying over\n");
	    localFrame.copyTo(frame);
        printf("%d\n", frame.at<uint8_t>(0,0));
        pthread_mutex_unlock(&mutexImage);

	    frameReady.increment();
	    fine_wait(0,frameRate - exposure,0,0);
	}
}

void *ImageProcessThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);

    cv::Size frameSize;
    cv::Mat localFrame;

    cv::Mat kernel;
    cv::Mat subImage;
    int height, width;
    cv::Range rowRange, colRange;
    matchKernel(kernel);

    cv::Point localFiducialLocations[NUM_LOCS];
    int localNumFiducials;
    
    while(1)
    {
	    if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
            pthread_exit( NULL );
        }
        
        if (cameraReady){
            while(1){
                try{
                    frameReady.decrement();\
                    break;
                }
                catch(const char* e){
                    fine_wait(0,frameRate/10,0,0);
                }
            }
    
            //printf("ImageProcessThread: trying to lock\n");
            if (pthread_mutex_trylock(&mutexImage) == 0){
                //printf("ImageProcessThread: got lock\n");
                if(!frame.empty()){
                    frame.copyTo(localFrame);
                    //printf("%d\n", localFrame.at<uint8_t>(0,0));
                    frameSize = localFrame.size();
                    height = frameSize.height;
                    width = frameSize.width;
                    //printf("working on chords now\n");
                    chordCenter((const unsigned char*) localFrame.data, height, width, CHORDS, THRESHOLD, chordOutput);

                    //printf("done working on image %d %d\n", chordOutput[0], chordOutput[1]);
                
                    if (chordOutput[0] > 0 && chordOutput[1] > 0 && chordOutput[0] < width && chordOutput[1] < height)
                    {
                        rowRange.end = (((int) chordOutput[1]) + SOLAR_RADIUS < height-1) ? (((int) chordOutput[1]) + SOLAR_RADIUS) : (height-1);
                        rowRange.start = (((int) chordOutput[1]) - SOLAR_RADIUS > 0) ? (((int) chordOutput[1]) - SOLAR_RADIUS) : 0;
                        colRange.end = (((int) chordOutput[0]) + SOLAR_RADIUS < width) ? (((int) chordOutput[0]) + SOLAR_RADIUS) : (width-1);
                        colRange.start = (((int) chordOutput[0]) - SOLAR_RADIUS > 0) ? (((int) chordOutput[0]) - SOLAR_RADIUS) : 0;
                        subImage = localFrame(rowRange, colRange);
                        localNumFiducials = matchFindFiducials(subImage, kernel, FID_MATCH_THRESH, localFiducialLocations, NUM_LOCS);
                    }
                    
                    numFiducials = localNumFiducials;
                    for (int k = 0; k < localNumFiducials; k++)
                    {
                        fiducialLocations[k].x = localFiducialLocations[k].x + colRange.start;
                        fiducialLocations[k].y = localFiducialLocations[k].y + rowRange.start;
                    }
                    
                    frameProcessed.increment();
                }
            pthread_mutex_unlock(&mutexImage);        
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
        sleep(1);
        
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

void *TelemetryPackagerThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);
    
    sleep(1);      // delay a little compared to the TelemetrySenderThread
    
    while(1)    // run forever
    {
        sleep(1);
        tm_frame_sequence_number++;
        
        //Telemetry packet from SAS containing an array
        TelemetryPacket tp(SAS_TM_TYPE, SAS_TARGET_ID);
        tp << (uint16_t)SAS_SYNC_WORD;     // SAS-1 syncword
        tp << tm_frame_sequence_number;
        tp << command_sequence_number;
        tp << latest_sas_command_key;

	    printf("camera temp is %lld\n", camera.getTemperature());

        //printf("cpu temp is %3d\n", get_cpu_temperature());
        //for(int i = 0; i < 5; i++){
        //    printf("voltage is %d V\n", get_cpu_voltage(i));
        //}
        
        tp << chordOutput[0];
        tp << chordOutput[1];
        //for(int i = 0; i < 14; i++){
        //    tp << (uint16_t)chordsX[i];
         //   tp << (uint16_t)chordsY[i];
        //}
        
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

void kill_all_threads( void ){
    for(int i = 0; i < NUM_THREADS; i++ ){
        stop_message[i] = 1;
    }
    
}

void start_all_threads( void ){
    int rc;
    long t;
 
    pthread_mutex_init(&mutexImage, NULL);
 
    // reset stop message
    for(int i = 0; i < NUM_THREADS; i++ ){
        stop_message[i] = 0;
    }
    
    // start all threads
    t = 0L;
    rc = pthread_create(&threads[0],NULL, TelemetryPackagerThread,(void *)t);
    if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
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
    rc = pthread_create(&threads[6],NULL, ImageProcessThread,(void *)t);
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
            //recvd_command_queue.pop_front();

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
                default:        // unknown command!
                    printf("Unknown command!\n");
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
    pthread_exit(NULL);
    
    return 0;
}