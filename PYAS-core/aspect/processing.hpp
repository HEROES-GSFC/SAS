#include <opencv.hpp>
#include <vector>
#include <opencv.hpp>

class CoordList : public std::vector<cv::Point2f>
{
public:
    void add(float x, float y) { this->push_back(cv::Point2f(x, y)); }
};

class IndexList : public std::vector<cv::Point>
{
public:
    void add(int x, int y) { this->push_back(cv::Point(x, y)); }
};

enum IntParameter
{
    NUM_CHORDS_SEARCHING = 0,
    NUM_CHORDS_OPERATING,
    LIMB_WIDTH,
    SOLAR_RADIUS,
    FIDUCIAL_LENGTH,
    FIDUCIAL_WIDTH,
    FIDUCIAL_NEIGHBORHOOD,
    NUM_FIDUCIALS
};

enum FloatParameter
{
    CHORD_THRESHOLD = 0,
    FIDUCIAL_THRESHOLD,
    FIDUCIAL_SPACING,
    FIDUCIAL_SPACING_TOL,
};
    
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

AspectCode GeneralizeError(AspectCode code);
std::ostream& operator<<(std::ostream&, const AspectCode& code);

class Aspect
{
public:
    Aspect();
    ~Aspect();

    AspectCode LoadFrame(cv::Mat inputFrame);
    AspectCode Run();
    AspectCode GetPixelMinMax(unsigned char& min, unsigned char& max);
    AspectCode GetPixelCrossings(CoordList& crossings);
    AspectCode GetPixelCenter(cv::Point2f& center);
    AspectCode GetPixelError(cv::Point2f& error);
    AspectCode GetPixelFiducials(CoordList& fiducials);
    AspectCode GetFiducialIDs(IndexList& fiducialIDs);
    AspectCode GetMapping(std::vector<float>& map);
    AspectCode GetScreenCenter(cv::Point2f& center);
    AspectCode GetScreenFiducials(CoordList& fiducials);
    
    float GetFloat(FloatParameter variable);
    int GetInteger(IntParameter variable);
    void SetFloat(FloatParameter, float value);
    void SetInteger(IntParameter, int value);


private:
    AspectCode state;

    int initialNumChords;
    int chordsPerAxis;
    float chordThreshold;
    int limbWidth;

    int solarRadius;

    int fiducialLength;
    int fiducialWidth;

    int fiducialThreshold;

    int fiducialNeighborhood;
    int numFiducials;
    
    float fiducialSpacing;
    float fiducialSpacingTol;
    std::vector<float> mDistances, nDistances;
    
    int FindLimbCrossings(cv::Mat chord, std::vector<float> &crossings);
    void FindPixelCenter();
    void FindPixelFiducials(cv::Mat image, cv::Point offset);
    void FindFiducialIDs();
    void FindMapping();
    cv::Point2f PixelToScreen(cv::Point2f point);
    
//    void LoadKernel();

    bool frameValid;
    cv::Mat frame;
    cv::Size frameSize;

    bool minMaxValid;
    unsigned char frameMax, frameMin;

    cv::Mat kernel;
    cv::Size kernelSize;

    bool crossingsValid;
    CoordList limbCrossings;

    bool centerValid;
    cv::Point2f pixelCenter;
    cv::Point2f pixelError;
    
    bool fiducialsValid;
    CoordList pixelFiducials;

    bool fiducialIDsValid;
    IndexList fiducialIDs;

    bool mappingValid;
    std::vector<float> conditionNumbers;
    std::vector<float> mapping;

    bool frameProcessed;
};


cv::Range SafeRange(int start, int stop, int size);
void LinearFit(const std::vector<float> &x, const std::vector<float> &y, std::vector<float> &fit);
int matchFindFiducials(cv::InputArray, cv::InputArray, int , cv::Point2f*, int);
void matchKernel(cv::OutputArray);
