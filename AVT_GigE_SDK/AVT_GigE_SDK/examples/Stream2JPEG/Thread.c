/*
| ==============================================================================
| Copyright (C) 2010 Prosilica.  All Rights Reserved.
|
| Redistribution of this header file, in original or modified form, without
| prior written consent of Prosilica is prohibited.
|
|==============================================================================
|
| Simple multi-platform thread C-wrapper.
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

#include "Thread.h"

bool spawnThread(tThread* aThread,tThreadFunc aFunction,void* aContext)
{
    #ifdef _WINDOWS
    *aThread = CreateThread(NULL,NULL,aFunction,aContext,NULL,NULL);
    
    return *aThread != NULL;
    #else
    return !pthread_create(aThread,NULL,aFunction,aContext);
    #endif
}

void wait4Thread(tThread* aThread)
{
    #ifdef _WINDOWS		
    WaitForSingleObject(*aThread,INFINITE);
    #else
    pthread_join(*aThread,NULL);
    #endif
}
