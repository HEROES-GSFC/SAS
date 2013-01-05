Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.

========================
Qt SDK
========================

The eBUS SDK is build using the Qt SDK 4.6.2 distributed with 
Red Hat Enterprise Linux Workstation release 6.1 (Santiago).

The sample Makefile is assuming that the development package is installed and that the
qmake location is in the system PATH.

========================
OpenCV SDK
========================

Some eBUS SDK samples are using the OpenCV SDK to manipulated images and has an example 
of 3rd party SDK. 

To compile these sample, the user will have to install the OpenCV SDK 2.3.1a (or adapt 
the sample to any other versions). The OpenCV SDK will also require the user to install 
the latest version of CMake (validation done using cmake version 2.8.7). Optionally, the 
package ffmpeg must be installed to use the sample PvTransmitVideoSample.

CMake 2.8.7 can be download from:
    http://www.cmake.org/cmake/resources/software.html
In a terminal, type the following commands:
    cd [ folder ] (folder is the location of the source code)
    ./bootstrap  
    make 
To complete the installation, run the following command as root (or sudoer)
    make install

If you want to use the PvTransmitVideoSample, you will need to install the ffmpeg package
before compiling the OpenCV SDK.
ffmpeg 0.10.3 can be downloaded from:
    http://www.ffmpeg.org/download.html
Uncompress the package in the folder of your choice
In a terminal, type the following commands:
    cd [ folder ] (folder is the location of the source code)
    ./configure --enable-shared
 --disable-yasm
    make 
To complete the installation, run the following command as root (or sudoer)
    make install
Ensure that you define the environmental variable so that when compiling OpenCV after, the 
ffmpeg package is located and used:
    export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig



OpenCV SDK 2.3.1a can be download from:
    http://sourceforge.net/projects/opencvlibrary/
In a terminal, type the following commands
    cd [ folder ] (folder is the location of the source code containing INSTALL, CMakeLists.txt etc.)
    mkdir release
    cd release
    cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_PYTHON_SUPPORT=ON ..
To complete the installation, run the following command as root (or sudoer)
    make install




