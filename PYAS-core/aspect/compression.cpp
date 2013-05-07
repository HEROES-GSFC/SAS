#include "compression.hpp"
#include <CCfits>
#include <cmath>
#include <valarray>
#include <vector>

#define ARCSEC_PER_MIL 1.72

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
    try {

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

//    pFits->setCompressionType(RICE_1);

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
    if( keys.cameraID == 6 ){
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
    pFits->pHDU().addKey("CDELT1", (double)keys.XYinterceptslope[2] * ARCSEC_PER_MIL, "Plate scale");
    pFits->pHDU().addKey("CDELT2", (double)keys.XYinterceptslope[3] * ARCSEC_PER_MIL, "Plate scale");
    pFits->pHDU().addKey("CRPIX1", (double)keys.sunCenter[0]+1, "Reference pixel");
    pFits->pHDU().addKey("CRPIX2", (double)keys.sunCenter[1]+1, "Reference pixel");

    pFits->pHDU().addKey("EXPTIME", (float)keys.exposure/1e6, "Exposure time in seconds"); 
    pFits->pHDU().addKey("DATE_OBS", timeKey , "Date and time when observation of this image started (UTC)");
    pFits->pHDU().addKey("TEMPCCD", keys.cameraTemperature, "Temperature of camera in celsius");
    pFits->pHDU().addKey("TEMPCPU", keys.cpuTemperature, "Temperature of cpu in celsius"); 

    pFits->pHDU().addKey("FILENAME", fileName , "Name of the data file");
    //pFits->pHDU().addKey("TIME", 0 , "Time of observation in seconds within a day");

    timeKey = asctime(gmtime(&(keys.captureTime).tv_sec));
    pFits->pHDU().addKey("DAY AND TIME", timeKey , "Frame Capture Time (UTC)");
    pFits->pHDU().addKey("TIME-FRACTION", (long)(keys.captureTime).tv_nsec, "Frame capture fractional seconds");

    timeKey = asctime(gmtime(&(keys.captureTimeMono).tv_sec));
    pFits->pHDU().addKey("DAY AND TIME MONO", timeKey , "Frame Capture Time (UTC)");
    pFits->pHDU().addKey("TIME-FRACTION MONO", (long)(keys.captureTimeMono).tv_nsec, "Frame capture fractional seconds");

    pFits->pHDU().addKey("EXPOSURE", (long)keys.exposure,"Exposure time in msec"); 
    pFits->pHDU().addKey("SUN-CENTER1", (float)keys.sunCenter[0], "Calculated sun center in x-pixel"); 
    pFits->pHDU().addKey("SUN-CENTER2", (float)keys.sunCenter[1], "Calculated sun center in y-pixel"); 
    pFits->pHDU().addKey("FRAMENUM", (float)keys.frameCount, "Frame number"); 
    pFits->pHDU().addKey("DATAMIN", (float)keys.imageMinMax[0], "Minimum value of data"); 
    pFits->pHDU().addKey("DATAMAX", (float)keys.imageMinMax[1], "Maximum value of data"); 
    pFits->pHDU().addKey("GAINPREAMP", (float)keys.preampGain, "Preamp gain of CCD");
    pFits->pHDU().addKey("GAINANALOG", (float)keys.analogGain, "Analog gain of CCD");
    pFits->pHDU().addKey("CTLSOLUTION1", (float)keys.CTLsolution[0], "Command for CTL X value in degrees");
    pFits->pHDU().addKey("CTLSOLUTION2", (float)keys.CTLsolution[1], "Command for CTL Y value in degrees");
    pFits->pHDU().addKey("INTERCEPT1", (float)keys.XYinterceptslope[0], "");
    pFits->pHDU().addKey("INTERCEPT2", (float)keys.XYinterceptslope[1], "");
    pFits->pHDU().addKey("SLOPE1", (float)keys.XYinterceptslope[2], "");
    pFits->pHDU().addKey("SLOPE2", (float)keys.XYinterceptslope[3], "");    

    // fiducials X positions
    pFits->pHDU().addKey("FIDUCIALX0", (float)keys.fiducialX[0], "");
    pFits->pHDU().addKey("FIDUCIALX1", (float)keys.fiducialX[1], "");
    pFits->pHDU().addKey("FIDUCIALX2", (float)keys.fiducialX[2], "");
    pFits->pHDU().addKey("FIDUCIALX3", (float)keys.fiducialX[3], "");    
    pFits->pHDU().addKey("FIDUCIALX4", (float)keys.fiducialX[4], "");
    pFits->pHDU().addKey("FIDUCIALX5", (float)keys.fiducialX[5], "");
    pFits->pHDU().addKey("FIDUCIALX6", (float)keys.fiducialX[6], "");
    pFits->pHDU().addKey("FIDUCIALX7", (float)keys.fiducialX[7], "");    
    pFits->pHDU().addKey("FIDUCIALX8", (float)keys.fiducialX[8], "");
    pFits->pHDU().addKey("FIDUCIALX9", (float)keys.fiducialX[9], "");

    // fiducials Y positions
    pFits->pHDU().addKey("FIDUCIALY0", (float)keys.fiducialY[0], "");
    pFits->pHDU().addKey("FIDUCIALY1", (float)keys.fiducialY[1], "");
    pFits->pHDU().addKey("FIDUCIALY2", (float)keys.fiducialY[2], "");
    pFits->pHDU().addKey("FIDUCIALY3", (float)keys.fiducialY[3], "");    
    pFits->pHDU().addKey("FIDUCIALY4", (float)keys.fiducialY[4], "");
    pFits->pHDU().addKey("FIDUCIALY5", (float)keys.fiducialY[5], "");
    pFits->pHDU().addKey("FIDUCIALY6", (float)keys.fiducialY[6], "");
    pFits->pHDU().addKey("FIDUCIALY7", (float)keys.fiducialY[7], "");    
    pFits->pHDU().addKey("FIDUCIALY8", (float)keys.fiducialY[8], "");
    pFits->pHDU().addKey("FIDUCIALY9", (float)keys.fiducialY[9], "");

    // fiducials X IDs
    pFits->pHDU().addKey("FIDUCIALX0ID", (int)keys.fiducialIDX[0], "");
    pFits->pHDU().addKey("FIDUCIALX1ID", (int)keys.fiducialIDX[1], "");
    pFits->pHDU().addKey("FIDUCIALX2ID", (int)keys.fiducialIDX[2], "");
    pFits->pHDU().addKey("FIDUCIALX3ID", (int)keys.fiducialIDX[3], "");    
    pFits->pHDU().addKey("FIDUCIALX4ID", (int)keys.fiducialIDX[4], "");
    pFits->pHDU().addKey("FIDUCIALX5ID", (int)keys.fiducialIDX[5], "");
    pFits->pHDU().addKey("FIDUCIALX6ID", (int)keys.fiducialIDX[6], "");
    pFits->pHDU().addKey("FIDUCIALX7ID", (int)keys.fiducialIDX[7], "");    
    pFits->pHDU().addKey("FIDUCIALX8ID", (int)keys.fiducialIDX[8], "");
    pFits->pHDU().addKey("FIDUCIALX9ID", (int)keys.fiducialIDX[9], "");

    // fiducials Y IDs
    pFits->pHDU().addKey("FIDUCIALY0ID", (int)keys.fiducialIDY[0], "");
    pFits->pHDU().addKey("FIDUCIALY1ID", (int)keys.fiducialIDY[1], "");
    pFits->pHDU().addKey("FIDUCIALY2ID", (int)keys.fiducialIDY[2], "");
    pFits->pHDU().addKey("FIDUCIALY3ID", (int)keys.fiducialIDY[3], "");    
    pFits->pHDU().addKey("FIDUCIALY4ID", (int)keys.fiducialIDY[4], "");
    pFits->pHDU().addKey("FIDUCIALY5ID", (int)keys.fiducialIDY[5], "");
    pFits->pHDU().addKey("FIDUCIALY6ID", (int)keys.fiducialIDY[6], "");
    pFits->pHDU().addKey("FIDUCIALY7ID", (int)keys.fiducialIDY[7], "");    
    pFits->pHDU().addKey("FIDUCIALY8ID", (int)keys.fiducialIDY[8], "");
    pFits->pHDU().addKey("FIDUCIALY9ID", (int)keys.fiducialIDY[9], "");

    // Limb X
    pFits->pHDU().addKey("LIMBX0", (float)keys.limbX[0], "");
    pFits->pHDU().addKey("LIMBX1", (float)keys.limbX[1], "");
    pFits->pHDU().addKey("LIMBX2", (float)keys.limbX[2], "");
    pFits->pHDU().addKey("LIMBX3", (float)keys.limbX[3], "");    
    pFits->pHDU().addKey("LIMBX4", (float)keys.limbX[4], "");
    pFits->pHDU().addKey("LIMBX5", (float)keys.limbX[5], "");
    pFits->pHDU().addKey("LIMBX6", (float)keys.limbX[6], "");
    pFits->pHDU().addKey("LIMBX7", (float)keys.limbX[7], "");    
    pFits->pHDU().addKey("LIMBX8", (float)keys.limbX[8], "");
    pFits->pHDU().addKey("LIMBX9", (float)keys.limbX[9], "");

    // Limb Y
    pFits->pHDU().addKey("LIMBY0", (float)keys.limbY[0], "");
    pFits->pHDU().addKey("LIMBY1", (float)keys.limbY[1], "");
    pFits->pHDU().addKey("LIMBY2", (float)keys.limbY[2], "");
    pFits->pHDU().addKey("LIMBY3", (float)keys.limbY[3], "");    
    pFits->pHDU().addKey("LIMBY4", (float)keys.limbY[4], "");
    pFits->pHDU().addKey("LIMBY5", (float)keys.limbY[5], "");
    pFits->pHDU().addKey("LIMBY6", (float)keys.limbY[6], "");
    pFits->pHDU().addKey("LIMBY7", (float)keys.limbY[7], "");    
    pFits->pHDU().addKey("LIMBY8", (float)keys.limbY[8], "");
    pFits->pHDU().addKey("LIMBY9", (float)keys.limbY[9], "");

    // Limb X Error
    // pFits->pHDU().addKey("LIMBX0error", (float)keys.limbXerror[0], "");
//     pFits->pHDU().addKey("LIMBX1error", (float)keys.limbXerror[1], "");
//     pFits->pHDU().addKey("LIMBX2error", (float)keys.limbXerror[2], "");
//     pFits->pHDU().addKey("LIMBX3error", (float)keys.limbXerror[3], "");    
//     pFits->pHDU().addKey("LIMBX4error", (float)keys.limbXerror[4], "");
//     pFits->pHDU().addKey("LIMBX5error", (float)keys.limbXerror[5], "");
//     pFits->pHDU().addKey("LIMBX6error", (float)keys.limbXerror[6], "");
//     pFits->pHDU().addKey("LIMBX7error", (float)keys.limbXerror[7], "");    
//     pFits->pHDU().addKey("LIMBX8error", (float)keys.limbXerror[8], "");
//     pFits->pHDU().addKey("LIMBX9error", (float)keys.limbXerror[9], "");
//     
//     // Limb Y Error
//     pFits->pHDU().addKey("LIMBY0error", (float)keys.limbYerror[0], "");
//     pFits->pHDU().addKey("LIMBY1error", (float)keys.limbYerror[1], "");
//     pFits->pHDU().addKey("LIMBY2error", (float)keys.limbYerror[2], "");
//     pFits->pHDU().addKey("LIMBY3error", (float)keys.limbYerror[3], "");    
//     pFits->pHDU().addKey("LIMBY4error", (float)keys.limbYerror[4], "");
//     pFits->pHDU().addKey("LIMBY5error", (float)keys.limbYerror[5], "");
//     pFits->pHDU().addKey("LIMBY6error", (float)keys.limbYerror[6], "");
//     pFits->pHDU().addKey("LIMBY7error", (float)keys.limbYerror[7], "");    
//     pFits->pHDU().addKey("LIMBY8error", (float)keys.limbYerror[8], "");
//     pFits->pHDU().addKey("LIMBY9error", (float)keys.limbYerror[9], "");

    // voltages
    pFits->pHDU().addKey("SBC_VOLT105", (float)keys.cpuVoltage[0], "");
    pFits->pHDU().addKey("SBC_VOLT25", (float)keys.cpuVoltage[1], "");
    pFits->pHDU().addKey("SBC_VOLT33", (float)keys.cpuVoltage[2], "");
    pFits->pHDU().addKey("SBC_VOLT50", (float)keys.cpuVoltage[3], "");    
    pFits->pHDU().addKey("SBC_VOLT120", (float)keys.cpuVoltage[4], "");

    try{
        imageExt->write(fpixel,nelements,array);
    }
    catch(FitsException e){
        std::cout << e.message();
        return -1;
    }

    } catch (FitsError fe) {
        std::cout << "Exception somewhere else in writeFITSImage()\n";
        std::cout << fe.message() << std::endl;
        //throw fe;
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
