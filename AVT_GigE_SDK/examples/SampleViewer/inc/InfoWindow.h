/*
| ==============================================================================
| Copyright (C) 2006-2011 Allied Vision Technologies.  All Rights Reserved.
|
| This code may be used in part, or in whole for your application developmen
|
|==============================================================================
|
| File:         InfoWindow.h
|
| Project/lib:  Linux Sample Viewer
|
| Target:       
|
| Description:  Define the camera info window
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

//===== INCLUDE FILES =========================================================

#include <PvApi.h>
#include <ChildWindow.h>

//===== CLASS DEFINITION ======================================================

/*
 * Class:    CMainWindow
 * Purpose:  Derive the standard frame class to create the main window of the
 *           application
 * Comments: none
 */
class CInfoWindow : public PChildWindow
{
    public: // cons./des.

        /*
         * Method:    CInfoWindow()
         * Purpose:   constructor
         * Arguments:
         *
         * [i] PChildWindowObserver& aObserver, observer
         * [i] const tPvCameraInfoEx& aInfo,      camera's information
         *
         * Return:    none
         * Comments:  none
         */ 
        CInfoWindow(PChildWindowObserver& aObserver,const tPvCameraInfoEx& aInfo);

         /*
         * Method:    ~CInfoWindow()
         * Purpose:   destructor
         * Arguments: none
         * Return:    none
         * Comments:  none
         */        
        ~CInfoWindow();

    public: // callbacks

        DECLARE_EVENT_TABLE()

    protected:
            
};
