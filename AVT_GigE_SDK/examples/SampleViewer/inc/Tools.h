/*
| ==============================================================================
| Copyright (C) 2006-2011 Allied Vision Technologies.  All Rights Reserved.
|
| This code may be used in part, or in whole for your application developmen
|
|==============================================================================
|
| File:         Tools.h
|
| Project/lib:  Linux Sample Viewer
|
| Target:       
|
| Description:  Define some useful functions
|
| Notes:
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

#ifndef _TOOLS_H
#define _TOOLS_H

//===== INCLUDE FILES =========================================================

#include <PvApi.h>

//===== FUNCTION PROTOTYPES ===================================================

/*
 * Function:  GetStringForError()
 * Purpose:   convert a PvAPI error code into a "user friendly" string
 * Arguments:
 *
 * [i] tPvErr aErr, error code
 *
 * Return:    the string
 * Comments:  none
 */       
const char* GetStringForError(tPvErr aErr);

#endif // _TOOLS_H
