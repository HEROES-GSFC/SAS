//Parameter codes for Aspect module

enum AspectInt
{
    NUM_CHORDS_SEARCHING = 0,
    NUM_CHORDS_OPERATING,
    MIN_LIMB_WIDTH,
    LIMB_FIT_WIDTH,
    SOLAR_RADIUS,
    FIDUCIAL_LENGTH,
    FIDUCIAL_WIDTH,
    FIDUCIAL_NEIGHBORHOOD,
    NUM_FIDUCIALS
};

enum AspectFloat
{
    CHORD_THRESHOLD = 0,
    ERROR_LIMIT,
    RADIUS_MARGIN,
    FIDUCIAL_THRESHOLD,
    FIDUCIAL_SPACING,
    FIDUCIAL_SPACING_TOL,
    FIDUCIAL_TWIST
};

const char *GetAspectIntName(const AspectInt& code);
const char *GetAspectFloatName(const AspectFloat& code);
