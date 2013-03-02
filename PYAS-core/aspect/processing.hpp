#include <opencv.hpp>
#include <vector>
#include <opencv.hpp>

class PointList : public std::vector<cv::Point2f>
{
  public:
    void add(float x, float y) { this->push_back(cv::Point2f(x, y)); }
};

class Aspect
{
public:
    Aspect();
    ~Aspect();

    void LoadFrame(cv::Mat inputFrame);
    void GetPixelCrossings(PointList& crossings);
    void GetPixelCenter(cv::Point2f& center);
    void GetPixelFiducials(PointList& fiducials);

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
    
    
    void FindLimbCrossings(std::vector<int> rows, std::vector<int> cols);
    int EvaluateChord(cv::Mat chord, std::vector<float> &crossings);    
    void FindPixelCenter();
    void FindPixelFiducials();
    void IdentifyFiducials();
    void LoadKernel();
    cv::Range GetSafeRange(int start, int stop, int size);
    

    cv::Mat frame;
    cv::Size frameSize;

    PointList limbCrossings;

    cv::Point2f pixelCenter;
    cv::Point2f pixelError;
    
    cv::Mat solarImage;
    cv::Size solarSize;
    cv::Mat kernel;
    cv::Size kernelSize;

    PointList pixelFiducials;
};

    



int matchFindFiducials(cv::InputArray, cv::InputArray, int , cv::Point2f*, int);
void matchKernel(cv::OutputArray);
