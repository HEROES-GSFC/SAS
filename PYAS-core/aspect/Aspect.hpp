#include <opencv.hpp>

class Aspect
{
public:
    Aspect();
    ~Aspect();

    void LoadFrame();
    void FindLimbCrossings(cv::Point chords);
    void FindPixelCenter();
    void FindPixelFiducials();
    void IdentifyFiducials();

private:
    int chordThreshold;
    int chordWidth;

    int solarRadius;

    int fiducialLength;
    int fiducialWidth;
    int fiducialThreshold;
    

    void EvaluateChord(cv::Mat chord);
    cv::Mat GetSubframe(cv::Point offset, cv::Size size);
    void LoadKernel();

    cv::Mat frame;

    bool crossingsValid;
    cv::Point limbCrossings;
    int numCrossings;

    bool centerValid;
    cv::Point2f pixelCenter;
    cv::Point2f pixelError;
    
    bool solarImageValid;
    cv::Mat solarImage;
    cv::Mat kernel;

    bool fiducialsValid;
    cv::Point2f pixelFiducials;
    int numFiducials;
};

    
