#include "compression.hpp"
#include <ctime>

int main()
{
    timespec thetime;
    HeaderData keys;
    cv::Mat image = cv::imread("/home/alex/HEROES-GSFC/SAS/PYAS-core/aspect/frames/frame001.png", 0);
    clock_gettime(CLOCK_REALTIME, &thetime);
    keys.captureTime = thetime;
    keys.exposureTime = 1000;
    writeFITSImage(image, keys, "./test.fits");
    return 0;
}
