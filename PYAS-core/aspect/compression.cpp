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
    // Create a FITS primary array containing a 2-D image               
    // declare axis arrays.    
    long naxis    =   2;      
    long naxes[2] = {size.width, size.height };   
    
    // declare auto-pointer to FITS at function scope. Ensures no resources
    // leaked if something fails in dynamic allocation.
    std::auto_ptr<FITS> pFits(0);
      
    try
    {                
        // Create a new FITS object, specifying the data type and axes for the primary
        // image. Simultaneously create the corresponding file.
        
        // this image is unsigned short data, demonstrating the cfitsio extension
        // to the FITS standard.
        
        pFits.reset( new FITS(fileName , BYTE_IMG , naxis , naxes ) );
    }
    catch (FITS::CantCreate)
    {
          // ... or not, as the case may be.
          return -1;       
    }
    
    // references for clarity.
    
    long nelements(1); 
    
    
    // Find the total size of the array. 
    // this is a little fancier than necessary ( It's only
    // calculating naxes[0]*naxes[1]) but it demonstrates  use of the 
    // C++ standard library accumulate algorithm.
    
    nelements = size.width*size.height;
           
    // create a dummy row with a ramp. Create an array and copy the row to 
    // row-sized slices. [also demonstrates the use of valarray slices].   
    // also demonstrate implicit type conversion when writing to the image:
    // input array will be of type float.

    std::valarray<unsigned char> array(nelements);
    for (int m = 0; m < size.height; m++)
    {
	for (int n = 0; n < size.width; n++)
	{
	    array[size.width*m + n] = image.at<unsigned char>(m,n);     
	}
    }
   
    long  fpixel(1);
       
    //add two keys to the primary header, one long, one complex.
    
    long exposure(1500);
    std::complex<float> omega(std::cos(2*M_PI/3.),std::sin(2*M_PI/3));
    pFits->pHDU().addKey("EXPOSURE", exposure,"Total Exposure Time"); 
    pFits->pHDU().addKey("OMEGA",omega," Complex cube root of 1 ");  

    
    // The function PHDU& FITS::pHDU() returns a reference to the object representing 
    // the primary HDU; PHDU::write( <args> ) is then used to write the data.
    
    pFits->pHDU().write(fpixel,nelements,array);
     
    // PHDU's friend ostream operator. Doesn't print the entire array, just the
    // required & user keywords, and is provided largely for testing purposes [see 
    // readImage() for an example of how to output the image array to a stream].
    
    std::cout << pFits->pHDU() << std::endl;

    return 0;
}
