/*
| ==============================================================================
| Copyright (C) 2006-2011 Allied Vision Technologies.  All Rights Reserved.
|
| This code may be used in part, or in whole for your application development.
|==============================================================================
|
| StreamPnp
|
| Based on Stream example. Stream "plug and play".
| 
| - Registers ePvLinkAdd and ePvLinkRemove callbacks, which execute on 
|   seperate thread when camera first recognized and unplugged. Frees up main
|   thread from spinning on camera discovery (Stream example).
| - Add callback inits camera, starts camera streaming, spawns new thread 
|   to display stats, and runs to completion.
| - Frame callback requeues frames only.
| - Main thread and spawned thread wait on CTRL+C break. Main thread stops cam
|   and uninits.
|
|==============================================================================
|
| THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
| WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
| NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
| DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
| INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
| LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
| OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF
| LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
| NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
| EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
|
|==============================================================================
*/

#ifdef _WINDOWS
#include "StdAfx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Winsock2.h>
#endif

#if defined(_LINUX) || defined(_QNX) || defined(_OSX)
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/times.h>
#include <arpa/inet.h>
#endif

#include <PvApi.h>

#ifdef _WINDOWS
#define _STDCALL __stdcall
#else
#define _STDCALL
#define TRUE     0
#endif

#define FRAMESCOUNT 10

typedef struct 
{
    unsigned long   UID;
    tPvHandle       Handle;
    tPvFrame        Frames[FRAMESCOUNT];
#ifdef _WINDOWS
    HANDLE          ThHandle;
    DWORD           ThId;
#else
    pthread_t       ThHandle;
#endif    
    bool            Abort;
    
} tCamera;


// global camera data
tCamera         GCamera;

#if defined(_LINUX) || defined(_QNX) || defined(_OSX)
struct tms      gTMS;
unsigned long   gT00 = times(&gTMS);

void Sleep(unsigned int time)
{
    struct timespec t,r;
    
    t.tv_sec    = time / 1000;
    t.tv_nsec   = (time % 1000) * 1000000;    
    
    while(nanosleep(&t,&r)==-1)
        t = r;
}

void SetConsoleCtrlHandler(void (*func)(int), int junk)
{
    signal(SIGINT, func);
}
#endif

//Thread start function. Displays camera stats.
#ifdef _WINDOWS
unsigned long __stdcall ThreadFunc(void *pContext)
#else
void *ThreadFunc(void *pContext)
#endif
{
    unsigned long FrameCompleted, FrameDropped, PacketReceived, PacketMissed;
    float FrameRate;
    tPvErr Err;
    
    //StatFramesCompleted increments when a queued frame returns with tPvFrame.Status = ePvErrSuccess
	//StatFramesDropped increments when a queued frame returns with tPvFrame.Status = ePvErrDataMissing
	//In a situation where a camera returns a frame, but there is no frame queued for it, THESE
	//STATS DO NOT INCREMENT. tPvFrame.FrameCount increments regardless of host queuing, and is a better measure
	//for what frame is being returned from the camera. See CameraStart, where we check this parameter,
	//for the case where a frame is returned from camera with no frame queued on host.
	while(!GCamera.Abort &&
          ((Err = PvAttrUint32Get(GCamera.Handle,"StatFramesCompleted",&FrameCompleted)) == ePvErrSuccess) &&
          ((Err = PvAttrUint32Get(GCamera.Handle,"StatFramesDropped",&FrameDropped)) == ePvErrSuccess) &&
		  ((Err = PvAttrUint32Get(GCamera.Handle,"StatPacketsMissed",&PacketMissed)) == ePvErrSuccess) &&
		  ((Err = PvAttrUint32Get(GCamera.Handle,"StatPacketsReceived",&PacketReceived)) == ePvErrSuccess) &&
          ((Err = PvAttrFloat32Get(GCamera.Handle,"StatFrameRate",&FrameRate)) == ePvErrSuccess))
    {
        printf("FrmCmp : %5lu  FrmDrp : %5lu PckCmp : %5lu PckMss : %5lu FrmRt : %5.2f\r", FrameCompleted, FrameDropped, PacketReceived, PacketMissed, FrameRate);
		Sleep(20);
	}

    return 0;
}


// Create a thread to display camera stats. This could also be done in the frame callback.
// By spawning a new thread, we're able to display stats independent of frames returning. 
void SpawnThread()
{
#ifdef _WINDOWS	
    GCamera.ThHandle = CreateThread(NULL,NULL,ThreadFunc,&GCamera,NULL,&(GCamera.ThId));
#else
    pthread_create(&GCamera.ThHandle,NULL,ThreadFunc,(void *)&GCamera);
#endif    
}

// wait for the thread to be over
void WaitThread()
{
    #ifdef _WINDOWS		
    WaitForSingleObject(GCamera.ThHandle,INFINITE);
    #else
    pthread_join(GCamera.ThHandle,NULL);
    #endif
}


// wait until ctrl+C abort
void WaitForEver()
{
  while(!GCamera.Abort)
        Sleep(500);
}

// Frame completed callback executes on seperate driver thread.
// One callback thread per camera. If a frame callback function has not 
// completed, and the next frame returns, the next frame's callback function is queued. 
// This situation is best avoided (camera running faster than host can process frames). 
// Spend as little time in this thread as possible and offload processing
// to other threads or save processing until later.
//
// Note: If a camera is unplugged, this callback will not get called until PvCaptureQueueClear.
// i.e. callback with pFrame->Status = ePvErrUnplugged doesn't happen -- so don't rely
// on this as a test for a missing camera. 
void _STDCALL FrameDoneCB(tPvFrame* pFrame)
{ 
	// if frame hasn't been cancelled, requeue frame
    if(pFrame->Status != ePvErrCancelled)
		PvCaptureQueueFrame(GCamera.Handle,pFrame,FrameDoneCB); 
}

// open camera, allocate memory
// return value: true == success, false == fail
bool CameraSetup()
{
    tPvErr errCode;
	bool failed = false;
	unsigned long FrameSize = 0;

	//open camera
	if ((errCode = PvCameraOpen(GCamera.UID,ePvAccessMaster,&(GCamera.Handle))) != ePvErrSuccess)
	{
		if (errCode == ePvErrAccessDenied)
			printf("PvCameraOpen returned ePvErrAccessDenied:\nCamera already open as Master, or camera wasn't properly closed and still waiting to HeartbeatTimeout.");
		else
			printf("PvCameraOpen err: %u\n", errCode);
		return false;
	}

	// Calculate frame buffer size
    if((errCode = PvAttrUint32Get(GCamera.Handle,"TotalBytesPerFrame",&FrameSize)) != ePvErrSuccess)
	{
		printf("CameraSetup: Get TotalBytesPerFrame err: %u\n", errCode);
		return false;
	}

	// allocate the frame buffers
    for(int i=0;i<FRAMESCOUNT && !failed;i++)
    {
        GCamera.Frames[i].ImageBuffer = new char[FrameSize];
        if(GCamera.Frames[i].ImageBuffer)
        {
			GCamera.Frames[i].ImageBufferSize = FrameSize;
		}
        else
		{
			printf("CameraSetup: Failed to allocate buffers.\n");
			failed = true;
		}
    }

	return !failed;
}

// close camera, free memory.
void CameraUnsetup()
{
    tPvErr errCode;
	
    if((errCode = PvCameraClose(GCamera.Handle)) != ePvErrSuccess)
	{
		printf("CameraUnSetup: PvCameraClose err: %u\n", errCode);
	}
	else
	{
		printf("Camera closed.\n");
	}
	// delete image buffers
    for(int i=0;i<FRAMESCOUNT;i++)
        delete [] (char*)GCamera.Frames[i].ImageBuffer;

    GCamera.Handle = NULL;
}


// setup and start streaming
// return value: true == success, false == fail
bool CameraStart()
{
    tPvErr errCode;
	bool failed = false;

    // NOTE: This call sets camera PacketSize to largest sized test packet, up to 8228, that doesn't fail
	// on network card. Some MS VISTA network card drivers become unresponsive if test packet fails. 
	// Use PvUint32Set(handle, "PacketSize", MaxAllowablePacketSize) instead. See network card properties
	// for max allowable PacketSize/MTU/JumboFrameSize. 
	if((errCode = PvCaptureAdjustPacketSize(GCamera.Handle,8228)) != ePvErrSuccess)
	{
		printf("CameraStart: PvCaptureAdjustPacketSize err: %u\n", errCode);
		return false;
	}

    // start driver capture stream 
	if((errCode = PvCaptureStart(GCamera.Handle)) != ePvErrSuccess)
	{
		printf("CameraStart: PvCaptureStart err: %u\n", errCode);
		return false;
	}

    // queue frames with FrameDoneCB callback function. Each frame can use a unique callback function
	// or, as in this case, the same callback function.
	for(int i=0;i<FRAMESCOUNT && !failed;i++)
	{           
		if((errCode = PvCaptureQueueFrame(GCamera.Handle,&(GCamera.Frames[i]),FrameDoneCB)) != ePvErrSuccess)
		{
			printf("CameraStart: PvCaptureQueueFrame err: %u\n", errCode);
			failed = true;
		}
	}

	if (failed)
		return false;
		
	// set the camera in freerun trigger, continuous mode, and start camera receiving triggers
	if((PvAttrEnumSet(GCamera.Handle,"FrameStartTriggerMode","Freerun") != ePvErrSuccess) ||
		(PvAttrEnumSet(GCamera.Handle,"AcquisitionMode","Continuous") != ePvErrSuccess) ||
		(PvCommandRun(GCamera.Handle,"AcquisitionStart") != ePvErrSuccess))
	{		
		printf("CameraStart: failed to set camera attributes\n");
		return false;
	}	

	return true;
}


// stop streaming
void CameraStop()
{
    tPvErr errCode;
	
	//stop camera receiving triggers
	if ((errCode = PvCommandRun(GCamera.Handle,"AcquisitionStop")) != ePvErrSuccess)
		printf("AcquisitionStop command err: %u\n", errCode);
	else
		printf("\nAcquisitionStop success.\n");
    
	//PvCaptureQueueClear aborts any actively written frame with Frame.Status = ePvErrDataMissing
	//Further queued frames returned with Frame.Status = ePvErrCancelled
	
	//Add delay between AcquisitionStop and PvCaptureQueueClear
	//to give actively written frame time to complete
	Sleep(200);
	
	printf("Calling PvCaptureQueueClear...\n");
	if ((errCode = PvCaptureQueueClear(GCamera.Handle)) != ePvErrSuccess)
		printf("PvCaptureQueueClear err: %u\n", errCode);
	else
		printf("...Queue cleared.\n");  

	//stop driver stream
	if ((errCode = PvCaptureEnd(GCamera.Handle)) != ePvErrSuccess)
		printf("PvCaptureEnd err: %u\n", errCode);
	else
		printf("Driver stream stopped.\n");
}

// CTRL-C handler
#ifdef _WINDOWS
BOOL WINAPI CtrlCHandler(DWORD dwCtrlType)
#else
void CtrlCHandler(int Signo)
#endif	
{  
    GCamera.Abort = true;    
        
    #ifndef _WINDOWS
    signal(SIGINT, CtrlCHandler);
    #else
    return true;
    #endif
}

//Setup camera to stream. Spawn a thread to display stats.
void HandleCameraPlugged(unsigned long UniqueId)
{
    if(!GCamera.UID && !GCamera.Abort)
    {
        GCamera.UID = UniqueId;    
      
        if(CameraSetup())
        {
            printf("Camera %lu opened\n",UniqueId);   
            
            // start streaming from the camera
            if(CameraStart())
            {
                //create a thread to display camera stats. 
                SpawnThread();                    
            }
			else
			{
				//failure. signal main thread to abort
				GCamera.Abort = true;
			}
        }
        else
		{
            //failure. signal main thread to abort
			GCamera.Abort = true;
		}
    }  
}

void HandleCameraUnplugged(unsigned long UniqueId)
{
    if(GCamera.UID == UniqueId)
    {    
		//signal main thread to abort
		GCamera.Abort = true;
    }    
}

// callback function called on seperate thread when a registered camera event received
void _STDCALL CameraEventCB(void* Context,
                             tPvInterface Interface,
                             tPvLinkEvent Event,
                             unsigned long UniqueId)
{
    switch(Event)
    {
        case ePvLinkAdd:
        {
			printf("Event: camera %lu recognized\n",UniqueId);
            HandleCameraPlugged(UniqueId);
            break;
        }
        case ePvLinkRemove:
        {
			printf("\nEvent: camera %lu unplugged\n",UniqueId);
            HandleCameraUnplugged(UniqueId);
                       
            break;
        }
        default:
            break;
    }
}

int main(int argc, char* argv[])
{
    tPvErr errCode;
	
	// initialize the PvAPI
	if((errCode = PvInitialize()) != ePvErrSuccess)
		printf("PvInitialize err: %u\n", errCode);
	else
	{
        //IMPORTANT: Initialize camera structure. See tPvFrame in PvApi.h for more info.
		memset(&GCamera,0,sizeof(tCamera));

        // set the CTRL-C handler
        SetConsoleCtrlHandler(&CtrlCHandler, TRUE);
        
		printf("Waiting for camera discovery...\n");

        // register camera plugged in callback
        if((errCode = PvLinkCallbackRegister(CameraEventCB,ePvLinkAdd,NULL)) != ePvErrSuccess)
			printf("PvLinkCallbackRegister err: %u\n", errCode);

        // register camera unplugged callback
        if((errCode = PvLinkCallbackRegister(CameraEventCB,ePvLinkRemove,NULL)) != ePvErrSuccess)
			printf("PvLinkCallbackRegister err: %u\n", errCode);
		
        // wait until ctrl+c break
		printf("Ctrl+C to break.\n");
		WaitForEver();
        
        CameraStop();
        CameraUnsetup();          
                
        // If thread spawned (see HandleCameraPlugged), wait to finish
        if(GCamera.ThHandle)
            WaitThread(); 
        
        if((errCode = PvLinkCallbackUnRegister(CameraEventCB,ePvLinkAdd)) != ePvErrSuccess)
			printf("PvLinkCallbackUnRegister err: %u\n", errCode);
        if((errCode = PvLinkCallbackUnRegister(CameraEventCB,ePvLinkRemove)) != ePvErrSuccess)
			printf("PvLinkCallbackUnRegister err: %u\n", errCode);       
               
        // uninitialize the API
        PvUnInitialize();
    }

	return 0;
}
