#ifndef _detectorsImage_h_     // prevent multiple includes
#define _detectorsImage_h_

#include <time.h>
#include <FL/Fl_Gl_Window.H>

#define XSTRIPS 128
#define YSTRIPS 128
#define NUM_DETECTORS 7

class detectorsImage : public Fl_Gl_Window {
public:
	detectorsImage(int x,int y,int w,int h,const char *l=0);
	void draw();
	int handle(int eventType);
private:
};

#endif
