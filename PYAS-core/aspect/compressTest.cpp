#include "compression.hpp"

int main()
{
    cv::Mat image = cv::imread("/home/alex/HEROES-GSFC/SAS/PYAS-core/aspect/frames/frame001.png", 0);
    writeFITSImage(image);
    return 0;
}
