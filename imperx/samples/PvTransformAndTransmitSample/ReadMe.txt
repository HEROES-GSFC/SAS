Copyright (c) 2011, Pleora Technologies Inc., All rights reserved.

========================
Using PvTransformAndTransmitSample
========================

This sample code illustrates how to:
* Receive video from a GigE Vision device using PvStream and PvPipeline
* Resample it and print text on it using OpenCV 
* Transmit it using PvTransmitterRaw

1. Introduction

Using the PvSystem and PvInterface class, this sample enumerates available interfaces (network adapters) and selects the first interface with a valid IP address to transmit from.
Using the PvVirtualDevice class, it listens for device discovery requests so that it can be detected by receiving applications.
Using the PvBuffer class, it shows how to allocate a set of buffers with a given width, height, and pixel format for use in receiving and transmitting video.
Using PvDeviceFinderWnd, the user can select a device to receive from.
Using PvStream and PvPipeline, images are received from a GigE Vision device on channel 0.
Using PvBufferConverter, the incoming video is converted into an RGB format.
Using OpenCV, the video file is resized to the desired width and height.
Using OpenCV, text is printed onto the resampled video.
Using the PvTransmitterRaw class, the transformed image is continuously transmitted to the specified destination.

By default, this sample prompts for a GigE Vision device to receive from, begins receiving from that device, resamples the video to RGB24 640X480, prints some statistics on it, and finally transmits it from the first valid interface to multicast address 239.192.1.1:1042. However, it can be used in many different ways by providing it with optional command line arguments. For further details on the command line options that are available, run the sample with the argument --help.

Procedure for receiving video from a GigE Vision transmitter, transforming it and finally multicasting to a GigE Vision receiver:
 * Ensure that the GigE Vision transmitter you wish to receive from is reachable from the PC that will be running this sample.
 * Ensure that the GigE Vision receiver you wish to transmit to is reachable from the PC that will be running this sample. 
 * Ideally, you have two different interfaces so that you can receive with one adapter and transmit with another (however if the incoming and outgoing throughput is low enough, it is possible to transmit and receive on the same interface).
 * If you are using more than one interface out of the same PC, it is recommended that they be on different subnets.
 * If only one network interface is installed and connected with a valid IP address on your PC, start the sample with default options (no command line arguments). Otherwise, note the IP address of the interface you wish to transmit from and add the following command line argument:
    * --sourceaddress=<the IP address of the NIC you wish to transmit from>
 * When prompted, select the device you wish to receive from.
 * Press any key when you are ready to begin transmitting (once the receiver is ready to receive the image stream).
 * By default, a vDisplay should be able to receive the stream "out of the box" since it subscribes to the same multicast address and port by default.
 * To receive the test pattern with GEVPlayer:
    * Start GEVPlayer
    * Click Tools->Setup
    * Under "GEVPlayer Role", select "Data receiver"
    * Under "Stream Destination", select "Multicast"
    * Click OK
    * Click Select/Connect.
    * Select the device in the list that matches the IP and MAC of the interface you are transmitting from. By default it will be labeled "eBUS Transmitter".
    * You should begin seeing images as soon as the sample starts transmitting.


2. Prerequisites

This sample assumes that:
 * You have installed version 2.3.1 of OpenCV (available at http://opencv.willowgarage.com/wiki/) to C:\OpenCV2.3 and that the OpenCV binaries are in the runtime path.
 * You have at least one network adapter installed on your PC with a valid IP address.
 * You have a GigE Vision transmitter that is reachable from the sample that will be transceiving.
 * You have a GigE Vision receiver that can receive and display the transformed video (such as a vDisplay, GEVPlayer, NetCommand or any other GigE Vision receiver that supports the GVSP protocol). The receiver should be reachable and on the same subnet as the interface from which it will be receiving.

3. Description

3.1 PvTransformAndTransmitSample.cpp

Sample code as described above. Please refer to the comments in the source code for information on the methods used in this sample.


