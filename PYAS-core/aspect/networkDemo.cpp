#define NUM_THREADS 3

#include <stdio.h>      /* for printf() and fprintf() */
#include <pthread.h>    /* for multithreading */
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for sleep()  */

#include "UDPSender.hpp"
#include "Packet.hpp"

int stop_message[NUM_THREADS];

void *sendTelemetryThread(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);
    TelemetrySender *telSender;
    char *ip = "192.168.1.2";

    telSender = new TelemetrySender( ip, (unsigned short) 5000);
    
	while(1)    // run forever
	{
	    sleep(1);
	    // create packet
	    // send packet
	
	    //Telemetry packet from SAS containing an array
        uint8_t image[5] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
        TelemetryPacket tp2(0x70, 0x30);
        tp2 << (uint32_t)0xEFBEADDE;
        tp2.append_bytes(image, 5);
        //std::cout << tp2 << std::endl;
        telSender->send(&tp2);

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
	while(1)    // run forever
	{
	    // listen for packet
	    // parse packet
	    // send out
	    
	    if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
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
    char *ip = "192.168.1.1";

    comSender = new CommandSender( ip, (unsigned short) 5000);
 
	while(1)    // run forever
	{
        sleep(1);
        CommandPacket cp(0x01, 100);
        cp << (uint16_t)0x1100;
        comSender->send( &cp );
        	    
	    if (stop_message[tid] == 1){
            printf("thread #%ld exiting\n", tid);
    	    pthread_exit(NULL);
        }
	}

    /* NEVER REACHED */
	return NULL;
}


int main(void)
{
	pthread_t threads[2];
    int rc;
    
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
	    sleep(10);
	    // kill thread 0, 1
	    stop_message[0] = 1;	    
	    stop_message[1] = 1;
	    
	    exit(1);
	} /* never stop */

   /* Last thing that main() should do */
   pthread_exit(NULL);
   
    /* NEVER REACHED */
	return 0;
}

