#include "processing.hpp"
#include "utilities.hpp"
#include "draw.hpp"
#include "compression.hpp"
#include "Transform.hpp"
#include "types.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define AVI 0
#define CSV 1

//Calibrated parameters
#define CLOCKING_ANGLE_PYASF -32.425 //model is -33.26
#define CENTER_X_PYASF    124.68 //mils
#define CENTER_Y_PYASF    -74.64 //mils
#define TWIST_PYASF 180.0 //needs to be ~180
#define CLOCKING_ANGLE_PYASR -52.175 //model is -53.26
#define CENTER_X_PYASR -105.59 //mils
#define CENTER_Y_PYASR   -48.64 //mils
#define TWIST_PYASR 0.0 //needs to be ~0

#define IS_PYASF true

int main(int argc, char* argv[])
{

    if (argc != 3)
    {
        std::cout << "Correct usage is: PointingTest frameList.txt outfile.csv\n";
        return -1;
    }

    float clocking_angle, center_x, center_y, twist;
    clocking_angle = (IS_PYASF ? CLOCKING_ANGLE_PYASF : CLOCKING_ANGLE_PYASR);
    center_x = (IS_PYASF ? CENTER_X_PYASF : CENTER_X_PYASR);
    center_y = (IS_PYASF ? CENTER_Y_PYASF : CENTER_Y_PYASR);
    twist = (IS_PYASF ? TWIST_PYASF : TWIST_PYASR);

    size_t found;
    char line[256];
    int index;
    std::string filename, label;
    char number[5] = "000";
    cv::Mat frame;
    cv::Mat image;
    cv::Point2f center,error, offset, IDCenter;

    Transform solarTransform(FORT_SUMNER, GROUND);
    solarTransform.set_clocking(clocking_angle);
    solarTransform.set_calibrated_center(Pair(center_x,center_y));

    HeaderData keys;
    Pair solution, screenCenter;

    cv::Scalar crossingColor(0,255,0);
    cv::Scalar centerColor(64,0,128);
    cv::Scalar fiducialColor(255,0,0);
    cv::Scalar IDColor(165,0,165);
    cv::Scalar textColor(0,165,255);
    cv::Range rowRange, colRange;
    CoordList crossings, fiducials;

    IndexList IDs, rowPairs, colPairs;

    std::vector<float> mapping;
        
    Aspect aspect;
    aspect.SetFloat(FIDUCIAL_TWIST, twist);

    AspectCode runResult;
    cv::namedWindow("Solution", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );

    std::string message;
    timespec startTime, stopTime, diffTime;

    std::string outfile(argv[2]);
    std::string outExtension(outfile.substr(outfile.length()-3, outfile.length()-1));
    outfile = outfile.substr(0, outfile.length()-4);
    std::ifstream frames(argv[1]);

    std::ofstream csvCenter, csvLimbs, csvFiducials, csvPointing;

    cv::VideoWriter summary;
    bool videoReady = false;
    int outType = -1;

    if (!outExtension.compare("avi") || !outExtension.compare("AVI"))
    {
        outType = AVI;
    }
    else if (!outExtension.compare("csv") || !outExtension.compare("CSV"))
    {
        outType = CSV;

        csvCenter.open(outfile+"_centers.csv");
        csvLimbs.open(outfile+"_limbs.csv");
        csvFiducials.open(outfile+"_fiducials.csv");
        csvPointing.open(outfile+"_pointing.csv");
    }
    else
    {
        std::cout << "File extension " << outExtension << " not supported" << std::endl;
        return -1;
    }

    if (!frames.good())
    {
        std::cout << "Failed to whatever file list" << std::endl;
    }
    else 
    {
        index = 0;
        if (outType == AVI)
            videoReady = false;
        while (frames.getline(line,256))
        {
            filename = line;
/*            found = filename.find("png",0);
            if (found != std::string::npos)
            {
                //std::cout << "Loading png file: " << filename << std::endl;
                frame = cv::imread(filename, 0);
            }
            else
            {
*/
            found = filename.find("fit",0);
            if (found!=std::string::npos)
            {
                //std::cout << "Loading fits file: " << filename << std::endl;
                readFITSImage(filename, frame); 
                readFITSHeader(filename, keys);
            }
            else
            {
                //std::cout << "ERROR: " << filename << "isn't a valid type";
                break;
            }
//            }
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
                aspect.GetMapping(mapping);
        
            case MAPPING_ERROR:
                //std::cout << "AspectTest: Get IDs" << std::endl;
                aspect.GetFiducialIDs(IDs);
                aspect.GetFiducialPairs(rowPairs, colPairs);
                
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
            //std::cout << "AspectTest: Just hangin' out" << std::endl;
            clock_gettime(CLOCK_REALTIME, &stopTime);
            diffTime = TimespecDiff(startTime, stopTime);
            //std::cout << "Runtime : " << diffTime.tv_sec << nanoString(diffTime.tv_nsec) << std::endl;
            
            cv::Mat list[] = {frame, frame, frame};
            cv::merge(list,3,image);
/*
            if(GeneralizeError(runResult) < CENTER_ERROR)
            {
                rowRange = SafeRange(center.y-120, center.y+120, image.rows);
                colRange = SafeRange(center.x-120, center.x+120, image.cols);
                image = image(rowRange, colRange);
                offset = cv::Point(colRange.start, rowRange.start);
            }
            else offset = cv::Point(0,0);
*/
            offset = cv::Point(0,0);

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
//                    std::cout << "[" << label << "] ";
                }
//                std::cout << std::endl;


/*                float rowDiff, colDiff;
                std::cout << "Pair distances: \n";
                for (int k = 0; k < rowPairs.size(); k++)
                {
                    std::cout << fiducials[rowPairs[k].x] << " ";
                    std::cout << fiducials[rowPairs[k].y] << " ";
                    std::cout << IDs[rowPairs[k].x] << " ";
                    std::cout << IDs[rowPairs[k].y] << " ";
                    rowDiff = fiducials[rowPairs[k].y].y - 
                        fiducials[rowPairs[k].x].y;
                    colDiff = fiducials[rowPairs[k].y].x - 
                        fiducials[rowPairs[k].x].x;
                    std::cout << " | " << rowDiff << " " << colDiff << std::endl;
                }

                for (int k = 0; k < colPairs.size(); k++)
                {
                    std::cout << fiducials[colPairs[k].x] << " ";
                    std::cout << fiducials[colPairs[k].y] << " ";
                    std::cout << IDs[colPairs[k].x] << " ";
                    std::cout << IDs[colPairs[k].y] << " ";
                    rowDiff = fiducials[colPairs[k].y].y - 
                        fiducials[colPairs[k].x].y;
                    colDiff = fiducials[colPairs[k].y].x - 
                        fiducials[colPairs[k].x].x;
                    std::cout << " | " << rowDiff << " " << colDiff << std::endl;
                    }
*/                
            case ID_ERROR:
                //std::cout << "AspectTest: Get Fiducials" << std::endl;
                for (int k = 0; k < fiducials.size(); k++)
                    DrawCross(image, fiducials[k] - offset, fiducialColor, 15, 1, 8);
                
            case FIDUCIAL_ERROR:
                //std::cout << "AspectTest: Get Center" << std::endl;
                DrawCross(image, center - offset, centerColor , 20, 1, 8);
                cv::circle(image, (center - offset)*pow(2,8), error.x*pow(2,8), centerColor, 1, CV_AA, 8);
                
                //std::cout << "AspectTest: Get Error" << std::endl;
                //std::cout << "AspectTest: Error:  " << error.x << " " << error.y << std::endl;

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
/*
            if(GeneralizeError(runResult) == NO_ERROR)
                std::cout << "Center (pixels): " << IDCenter << std::endl;
            else
                std::cout << "Center (pixels): " << "Not valid" << std::endl;

            if(GeneralizeError(runResult) < MAPPING_ERROR)
                std::cout << "Center (screen): " << IDCenter << std::endl;
            else
                std::cout << "Center (screen): " << "Not valid" << std::endl;
*/            
            cv::putText(image, filename, cv::Point(0,(frame.size()).height-20), cv::FONT_HERSHEY_SIMPLEX, .5, textColor,1.5);
            message = GetMessage(runResult);
            cv::putText(image, message, cv::Point(0,(frame.size()).height-10), cv::FONT_HERSHEY_SIMPLEX, .5, textColor,1.5);
            if (outType == AVI)
            {
                if (!videoReady)
                {
                    summary.open(argv[2], CV_FOURCC('X','V','I','D'), 10, frame.size(), true);
                    videoReady = true;
                }
                summary << image;
            }
            else if(outType == CSV)
            {
                // Generate CSV of center data
                csvCenter << index << ";";
                csvCenter << filename << ";";
                csvCenter << (int) runResult << ";";
                csvCenter << center.x << ";" << center.y << ";";
                csvCenter << IDCenter.x << ";" << IDCenter.y << ";";
                csvCenter << diffTime.tv_sec+((float)diffTime.tv_nsec)/1e9 << ";";
                csvCenter << "\n";

                // Generate CSV of pointing data
                csvPointing << index << ";";
                csvPointing << filename << ";";
                csvPointing << (int) runResult << ";";
                if (runResult == 0) {
                    solarTransform.set_conversion(Pair(mapping[0],mapping[2]),
                                                  Pair(mapping[1],mapping[3]));
                    solution = solarTransform.calculateOffset(Pair(center.x,center.y),keys.captureTime);
                    screenCenter = solarTransform.getPointingAzEl(Pair(center.x,center.y));
                    csvPointing << solution.x() << ";" << solution.y() << ";";
                    csvPointing << solarTransform.getSunAzEl().x() << ";" << solarTransform.getSunAzEl().y() << ";";
                    csvPointing << screenCenter.x() << ";" << screenCenter.y() << ";";
                } else {
                    csvPointing << 0 << ";" << 0 << ";";
                    csvPointing << 0 << ";" << 0 << ";";
                    csvPointing << 0 << ";" << 0 << ";";
                }
                csvPointing << "\n";

                // Generate CSV of limb data
                csvLimbs << index << ";";
                csvLimbs << filename << ";";
                csvLimbs << crossings.size() << ";";
                for (int k = 0; k < crossings.size(); k++)
                    csvLimbs << "[" << crossings[k].x << " " << crossings[k].y << "],";
                csvLimbs << "\n";

                // Generate CSV of fiducial data
                csvFiducials << index << ";";
                csvFiducials << filename << ";";
                csvFiducials << fiducials.size() << ";";
                for (int k = 0; k < fiducials.size(); k++)
                {
                    csvFiducials << "[" << fiducials[k].x << " " << fiducials[k].y;
                    if(IDs.size() == fiducials.size())
                        csvFiducials << " " << IDs[k].x << " " << IDs[k].y << "],";
                    else
                        csvFiducials << " -300 -300],";
                }
                csvFiducials << "\n";
            }

            cv::imshow("Solution", image);
            //cv::waitKey(1);
            index++;
        }
    }
    if (outType == CSV)
    {
        csvCenter.close();
        csvLimbs.close();
        csvFiducials.close();
        csvPointing.close();
    }
    frames.close();
    return 0;
}


