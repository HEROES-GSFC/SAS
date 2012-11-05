#include <compression.hpp>
#include <CCfits>
#include <cmath>
#include <valarray>
#include <vector>


int writeFITSImage(cv::InputArray _image)
{
    
    cv::Mat image = _image.getMat();
    cv::Size imSize = image.size();
    
    long imDim[2] = {imSize.width, imSize.height};
	std::vector<long int> tileDim (2,64);
    // declare auto-pointer to FITS at function scope. Ensures no resources
    // leaked if something fails in dynamic allocation.
    std::auto_ptr<CCfits::FITS> pFits(0);
      
    try
    {                
        // overwrite existing file if the file already exists.
            
        const std::string fileName("whocares.fit[compress R 64,64] ");            
        
        // Create a new FITS object, specifying the data type and axes for the primary
        // image. Simultaneously create the corresponding file.
        
        // this image is unsigned short data, demonstrating the cfitsio extension
        // to the FITS standard.
        
        pFits.reset( new CCfits::FITS(fileName , 8 , (int) 2 , imDim));
    }
    catch (CCfits::FITS::CantCreate)
    {
          // ... or not, as the case may be.
          return -1;       
    }
    
    std::cout << "Opened file\n";
    long numElements = imDim[0]*imDim[1];
    std::valarray<unsigned char> data (image.data, numElements*sizeof(unsigned char));
    std::cout << "Wrapped image data\n";
    
 //   pFits->setCompressionType(PLIO_1);
 //   pFits->setTileDimensions(tileDim);
    std::cout << "Who knows?\n";
    
//    pFits->pHDU().addKey("EXPOSURE", 1,"Total Exposure Time"); 
//    pFits->pHDU().addKey("OMEGA", 2," Complex cube root of 1 ");  
	std::cout << "Dumped some useless filler into the header\n";    
    
    try
    {
	    pFits->pHDU().write(((long) 1), numElements, data);
    	std::cout << "Yes?\n";
	}
	catch(CCfits::FitsError wrong)
	{
		std::cout << wrong.message() << "\n";
	}    
    // PHDU's friend ostream operator. Doesn't print the entire array, just the
    // required & user keywords, and is provided largely for testing purposes [see 
    // readImage() for an example of how to output the image array to a stream].
    
    std::cout << pFits->pHDU() << std::endl;

    return 0;
}
