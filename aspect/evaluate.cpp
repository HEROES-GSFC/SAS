/*
Utility to scan through a bunch of FITS files

Pass in a list of FITS files on the command line, e.g.:
    ./evaluate *.fits
    ./evaluate `ls /mnt/disk2/*.fits`

What's reported is the filename, the image min, the image max, and a histogram:
-----
/mnt/disk2/pyas_130511_180030_000001.fits   0 210 c7655555555555566666777875200000
-----
The histogram uses 32 bins (so 8 values per bin), and the value in each bin is
represented by 2*log10(value+1), rounded, in hex (tops out at "c").
Thus, "0" really means 0 pixels,
      "1" means 1 to 4 pixels,
      "2" means 5 to 16 pixels,
      "3" means 17 to 55 pixels, and so on
Note that a "good" PYAS histogram can be recognized by being mostly flat for
the lower half of the histogram and then peaking in the upper half of the
histogram
*/

#include <cstdio>
#include <cstring>
#include <opencv.hpp>

#include "compression.hpp"

//Assumes 1-D float
void prettyprint(const cv::Mat *in)
{
    for (int j = 0; j < in->rows; j++) {
        printf("%1x", (int)(2*log10(*in->ptr<float>(j)+1)+0.5));
    }
}

int main(int argc, char *argv[])
{
    cv::Mat frame;
    cv::Mat hist;
    double min, max;
    int size[] = {32};
    float range[] = {0, 256};
    const float *ranges[] = {range};

    for (int i = 1; i < argc; i++) {
        if (strstr(argv[i], ".fits") != NULL) {
            if (readFITSImage(argv[i], frame) == 0) {
                cv::minMaxLoc(frame, &min, &max, NULL, NULL);
                cv::calcHist(&frame, 1, //just one matrix
                             {0},       //just the first channel
                             cv::Mat(), //no mask
                             hist,      //output histogram
                             1, size,   //number of dimensions and bins
                             ranges,  //range of histogram
                             true,      //uniform?
                             false);    //accumulate?

                printf("%s %3d %3d ", argv[i], (int)min, (int)max);
                prettyprint(&hist);
                printf("\n");
            } else {
                printf("%s ERROR (see above)\n", argv[i]);
            }
        }
    }

    return 0;
}
