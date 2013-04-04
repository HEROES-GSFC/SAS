#include "processing.hpp"
#include "utilities.hpp"
#include "compression.hpp"
#include <fstream>
#include <iostream>
#include <string>

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
    AspectCode runResult;
    cv::VideoWriter summary;
    std::ifstream frames(argv[1]);
    std::string message;
    
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
            runResult = aspect.Run();
            switch(GeneralizeError(runResult))
            {
            case NO_ERROR:
                aspect.GetScreenCenter(IDCenter);
                aspect.GetScreenFiducials(fiducials);
        
            case MAPPING_ERROR:
                //std::cout << "AspectTest: Get IDs" << std::endl;
                aspect.GetFiducialIDs(IDs);
                
            case ID_ERROR:
                //std::cout << "AspectTest: Get Fiducials" << std::endl;
                aspect.GetPixelFiducials(fiducials);
                
            case FIDUCIAL_ERROR:
                //std::cout << "AspectTest: Get Center" << std::endl;
                aspect.GetPixelCenter(center);
                aspect.GetPixelError(error);
                
            case CENTER_ERROR:
                //std::cout << "AspectTest: Get Crossings" << std::endl;
                aspect.GetPixelCrossings(crossings);
                    
            case LIMB_ERROR:
                break;
            default:
                break;
            }

            switch(GeneralizeError(runResult))
            {
            case NO_ERROR:
            case MAPPING_ERROR:
                //std::cout << "AspectTest: Get IDs" << std::endl;
                for (int k = 0; k < IDs.size(); k++)
                {
                    label = "";
                    sprintf(number, "%d", (int) IDs[k].x);
                    label += number;
                    label += ",";
                    sprintf(number, "%d", (int) IDs[k].y);
                    label += number;
                    DrawCross(image, fiducials[k], fiducialColor, 15, 1, 8);
                    cv::putText(image, label, fiducials[k], cv::FONT_HERSHEY_SIMPLEX, .5, IDColor,2);
                }
                
            case ID_ERROR:
                //std::cout << "AspectTest: Get Fiducials" << std::endl;
                for (int k = 0; k < fiducials.size(); k++)
                    DrawCross(image, fiducials[k], fiducialColor, 15, 1, 8);
                
            case FIDUCIAL_ERROR:
                //std::cout << "AspectTest: Get Center" << std::endl;
                DrawCross(image, center, centerColor, 20, 1, 8);
            
                //std::cout << "AspectTest: Get Error" << std::endl;
                //std::cout << "AspectTest: Error:  " << error.x << " " << error.y << std::endl;
                
            case CENTER_ERROR:
                //std::cout << "AspectTest: Get Crossings" << std::endl;;
                for (int k = 0; k < crossings.size(); k++)
                    DrawCross(image, crossings[k], crossingColor, 10, 1, 8);

            case LIMB_ERROR:
                break;
            default:
                break;
            }
           
            
            cv::putText(image, filename, cv::Point(0,(frame.size()).height-20), cv::FONT_HERSHEY_SIMPLEX, .5, textColor,1.5);
            message = GetMessage(runResult);
            cv::putText(image, message, cv::Point(0,(frame.size()).height-10), cv::FONT_HERSHEY_SIMPLEX, .5, textColor,1.5);
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


