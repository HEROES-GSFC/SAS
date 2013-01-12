Copyright (c) 2011, Pleora Technologies Inc., All rights reserved.

========================
Using PvReceiveRawSample
========================

This sample code illustrates how to use the PvStream and PvPipeline classes to receive raw data using the eBUS SDK.

1. Introduction

Using the PvDeviceFinderWnd class, this sample enumerates available GigE Vision devices to the user and allows the user to select a GigE Vision device. Note that the selected device must transmit raw data on channel 0.
Using the PvStream and PvPipeline classes, this sample illustrates how to receive raw data from a GigE Vision transmitter.
A PvBuffer is used to hold the raw data once it is received.

By default, this sample receives raw buffers passively from multicast address 239.192.1.1:1042. However, it can be used in many different ways by providing it with optional command line arguments. For further details on the command line options that are available, run the sample with the argument --help.

Also note that by default, this sample receives data passively (it connects as a pure data receiver). However, if the --connectdevice command line argument is supplied, a PvDevice is instantiated to control the transmitting entity and start streaming. This mode of operation is not supported by all GigE Vision transmitters.

Refer to the readme in PvTransmitRawSample for how to test transmission and reception of raw buffers by having PvReceiveRawSample receive data from PvTransmitRawSample.


2. Prerequisites

This sample assumes that:
 * You have a network adapter installed on your PC with a valid IP address.
 * You have a GigE Vision transmitter that can transmit raw data on channel 0 and respond to device discovery requests (such as the PvTransmitRaw sample). The transmitter should be reachable and on the same subnet as the interface from which this sample will be receiving.

3. Description

3.1 PvReceiveRawSample.cpp

Sample code as described above. Please refer to the comments in the source code for information on the methods used in this sample.


