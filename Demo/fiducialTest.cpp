#include <opencv.hpp>
#include <highgui/highgui.hpp>
#include <chord.hpp>
#include <fiducials.hpp>
#include <iostream>

#define CHORDS 80
#define THRESHOLD 154

int main( int argc, char** argv )
{ 
    if( argc != 4) 
    {
     std::cout <<" Usage: test <image> <center row> <center col>\n";
     return -1;
    }
    
    int center[2];
    int numLocs = 20;
    int locs[2*numLocs];
    double* temp;
    int fidLocs;
    
    cv::Mat image;
    image = cv::imread(argv[1], 0); 

    if(!image.data)
    {
        std::cout <<  "Could not open or find the image\n";
        return -1;
    }
    
    cv::Size imSize = image.size();
	std::cout << "Raw Image Size: " << imSize.width << " by " << imSize.height
			  << "\n";

	std::stringstream ss;
	ss << argv[2] << " " << argv[3];
	ss >> center[0];
	ss >> center[1];

	std::cout << "Center: " << center[0] << " , " << center[1] << "\n";
	cv::Mat subImage;
	cv::Range rowRange, colRange;
	rowRange.end = (((int) center[0]) + 125 < imSize.height-1) ? (((int) center[0]) + 125) : (imSize.height-1);
	rowRange.start = (((int) center[0]) - 125 > 0) ? (((int) center[0]) - 125) : 0;
	colRange.end = (((int) center[1]) + 125 < imSize.width) ? (((int) center[1]) + 125) : (imSize.width-1);
	colRange.start = (((int) center[1]) - 125 > 0) ? (((int) center[1]) - 125) : 0;
	
	std::cout << "Subimage paramters:\n" << "Rows: " << rowRange.start << " to " 
			  << rowRange.end << "\nCols: " << colRange.start << " to " 
			  << colRange.end << "\n";
	subImage = image(rowRange, colRange);

	morphParams rowParams;
    rowParams.dim = 0;
	rowParams.tophatWidth = 23;
	rowParams.threshold = 3;
        
    morphParams colParams;
	colParams.dim = 1;
	colParams.tophatWidth = 5;
	colParams.threshold = 0;

	fidLocs = morphFindFiducials(subImage, rowParams, colParams, 23, locs, numLocs);
	std::cout << "Final Fiducial Count: " << fidLocs << "\n";
	for (int k = 0; k < fidLocs; k++)
	{
		cv::circle(subImage, cv::Point(locs[k], locs[numLocs + k]), 5, cv::Scalar(0), 1);
	}
    cv::namedWindow( "Display window", CV_WINDOW_AUTOSIZE ); 
    cv::imshow( "Display window", subImage ); 

    cv::waitKey(0);
    return 0;
}
