Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.

**Last Modified 10/03/08**

=========================
Multicasting slave sample
=========================

This sample shows how to use the PvPipeline class to receive as multicast slave. In order to run this sample, you must first connect to an IP Engine with the Multicasting master sample (PvMulticastMasterSample). You must also connect to a switch that supports IGMP (Internet Group Management Protocol).

1. Introduction

We use the PvPipeline (and contained PvStream) to
 * Subscribe (open) a stream to a multicast group
 * Receive images
 * ...all that as a pure slave - we do not control the device

2. Pre-conditions

This samples assumes that:
 * You have a GigE Vision Device connected to a network adapter.

3. Description

3.1 PvMulticastSlave.cpp

Shows how to use the PvPipeline (and PvStream) to reiceve data as a multicast slave. Please refer to the comments in the source code for information on the methods used in this sample.

