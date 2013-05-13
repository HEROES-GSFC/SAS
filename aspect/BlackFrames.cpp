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
    std::string filename;
    cv::Mat frame;
    double min, max;

    std::ifstream frames(argv[1]);

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

            cv::minMaxLoc(frame, &min, &max, NULL, NULL);

            if (frame.empty())
                std::cout << "Empty Frame: " << filename << std::endl;
            else if (min == max || max == 0)
                std::cout << "Constant Frame: " << filename << std::endl;
                
        }
    }
    frames.close();
    return 0;
}


