#include "AspectParameter.hpp"

const char * GetAspectIntName(const AspectInt& code)
{
    switch(code)
    {
    case NUM_CHORDS_SEARCHING:
        return "# of Chords (search)";
        
    case NUM_CHORDS_OPERATING:
        return "# of Chords (locked)";
        
    case MIN_LIMB_WIDTH:
        return "Min Limb Width";

    case LIMB_FIT_WIDTH:
        return "Limb Fit Nbhd";

    case SOLAR_RADIUS:
        return "Solar Radius";

    case FIDUCIAL_LENGTH:
        return "Fiducial Length";

    case FIDUCIAL_WIDTH:
        return "Fiducial Width";

    case FIDUCIAL_NEIGHBORHOOD:
        return "Fiducial Nbhd";

    case NUM_FIDUCIALS:
        return "Max # Fiducials";

    default:
        return "How did I get here?";
    }
}

const char * GetAspectFloatName(const AspectFloat& code)
{
    switch(code)
    {
    case CHORD_THRESHOLD:
        return "Limb Threshold";

    case ERROR_LIMIT:
        return "Center Error Limit";

    case RADIUS_MARGIN:
        return "Radius Margin";

    case FIDUCIAL_THRESHOLD:
        return "Fiducial Threshold";

    case FIDUCIAL_SPACING:
        return "Fiducial Spacing";

    case FIDUCIAL_SPACING_TOL:
        return "Fiducial Spacing Tol.";

    case FIDUCIAL_TWIST:
        return "Fiducial Twist";

    default:
        return "How did I get here?";
    }
}
