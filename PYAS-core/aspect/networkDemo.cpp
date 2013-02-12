#define NUM_THREADS 4

#include <stdio.h>      /* for printf() and fprintf() */
#include <pthread.h>    /* for multithreading */
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for sleep()  */

#include "UDPSender.hpp"
#include "UDPReceiver.hpp"
#include "Command.hpp"
#include "Telemetry.hpp"

unsigned int stop_message[NUM_THREADS];
uint16_t command_count = 0;
uint16_t latest_sas_command_key = 0x0000;
uint32_t tm_frame_sequence_number = 0;

// loopback IP, just talking to myself
char ip[] = "127.0.0.1";

CommandQueue *recvd_command_queue;
TelemetryPacketQueue *tm_packet_queue;
CommandPacketQueue *cm_packet_queue;

void *TelemetrySenderThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);

    TelemetrySender *telSender;
    telSender = new TelemetrySender( ip, (unsigned short) 5002);
  
    while(1)    // run forever
	{
	    sleep(1);
	    
        if( !tm_packet_queue.empty() ){
            TelemetryPacket *tp
            tm_packet_queue >> tp;
            telSender->send( &tp );
        }
        
        if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
    	    pthread_exit(NULL);
    	    free telSender;
    	}
}

void *TelemetryPackagerThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);
    
    sleep(0.2)      // delay a little compared to the TelemetrySenderThread
    
	while(1)    // run forever
	{
	    sleep(1);
	    tm_frame_sequence_number++;
	    
	    //Telemetry packet from SAS containing an array
        TelemetryPacket tp(0x70, 0x30);
        tp << (uint16_t)0xEB90;     // SAS-1 syncword
        tp << tm_frame_sequence_number;
        tp << command_count;
        tp << latest_sas_command_key;
        
        //add telemetry packet to the queue
        tm_packet_queue << tp;
        
        std::cout << "TelemetryPackagerThread:" << tp << std::endl;
        
	    if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
    	    pthread_exit(NULL);
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
    CommandReceiver *comReceiver;
    comReceiver = new CommandReceiver( (unsigned short) 5001);
    comReceiver->init_connection();
        
	while(1)    // run forever
	{
	    unsigned int packet_length;
	    
	    packet_length = comReceiver->listen( );
	    printf("listenForCommandsThread: %i\n", packet_length);
	    uint8_t *packet;
	    packet = new uint8_t[packet_length];
	    comReceiver->get_packet( packet );
	    
	    CommandPacket *command_packet;
	    command_packet = new CommandPacket( packet, packet_length );
	    
	    if (command_packet->valid()){
	        printf("listenForCommandsThread: good command packet\n");
	        
            // TODO: Send out correct telemetry received packet!
            // the packet below is not correct
            CommandPacket cp(0x01, 101);
            cp << (uint16_t)0x1100;
            command_packet_queue << cp;
            
            Command cm1(0x10ff, 0x0001);
    
            CommandPacket cp(0x30, 0x0001);
            cp << cm1;

            // update the command count
            command_count++;
            printf("command count to %i", command_count);
            
            try { recvd_command_queue->add_packet(*command_packet); } 
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
	        
	    } else {
	        printf("listenForCommandsThread: bad command packet\n");
	    }
	    
	    if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
            comReceiver->close_connection();
            free command_packet;
            free comSender;
            free comReceiver;
    	    pthread_exit(NULL);
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

    CommandSender *comSender;
    comSender = new CommandSender( ip, (unsigned short) 5000);

	while(1)    // run forever
	{
	    sleep(1);
	    
	    if( !cm_packet_queue.empty() ){
            CommandPacket *cp
            command_packet_queue >> cp;
            comSender->send( &cp );
        }
	 
        if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
            comSender->close_connection();
            free comSender;
    	    pthread_exit(NULL);
        }
	}
      
}

void *sendCTLCommandsThread( void *threadid )
{  
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);

    sleep(0.2)      // delay a little compared to the SenderThread

	while(1)    // run forever
	{
        CommandPacket cp(0x01, 100);
        cp << (uint16_t)0x1100;
        std::cout << "sendCTLCommands:" << cp << std::endl;
        comSender->send( &cp );
        	    
	    if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
            comSender->close_connection();
    	    pthread_exit(NULL);
        }
	}

    /* NEVER REACHED */
	return NULL;
}

int main(void)
{
	pthread_t threads[NUM_THREADS];
    int rc;
    
    recvd_command_queue = new CommandQueue;
    tm_packet_queue = new TelemetryPacketQueue;
    cm_packet_queue = new CommandPacketQueue;
    
	/* Create worker threads */
	printf("In main: creating threads\n");
	long t;
	
	start_all_threads();
	
	while(1){

        // check if new command have been added to command queue and service them
        if (!recvd_command_queue->empty()){
            printf("size of queue: %zu\n", recvd_command_queue->size());
            Command *command;
            command = new Command;
            *recvd_command_queue >> *command;
            //recvd_command_queue->pop_front();

            latest_sas_command_key = command->get_sas_command();
            printf("sas command key: %X\n", (uint16_t) latest_sas_command_key);
            
            switch( latest_sas_command_key ){
                case 0x0100:     // test, do nothing
                    break;
                case 0x0101:    // kill all worker threads
                    for(int i = 0; i < NUM_THREADS; i++ ){
                        stop_message[i] = 1;
                    }
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
        
	} /* never stop */

   /* Last thing that main() should do */
   pthread_exit(NULL);
   
    /* NEVER REACHED */
	return 0;
}

void kill_all_threads( void ){
    // kill them all just in case
    for(int i = 0; i < NUM_THREADS; i++ ){
        stop_message[i] = 1;
    }
}

void start_all_threads( void ){
    // reset stop message
    for(int i = 0; i < NUM_THREADS; i++ ){
        stop_message[i] = 0;
    }
    // restart them all
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
    rc = pthread_create(&threads[2],NULL, TelemetrySenderThread,(void *)t);
    if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    t = 4L;
    rc = pthread_create(&threads[2],NULL, CommandSenderThread,(void *)t);
    if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
    }
    
}