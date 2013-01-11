Copyright (c) 2011, Pleora Technologies Inc., All rights reserved.

========================
Using PvTransmitRawSample
========================

This sample code illustrates how to use the PvVirtualDevice and PvTransmitterRaw 
classes to transmit raw data (not images) using the eBUS SDK.

1. Introduction

Using the PvSystem and PvInterface class, this sample enumerates available 
interfaces (network adapters) and selects the first interface with a valid IP 
address.
Using the PvVirtualDevice class, it listens for device discovery requests so that 
it can be detected by receiving applications.
Using the PvBuffer class, it shows how to allocate a set of raw buffers with a 
given size for use by the transmitter API.
Using the PvTransmitterRaw class, a stream of raw buffers is transmitted to the 
specified destination.

By default, this sample transmits raw buffers with continuously varying data from 
the first valid interface to multicast address 239.192.1.1:1042. However, it can 
be used in many different ways by providing it with optional command line arguments. 
For further details on the command line options that are available, run the sample 
with the argument --help.

By default, this sample transmits as fast as the computer permits. This can cause 
the CPU to seem high. Use the fps command line option to control the data rate of 
the sample which reduces the CPU usage.

Procedure for multicasting raw data to a GigE Vision receiver:
 * Ensure that the GigE Vision receiver you wish to transmit to is reachable from the PC 
   that will be running this sample. Their IP addresses should be set such that 
   they are on the same subnet.
 * If only one network interface is installed and connected with a valid IP address 
   on your PC, start the sample with default options (no command line arguments). 
   Otherwise, note the IP address of the interface you wish to transmit from and 
   start the sample with command line argument --sourceaddress=<your IP address>.
 * To receive the raw data stream with PvReceiveRawSample:
   * Start PvReceiveRawSample with default options (no command line arguments)
   * PvReceiveRawSample will prompt for a device. Select the one corresponding to 
     the PvTransmitRawSample labeled "eBUS Transmitter".
 * Press any key when you are ready to begin transmitting (once the receiver is 
   ready to receive the image stream).
 * The receiver and transmitter samples should continually update with statistics 
   on number of blocks transmitted (received) and data rate.


2. Prerequisites

This sample assumes that:
 * You have a network adapter installed on your PC with a valid IP address.
 * You have a GigE Vision receiver that can receive raw data (such as the PvReceiveRaw sample). 
   The receiver should be reachable and on the same subnet as the interface from which 
   it will be receiving.
 * You have a valid eBUS SDK license. 

3. Description

3.1 PvTransmitRawSample.cpp

Sample code as described above. Please refer to the comments in the source code for 
information on the methods used in this sample.


