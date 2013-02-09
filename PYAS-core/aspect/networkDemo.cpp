#define NUM_THREADS 3

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

char ip[] = "192.168.10.254";

CommandQueue *recvd_command_queue;

void *sendTelemetryThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);
    TelemetrySender *telSender;
    //char ip[] = "192.168.1.114";
    
    telSender = new TelemetrySender( ip, (unsigned short) 5000);
    
	while(1)    // run forever
	{
	    sleep(1);
	
	    tm_frame_sequence_number++;
	    //Telemetry packet from SAS containing an array
        //uint8_t image[5] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
        TelemetryPacket tp(0x70, 0x30);
        tp << (uint16_t)0xEB90;     // SAS-1 syncword
        tp << tm_frame_sequence_number;
        tp << command_count;
        tp << latest_sas_command_key;
        
        //tp.append_bytes(image, 5);
        //std::cout << tp2 << std::endl;
        telSender->send(&tp);

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
    comReceiver = new CommandReceiver( (unsigned short) 5000);
    comReceiver->init_connection();
    
    // send respond as soon as a good command is received
    // this thread needs its own command sender
    CommandSender *comSender;
    //char fdr_ip[] = "192.168.1.114";
    comSender = new CommandSender( ip, (unsigned short) 5000);
    
	while(1)    // run forever
	{
	    unsigned int packet_length;
	    //packet_length = new unsigned int;
	    
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
	        comSender->init_connection();
            CommandPacket cp(0x01, 101);
            cp << (uint16_t)0x1100;
            comSender->send( &cp );
            comSender->close_connection();
    
            // update the command count
            command_count++;
            
            try { recvd_command_queue->add_packet(*command_packet); } 
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
	        
	    } else {
	        printf("listenForCommandsThread: bad command packet\n");
	    }
	    //for(unsigned int i = 0; i < packet_length; i++){
	    //    printf("%d\n", packet[i]);
	    //}
	    
	    // listen for packet
	    // parse packet

	    
	    if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
            comReceiver->close_connection();
    	    pthread_exit(NULL);
        }
	}

    /* NEVER REACHED */
	return NULL;
}

void *sendCTLCommands(void *threadid)
{  
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);
    
    CommandSender *comSender;
    //char ip[] = "192.168.1.114";

    comSender = new CommandSender( ip, (unsigned short) 5000);
 
	while(1)    // run forever
	{
        sleep(1);
        CommandPacket cp(0x01, 100);
        cp << (uint16_t)0x1100;
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
    
	/* Create worker threads */
	printf("In main: creating threads\n");
	long t;
	
	t = 0L;
	rc = pthread_create(&threads[0],NULL, sendTelemetryThread,(void *)t);
	if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
    }
    t = 1L;
	rc = pthread_create(&threads[1],NULL, listenForCommandsThread,(void *)t);
	if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
    }
    t = 2L;
	rc = pthread_create(&threads[2],NULL, sendCTLCommands,(void *)t);
	if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
    }

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
                    for(int i = 0; i < NUM_THREADS; i++ ){
                        stop_message[i] = 1;
                    }
                    sleep(1);
                    // reset stop message
                    for(int i = 0; i < NUM_THREADS; i++ ){
                        stop_message[i] = 0;
                    }
                    // restart them all
                    t = 0L;
                    rc = pthread_create(&threads[0],NULL, sendTelemetryThread,(void *)t);
                    if (rc){
                         printf("ERROR; return code from pthread_create() is %d\n", rc);
                    }
                    t = 1L;
                    rc = pthread_create(&threads[1],NULL, listenForCommandsThread,(void *)t);
                    if (rc){
                         printf("ERROR; return code from pthread_create() is %d\n", rc);
                    }
                    t = 2L;
                    rc = pthread_create(&threads[2],NULL, sendCTLCommands,(void *)t);
                    if (rc){
                         printf("ERROR; return code from pthread_create() is %d\n", rc);
                    }
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

