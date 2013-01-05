Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.

========================
Using PvRGBFilterSample
========================

This sample code illustrates how to use the PvFilterRGB class to apply RGB filtering to the contents of a PvBuffer.

This sample covers all of the steps necessary to do the following:
 * Select a GigE Vision device on the network
 * Grab a single image into a PvBuffer
 * Convert the image to RGB32 using the PvBufferConverter class
 * Apply the RGB Filter and white balance using the PvFilterRGB class
 * Save the RGB32 image into a Windows bitmap file using the PvBufferWriter classes
