#define _x64
#define _LINUX
#define PVDECL

#include <PvApi.h>

typedef struct
{
    unsigned long UID;
    tPvHandle Handle;
    tPvFrame Frame;
    tPvUint32 Counter;

} tCamera;

void Sleep(unsigned int);
void WaitForCamera();
bool CameraGet(tCamera*);
bool CameraSetup(tCamera*, tPvUint32 &width, tPvUint32 &height);
bool CameraStart(tCamera*);
void CameraStop(tCamera*);
void CameraUnsetup(tCamera*);
int CameraSnap(tCamera*);
