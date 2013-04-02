#include "processing.hpp"
#include "utilities.hpp"
#include "compression.hpp"
#include <fstream>
#include <iostream>
#include <string>

void DrawCross(cv::Mat &image, cv::Point2f point, cv::Scalar color, int length, int thickness)
{
    cv::Point2f pt1, pt2;

    length = (length+1)/2;
    pt1.x = point.x-length;
    pt1.y = point.y-length;
    pt2.x = point.x+length;
    pt2.y = point.y+length;
    cv::line(image, pt1*128, pt2*128, color, thickness, CV_AA, 7);
    pt1.x = point.x+length;
    pt1.y = point.y-length;
    pt2.x = point.x-length;
    pt2.y = point.y+length;
    cv::line(image, pt1*128, pt2*128, color, thickness, CV_AA, 7);
}

int main(int argc, char* argv[])
{

    if (argc != 3)
    {
	std::cout << "Correct usage is: AspectTest frameList.txt outfile.avi\n";
	return -1;
    }

    size_t found;
    char line[256];
    std::string filename, label;
    char number[4] = "000";
    cv::Mat frame;
    cv::Mat image;
    cv::Point2f center,error, IDCenter;

    cv::Scalar crossingColor(0,255,0);
    cv::Scalar centerColor(0,0,255);
    cv::Scalar fiducialColor(255,0,0);
    cv::Scalar IDColor(165,0,165);
    cv::Scalar textColor(0,165,255);

    CoordList crossings, fiducials;
    IndexList IDs;
    std::vector<float> mapping;
        
    Aspect aspect;
    ErrorCode runResult;
    cv::VideoWriter summary;
    std::ifstream frames(argv[1]);
    if (!frames.good())
    {
	std::cout << "Failed to whatever file list" << std::endl;
    }
    else	
    {
	bool videoReady = false;
	while (frames.getline(line,256))
	{
	    filename = line;
	    found = filename.find("png",0);
	    if (found != std::string::npos)
	    {
		std::cout << "Loading png file: " << filename << std::endl;
		frame = cv::imread(filename, 0);
	    }
	    else
	    {
		found = filename.find("fit",0);
		if (found!=std::string::npos)
		{
		    std::cout << "Loading fits file: " << filename << std::endl;
		    readFITSImage(filename, frame);
		}
		else
		{
		    std::cout << "ERROR: " << filename << "isn't a valid type";
		    break;
		}
	    }
		    
	    
	    aspect.LoadFrame(frame);
	
	    cv::Mat list[] = {frame, frame, frame};
	    cv::merge(list,3,image);

	    //std::cout << "AspectTest: Load Frame" << std::endl;
	    aspect.LoadFrame(frame);

	    //std::cout << "AspectTest: Run Aspect" << std::endl;
	    if((runResult = aspect.Run()) == NO_ERROR)
	    {
		switch(runResult)
		{
		case NO_ERROR:
		    aspect.GetScreenCenter(IDCenter);
		    aspect.GetScreenFiducials(fiducials);
	
		case MAPPING_ILL_CONDITIONED:
		    //std::cout << "AspectTest: Get IDs" << std::endl;
		    aspect.GetFiducialIDs(IDs);
		
		case FEW_IDS:
		case NO_IDS:
		    //std::cout << "AspectTest: Get Fiducials" << std::endl;
		    aspect.GetPixelFiducials(fiducials);

		case FEW_FIDUCIALS:
		case NO_FIDUCIALS:
		case SOLAR_IMAGE_OFFSET_OUT_OF_BOUNDS:
		case SOLAR_IMAGE_SMALL:
		case SOLAR_IMAGE_EMPTY:
		    //std::cout << "AspectTest: Get Center" << std::endl;
		    aspect.GetPixelCenter(center);
		    aspect.GetPixelError(error);
		
		case CENTER_ERROR_LARGE:
		case CENTER_OUT_OF_BOUNDS:
		    //std::cout << "AspectTest: Get Crossings" << std::endl;
		    aspect.GetPixelCrossings(crossings);

		case FEW_LIMB_CROSSINGS:
		case NO_LIMB_CROSSINGS:
		    break;
		default:
		    break;
		}

		switch(runResult)
		{
		case NO_ERROR:
		case MAPPING_ILL_CONDITIONED:
		    //std::cout << "AspectTest: Get IDs" << std::endl;
		    for (int k = 0; k < IDs.size(); k++)
		    {
			label = "";
			sprintf(number, "%d", (int) IDs[k].x);
			label += number;
			label += ",";
			sprintf(number, "%d", (int) IDs[k].y);
			label += number;
			DrawCross(image, fiducials[k], fiducialColor, 15, 1);
			cv::putText(image, label, fiducials[k], cv::FONT_HERSHEY_SIMPLEX, .5, IDColor,2);
		    }
		
		case FEW_IDS:
		case NO_IDS:
		    //std::cout << "AspectTest: Get Fiducials" << std::endl;
		    for (int k = 0; k < fiducials.size(); k++)
			DrawCross(image, fiducials[k], fiducialColor, 15, 1);
		
		case FEW_FIDUCIALS:
		case NO_FIDUCIALS:
		case SOLAR_IMAGE_OFFSET_OUT_OF_BOUNDS:
		case SOLAR_IMAGE_SMALL:
		case SOLAR_IMAGE_EMPTY:
		    //std::cout << "AspectTest: Get Center" << std::endl;
		    DrawCross(image, center, centerColor, 20, 1);
	    
		    //std::cout << "AspectTest: Get Error" << std::endl;
		    //std::cout << "AspectTest: Error:  " << error.x << " " << error.y << std::endl;
		
		case CENTER_ERROR_LARGE:
		case CENTER_OUT_OF_BOUNDS:
		    //std::cout << "AspectTest: Get Crossings" << std::endl;;
		    for (int k = 0; k < crossings.size(); k++)
			DrawCross(image, crossings[k], crossingColor, 10, 1);

		case FEW_LIMB_CROSSINGS:
		case NO_LIMB_CROSSINGS:
		    break;
		default:
		    std::cout << "why?\n";
		}
		

	    }
	    else
	    
	    cv::putText(image, filename, cv::Point(0,(frame.size()).height-5), cv::FONT_HERSHEY_SIMPLEX, .5, textColor,1.5);
	    if (!videoReady)
	    {
		summary.open(argv[2], CV_FOURCC('F','F','V','1'), 10, frame.size(), true);
		videoReady = true;
	    }
	    summary << image;

	}
    }
    frames.close();
    return 0;
}


