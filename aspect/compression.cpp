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
        std::cerr << "Image dimension is 0. Not saving." << std::endl;
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
        std::cerr << "OpenCV Error: " << e.err << std::endl;
        std::cerr << "    In file: " << e.file << std::endl;
        std::cerr << "    on line: " << e.line << std::endl;
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
        std::cerr << "Image dimension is 0. Not saving." << std::endl;
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
    catch (FITS::CantOpen)
    {
        // ... or not, as the case may be.
        std::cerr << "Could not open file to save FITS image\n";
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
        std::cerr << "Error while creating image extension\n";
        std::cerr << e.message() << "\n";
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
        pFits->pHDU().addKey("WAVE_STR", std::string("630 Nm"), "Wavelength of observation string");
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
    pFits->pHDU().addKey("BITPIX", (long) 8,"Bit depth of image");
    pFits->pHDU().addKey("WAVEUNIT", std::string("angstrom"), "Units of WAVELNTH");
    pFits->pHDU().addKey("PIXLUNIT", std::string("DN"), "Pixel units");
    pFits->pHDU().addKey("IMG_TYPE", std::string("LIGHT"), "Image type");
    pFits->pHDU().addKey("RSUN_REF", (double)6.9600000e+08, "");
    pFits->pHDU().addKey("CTLMODE", std::string("LIGHT"), "Image type");
    pFits->pHDU().addKey("LVL_NUM", (int) 0 , "Level of data");

    pFits->pHDU().addKey("RSUN_OBS", (double) 0, "");

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

    timeKey = asctime(gmtime(&(keys.captureTime).tv_sec));
    pFits->pHDU().addKey("EXPTIME", (float)keys.exposure/1e6, "Exposure time in seconds");
    pFits->pHDU().addKey("DATE_OBS", timeKey , "Date and time when observation of this image started (UTC)");
    pFits->pHDU().addKey("TEMPCCD", (float)keys.cameraTemperature, "Temperature of camera in Celsius");
    pFits->pHDU().addKey("TEMPCPU", (int)keys.cpuTemperature, "Temperature of cpu in Celsius");

    pFits->pHDU().addKey("FILENAME", fileName , "Name of the data file");
    //pFits->pHDU().addKey("TIME", 0 , "Time of observation in seconds within a day");

    pFits->pHDU().addKey("RT_SEC", (long)keys.captureTime.tv_sec , "Realtime clock, seconds");
    pFits->pHDU().addKey("RT_NSEC", (long)keys.captureTime.tv_nsec, "Realtime clock, nanoseconds");

    pFits->pHDU().addKey("MON_SEC", (long)keys.captureTimeMono.tv_sec , "Monotonic clock, seconds");
    pFits->pHDU().addKey("MON_NSEC", (long)keys.captureTimeMono.tv_nsec, "Monotonic clock, nanoseconds");

    pFits->pHDU().addKey("EXPOSURE", (int)keys.exposure,"Exposure time in usec");
    pFits->pHDU().addKey("GAIN_PRE", (int)keys.preampGain, "Preamp gain of CCD");
    pFits->pHDU().addKey("GAIN_ANA", (int)keys.analogGain, "Analog gain of CCD");
    pFits->pHDU().addKey("FRAMENUM", (long)keys.frameCount, "Frame number");
    pFits->pHDU().addKey("DATAMIN", (float)keys.imageMinMax[0], "Minimum value of data"); 
    pFits->pHDU().addKey("DATAMAX", (float)keys.imageMinMax[1], "Maximum value of data"); 

    if ((keys.cameraID == 1) || (keys.cameraID == 2)) {
        pFits->pHDU().addKey("F_TRACK", (bool)keys.isTracking, "Is SAS currently tracking?");
        pFits->pHDU().addKey("F_OUTPUT", (bool)keys.isOutputting, "Is this SAS outputting to CTL?");

        pFits->pHDU().addKey("GPS_LAT", (float)keys.latitude, "GPS latitude (degrees)");
        pFits->pHDU().addKey("GPS_LON", (float)keys.longitude, "GPS longitude (degrees)");

        pFits->pHDU().addKey("TARGET_X", (float)keys.solarTarget[0], "Intended target in HPC (arcsec)"); 
        pFits->pHDU().addKey("TARGET_Y", (float)keys.solarTarget[1], "Intended target in HPC (arcsec)"); 
        pFits->pHDU().addKey("CLOCKANG", (float)keys.clockingAngle, "CCW angle from screen +Y to vertical"); 
        pFits->pHDU().addKey("NORTHANG", (double)keys.northAngle, "CW angle from zenith to solar north"); 

        pFits->pHDU().addKey("SUNCENT1", (float)keys.sunCenter[0], "Calculated sun center in x-pixel"); 
        pFits->pHDU().addKey("SUNCENT2", (float)keys.sunCenter[1], "Calculated sun center in y-pixel"); 
        pFits->pHDU().addKey("CTL_AZIM", (double)keys.CTLsolution[0], "Azimuth offset for CTL (deg)");
        pFits->pHDU().addKey("CTL_ELEV", (double)keys.CTLsolution[1], "Elevation offset for CTL (deg)");
        pFits->pHDU().addKey("INTRCPT1", (float)keys.XYinterceptslope[0], "");
        pFits->pHDU().addKey("INTRCPT2", (float)keys.XYinterceptslope[1], "");
        pFits->pHDU().addKey("SLOPE1", (float)keys.XYinterceptslope[2], "");
        pFits->pHDU().addKey("SLOPE2", (float)keys.XYinterceptslope[3], "");

        pFits->pHDU().addKey("FID_NUM", keys.fiducialCount, "Number of fiducials");

        //Fiducial positions (pixels)
        for (int j = 0; j < 10; j++) {
            char tag[9];
            sprintf(tag, "FID%1d_-", j);
            tag[5] = 'X';
            pFits->pHDU().addKey(tag, (float)keys.fiducialX[j], "");
            tag[5] = 'Y';
            pFits->pHDU().addKey(tag, (float)keys.fiducialY[j], "");
        }

        //Fiducial IDs
        for (int j = 0; j < 10; j++) {
            char tag[9];
            sprintf(tag, "FID%1dID_-", j);
            tag[7] = 'X';
            pFits->pHDU().addKey(tag, (int)keys.fiducialIDX[j], "");
            tag[7] = 'Y';
            pFits->pHDU().addKey(tag, (int)keys.fiducialIDY[j], "");
        }

        pFits->pHDU().addKey("LIMB_NUM", keys.limbCount, "Number of limbs");

        //Limb positions (pixels)
        for (int j = 0; j < 10; j++) {
            char tag[9];
            sprintf(tag, "LIMB%1d_-", j);
            tag[6] = 'X';
            pFits->pHDU().addKey(tag, (float)keys.limbX[j], "");
            tag[6] = 'Y';
            pFits->pHDU().addKey(tag, (float)keys.limbY[j], "");
        }
    }

    // voltages
    pFits->pHDU().addKey("SBC_V105", (float)keys.cpuVoltage[0], "");
    pFits->pHDU().addKey("SBC_V25", (float)keys.cpuVoltage[1], "");
    pFits->pHDU().addKey("SBC_V33", (float)keys.cpuVoltage[2], "");
    pFits->pHDU().addKey("SBC_V50", (float)keys.cpuVoltage[3], "");
    pFits->pHDU().addKey("SBC_V120", (float)keys.cpuVoltage[4], "");

    try{
        imageExt->write(fpixel,nelements,array);
    }
    catch(FitsException e){
        std::cerr << "Exception while writing image to extension\n";
        std::cerr << e.message() << std::endl;
        return -1;
    }

    } catch (FitsError fe) {
        std::cerr << "Exception somewhere else in writeFITSImage()\n";
        std::cerr << fe.message() << std::endl;
        //throw fe;
    }

    return 0;

}

int readFITSImage(const std::string fileName, cv::OutputArray _image)
{
    try {

    cv::Size frameSize;
    std::auto_ptr<FITS> pInfile(new FITS(fileName,Read,true));
        
    //PHDU& fileHeader = pInfile->pHDU();
    ExtHDU& image = pInfile->extension("Raw Frame");

    frameSize.height = image.axis(1);
    frameSize.width = image.axis(0);

    std::valarray<unsigned char> contents;
    image.read(contents);
            
    cv::Mat frame(frameSize, CV_8UC1, &contents[0]);

    frame.copyTo(_image);

    } catch (FitsError fe) {
        std::cerr << "Exception somewhere in readFITSImage()\n";
        std::cerr << fe.message() << std::endl;
        return -1;
    } catch (FITS::NoSuchHDU fe) {
        std::cerr << "Missing HDU for frame!\n";
        return -1;
    }

    return 0;   
}

int readFITSHeader(const std::string fileName, HeaderData &keys)
{
    int value;
    try 
    {
        std::auto_ptr<FITS> pInfile(new FITS(fileName,Read,true));   
        PHDU& fileHeader = pInfile->pHDU();
  
        fileHeader.readKey("RT_SEC", value); //Realtime clock, seconds
        keys.captureTime.tv_sec = value;
        fileHeader.readKey("RT_NSEC", value); //Realtime clock, nanoseconds
        keys.captureTime.tv_nsec = value;
        fileHeader.readKey("MON_SEC", value); //Monotonic clock, seconds
        keys.captureTimeMono.tv_sec = value;
        fileHeader.readKey("MON_NSEC", value); //Monotonic clock, nanoseconds
        keys.captureTimeMono.tv_nsec = value;

    } 
    catch (FitsError fe) 
    {
        std::cerr << "Exception somewhere in readFITSImage()\n";
        std::cerr << fe.message() << std::endl;
        return -1;
    }
    return 0;
}
