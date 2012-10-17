#include <highgui/highgui.hpp>
#include <chord.hpp>
#include <fiducials.hpp>
#include <cstdio>
#include <string>

// FIDTYPE of 0 for projection-based method (morphFindFiducials)
// FIDTYPE of 1 for matched filter method (matchFindFiducials)
#define FIDTYPE 1

// DEBUG of 1 plots images and makes a lot of noise
// DEBUG of 0 doesn't do that
#define DEBUG 1

#define FID_WIDTH 5
#define FID_LENGTH 23
#define FID_ROW_THRESH 5
#define FID_COL_THRESH 0
#define FID_MATCH_THRESH 5

#define CHORDS 50
#define THRESHOLD 55
#define SOLAR_RADIUS 105

int main( int argc, char** argv )
{ 
    if( argc != 3)
    {
     std::cout <<" Usage: test <path> <extension>\n";
     return -1;
    }
    
    double center[6];
    int numLocs = 20;
    double* temp;
    int fidLocs;
    std::string file;
    char number;


	#if FIDTYPE == 0
		int locs[2*numLocs];
	#else
		cv::Point locs[numLocs];
    #endif
    
    cv::Mat image;
   	cv::Mat subImage;
	cv::Range rowRange, colRange;
	for (int q = 0; q <=1000; q++){
	for (int k = 1; k <= 9; k++)
	{
	
	sprintf(&number, "%d", k);
	file = "";
	file += argv[1];
	file += number;
	file += argv[2];

	#if DEBUG
		std::cout << file << "\n";
    #endif
    
    image = cv::imread(file, 0); 
	
    if(!image.data)
    {
        std::cout <<  "Could not open or find the image\n";
        return -1;
    }
  	cv::Size imSize = image.size(); 	

	int height = imSize.height;
	int width = imSize.width;
	chordCenter(image.data, height, width, CHORDS, THRESHOLD, center);



	#if DEBUG
		std::cout << "Chord Center: " << center[0] << "+/-"<< center[4] << " (" << center[2] << "), " 
			  << center[1] << "+/-" << center[5] << " (" << center[3] << ")\n";
	#endif
	
	if (center[0] > 0 && center[1] > 0 && center[0] < width && center[1] < height)
	{
		rowRange.end = (((int) center[1]) + SOLAR_RADIUS < height-1) ? (((int) center[1]) + SOLAR_RADIUS) : (height-1);
		rowRange.start = (((int) center[1]) - SOLAR_RADIUS > 0) ? (((int) center[1]) - SOLAR_RADIUS) : 0;
		colRange.end = (((int) center[0]) + SOLAR_RADIUS < width) ? (((int) center[0]) + SOLAR_RADIUS) : (width-1);
		colRange.start = (((int) center[0]) - SOLAR_RADIUS > 0) ? (((int) center[0]) - SOLAR_RADIUS) : 0;
		subImage = image(rowRange, colRange);
	}
	else
	{
		std::cout << "Bad Center. Giving up completely\n";
		return 1;
	}

	#if DEBUG
		std::cout << "Subimage paramters:\n" << "Rows: " << rowRange.start << " to " 
			  << rowRange.end << "\nCols: " << colRange.start << " to " 
			  << colRange.end << "\n";
	#endif

	subImage = image(rowRange, colRange);
	#if FIDTYPE == 0
		morphParams rowParams;
		rowParams.dim = 0;
		rowParams.tophatWidth = FID_LENGTH;
		rowParams.threshold = FID_ROW_THRESH;
        
		morphParams colParams;
		colParams.dim = 1;
		colParams.tophatWidth = FID_WIDTH;
		colParams.threshold = FID_COL_THRESH;
		fidLocs = morphFindFiducials(subImage, rowParams, colParams, FID_LENGTH, locs, numLocs);
	#else
	    cv::Mat image, kernel;
		matchKernel(kernel);  		  
		fidLocs = matchFindFiducials(subImage, kernel, FID_MATCH_THRESH, locs, numLocs);
	#endif

	#if DEBUG		
	std::cout << "Final Fiducial Count: " << fidLocs << "\n";
	for (int k = 0; k < fidLocs; k++)
	{
		#if FIDTYPE == 0
			cv::circle(subImage, cv::Point(locs[k], locs[numLocs + k]), 5, cv::Scalar(0), 1);
			std::cout << locs[k]  << " " << locs[numLocs + k] << "\n";
		#else
			cv::circle(subImage, locs[k], 5, cv::Scalar(0), 1);
			std::cout << locs[k].y << " " << locs[k].x << "\n";
		#endif
	}
    cv::namedWindow( "Display window", CV_WINDOW_AUTOSIZE ); 
    cv::imshow( "Display window", subImage ); 
	
    cv::waitKey(0);
	#endif
	    
    }}
    return 0;
}
