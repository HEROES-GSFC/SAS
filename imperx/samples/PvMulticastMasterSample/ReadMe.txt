Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.

**Last Modified 10/03/08**

=========================
Multicasting master sample
=========================

This sample shows how to use the PvDevice to control a multicast master. once connected as the master, you can run the Multicasting Slave Sample (PvMulticastSlaveSample) to connect as a slave. You must also connect to a switch that supports IGMP (Internet Group Management Protocol).

1. Introduction

We use the PvDevice to
 * Connect to a device
 * Configure the device for multicasting
 * We do not receive data on this master - but we could if we want
 * Start streaming
 * Stop streaming

2. Pre-conditions

This samples assumes that:
 * You have a GigE Vision Device connected to a network adapter.

3. Description

3.1 PvMulticastMaster.cpp

Shows how to use the PvDevice class to control a multicast master. Please refer to the comments in the source code for information on the methods used in this sample.

