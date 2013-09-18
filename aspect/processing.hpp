#include <opencv.hpp>
#include <vector>
#include <list>
#include <cstring>
#include "AspectError.hpp"
#include "AspectParameter.hpp"

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

class Aspect
{
public:
    Aspect();
    ~Aspect();

    AspectCode LoadFrame(cv::Mat inputFrame);
    AspectCode Run();
    AspectCode FiducialRun();

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
    


    float GetFloat(AspectFloat variable);
    int GetInteger(AspectInt variable);
    void SetFloat(AspectFloat, float value);
    void SetInteger(AspectInt, int value);

    AspectCode ReportFocus();

private:
    AspectCode state;

    int initialNumChords;
    int chordsPerAxis;
    float limbThreshold;
    float diskThreshold;
    int minLimbWidth;
    int limbFitWidth;

    float errorLimit;

    int solarRadius;
    float radiusMargin;

    int fiducialLength;
    int fiducialWidth;

    int fiducialThreshold;

    int fiducialNeighborhood;
    int numFiducials;
    
    float fiducialSpacing;
    float fiducialSpacingTol;
    float fiducialTwist;
    std::vector<float> mDistances, nDistances;
    
    void GenerateKernel();
    int FindLimbCrossings(const cv::Mat &chord, std::vector<float> &crossings);
    void FindPixelCenter();
    void FindPixelFiducials();
    void FindFiducialIDs();
    void FindMapping();
    cv::Point2f PixelToScreen(cv::Point2f point);

    cv::Mat frame;
    cv::Size frameSize;

    cv::Mat solarImage;
    cv::Size solarImageSize;
    cv::Point2i solarImageOffset;

    unsigned char frameMax, frameMin;

    cv::Mat kernel;
    
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

cv::Point2f rotate(float angle, cv::Point2f point);
void rotate(float angle, const CoordList &inPoints, CoordList &outPoints); 

//This calculates the min/max of an image after ignoring the extremes of the
//histogram (approximately the 0.5% on each end)
void calcMinMax(cv::Mat frame, unsigned char& min, unsigned char& max);

cv::Point2f fiducialIDtoScreen(cv::Point2i id);
