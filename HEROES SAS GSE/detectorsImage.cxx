
#include "detectorsImage.h"

#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_File_Chooser.H>

#include <time.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>

#include "gui.h"
#include "Application.h"

//todo
//need to auto scale the image, add scalling types (auto, set/linear, logarithmic)
//this needs maximumValue function, add in a functions.cxx file

//each pixel is exactly 1x1 in GL coordinates to make it easier

extern Gui *gui;

// preference variable

detectorsImage::detectorsImage(int x,int y,int w,int h,const char *l)
: Fl_Gl_Window(x,y,w,h,l)
{

}

// the drawing method: draws the histFunc into the window
void detectorsImage::draw()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0,0,w(),h());
	//glOrtho(0,3*XSTRIPS+4 + detector_buffer[0]*2 + 2*border_buffer,0,3*YSTRIPS+4 + detector_buffer[1]*2 + 2*border_buffer,0,-1);
   	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
   	glPushMatrix();
   	glLoadIdentity();
   	glClearColor(0.0,0.0,0.0,0.0);
   	glClear(GL_COLOR_BUFFER_BIT);


	//draw a box around the detectors

	glPopMatrix();
	glFinish();
}

int detectorsImage::handle(int eventType)
{
	int button;
	char text[8];

	button=Fl::event_button();

	redraw();
	return(1);
}
