#include <string.h>
#include <iostream>
#include <vector>
#include <fstream>

#include "compression.hpp"
#include "utilities.hpp"
 
#define RATE 9

int main()
{
    char number[4] = "000";
    std::string infile, savefile, fitsfile, pngfile;
    std::vector<int> pngstuff;
    cv::Mat frame;
    
    for (int filenumber = 0; filenumber < 20; filenumber++)
    {
	infile = "./frames/frame";
	sprintf(number, "%03d", filenumber);
	infile += number;
	infile += ".png";
	
	frame = cv::imread(infile,0);
	
	pngstuff.push_back(CV_IMWRITE_PNG_COMPRESSION);
	pngstuff.push_back(RATE);
								       	
	savefile = "./test/frame";
	savefile += number;

	fitsfile = savefile;
	pngfile = savefile;
	pngfile += ".png";
	fitsfile += ".fit";
	
	double pngDuration = -1*GetSystemTime();
	cv::imwrite(pngfile, frame, pngstuff);
	pngDuration += GetSystemTime();
    
	double FITSDuration = -1*GetSystemTime();
	writeFITSImage(frame, fitsfile);

	FITSDuration += GetSystemTime();

	std::cout << "PNG Time: " << pngDuration << "\n";
	std::cout << "FITS Time: " << FITSDuration << "\n";
    }
    return 0;

}
