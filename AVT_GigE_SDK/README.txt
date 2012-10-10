###
### AVT PvAPI GigE SDK 1.26 for Linux
### README
###
### 10/27/2011
###

*************************************************************************
* Notes
*************************************************************************

* PvAPI use some signals internally. It is required to protect some of your system calls (such as sem_wait) from been interrupted. This can be done by calling the system call from within a loop and only exiting from it if there was no error or if errno is different from EINTR.


*************************************************************************
* Compilation
*************************************************************************

* This distribution supports x86, PPC, ARM, and x64 (AMD64) Linux (2.6.X). Modify the "ARCH" file located in the Examples directory before building the samples to select the proper architecture: x86, ppc, arm or x64.

* If you need this driver for any specific platform, contact support@alliedvisiontec.com (e.g. armbe soft-float GCC 4.1, uCLinux ...).

* The shared library in ./bin-pc is compiled with GCC 4.5

* Static libraries are provided for GCC 4.1, 4.3, 4.4 and 4.5 they can be found in ./lib-pc

* Each sample code can be built as follows:

	> make sample ; make install
	
  To compile against a static library rather than the dynamic library, build as follows:
  
	> make sample-static ; make install

  The executables will be copied into the ./bin-pc folder.

*************************************************************************
* SampleViewer Compilation
*************************************************************************

* SampleViewer requires the wxGTK library. Compiled versions of SampleViewer are provided for each architecture, statically compiled against wxGTK. 

* To compile the SampleViewer source code:

  - Install the 'libgtk2.0-dev' package
  - Download wxGTK (>= 2.6) library. http://www.wxwidgets.org/downloads 
    - Compile wxGTK as Unicode. E.g:
	From within wxGTK download directory
	> mkdir static
	> cd static
	> ../configure --enable-shared=no --enable-static=yes --enable-unicode=yes
        > make
  - Edit the SampleViewer makefile to point to WxGTK build location. E.g:
        WX_DIR   = /home/you/Desktop/wxGTK-2.8.12/static	
  - > make sample ; make install


*************************************************************************
* Java
*************************************************************************

* The Java folder (in examples) contains a JNI interface to PvAPI and a set of samples. In the bin-pc folder, you will find a build version of the interface (libPvJNI.so). To import the samples in Eclipse:

    1. Start Eclipse & set workspace to "AVT GigE SDK/examples/Java"
    2. In the "Package Explorer", right click then select "Import". From there expand "General" and select "Existing Projects into Workspace"
       then "Next".
    3. In "Select Root directory" browse to "AVT GigE SDK/examples/Java/PvJPI" then click on "Finish". The project will appears in the "Explorer".
    4. Do the same thing for any of sample code you want, e.g. "JListCameras"
    5. Select the "JListCameras" project in the "Explorer", right click and select "Run as" -> "Java Application".
    6. In the "Select Java Application" window, scroll down to "JListCameras - Prosilica" and select it. Press "Ok". The sample code will launch,
       but fail with a java.lang.unsatisfiedLinkError exception. Terminate the app.
    7. In the "Eclipse" menu "Run", select "Run Configuration ...". Select "JListCameras" and click on the "Arguments" tab and add the following
       string to "VM arguments":
       
       -Djava.library.path="/Full/Path/To/AVT GigE SDK/bin-pc/x64"
       
       Replace Full/Path/To by the actual path to the SDK on your system. If you are on a 32bits system, replace x64 by x86.
    8. In the same window, change the "Working Directory" to "Other" and enter the exact same path as above (no need to have it quoted).
    9. Click on "Apply", then "Run".
    

*************************************************************************
* Network Optimization
*************************************************************************

* Set MTU of the GigE adapter to 9000. Set camera PacketSize = 8228. Whether your adapter can support MTU 9000 will be dependent on the NIC hardware and driver. If the MTU must be set to a lower value, adjust camera PacketSize to match.

    - (Ubuntu) Set MTU in System->Preferences->Network Connections. If 9000 is out of range for the NIC, the setting will automatically decrease to the max allowable.

    - If there is no above option, in terminal: sudo ifconfig eth0 mtu 9000
      where eth0 is the adapter used for camera. This must be done on every system startup. It may be possible to edit the /etc/network/interfaces for permanence. This will vary between distributions. Look online for more info.

* Users experiencing dropped frames or packets should run their application with root privileges. This allows the OS to boost the priority of the Prosilica driver thread.
Running as root also allows the driver to bind directly to the NIC adapter. Doing so shortens the path the packets take within the OS network stack. 
Users who feel that running their application as root compromises their system security may find the following implementation satisfactory:
	-set the executable owner as root
	-set the "setuid" permission bit on the executable
	-in code, when application starts use capset() to release all but these privileges:
		CAP_SYS_NICE
		CAP_NET_ADMIN
		CAP_NET_BROADCAST
		CAP_NET_RAW  
The application will start with all root privileges, but it will drop them immediately after startup.

* In order to use multicasting, you may have to add manually a route. The syntax is as follows:

	> sudo route -n add -net 224.0.0.0 netmask 240.0.0.0 dev eth0

where eth0 is the adapter used for camera. Multicasting will only work if the application is run as root. 
