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

class Aspect
{
public:
    Aspect();
    ~Aspect();

    void LoadFrame(cv::Mat inputFrame);
    void GetPixelCrossings(CoordList& crossings);
    void GetPixelCenter(cv::Point2f& center);
    void GetPixelError(cv::Point2f& error);
    void GetPixelFiducials(CoordList& fiducials);
    void GetFiducialIDs(IndexList& fiducialIDs);
    void GetScreenCenter(cv::Point2f& center);

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
    cv::Point2f PixelToScreen(cv::Point2f point);

    cv::Range GetSafeRange(int start, int stop, int size);
//    void LoadKernel();

    cv::Mat frame;
    cv::Size frameSize;

    cv::Mat kernel;
    cv::Size kernelSize;

    bool centerValid;
    CoordList limbCrossings;
    cv::Point2f pixelCenter;
    cv::Point2f pixelError;
    
    bool fiducialsValid;
    CoordList pixelFiducials;

    bool fiducialIDsValid;
    IndexList fiducialIDs;
};

void GetLinearFit(const std::vector<float> &x, const std::vector<float> &y, std::vector<float> &fit);
int matchFindFiducials(cv::InputArray, cv::InputArray, int , cv::Point2f*, int);
void matchKernel(cv::OutputArray);
