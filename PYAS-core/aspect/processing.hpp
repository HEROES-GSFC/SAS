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
    InitialNumChords = 0,
    ChordsPerAxis = 1,
    LimbWidth = 2,
    FiducialTolerance = 3,
    SolarRadius = 4,
    FiducialLength = 5,
    FiducialWidth = 6,
    FiducialThreshold = 7,
    FiducialNeighborhood = 8,
    NumFiducials = 9
};

enum FloatParameter
{
    FiducialSpacing = 0,
    FiducialSpacingTol = 1
};
    

class Aspect
{
public:
    Aspect();
    ~Aspect();

    int LoadFrame(cv::Mat inputFrame);
    int Run();
    int GetPixelCrossings(CoordList& crossings);
    int GetPixelCenter(cv::Point2f& center);
    int GetPixelError(cv::Point2f& error);
    int GetPixelFiducials(CoordList& fiducials);
    int GetFiducialIDs(IndexList& fiducialIDs);
    int GetMapping(std::vector<float>& map);
    int GetScreenCenter(cv::Point2f& center);
    int GetScreenFiducials(CoordList& fiducials);
    
    float GetFloat(FloatParameter variable);
    int GetInteger(IntParameter variable);
    void SetFloat(FloatParameter, float value);
    void SetInteger(IntParameter, int value);


private:
    int initialNumChords;
    int chordsPerAxis;
    int chordThreshold;
    int limbWidth;
    int fiducialTolerance;

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
