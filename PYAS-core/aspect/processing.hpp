#include <opencv.hpp>
#include <vector>
#include <list>
#include <opencv.hpp>
#include <cstring>

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

class Circle : public cv::Vec3f
{
public:
    Circle() : cv::Vec3f(){};
    Circle(float x, float y, float r) : cv::Vec3f(x,y,r){};
    cv::Point2f center() {return cv::Point2f(cv::Vec3f::operator[](0), 
                                             cv::Vec3f::operator[](1)); }
    float x() {return cv::Vec3f::operator[](0); }
    float y() {return cv::Vec3f::operator[](1); }
    float r() {return cv::Vec3f::operator[](2); }
};

class CircleList : public std::vector<Circle>
{
public:
    void add(float x, float y, float r) {this->push_back(Circle(x, y, r)); }
    void add(cv::Point2f c, float r) {this->push_back(Circle(c.x, c.y, r)); }
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
const char * GetMessage(const AspectCode& code);

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
    AspectCode GetFiducialPairs(IndexList& rowPairs, IndexList& colPairs);
    AspectCode GetFiducialIDs(IndexList& fiducialIDs);
    AspectCode GetMapping(std::vector<float>& map);
    AspectCode GetScreenCenter(cv::Point2f& center);
    AspectCode GetScreenFiducials(CoordList& fiducials);
    
    float GetFloat(FloatParameter variable);
    int GetInteger(IntParameter variable);
    void SetFloat(FloatParameter, float value);
    void SetInteger(IntParameter, int value);

    AspectCode ReportFocus();

private:
    AspectCode state;

    int initialNumChords;
    int chordsPerAxis;
    float chordThreshold;
    int limbWidth;

    int solarRadius;
    float radiusTol;

    int fiducialLength;
    int fiducialWidth;

    int fiducialThreshold;

    int fiducialNeighborhood;
    int numFiducials;
    
    float fiducialSpacing;
    float fiducialSpacingTol;
    std::vector<float> mDistances, nDistances;
    
    void GenerateKernel();
    int FindLimbCrossings(cv::Mat chord, std::vector<float> &crossings);
    void FindPixelCenter();
    void FindPixelFiducials(cv::Mat image, cv::Point offset);
    void FindFiducialIDs();
    void FindMapping();
    cv::Point2f PixelToScreen(cv::Point2f point);

    cv::Mat frame;
    cv::Size frameSize;

    unsigned char frameMax, frameMin;

    cv::Mat kernel;
    cv::Size kernelSize;

    CoordList limbCrossings;

    cv::Point2f pixelCenter;
    cv::Point2f pixelError;
    
    CoordList pixelFiducials;

    IndexList rowPairs, colPairs;
    IndexList fiducialIDs;

    std::vector<float> conditionNumbers;
    std::vector<float> mapping;

    std::list<float> slopes;
};

cv::Range SafeRange(int start, int stop, int size);

void LinearFit(const std::vector<float> &x, const std::vector<float> &y, std::vector<float> &fit);

void CircleFit(const std::vector<float> &x, const std::vector<float> &y, Circle &fit);
void CircleFit(const CoordList &points, Circle &fit);

cv::Point2f VectorToCircle(Circle circle, cv::Point2f point);
void VectorToCircle(Circle circle, CoordList points, CoordList vectors); 

cv::Point2f Mean(const CoordList &points);
float Mean(const std::vector<float> &d);
std::vector<float> Euclidian(CoordList& vectors);
float Euclidian(cv::Point2f d);
float Euclidian(cv::Point2f p1, cv::Point2f p2);

template <class T> std::vector<T> Mode(std::vector<T> data);
