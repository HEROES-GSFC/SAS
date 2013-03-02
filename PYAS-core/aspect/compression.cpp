#include "compression.hpp"
#include <CCfits>
#include <cmath>
#include <valarray>
#include <vector>

using namespace CCfits;

int writeFITSImage(cv::InputArray _image, const std::string fileName)
{
   
    cv::Mat image = _image.getMat();
    cv::Size size = image.size();

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
    }

    nelements = size.width*size.height*sizeof(unsigned char);
           
    std::valarray<unsigned char> array(image.data, nelements);

    long  fpixel(1);
       
    //add two keys to the primary header, one long, one complex.
    long exposure(1500);
    pFits->pHDU().addKey("EXPOSURE", exposure,"Total Exposure Time"); 

    try
    {
	imageExt->write(fpixel,nelements,array);
    }
    catch(FitsException e)
    {
	std::cout << e.message();
    }

    return 0;
}
