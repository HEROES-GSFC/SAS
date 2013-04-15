#include "compression.hpp"
#include <CCfits>
#include <cmath>
#include <valarray>
#include <vector>

#define ARCSEC_IN_MILS 2.62

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
    try{
        imageExt = pFits->addImage(newName, BYTE_IMG, extAx, 1);
    }
    catch(FitsError e){
        std::cout << e.message() << "\n";
        return -1;
    }

    nelements = size.width*size.height*sizeof(unsigned char);
           
    std::valarray<unsigned char> array(image.data, nelements);

    long  fpixel(1);
    
    timeKey = asctime(gmtime(&(keys.captureTime).tv_sec));
    //add keys to the primary header
    pFits->pHDU().addKey("TELESCOP",std::string("HEROES/SAS"),"Name of source telescope package");
//    pFits->pHDU().addKey("SIMPLE",(int)1,"always T for True, if conforming FITS file");
    
    if( keys.cameraID == 1 ){
        pFits->pHDU().addKey("INSTRUME",std::string("PYAS-F"),"Name of instrument");
        pFits->pHDU().addKey("ORIGIN", std::string("HEROES/SAS-1") , "Location where file was made");
        pFits->pHDU().addKey("WAVELNTH", (long)6300, "Wavelength of observation (ang)");
        pFits->pHDU().addKey("WAVE_STR", std::string("630 Nm"), "Wavelength of observation (ang)");
    }
    if( keys.cameraID == 2 ){
        pFits->pHDU().addKey("INSTRUME",std::string("PYAS-R"),"Name of instrument");
        pFits->pHDU().addKey("ORIGIN", std::string("HEROES/SAS-2") , "Location where file was made");
        pFits->pHDU().addKey("WAVELNTH", (long)6300, "Wavelength of observation (ang)");
        pFits->pHDU().addKey("WAVE_STR", std::string("630 Nm"), "Wavelength of observation string");
    }
    if( keys.cameraID == 3 ){
        pFits->pHDU().addKey("INSTRUME",std::string("RAS"),"Name of instrument");
        pFits->pHDU().addKey("ORIGIN", std::string("HEROES/SAS-2") , "Location where file was made");
        pFits->pHDU().addKey("WAVELNTH", (long)6000, "Wavelength of observation (ang)");
        pFits->pHDU().addKey("WAVE_STR", std::string("600 Nm"), "Wavelength of observation string");
    }
    pFits->pHDU().addKey("BITPIX", (long)8,"Bit depth of image");
    pFits->pHDU().addKey("WAVEUNIT", std::string("angstrom"), "Units of WAVELNTH");
    pFits->pHDU().addKey("PIXLUNIT", std::string("DN"), "Pixel units");
    pFits->pHDU().addKey("IMG_TYPE", std::string("LIGHT"), "Image type");
    pFits->pHDU().addKey("RSUN_REF", (double)6.9600000e+08, "Units of WAVELNTH");
    pFits->pHDU().addKey("CTLMODE", std::string("LIGHT"), "Image type");
    pFits->pHDU().addKey("LVL_NUM", 0 , "Level of data");

    pFits->pHDU().addKey("RSUN_OBS", 0, "Units of WAVELNTH");
     
    pFits->pHDU().addKey("CTYPE1", std::string("HPLN-TAN"), "A string value labeling each coordinate axis");
    pFits->pHDU().addKey("CTYPE2", std::string("HPLN-TAN"), "A string value labeling each coordinate axis");
    pFits->pHDU().addKey("CUNIT1", std::string("arcsec"), "Coordinate Units");
    pFits->pHDU().addKey("CUNIT2", std::string("arcsec"), "Coordinate Units");
    pFits->pHDU().addKey("CRVAL1", (double)0.0, "Coordinate value of the reference pixel");
    pFits->pHDU().addKey("CRVAL2", (double)0.0, "Coordinate value of the reference pixel");
    pFits->pHDU().addKey("CDELT1", (double)keys.XYinterceptslope[2]/ARCSEC_IN_MILS, "Plate scale");
    pFits->pHDU().addKey("CDELT2", (double)keys.XYinterceptslope[3]/ARCSEC_IN_MILS, "Plate scale");
    pFits->pHDU().addKey("CRPIX1", (double)keys.sunCenter[0]+1, "Reference pixel");
    pFits->pHDU().addKey("CRPIX2", (double)keys.sunCenter[1]+1, "Reference pixel");
    
    pFits->pHDU().addKey("EXPTIME", (long)keys.exposure * 1000, "Exposure time in seconds"); 
    pFits->pHDU().addKey("DATE_OBS", timeKey , "Date and time when observation of this image started (UTC)");
    pFits->pHDU().addKey("TEMPCCD", keys.cameraTemperature, "Temperature of camera in celsius");
    pFits->pHDU().addKey("TEMPCPU", keys.cpuTemperature, "Temperature of cpu in celsius"); 

    pFits->pHDU().addKey("FILENAME", fileName , "Name of the data file");
    //pFits->pHDU().addKey("TIME", 0 , "Time of observation in seconds within a day");
    
    pFits->pHDU().addKey("DAY AND TIME", timeKey , "Frame Capture Time (UTC)");
    timeKey = nanoString((keys.captureTime).tv_nsec);
    pFits->pHDU().addKey("TIME-FRACTION", timeKey, "Frame capture fractional seconds");
    pFits->pHDU().addKey("EXPOSURE", (long)keys.exposure,"Exposure time in msec"); 
    pFits->pHDU().addKey("SUN-CENTER1", (double)keys.sunCenter[0], "Calculated sun center in x-pixel"); 
    pFits->pHDU().addKey("SUN-CENTER2", (double)keys.sunCenter[1], "Calculated sun center in y-pixel"); 
    pFits->pHDU().addKey("FRAMENUM", (double)keys.frameCount, "Frame number"); 
    pFits->pHDU().addKey("DATAMIN", (double)keys.imageMinMax[0], "Minimum value of data"); 
    pFits->pHDU().addKey("DATAMAX", (double)keys.imageMinMax[1], "Maximum value of data"); 
    pFits->pHDU().addKey("GAINPREAMP", (double)keys.preampGain, "Preamp gain of CCD");
    pFits->pHDU().addKey("GAINANALOG", (double)keys.analogGain, "Analog gain of CCD");
    pFits->pHDU().addKey("CTLSOLUTION1", (double)keys.CTLsolution[0], "Command for CTL X value");
    pFits->pHDU().addKey("CTLSOLUTION2", (double)keys.CTLsolution[1], "Command for CTL Y value");
    pFits->pHDU().addKey("INTERCEPT1", (double)keys.XYinterceptslope[0], "");
    pFits->pHDU().addKey("INTERCEPT2", (double)keys.XYinterceptslope[1], "");
    pFits->pHDU().addKey("SLOPE1", (double)keys.XYinterceptslope[2], "");
    pFits->pHDU().addKey("SLOPE2", (double)keys.XYinterceptslope[3], "");    

    unsigned long rows(10);
    string hduName("LIMB_SOLUTIONS");
    std::vector<string> colName(4,"");
    std::vector<string> colForm(4,"");
    std::vector<string> colUnit(4,"");
    
    /* define the name, datatype, and physical units for the 3 columns */    
    colName[0] = "X";
    colName[1] = "Y";
    colName[2] = "Xerror";
    colName[3] = "Yerror";
    colForm[0] = "f4.2";
    colForm[1] = "f4.2";
    colForm[2] = "f4.2";
    colForm[3] = "f4.2";
    colUnit[0] = "pixels";
    colUnit[1] = "pixels";
    colUnit[2] = "pixels";
    colUnit[3] = "pixels";

    Table *newTable = pFits->addTable(hduName,rows,colName,colForm,colUnit,AsciiTbl);    

    try{
        imageExt->write(fpixel,nelements,array);
    }
    catch(FitsException e){
        std::cout << e.message();
        return -1;
    }
    try{
        newTable->column(colName[0]).write(keys.limbsX,1);  
        newTable->column(colName[1]).write(keys.limbsY,rows,1);
        newTable->column(colName[2]).write(keys.limbsXerror,1);  
        newTable->column(colName[3]).write(keys.limbsYerror,rows,1);
    }
    catch(FitsException e){
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
