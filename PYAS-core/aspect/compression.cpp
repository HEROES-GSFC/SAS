#include "compression.hpp"
#include <CCfits>
#include <cmath>
#include <valarray>
#include <vector>

using namespace CCfits;

int writePNGImage(cv::InputArray _image, const std::string fileName)
{
    std::vector<int> pngParams;
    cv::Mat image = _image.getMat();
    cv::Size size = image.size();
    if (size.width == 0 || size.height == 0)
    {
	std::cout << "Image dimension is 0. Not saving." << std::endl;
	return -1;
    } 
    pngParams.push_back(CV_IMWRITE_PNG_COMPRESSION);
    pngParams.push_back(0);
    try
    {
	cv::imwrite(fileName, image, pngParams);
    }
    catch(cv::Exception e)
    {
	std::cout << "OpenCV Error: " << e.err << std::endl;
	std::cout << "    In file: " << e.file << std::endl;
	std::cout << "    on line: " << e.line << std::endl;
	return -1;
    }
    return 0;
}

int writeFITSImage(cv::InputArray _image, HeaderData keys, const std::string fileName)
{
   
    cv::Mat image = _image.getMat();
    cv::Size size = image.size();
    std::string timeKey;
    if (size.width == 0 || size.height == 0)
    {
	std::cout << "Image dimension is 0. Not saving." << std::endl;
	return -1;
    }    
    // declare auto-pointer to FITS at function scope. Ensures no resources
    // leaked if something fails in dynamic allocation.
    std::auto_ptr<FITS> pFits(0);
      
    try
    {                
        pFits.reset(new FITS(fileName, BYTE_IMG, 0, 0));
    }
    catch (FITS::CantCreate)
    {
	// ... or not, as the case may be.
	return -1;       
    }

    long nelements(1); 
    
    std::vector<long int> extAx;
    extAx.push_back(size.width);
    extAx.push_back(size.height);
    string newName ("Raw Frame");
   
    pFits->setCompressionType(RICE_1);

    ExtHDU* imageExt;
    try
    {
	imageExt = pFits->addImage(newName, BYTE_IMG, extAx, 1);
    }
    catch(FitsError e)
    {
	std::cout << e.message() << "\n";
	return -1;
    }

    nelements = size.width*size.height*sizeof(unsigned char);
           
    std::valarray<unsigned char> array(image.data, nelements);

    long  fpixel(1);
       
    //add keys to the primary header
    timeKey = asctime(gmtime(&(keys.captureTime).tv_sec));
    pFits->pHDU().addKey("DAY AND TIME", timeKey , "Frame Capture Time (UTC)");
    timeKey = nanoString((keys.captureTime).tv_nsec);
    pFits->pHDU().addKey("TIME FRACTION", timeKey, "Frame capture fractional seconds");
    pFits->pHDU().addKey("EXPOSURE", (long)keys.exposureTime,"Total Exposure Time"); 

    try
    {
	imageExt->write(fpixel,nelements,array);
    }
    catch(FitsException e)
    {
	std::cout << e.message();
	return -1;
    }

    return 0;
}

int readFITSImage(const std::string fileName, cv::OutputArray _image)
{
    cv::Size frameSize;
    std::auto_ptr<FITS> pInfile(new FITS(fileName,Read,true));
        
//    PHDU& fileHeader = pInfile->pHDU();
    ExtHDU& image = pInfile->extension("Raw Frame");

    frameSize.height = image.axis(1);
    frameSize.width = image.axis(0);

    std::valarray<unsigned char> contents;
    image.read(contents);
	    
    cv::Mat frame(frameSize, CV_8UC1, &contents[0]);

    frame.copyTo(_image);
    return 0;   
 }
