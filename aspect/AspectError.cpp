#include "AspectError.hpp"

//Functions for printing or categorizing Aspect error codes

AspectCode GeneralizeError(AspectCode code)
{
    switch(code)
    {
    case NO_ERROR:
        return NO_ERROR;

    case MAPPING_ERROR:
    case MAPPING_ILL_CONDITIONED:
        return MAPPING_ERROR;

    case ID_ERROR:
    case FEW_IDS:
    case NO_IDS:
        return ID_ERROR;

    case FIDUCIAL_ERROR:
    case FEW_FIDUCIALS:
    case NO_FIDUCIALS:
    case SOLAR_IMAGE_ERROR:
    case SOLAR_IMAGE_OFFSET_OUT_OF_BOUNDS:
    case SOLAR_IMAGE_SMALL:
    case SOLAR_IMAGE_EMPTY:
        return FIDUCIAL_ERROR;

    case CENTER_ERROR:
    case CENTER_ERROR_LARGE:
    case CENTER_OUT_OF_BOUNDS:
        return CENTER_ERROR;

    case LIMB_ERROR:
    case FEW_LIMB_CROSSINGS:
    case NO_LIMB_CROSSINGS:
        return LIMB_ERROR;

    case RANGE_ERROR:
    case MIN_MAX_BAD:
    case DYNAMIC_RANGE_LOW:
        return RANGE_ERROR;

    case FRAME_EMPTY:
        return FRAME_EMPTY;
    case STALE_DATA:
        return STALE_DATA;
    default:
        return STALE_DATA;
    }
    return STALE_DATA;
}



const char * GetMessage(const AspectCode& code)
{
    switch(code)
    {
    case NO_ERROR:
        return "No error";
        break;
    case MAPPING_ERROR:
        return "Generic Mapping error";
        
    case MAPPING_ILL_CONDITIONED:
        return "Mapping was ill-conditioned";
        
    case ID_ERROR:
        return "Generic IDing error";
            
    case FEW_IDS:
        return "Too few valid IDs";
        
    case NO_IDS:
        return "No valid IDs found";
        
    case FIDUCIAL_ERROR:
        return "Generic fiducial error";
        
    case FEW_FIDUCIALS:
        return "Too Few Fiducials";
        
    case NO_FIDUCIALS:
        return "No fiducials found";
        
    case SOLAR_IMAGE_ERROR:
        return "Generic solar image error";
        
    case SOLAR_IMAGE_OFFSET_OUT_OF_BOUNDS:
        return "Solar image offset is out of bounds";
        
    case SOLAR_IMAGE_SMALL:
        return "Solar image is too small";
        
    case SOLAR_IMAGE_EMPTY:
        return "Solar image is empty";
        
    case CENTER_ERROR:
        return "Generic error with pixel center";
        
    case CENTER_ERROR_LARGE:
        return "Pixel center error is too large";
        
    case CENTER_OUT_OF_BOUNDS:
        return "Pixel center is out of bounds";
        
    case LIMB_ERROR:
        return "Generic limb error";
        
    case FEW_LIMB_CROSSINGS:
        return "Too few limb crossings";
        
    case NO_LIMB_CROSSINGS:
        return "No limb crossings";
        
    case RANGE_ERROR:
        return "Generic dynamic range error";
        
    case DYNAMIC_RANGE_LOW:
        return "dynamic range is too low";
        
    case MIN_MAX_BAD:
        return "Dynamic values aren't real";
        
    case FRAME_EMPTY:
        return "Frame is empty.";
        
    case STALE_DATA:
        return "Data is stale.";
        
    default:
        return "How did I get here?";
    }
}
