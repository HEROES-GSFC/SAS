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
        std::cout << "Correct usage is: MeasureScreen frameList.txt\n";
        return -1;
    }

    size_t found;
    char line[256];
    std::string filename, label;
    char number[4] = "000";
    cv::Mat frame;
    cv::Mat image;

    cv::Scalar fiducialColor(255,0,0);
    cv::Scalar IDColor(165,0,165);
    cv::Scalar textColor(0,165,255);
    cv::Range rowRange, colRange;

    CoordList fiducials;

    IndexList IDs, rowPairs, colPairs, pairs;

    Circle circle[2];

    std::vector<float> mapping, crap;
        
    Aspect aspect;
    AspectCode runResult;
    cv::namedWindow("Solution", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );
    std::ifstream frames(argv[1]);
    std::string message;
    timespec startTime, stopTime, diffTime;
    
    aspect.SetInteger(NUM_FIDUCIALS, 225);
    aspect.SetFloat(ERROR_LIMIT, 1296);
    aspect.SetFloat(CHORD_THRESHOLD, .1);
    aspect.SetFloat(RADIUS_MARGIN, 20);
    aspect.SetFloat(FIDUCIAL_THRESHOLD, 3.5);

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
            case MAPPING_ERROR:
                //std::cout << "AspectTest: Get IDs" << std::endl;
                aspect.GetFiducialIDs(IDs);
                aspect.GetFiducialPairs(rowPairs, colPairs);
                
            case ID_ERROR:
                //std::cout << "AspectTest: Get Fiducials" << std::endl;
                aspect.GetPixelFiducials(fiducials);
        
            default:
                break;
            }
            clock_gettime(CLOCK_REALTIME, &stopTime);
            diffTime = TimespecDiff(startTime, stopTime);
            //std::cout << "Runtime : " << diffTime.tv_sec << nanoString(diffTime.tv_nsec) << std::endl;
            
            cv::Mat list[] = {frame, frame, frame};
            cv::merge(list,3,image);

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

                    cv::putText(image, label, fiducials[k], cv::FONT_HERSHEY_SIMPLEX, .5, IDColor,2);
                    std::cout << IDs[k];
                }
                std::cout << std::endl;

                float rowDiff, colDiff;
                std::cout << "Pair distances: \n";
                crap.clear();
                pairs.clear();
                for (int d = 0; d < 2; d++)
                {
                    pairs = d ? rowPairs : colPairs;
                    for (int k = 0; k < pairs.size(); k++)
                    {
                        std::cout << fiducials[pairs[k].x] << " ";
                        std::cout << fiducials[pairs[k].y] << " ";
                        std::cout << IDs[pairs[k].x] << " ";
                        std::cout << IDs[pairs[k].y] << " ";
                        rowDiff = fiducials[pairs[k].y].y - 
                            fiducials[pairs[k].x].y;
                        colDiff = fiducials[pairs[k].y].x - 
                            fiducials[pairs[k].x].x;

                        std::cout << " | " << rowDiff << " " << colDiff << std::endl;
                        if (fabs(rowDiff - 15.7) < 1.5)
                            crap.push_back(rowDiff);
                        else if (fabs(colDiff - 15.7) < 1.5)
                            crap.push_back(colDiff);
                    }
                }
            case ID_ERROR:
                //std::cout << "AspectTest: Get Fiducials" << std::endl;
                for (int k = 0; k < fiducials.size(); k++)
                    DrawCross(image, fiducials[k], fiducialColor, 15, 1, 8);
             
            default:
                break;
            }            

            std::cout << "Mean whatever bullshit is: " << Mean(crap) << std::endl;

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


