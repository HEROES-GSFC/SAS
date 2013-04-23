#include "processing.hpp"
#include "utilities.hpp"
#include "compression.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{

    if (argc != 2)
    {
        std::cout << "Correct usage is: AspectTest frameList.txt\n";
        return -1;
    }

    size_t found;
    char line[256];
    std::string filename, label;
    char number[4] = "000";
    cv::Mat frame;
    cv::Mat image;
    cv::Point2f center,error, offset, IDCenter;

    cv::Scalar crossingColor(0,255,0);
    cv::Scalar centerColor(64,0,128);
    cv::Scalar fiducialColor(255,0,0);
    cv::Scalar IDColor(165,0,165);
    cv::Scalar textColor(0,165,255);
    cv::Range rowRange, colRange;

    CoordList crossings, fiducials;
    Circle circle[2];
    IndexList IDs;
    std::vector<float> mapping;
        
    Aspect aspect;
    AspectCode runResult;
    cv::namedWindow("Solution", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );
    std::ifstream frames(argv[1]);
    std::string message;
    timespec startTime, stopTime, diffTime;

    if (!frames.good())
    {
        std::cout << "Failed to whatever file list" << std::endl;
    }
    else        
    {
        while (frames.getline(line,256))
        {
            filename = line;
            found = filename.find("png",0);
            if (found != std::string::npos)
            {
                //std::cout << "Loading png file: " << filename << std::endl;
                frame = cv::imread(filename, 0);
            }
            else
            {
                found = filename.find("fit",0);
                if (found!=std::string::npos)
                {
                    //std::cout << "Loading fits file: " << filename << std::endl;
                    readFITSImage(filename, frame);
                }
                else
                {
                    std::cout << "ERROR: " << filename << "isn't a valid type";
                    break;
                }
            }
                    
            
            aspect.LoadFrame(frame);
        
            clock_gettime(CLOCK_REALTIME, &startTime);
            //std::cout << "AspectTest: Load Frame" << std::endl;
            aspect.LoadFrame(frame);
            //std::cout << "AspectTest: Run Aspect" << std::endl;
            runResult = aspect.Run();
            
            //Get aspect data products depending on error severity
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
                aspect.GetPixelError(error);
            case LIMB_ERROR:
                break;
            default:
                break;
            }
            clock_gettime(CLOCK_REALTIME, &stopTime);
            diffTime = TimespecDiff(startTime, stopTime);
            //std::cout << "Runtime : " << diffTime.tv_sec << nanoString(diffTime.tv_nsec) << std::endl;
            
            cv::Mat list[] = {frame, frame, frame};
            cv::merge(list,3,image);
            if(GeneralizeError(runResult) < CENTER_ERROR)
            {
                rowRange = SafeRange(center.y-120, center.y+120, image.rows);
                colRange = SafeRange(center.x-120, center.x+120, image.cols);
                image = image(rowRange, colRange);
                offset = cv::Point(colRange.start, rowRange.start);
            }
            else offset = cv::Point(0,0);


            //Generate summary image with accurate data products marked.
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
                    cv::putText(image, label, fiducials[k] - offset, cv::FONT_HERSHEY_SIMPLEX, .5, IDColor,2);
                }
                
            case ID_ERROR:
                //std::cout << "AspectTest: Get Fiducials" << std::endl;
                for (int k = 0; k < fiducials.size(); k++)
                    DrawCross(image, fiducials[k] - offset, fiducialColor, 15, 1, 8);
                
            case FIDUCIAL_ERROR:
                //std::cout << "AspectTest: Get Center" << std::endl;
                DrawCross(image, center - offset, centerColor , 20, 1, 8);
                //std::cout << "AspectTest: Get Error" << std::endl;
                //std::cout << "AspectTest: Error:  " << error.x << " " << error.y << std::endl;
                for (int k = 1; k < 2; k++)
                {
                    CircleFit(crossings, k, circle[k]);
                    DrawCross(image, circle[k].center() - offset, cv::Scalar(0,50+k*100,255) , 20, 1, 8);
//                    std::cout << circle[k].r() << std::endl;
                    if (circle[k].r() > 0)
                        cv::circle(image, (circle[k].center() - offset)*pow(2,8), circle[k].r()*pow(2,8), cv::Scalar(0,50+k*100,255), 1, CV_AA, 8);
                }

            case CENTER_ERROR:
                //std::cout << "AspectTest: Get Crossings" << std::endl;;
                for (int k = 0; k < crossings.size(); k++)
                    DrawCross(image, crossings[k] - offset, crossingColor, 10, 1, 8);

            case LIMB_ERROR:
                break;
            default:
                break;
            }


            //Print data to screen.

            if(GeneralizeError(runResult) < CENTER_ERROR)
                std::cout << "Center: " << center << std::endl;
            else
                std::cout << "Center: " << "[-1 -1]" << std::endl;
/*
            if(GeneralizeError(runResult) < MAPPING_ERROR)
                std::cout << "Center (screen): " << IDCenter << std::endl;
            else
                std::cout << "Center (screen): " << "Not valid" << std::endl;
*/            
            cv::putText(image, filename, cv::Point(0,(frame.size()).height-20), cv::FONT_HERSHEY_SIMPLEX, .5, textColor,1.5);
            message = GetMessage(runResult);
            cv::putText(image, message, cv::Point(0,(frame.size()).height-10), cv::FONT_HERSHEY_SIMPLEX, .5, textColor,1.5);
            
            cv::imshow("Solution", image);
            
            
            cv::waitKey(0);

        }
    }
    frames.close();
    return 0;
}


