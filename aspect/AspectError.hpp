//Codes returned by the Aspect module.
//   A code of 0 indicates error. Codes are ranked by increasing severity:
//   MAPPING_ERROR has a lower value than FRAME_EMPTY because a missing
//   frame happens earlier in the processing chain than an error with
//   the mapping
//

enum AspectCode
{
    NO_ERROR = 0,

    MAPPING_ERROR,
    MAPPING_ILL_CONDITIONED,

    ID_ERROR,
    FEW_IDS,
    NO_IDS,
    FIDUCIAL_ERROR,
    FEW_FIDUCIALS,
    NO_FIDUCIALS,

    SOLAR_IMAGE_ERROR,
    SOLAR_IMAGE_OFFSET_OUT_OF_BOUNDS,
    SOLAR_IMAGE_SMALL,
    SOLAR_IMAGE_EMPTY,

    CENTER_ERROR,
    CENTER_ERROR_LARGE,
    CENTER_OUT_OF_BOUNDS,

    LIMB_ERROR,
    FEW_LIMB_CROSSINGS,
    NO_LIMB_CROSSINGS,

    RANGE_ERROR,
    DYNAMIC_RANGE_LOW,
    MIN_MAX_BAD,

    FRAME_EMPTY,

    STALE_DATA
};

// GeneralizeError lumps error codes together into broad categories
// Idea is to use this to check which data products are valid.
AspectCode GeneralizeError(AspectCode code);

//  Translates an error code into a human-readable message
const char * GetMessage(const AspectCode& code);
