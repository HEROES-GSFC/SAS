
#include <FL/Fl.H>
#include "gui.h"
Gui *gui;

int main(int argc, char *argv[])
{
	gui=new Gui;
	
	Fl::visual(FL_DOUBLE|FL_INDEX);
	Fl::scheme("plastic");		// optional skin scheme
	gui->show();
	
	return(Fl::run());
}
