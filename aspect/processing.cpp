/* This code should encapsulate all the code necessary for generating solar aspect.
   center, fiducials, etc, as well as a local copy of the current frame. 

   The idea would be to call "LoadFrame" once, at which point this module would
   reset all its values. Next, the "Run" function computes all the data products possible.
   Requests for data are made with the Get functions, and the necessary data is provided if
   its available, otherwise an error code is returned. Most data is stored as either CoordList or a
   cv::Point. All the functions doing real computation are private, other than "Run."
*/
#include "processing.hpp"
#include "utilities.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cmath>

const float pi = std::atan(1.0)*4;

cv::Point2f fiducialIDtoScreen(cv::Point2i id) 
{
    cv::Point2f result;

    result.x = 6 * ((id.x >= 0 ? 45*id.x+3*id.x*(id.x-1) : 48*id.x-3*id.x*(id.x+1)) - 15*id.y);
    result.y = 6 * ((id.y >= 0 ? 45*id.y+3*id.y*(id.y-1) : 48*id.y-3*id.y*(id.y+1)) + 15*id.x);

    return result;
}

AspectCode GeneralizeError(AspectCode code)
{
    switch(code)
    {
    case NO_ERROR:
        return NO_ERROR;

    case MAPPING_ERROR:
    case MAPPING_ILL_CONDITIONED:
        return MAPPING_ERROR;

    case ID_ERROR:
    case FEW_IDS:
    case NO_IDS:
        return ID_ERROR;

    case FIDUCIAL_ERROR:
    case FEW_FIDUCIALS:
    case NO_FIDUCIALS:
    case SOLAR_IMAGE_ERROR:
    case SOLAR_IMAGE_OFFSET_OUT_OF_BOUNDS:
    case SOLAR_IMAGE_SMALL:
    case SOLAR_IMAGE_EMPTY:
        return FIDUCIAL_ERROR;

    case CENTER_ERROR:
    case CENTER_ERROR_LARGE:
    case CENTER_OUT_OF_BOUNDS:
        return CENTER_ERROR;

    case LIMB_ERROR:
    case FEW_LIMB_CROSSINGS:
    case NO_LIMB_CROSSINGS:
        return LIMB_ERROR;

    case RANGE_ERROR:
    case MIN_MAX_BAD:
    case DYNAMIC_RANGE_LOW:
        return RANGE_ERROR;

    case FRAME_EMPTY:
        return FRAME_EMPTY;
    case STALE_DATA:
        return STALE_DATA;
    default:
        return STALE_DATA;
    }
    return STALE_DATA;
}

const char * GetMessage(const AspectCode& code)
{
    switch(code)
    {
    case NO_ERROR:
        return "No error";
        break;
    case MAPPING_ERROR:
        return "Generic Mapping error";
        
    case MAPPING_ILL_CONDITIONED:
        return "Mapping was ill-conditioned";
        
    case ID_ERROR:
        return "Generic IDing error";
            
    case FEW_IDS:
        return "Too few valid IDs";
        
    case NO_IDS:
        return "No valid IDs found";
        
    case FIDUCIAL_ERROR:
        return "Generic fiducial error";
        
    case FEW_FIDUCIALS:
        return "Too Few Fiducials";
        
    case NO_FIDUCIALS:
        return "No fiducials found";
        
    case SOLAR_IMAGE_ERROR:
        return "Generic solar image error";
        
    case SOLAR_IMAGE_OFFSET_OUT_OF_BOUNDS:
        return "Solar image offset is out of bounds";
        
    case SOLAR_IMAGE_SMALL:
        return "Solar image is too small";
        
    case SOLAR_IMAGE_EMPTY:
        return "Solar image is empty";
        
    case CENTER_ERROR:
        return "Generic error with pixel center";
        
    case CENTER_ERROR_LARGE:
        return "Pixel center error is too large";
        
    case CENTER_OUT_OF_BOUNDS:
        return "Pixel center is out of bounds";
        
    case LIMB_ERROR:
        return "Generic limb error";
        
    case FEW_LIMB_CROSSINGS:
        return "Too few limb crossings";
        
    case NO_LIMB_CROSSINGS:
        return "No limb crossings";
        
    case RANGE_ERROR:
        return "Generic dynamic range error";
        
    case DYNAMIC_RANGE_LOW:
        return "dynamic range is too low";
        
    case MIN_MAX_BAD:
        return "Dynamic values aren't real";
        
    case FRAME_EMPTY:
        return "Frame is empty.";
        
    case STALE_DATA:
        return "Data is stale.";
        
    default:
        return "How did I get here?";
    }
}

Aspect::Aspect()
{
    // Initialize min and max values for the image
    frameMin = 255;
    frameMax = 0;
    
    // InitialNumChords is the number of chords per axis
    // to use when searching for the sun
    initialNumChords = 30;

    // chordsPerAxis is the number of chords per axis to use
    // when the sun has been found
    chordsPerAxis = 10;

    chordThreshold = .25;

    solarRadius = 98;
    radiusMargin = .25;
    errorLimit = 50;

    limbWidth = 2;
    fiducialLength = 15;
    fiducialWidth = 2; 
    fiducialThreshold = 5;

    numFiducials = 12;
    
    // fiducialSpacing is how far apart fiducial pairs are in one axis.
    // fiducialSpacingTol is how much slack to allow on this distance
    // Value measured in lab for fiducialSpacing is 15.7
    fiducialSpacing = 15.7;
    fiducialSpacingTol = 1.5;
    
    fiducialTwist = 0.0;

    pixelCenter = cv::Point2f(-1.0, -1.0);
    pixelError = cv::Point2f(0.0, 0.0);
    
    GenerateKernel();
    //matchKernel(kernel);
    mDistances.clear();
    nDistances.clear();
    for (int k = 0; k < 14; k++)
    {
        if(k < 7)
        {
            mDistances.push_back((84-k*6)*fiducialSpacing/15);
            nDistances.push_back((84-k*6)*fiducialSpacing/15);
        }
        else
        {
            mDistances.push_back((45 + (k-7)*6)*fiducialSpacing/15);
            nDistances.push_back((45 + (k-7)*6)*fiducialSpacing/15);
        }
    }
    mapping.resize(4);
    state = STALE_DATA;
}

Aspect::~Aspect()
{

}

AspectCode Aspect::LoadFrame(cv::Mat inputFrame)
{
    timespec duration;
    //std::cout << "Aspect: Loading Frame" << std::endl;
    if(inputFrame.empty())
    {
        //std::cout << "Aspect: Tried to load empty frame" << std::endl;
        state = FRAME_EMPTY;
        return state;
    }
    else
    {
        cv::Size inputSize = inputFrame.size();
        if (inputSize.width == 0 || inputSize.height == 0)
        {
            //std::cout << "Aspect: Tried to load a frame with dimension 0" << std::endl;
            state = FRAME_EMPTY;
            return state;
        }
        else
        {
            frame = inputFrame;
            frameSize = frame.size();

            state = NO_ERROR;
            return state;
        }
    }
}

AspectCode Aspect::Run()
{
    cv::Range rowRange, colRange;
    cv::Mat solarImage;
    cv::Size solarSize;
    cv::Point offset;
    double max, min;
    timespec heythere;
    limbCrossings.clear();    
    slopes.clear();    
    pixelFiducials.clear();
    fiducialIDs.clear();
    mapping.clear();
    mapping.resize(4);
    conditionNumbers.clear();
    conditionNumbers.resize(2);

    if (state == FRAME_EMPTY)
    {
        //std::cout << "Aspect: Frame is empty." << std::endl;
        state = FRAME_EMPTY;
        return state;
    }
    else
    {
        //std::cout << "Aspect: Finding max and min pixel values" << std::endl;
        cv::minMaxLoc(frame, &min, &max, NULL, NULL);
        frameMin = (unsigned char) min;
        frameMax = (unsigned char) max;
        if (min >= max || std::isnan(min) || std::isnan(max))
        {
            //std::cout << "Aspect: Max/Min value bad" << std::endl;
            state = MIN_MAX_BAD;
            return state;
        }
        else if(max - min < 32)
        {
            state = DYNAMIC_RANGE_LOW;
            return state;
        }
        //std::cout << "Aspect: Finding Center" << std::endl;
        FindPixelCenter();
        if (limbCrossings.size() == 0)
        {
            //std::cout << "Aspect: No Limb Crossings." << std::endl;
            state = NO_LIMB_CROSSINGS;
            pixelCenter = cv::Point2f(-1,-1);
            return state;
        }
        else if (limbCrossings.size() < 4)
        {
            //std::cout << "Aspect: Too Few Limb Crossings." << std::endl;
            state = FEW_LIMB_CROSSINGS;
            pixelCenter = cv::Point2f(-1,-1);
            return state;
        }

        if (pixelCenter.x < 0 || pixelCenter.x >= frameSize.width ||
            pixelCenter.y < 0 || pixelCenter.y >= frameSize.height ||
            !std::isfinite(pixelCenter.x) || !std::isfinite(pixelCenter.y))
        {
            std::cout << "Aspect: Center Out-of-bounds:" << pixelCenter << std::endl;
            pixelCenter = cv::Point2f(-1,-1);
            state = CENTER_OUT_OF_BOUNDS;
            return state;
        }
        else if (pixelError.x > errorLimit || pixelError.y > errorLimit || 
                 !std::isfinite(pixelError.x) || !std::isfinite(pixelError.y))
        {
            //std::cout << "Aspect: Pixel Error is above an arbitrary threshold: << pixelError << std::endl;
            pixelCenter = cv::Point2f(-1,-1);
            state = CENTER_ERROR_LARGE;
            return state;
        }

        //Find solar subImage
        //std::cout << "Aspect: Finding solar subimage" << std::endl;
        int subimageSize = solarRadius*(1+radiusMargin);

        rowRange = SafeRange(pixelCenter.y-subimageSize, 
                             pixelCenter.y+subimageSize, 
                             frameSize.height);
        colRange = SafeRange(pixelCenter.x-subimageSize, 
                             pixelCenter.x+subimageSize, 
                             frameSize.width);

        solarImage = frame(rowRange, colRange);

        if (solarImage.empty())
        {
            //std::cout << "Aspect: Solar Image too empty." << std::endl;
            state = SOLAR_IMAGE_EMPTY;
            return state;  
        }
        else
        {
            solarSize = solarImage.size();
        }

        if (solarSize.width < (int) fiducialSpacing + 2*fiducialLength || 
            solarSize.height < (int) fiducialSpacing + 2*fiducialLength)
        {
            //std::cout << "Aspect: Solar Image too small." << std::endl;
            state = SOLAR_IMAGE_SMALL;
            return state;
        }

        //Define offset for converting subimage locations to frame locations
        offset = cv::Point(colRange.start, rowRange.start);
        if (offset.x < 0 || offset.x >= (frameSize.width - solarSize.width + 1) ||
            offset.y < 0 || offset.y >= (frameSize.height - solarSize.height + 1))
        {
            //std::cout << "Aspect: Solar Image Offset out of bounds." << std::endl;
            state = SOLAR_IMAGE_OFFSET_OUT_OF_BOUNDS;
            return state;
        }
          
        //Find fiducials
        //std::cout << "Aspect: Finding Fiducials" << std::endl;
        FindPixelFiducials(solarImage, offset);
        if (pixelFiducials.size() == 0)
        {
            //std::cout << "Aspect: No Fiducials found" << std::endl;
            state = NO_FIDUCIALS;
            return state;
        }
        else if (pixelFiducials.size() < 3)
        {
            //std::cout << "Aspect: Too Few Fiducials" << std::endl;
            state = FEW_FIDUCIALS;
            return state;
        }

        //Find fiducial IDs
        //std::cout << "Aspect: Finding fiducial IDs" << std::endl;
        FindFiducialIDs();
        if (fiducialIDs.size() == 0)
        {
            //std::cout << "Aspect: No Valid IDs" << std::endl;
            state = NO_IDS;
            return state;
        }
        
        //std::cout << "Aspect: Finding Mapping" << std::endl;
        FindMapping();
        if (/*ILL CONDITIONED*/ false)
        {
            //std::cout << "Aspect: Mapping is ill-conditioned." << std::endl;
            state = MAPPING_ILL_CONDITIONED;
            return state;
        }

    }
    state = NO_ERROR;
    return state;
}


/****************************************************

Data product Get functions

***************************************************/

AspectCode Aspect::GetPixelMinMax(unsigned char& min, unsigned char& max)
{
    if (state < FRAME_EMPTY)
    {
        max = frameMax;
        min = frameMin;
        return NO_ERROR;
    }
    else return state;
}

AspectCode Aspect::GetPixelCrossings(CoordList& crossings)
{
    if (state < LIMB_ERROR)
    {
        crossings.clear();
        for (unsigned int k = 0; k <  limbCrossings.size(); k++)
            crossings.push_back(limbCrossings[k]);
        return NO_ERROR;
    }
    else return state;
}

AspectCode Aspect::ReportFocus()
{
    if (state < LIMB_ERROR)
    {
        slopes.sort();
        slopes.reverse();
        std::cout << "Focus report: ";
        for (std::list<float>::iterator it = slopes.begin(); it != slopes.end(); ++it)
            std::cout << *it << " ";
        std::cout << std::endl;
        return NO_ERROR;
    }
    else return state;
}

AspectCode Aspect::GetPixelCenter(cv::Point2f &center)
{
    if (state < CENTER_ERROR)
    {
        center = pixelCenter;
        return NO_ERROR;
    }
    else return state;
}

AspectCode Aspect::GetPixelError(cv::Point2f &error)
{
    if (state < CENTER_ERROR)
    {
        error = pixelError;
        return NO_ERROR;
    }
    else return state;
}

AspectCode Aspect::GetPixelFiducials(CoordList& fiducials)
{
    if (state < FIDUCIAL_ERROR)
    {
        fiducials.clear();
        for (unsigned int k = 0; k < pixelFiducials.size(); k++)
            fiducials.push_back(pixelFiducials[k]);
        return NO_ERROR;
    }
    else return state;
        
}

AspectCode Aspect::GetFiducialPairs(IndexList& rows, IndexList& cols)
{
    if (state < ID_ERROR)
    {
        rows.clear();
        for (unsigned int k = 0; k <  rowPairs.size(); k++)
            rows.push_back(rowPairs[k]);
        cols.clear();
        for (unsigned int k = 0; k <  colPairs.size(); k++)
            cols.push_back(colPairs[k]);
        return NO_ERROR;
    }
    else return state;
}
AspectCode Aspect::GetFiducialIDs(IndexList& IDs)
{
    if (state < ID_ERROR)
    {
        IDs.clear();
        for (unsigned int k = 0; k <  fiducialIDs.size(); k++)
            IDs.push_back(fiducialIDs[k]);
        return NO_ERROR;
    }
    else return state;
}

AspectCode Aspect::GetMapping(std::vector<float>& map)
{
    if(state < MAPPING_ERROR)
    {
        map.clear();
        for (unsigned int k = 0; k < mapping.size(); k++)
            map.push_back(mapping[k]);
        return NO_ERROR;
    }
    else return state;
}

AspectCode Aspect::GetScreenCenter(cv::Point2f &center)
{
    
    if(state < MAPPING_ERROR)
    {
        center = PixelToScreen(pixelCenter);
        return NO_ERROR;
    }
    else return state;
}

AspectCode Aspect::GetScreenFiducials(CoordList& fiducials)
{
    fiducials.clear();
    if (state < MAPPING_ERROR)
    {
        fiducials.clear();
        fiducials.resize(pixelFiducials.size());
        for (unsigned int k = 0; k < pixelFiducials.size(); k++)
            fiducials[k] =  PixelToScreen( pixelFiducials[k] );
        return NO_ERROR;
    }
    else return state;
        
}

/*************************************************************************

Aspect Parameter Set/Get Functions

*************************************************************************/

float Aspect::GetFloat(FloatParameter variable)
{
    switch(variable)
    {
    case CHORD_THRESHOLD:
        return chordThreshold;
    case ERROR_LIMIT:
        return errorLimit;
    case RADIUS_MARGIN:
        return radiusMargin;
    case FIDUCIAL_THRESHOLD:
        return fiducialThreshold;
    case FIDUCIAL_SPACING:
        return fiducialSpacing;
    case FIDUCIAL_SPACING_TOL:
        return fiducialSpacingTol;
    case FIDUCIAL_TWIST:
        return fiducialTwist;
    default:
        return 0;
    }
}

int Aspect::GetInteger(IntParameter variable)
{
    switch(variable)
    {
    case NUM_CHORDS_SEARCHING:
        return initialNumChords;
    case NUM_CHORDS_OPERATING:
        return chordsPerAxis;
    case LIMB_WIDTH:
        return limbWidth; 
    case SOLAR_RADIUS:
        return solarRadius;
    case FIDUCIAL_LENGTH:
        return fiducialLength;
    case FIDUCIAL_WIDTH:
        return fiducialWidth;
    case NUM_FIDUCIALS:
        return numFiducials;
    default:
        return 0;
    }
}

void Aspect::SetFloat(FloatParameter variable, float value)
{
    switch(variable)
    {
    case CHORD_THRESHOLD:
        chordThreshold = value;
        break;
    case ERROR_LIMIT:
        errorLimit = value;
        break;
    case RADIUS_MARGIN:
        radiusMargin = value;
        break;
    case FIDUCIAL_THRESHOLD:
        fiducialThreshold = value;
        break;
    case FIDUCIAL_SPACING:
        fiducialSpacing = value;
        break;
    case FIDUCIAL_SPACING_TOL:
        fiducialSpacingTol = value;
        break;
    case FIDUCIAL_TWIST:
        fiducialTwist = value;
        break;
    default:
        return;
    }
    return;
}

void Aspect::SetInteger(IntParameter variable, int value)
{
    switch(variable)
    {
    case NUM_CHORDS_SEARCHING:
        initialNumChords= value;
        break;
    case NUM_CHORDS_OPERATING:
        chordsPerAxis = value;
        break;
    case LIMB_WIDTH:
        limbWidth = value; 
        break;
    case SOLAR_RADIUS:
        solarRadius = value;
        break;
    case FIDUCIAL_LENGTH:
        fiducialLength = value;
        break;
    case FIDUCIAL_WIDTH:
        fiducialWidth = value;
        break;
    case NUM_FIDUCIALS:
        numFiducials = value;
        break;
    default:
        return;
    }
    return;
}
        
/**********************************************************

Aspect Private processing functions

***********************************************************/

void Aspect::GenerateKernel()
{
    int edge = 1, d = 20;
    cv::Mat distanceField, subField, bar, mask, shape;
    cv::Range crossLength, crossWidth;
    double minVal;
    kernel = cv::Mat(2*(fiducialLength/2 + edge) + 1, 
                     2*(fiducialLength/2 + edge) + 1,
                     CV_32FC1);
    shape = cv::Mat::zeros(kernel.size(), CV_32FC1);
 
    crossLength = SafeRange(edge, shape.rows-edge, shape.rows);
    crossWidth = SafeRange((fiducialLength/2) + 1 - (fiducialWidth/2),
                           (fiducialLength/2) + 1 + (fiducialWidth/2) + 1,
                           shape.rows);
    mask = cv::Mat(shape.rows, shape.cols, CV_8UC1);

    bar = shape(crossLength, crossWidth);
    bar += cv::Mat::ones(bar.rows, bar.cols, CV_32FC1);

    bar = shape(crossWidth, crossLength);
    bar += cv::Mat::ones(bar.rows, bar.cols, CV_32FC1);

    distanceField = cv::Mat::zeros(2*shape.rows + 1, 2*shape.cols + 1, CV_32FC1);
    for (int m = 0; m < distanceField.rows; m++)
    {
        for (int n = 0; n < distanceField.cols; n++)
        {
            distanceField.at<float>(m,n) = Euclidian(cv::Point2f(shape.rows - m,
                                                                 shape.cols - n));
        }
    }

    for (int m = 0; m < shape.rows; m++)
    {
        for (int n = 0; n < shape.cols; n++)
        {
            subField = distanceField(cv::Range(shape.rows - m, 2*shape.rows - m),
                                     cv::Range(shape.cols - n, 2*shape.cols - n));
            
            if (shape.at<float>(m,n) > 0)
                compare(shape, 0, mask, cv::CMP_EQ);
            else
                compare(shape, 0, mask, cv::CMP_GT);

            cv::minMaxIdx(subField, &minVal, NULL, NULL, NULL, mask);
            kernel.at<float>(m,n) = ((shape.at<float>(m,n) > 0) ? 1 : -1) * (-pow(d,2)/2)*exp(-d*((float) minVal));
        }
    }

    cv::normalize(kernel, kernel, -1, 1,cv::NORM_MINMAX);
    
/*
  for (int m = 0; m < shape.rows; m++)
  {
  for (int n = 0; n < shape.cols; n++)
  {
  std::cout << kernel.at<float>(m,n) << " ";
  }
  std::cout << std::endl;
  }
*/
    return;
}

int Aspect::FindLimbCrossings(const cv::Mat &chord, std::vector<float> &crossings)
{
    std::vector<int> edges;
    std::vector<float> x, y, fit;
    unsigned char thisValue, lastValue, pixelThreshold;
    int K = chord.total();
    int edgeSpread;
    int edge, min, max;
    int N;
    double fittedEdge;

    float threshold = frameMin + chordThreshold*(frameMax-frameMin);
    pixelThreshold = (unsigned char) threshold;
    

    //for each pixel, check if the pixel lies on a potential limb
    lastValue = (int) chord.at<unsigned char>(0);
    for (int k = 1; k < K; k++)
    {
        thisValue = (int) chord.at<unsigned char>(k);

        //check for a rising edge, save the index above the threshold
        if (lastValue <= pixelThreshold && thisValue > pixelThreshold)
        {
            edges.push_back(k);
        }
        //check for a falling edge
        else if(lastValue > pixelThreshold && thisValue <= pixelThreshold)
        {
            edges.push_back(-(k-1));
        }
        lastValue = thisValue;
    }

    //Remove edge pairs that seem to correspond to fiducials
    //also remove edge pairs that are too close together
    for (unsigned int k = 1; k <  edges.size(); k++)
    {
        //find distance between next edge pair
        //positive if the region is below the threshold
        edgeSpread = edges[k] + edges[k-1];

        //if the pair is along a fiducial
        if(abs(edgeSpread - fiducialLength) <= limbWidth || 
           // or across a fiducial
           abs(edgeSpread - fiducialWidth) <= limbWidth ||
           // or too close together
           abs(edgeSpread) < limbWidth)
        {
            // remove the pair and update the index accordingly
            edges.erase(edges.begin() + (k-1), edges.begin() + (k+1));
            if (k == 1) k -= 1;
            else k -= 2;
        }
    }

    //for (int i=0; i<edges.size(); i++) std::cout << edges[i] << " ";
    //std::cout << std::endl;

    //Going to accept three cases: Either we have 
    // -a pair with a rising edge followed by a falling edge,
    // -a single edge falling within a solar diameter of the start of the image strip
    // -a single edge rising within a solar diameter of the end of the strip
    
    // To handle the second two cases, we'll add an artificial crossing at the image edge.
    if ((edges.size() == 1) && (abs(edges[0]) < 2*solarRadius) && (edges[0] < 0))
    {
        edge = abs(edges[0]);
        //std::cout << "Special falling edge for " << edge << std::endl;
        edges.resize(2);
        edges[0] = 0;
        edges[1] = -edge;
    }
    else if ((edges.size() == 1) && (abs(edges[0]) > K-2*solarRadius) && (edges[0] > 0))
    {
        edge = abs(edges[0]);
        //std::cout << "Special rising edge for " << edge << std::endl;
        edges.resize(2);
        edges[0] = edge;
        edges[1] = -(K-1);
    }

    // at this point we're reasonably certain we've found a valid chord
    if ((edges.size() == 2) && (edges[0] >= 0)  && (edges[1] < 0))
    {
       // for each edge, perform a fit to find the limb crossing
       crossings.clear();
       for (int k = 0; k < 2; k++)
       {
           // Throw away slope information, use just edge
           edge = abs(edges[k]);
           // If the edge was artificially added, proceed without fitting
           if (edge == 0 || edge == K-1)
           {
               crossings.push_back(edge);
           }
           // Otherwise, fit a line to the edge to refine its position
           else
           {
               if ((edge-limbWidth) < 0) min = 0;
               else min = edge-limbWidth;
               
               if ((edge+limbWidth) > K) max = K;
               else max = edge+limbWidth;
               
               //if that neighborhood is large enough
               N = max-min+1;
               if (N < 2)
               {
                   return -1;
               }
               //compute fit to neighborhood
               x.clear(); y.clear();
               for (int l = min; l <= max; l++)
               {
                   x.push_back(l);
                   y.push_back((float) chord.at<unsigned char>(l));
               }
               LinearFit(x,y,fit);
               fittedEdge = (threshold - fit[0])/fit[1];
               if (std::isfinite(fittedEdge))
               {
                   crossings.push_back(fittedEdge);
                   slopes.push_back(fabs(fit[1]));
               }
               else
                   return -1;
           }
       }
       return 0;
    }   
    return -1;
}

void Aspect::FindPixelCenter()
{
    std::vector<int> rows, cols;
    std::vector<float> crossings, midpoints;
    float mean, std;
    int rowStart, colStart, rowStep, colStep, limit, K, M;
    cv::Range rowRange, colRange;

    rows.clear();
    cols.clear();

    //Determine new row and column locations for chords
    //If the past center was invalid, search the whole frame
    if(pixelCenter.x < 0 || pixelCenter.x >= frameSize.width ||
       pixelCenter.y < 0 || pixelCenter.y >= frameSize.height ||
       !std::isfinite(pixelCenter.x) || !std::isfinite(pixelCenter.y))
    {
        ////std::cout << "Aspect: Finding new center" << std::endl;
        limit = initialNumChords;

        rowStep = frameSize.height/limit;
        colStep = frameSize.width/limit;

        rowStart = rowStep/2;
        colStart = colStep/2;
    }
    //Otherwise, only use the solar subimage
    else
    {
        //std::cout << "Aspect: Searching around old center" << std::endl;
        limit = chordsPerAxis;
        rowRange = SafeRange(pixelCenter.y - solarRadius,
                             pixelCenter.y + solarRadius, 
                             frameSize.height);

        colRange = SafeRange(pixelCenter.x - solarRadius,
                             pixelCenter.x + solarRadius, 
                             frameSize.width);

        rowStep = (rowRange.end - rowRange.start)/limit;
        colStep = (colRange.end - colRange.start)/limit;

        rowStart = rowRange.start + rowStep/2;
        colStart = colRange.start + colStep/2;
    }

    //Generate vectors of chord locations
    //std::cout << "Aspect: Generating chord location list" << std::endl;
    for (int k = 0; k < limit; k++)
    {
        //std::cout << rowStart + k*rowStep << " " << colStart + k*colStep << std::endl;
        rows.push_back(rowStart + k*rowStep);
        cols.push_back(colStart + k*colStep);
    }

    //Initialize
    pixelCenter = cv::Point2f(0,0);
    limbCrossings.clear();
    slopes.clear();

    //For each dimension
    for (int dim = 0; dim < 2; dim++)
    {
        //if (dim) std::cout << "Aspect: Searching Rows" << std::endl;
        //else std::cout << "Aspect: Searching Cols" << std::endl;

        if (dim) K =  rows.size();
        else K =  cols.size();

        //For each chord
        midpoints.clear();
        for (int k = 0; k < K; k++)
        {
            //Determine the limb crossings in that chord
            crossings.clear();
            if (dim) FindLimbCrossings(frame.row(rows[k]), crossings);
            else FindLimbCrossings(frame.col(cols[k]), crossings);
            
            //If there seems to be a pair of crossings
            if (crossings.size() != 2) continue;

            //Save the crossings if they're finite
            else if (std::isfinite(crossings[0]) && std::isfinite(crossings[1]))
            {
                for (unsigned int l = 0; l < crossings.size(); l++)
                {
                    if (dim) limbCrossings.add(crossings[l], rows[k]);
                    else limbCrossings.add(cols[k], crossings[l]);
                }
                midpoints.push_back((crossings[0]+crossings[1])/2.0);
            }
        }

        //Determine the mean of the midpoints for this dimension
        mean = 0;
        M =  midpoints.size();
        for (int m = 0; m < M; m++)
            mean += midpoints[m];
        mean = (float)mean/M;
        
        //Determine the std dev of the midpoints
        std = 0;
        for (int m = 0; m < M; m++)
        {
            std += pow(midpoints[m]-mean,2);
        }
        std = sqrt(std/M);

        //Store the Center and RMS Error for this dimension
        //std::cout << "Aspect: Setting Center and Error for this dimension." << std::endl;
        if (dim)
        {
            pixelCenter.x = mean;
            pixelError.x = std;
        }
        else
        {
            pixelCenter.y = mean;
            pixelError.y = std;
        }       
    }
    //std::cout << "Aspect: Leaving FindPixelCenter" << std::endl;

    return;
}

void Aspect::FindPixelFiducials(const cv::Mat &image, cv::Point offset)
{
    cv::Mat input;
    cv::Scalar mean, stddev;
    cv::Size imageSize;
    cv::Mat correlation, nbhd;
    cv::Range rowRange, colRange;
    float threshold, thisValue, thatValue, minValue;
    float Cm, Cn, average;
    int minIndex;
    bool redundant;

    pixelFiducials.clear();
    image.convertTo(input, CV_32FC1);
    correlation.create(image.size(), CV_32FC1);

    //cv::namedWindow("Correlation", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED );
    
    matchTemplate(input, kernel, correlation, CV_TM_CCORR);

    offset.x += (kernel.cols/2);
    offset.y += (kernel.rows/2);

    //cv::normalize(correlation,correlation,0,1,cv::NORM_MINMAX);
    //cv::imshow("Correlation", kernel);
    //cv::waitKey(0);
    cv::meanStdDev(correlation, mean, stddev);

    threshold = mean[0] + fiducialThreshold*stddev[0];
        
    for (int m = 1; m < correlation.rows-1; m++)
    {
        for (int n = 1; n < correlation.cols-1; n++)
        {        
            thisValue = correlation.at<float>(m,n);
            if(thisValue > threshold)
            {
                if((thisValue > correlation.at<float>(m, n + 1)) &
                   (thisValue > correlation.at<float>(m, n - 1)) &
                   (thisValue > correlation.at<float>(m + 1, n)) &
                   (thisValue > correlation.at<float>(m - 1, n)))
                {
                    redundant = false;
                    for (unsigned int k = 0; k <  pixelFiducials.size(); k++)
                    {
                        if (abs(pixelFiducials[k].y - m) < fiducialLength*2 &&
                            abs(pixelFiducials[k].x - n) < fiducialLength*2)
                        {
                            redundant = true;
                            thatValue = correlation.at<float>((int) pixelFiducials[k].y,
                                                               (int) pixelFiducials[k].x);
                            if ( thisValue > thatValue)
                            {
                                pixelFiducials[k] = cv::Point2f(n,m);
                            }
                            break;
                        }
                    }
                    if (redundant == true)
                        continue;

                    if ( (int) pixelFiducials.size() < numFiducials)
                    {
                        pixelFiducials.add(n, m);
                    }
                    else
                    {
                        minValue = std::numeric_limits<float>::infinity();;
                        minIndex = -1;
                        for (int k = 0; k < numFiducials; k++)
                        {
                            if (correlation.at<float>((int) pixelFiducials[k].y,
                                                       (int) pixelFiducials[k].x) 
                                < minValue)
                            {
                                minIndex = k;
                                minValue = correlation.at<float>((int) pixelFiducials[k].y,
                                                                  (int) pixelFiducials[k].x);
                            }   
                        }
                        if (thisValue > minValue)
                        {
                            pixelFiducials[minIndex] = cv::Point2f(n, m);
                        }
                    }
                }
            }
        }
    }

    //Refine positions to sub-pixel
    //For each fiducial location
    for (unsigned int k = 0; k <  pixelFiducials.size(); k++)
    {
        //Get safe ranges for for the neighborhood around the fiducial
        rowRange = SafeRange(round(pixelFiducials[k].y) - fiducialWidth,
                             round(pixelFiducials[k].y) + fiducialWidth + 1,
                             correlation.rows);

        colRange = SafeRange(round(pixelFiducials[k].x) - fiducialWidth,
                             round(pixelFiducials[k].x) + fiducialWidth + 1,
                             correlation.cols);

        //Compute the centroid of the region around the local max
        //in the correlation image
        Cm = 0.0; Cn = 0.0; average = 0.0;
        threshold = mean[0] + (fiducialThreshold/2)*stddev[0];
        for (int m = rowRange.start; m < rowRange.end; m++)
        {
            for (int n = colRange.start; n < colRange.end; n++)
            {
                thisValue = correlation.at<float>(m,n);
                if (thisValue > threshold)
                {
                    Cm += ((float) m)*thisValue;
                    Cn += ((float) n)*thisValue;
                    average += thisValue;
                }
            }
        }

        //Set the fiducials to the centroid location, plus an offset
        //to convert from the solar subimage to the original frame
        pixelFiducials[k].y = (float) (Cm/average + ((float) offset.y));
        pixelFiducials[k].x = (float) (Cn/average + ((float) offset.x));
    }

    for (int k = 0; k < pixelFiducials.size(); k++)
    {
        if(!std::isfinite(pixelFiducials[k].x) || !std::isfinite(pixelFiducials[k].y))
            pixelFiducials.erase(pixelFiducials.begin() + k);

    }
    return;
}

void Aspect::FindFiducialIDs()
{
    unsigned int d, k, l, K;
    float rowDiff, colDiff;
    CoordList trash;
    CoordList rotatedFiducials;
    
    K = pixelFiducials.size();
    rowPairs.clear();
    colPairs.clear();
    fiducialIDs.clear();
    fiducialIDs.resize(K);
    
    std::vector<std::vector<int> > rowVotes, colVotes;
    rowVotes.resize(K); colVotes.resize(K);
    
    std::vector<int> modes;

    rotate(fiducialTwist, pixelFiducials, rotatedFiducials);

    //Find fiducial pairs that are spaced correctly
    //std::cout << "Aspect: Find valid fiducial pairs" << std::endl;
    //std::cout << "Aspect: Searching through " << K << " Fiducials" << std::endl;
    for (k = 0; k < K; k++)
    {
        for (l = k+1; l < K; l++)
        {
            rowDiff = rotatedFiducials[k].y - rotatedFiducials[l].y;
            colDiff = rotatedFiducials[k].x - rotatedFiducials[l].x;

            if (fabs(rowDiff) > (float) fiducialSpacing - fiducialSpacingTol &&
                fabs(rowDiff) < (float) fiducialSpacing + fiducialSpacingTol && 
                fabs(colDiff) > (float) 43 - fiducialSpacingTol &&
                fabs(colDiff) < (float) 84 + fiducialSpacingTol)

                colPairs.push_back(cv::Point(k,l));

            else if (fabs(colDiff) > (float) fiducialSpacing - fiducialSpacingTol &&
                     fabs(colDiff) < (float) fiducialSpacing + fiducialSpacingTol && 
                     fabs(rowDiff) > (float) 43 - fiducialSpacingTol &&
                     fabs(rowDiff) < (float) 84 + fiducialSpacingTol)
                
                rowPairs.push_back(cv::Point(k,l));
            else
                continue;
        }
    }
    
    //std::cout << "Aspect: Compute intra-pair distances for row pairs." << std::endl;
    //std::cout << "Rows: " << std::endl;
    for (k = 0; k <  rowPairs.size(); k++)
    {
        rowDiff = rotatedFiducials[rowPairs[k].y].y 
            - rotatedFiducials[rowPairs[k].x].y;

        for (d = 0; d < mDistances.size(); d++)
        {
            if (fabs(fabs(rowDiff) - mDistances[d]) < fiducialSpacingTol)
            {
                //std::cout << fabs(rowDiff) - mDistances[d] << " ";
                if (rowDiff > 0) 
                {
                    rowVotes[rowPairs[k].x].push_back(d-7);
                    rowVotes[rowPairs[k].y].push_back(d+1-7);
                }
                else
                {
                    rowVotes[rowPairs[k].x].push_back(d+1-7);
                    rowVotes[rowPairs[k].y].push_back(d-7);
                }
            }
        }
    }
    //std::cout << std::endl;
    //std::cout << "Columns: " << std::endl;
    //std::cout << "Aspect: Compute intra-pair distances for col pairs." << std::endl;
    for (k = 0; k <  colPairs.size(); k++)
    {
        colDiff = rotatedFiducials[colPairs[k].x].x 
            - rotatedFiducials[colPairs[k].y].x;
        for (d = 0; d <  nDistances.size(); d++)
        {
            if (fabs(fabs(colDiff) - nDistances[d]) < fiducialSpacingTol)
            {
                //std::cout << fabs(colDiff) - nDistances[d] << " ";
                if (colDiff > 0) 
                {
                    colVotes[colPairs[k].x].push_back(d-7);
                    colVotes[colPairs[k].y].push_back(d+1-7);
                }
                else
                {
                    colVotes[colPairs[k].x].push_back(d+1-7);
                    colVotes[colPairs[k].y].push_back(d-7);
                }
            }
        }
    }
    
    //std::cout << std::endl;
    // Accumulate results of first pass
    for (k = 0; k < K; k++)
    {
        modes = Mode(rowVotes[k]);
        if (modes.size() > 1)
        {
            fiducialIDs[k].y = -200;
        }
        else if (modes.size() == 1)
        {
            fiducialIDs[k].y = modes[0];
        }
        else
        {
            fiducialIDs[k].y = -100;
        }

        modes = Mode(colVotes[k]);
        if (modes.size() > 1)
        {
            fiducialIDs[k].x = -200;
        }
        else if (modes.size() == 1)
        {
            fiducialIDs[k].x = modes[0];
        }
        else
        {
            fiducialIDs[k].x = -100;
        }
    }

    // Start second pass
    rowVotes.clear(); rowVotes.resize(K);
    colVotes.clear(); colVotes.resize(K);
    //std::cout << "Aspect: Compute intra-pair distances for row pairs." << std::endl;
    for (k = 0; k <  rowPairs.size(); k++)
    {
        rowDiff = rotatedFiducials[rowPairs[k].y].y 
            - rotatedFiducials[rowPairs[k].x].y;

        //If part of a row pair has an unidentified column index, it should match its partner
        if (fiducialIDs[rowPairs[k].x].x == -100 && fiducialIDs[rowPairs[k].y].x != -100)
        {
            colVotes[rowPairs[k].x].push_back(fiducialIDs[rowPairs[k].y].x);
        }
        else if (fiducialIDs[rowPairs[k].x].x != -100 && fiducialIDs[rowPairs[k].y].x == -100)
        {
            colVotes[rowPairs[k].y].push_back(fiducialIDs[rowPairs[k].x].x);
        }
    
        //If part of a row pair has an unidentified row index, it should be incremented from its partner
        if (fiducialIDs[rowPairs[k].x].y == -100 && fiducialIDs[rowPairs[k].y].y != -100)
        {
            if (rowDiff >= 0)
                rowVotes[rowPairs[k].x].push_back(fiducialIDs[rowPairs[k].y].y - 1);
            else 
                rowVotes[rowPairs[k].x].push_back(fiducialIDs[rowPairs[k].y].y + 1);
        }
        else if (fiducialIDs[rowPairs[k].x].y != -100 && fiducialIDs[rowPairs[k].y].y == -100)
        {
            if (rowDiff >= 0)
                rowVotes[rowPairs[k].y].push_back(fiducialIDs[rowPairs[k].x].y + 1);
            else 
                rowVotes[rowPairs[k].y].push_back(fiducialIDs[rowPairs[k].x].y - 1);
        }
    }

    for (k = 0; k <  colPairs.size(); k++)
    {
        colDiff = rotatedFiducials[colPairs[k].x].x 
            - rotatedFiducials[colPairs[k].y].x;

        //For columns, pairs should match in row
        if (fiducialIDs[colPairs[k].x].y == -100 && fiducialIDs[colPairs[k].y].y != -100)
        {
            rowVotes[colPairs[k].x].push_back(fiducialIDs[colPairs[k].y].y);
        }
        else if (fiducialIDs[colPairs[k].x].y != -100 && fiducialIDs[colPairs[k].y].y == -100)
        {
            rowVotes[colPairs[k].y].push_back(fiducialIDs[colPairs[k].x].y);
        }

        //For columns, pairs should increment in column.
        if (fiducialIDs[colPairs[k].x].x == -100 && fiducialIDs[colPairs[k].y].x != -100)
        {
            if (colDiff >= 0)
                colVotes[colPairs[k].x].push_back(fiducialIDs[colPairs[k].y].x - 1);
            else 
                colVotes[colPairs[k].x].push_back(fiducialIDs[colPairs[k].y].x + 1);
        }
        else if (fiducialIDs[colPairs[k].x].x != -100 && fiducialIDs[colPairs[k].y].x == -100)
        {
            if (colDiff >= 0)
                colVotes[colPairs[k].y].push_back(fiducialIDs[colPairs[k].x].x + 1);
            else 
                colVotes[colPairs[k].y].push_back(fiducialIDs[colPairs[k].x].x - 1);
        }
    }
    
    //Vote on second pass
    for (k = 0; k < K; k++)
    {
        modes = Mode(rowVotes[k]);
        if (modes.size() > 1)
        {
            fiducialIDs[k].y = -200;
        }
        else if (modes.size() == 1)
        {
            fiducialIDs[k].y = modes[0];
        }

        modes = Mode(colVotes[k]);
        if (modes.size() > 1)
        {
            fiducialIDs[k].x = -200;
        }
        else if (modes.size() == 1)
        {
            fiducialIDs[k].x = modes[0];
        }
    }
}       

void Aspect::FindMapping()
{
    std::vector<float> x, y, fit;
    cv::Point2f screenPoint;
    mapping.clear();
    mapping.resize(4);

    for (int dim = 0; dim < 2; dim++)
    {
        x.clear();
        y.clear();
        for (unsigned int k = 0; k <  pixelFiducials.size(); k++)
        {
            if(fiducialIDs[k].x < -10 || fiducialIDs[k].y < -10) continue;
            
            screenPoint = fiducialIDtoScreen(fiducialIDs[k]);         
            if(dim == 0)
            {
                x.push_back(pixelFiducials[k].x);
                y.push_back(screenPoint.x);
            }
            else
            {
                x.push_back(pixelFiducials[k].y);
                y.push_back(screenPoint.y);
            }
        }
        LinearFit(x,y,fit);
        mapping[2*dim + 0] = fit[0];
        mapping[2*dim + 1] = fit[1];
    }
}

cv::Point2f Aspect::PixelToScreen(cv::Point2f pixelPoint)
{
    cv::Point2f screenPoint;

    screenPoint.x = mapping[0] + mapping[1]*pixelPoint.x;
    screenPoint.y = mapping[2] + mapping[3]*pixelPoint.y;
    
    return screenPoint;
}       


/*****************************************************

Random utility functions

*****************************************************/

cv::Range SafeRange(int start, int stop, int size)
{
    cv::Range range;
    range.start = (start > 0) ? (start) : 0;
    range.end = (stop < size) ? (stop) : (size);
    return range;
}

void LinearFit(const std::vector<float> &x, const std::vector<float> &y, std::vector<float> &fit)
{
    cv::Scalar init(0);
    cv::Mat A(2,2,CV_32FC1, init), B(2,1,CV_32FC1, init), X(2,1,CV_32FC1, init);
    cv::Mat eigenvalues;
    float N, cond;
    unsigned int l;

    if (x.size() != y.size())
    {
        std::cout << "Error in LinearFit: x and y vectors should be same length\n";
        return;
    }

    N = (float) x.size();
    A.at<float>(1,1) = N;

    for (l = 0; l <  x.size(); l++)
    {
        A.at<float>(0,0) += x[l]*x[l];
        A.at<float>(0,1) += x[l];
        B.at<float>(1) += y[l];
        B.at<float>(0) += x[l]*y[l];
    }

    A.at<float>(1,0) = A.at<float>(0,1);
    
    cv::eigen(A, eigenvalues);
    cond = eigenvalues.at<float>(0)/eigenvalues.at<float>(1);
    //std::cout << "Condition number: " << cond << std::endl;

    cv::solve(A,B,X,cv::DECOMP_CHOLESKY);
    
    fit.clear();
    fit.resize(2);
    fit[0] = X.at<float>(1); //intercept
    fit[1] = X.at<float>(0); //slope
}

void CircleFit(const std::vector<float> &x, const std::vector<float> &y, Circle& fit)
{
    if (x.size() != y.size())
    {
        std::cout << "CircleFit: Vector lengths don't match." << std::endl;
        return;
    }
    CoordList points;
    for (unsigned int k = 0; k < x.size(); k++)
        points.add(x[k], y[k]);
    return CircleFit(points, fit);
}

void CircleFit(const CoordList& points, Circle& fit)
{
    cv::Mat B, D, Y;
    float x, y;

    cv::Point2f center;
    float radius, MSE;
    std::vector<float> residual, CookDistance;
    CoordList CookPoints;
    cv::Mat H;

    B = cv::Mat(points.size(), 3, CV_32F);
    D = cv::Mat(points.size(), 1, CV_32F);
    Y = cv::Mat(3,1, CV_32F);
    H = cv::Mat(points.size(), points.size(), CV_32F);
    CookDistance = cv::Mat(points.size(), 1, CV_32F);

    for (unsigned int k = 0; k < points.size(); k++)
    {
        x = points[k].x;
        y = points[k].y;
        B.at<float>(k,0) = x;
        B.at<float>(k,1) = y;
        B.at<float>(k,2) = 1;
        D.at<float>(k) = pow(x,2) + pow(y,2);
    }
    
    cv::solve((B.t()*B), (B.t()*D), Y, cv::DECOMP_CHOLESKY);
    center.x = Y.at<float>(0)/2;
    center.y = Y.at<float>(1)/2;
    radius = sqrt(Y.at<float>(2) + pow(center.x,2) + pow(center.y,2));

    H = B*((B.t()*B).inv())*B.t();
    for (unsigned int k = 0; k < points.size(); k++)
    {
        residual.push_back(pow(Euclidian(points[k] - center),2));
        CookDistance[k] = residual[k] * H.at<float>(k,k) / 
            pow(1 - H.at<float>(k,k), 2);
    }
    MSE = Mean(residual); 
    CookPoints = points;
    for (unsigned int k = 0; k < CookPoints.size(); k++)
    {
        if (CookDistance[k] > MSE)
        {
            CookDistance.erase(CookDistance.begin() + k);
            CookPoints.erase(CookPoints.begin() + k);
            k--;
        }
    }

    if (CookPoints.size() < points.size() && CookPoints.size() > 4)
    {
        std::cout << "Go again with " << CookPoints.size() << " points" << std::endl;
        CircleFit(CookPoints, fit);
    }

    fit[0] = center.x;
    fit[1] = center.y;
    fit[2] = radius;

    return;
}

cv::Point2f VectorToCircle(Circle circle, cv::Point2f point)
{
    cv::Point2f biasVector;
    float bias;

    biasVector = circle.center() - point;
    bias = Euclidian(biasVector);
    return (1-circle.r()/bias)*biasVector;    
}

void VectorToCircle(Circle circle, const CoordList& points, CoordList& vectors)
{
    vectors.clear();
    for (unsigned int k = 0; k < points.size(); k++)
        vectors.push_back(VectorToCircle(circle, points[k]));
}

cv::Point2f Mean(const CoordList& points)
{
    std::vector<float> x, y;
    for (unsigned int k = 0; k < points.size(); k++)
    {
        x.push_back(points[k].x);
        y.push_back(points[k].y);
    }
    return cv::Point2f(Mean(x), Mean(y));
}

float Mean(const std::vector<float>& d)
{
    float average = 0;
    for (unsigned int k = 0; k < d.size(); k++)
    {
        average += d[k];
    }
    return average/d.size();
}

std::vector<float> Euclidian(CoordList& vectors)
{
    std::vector<float> lengths;
    for (unsigned int k = 0; k < vectors.size(); k++)
    {
        lengths.push_back(Euclidian(vectors[k]));
    }
    return lengths;
}

float Euclidian(cv::Point2f d)
{
    return sqrt(pow(d.x, 2) + pow(d.y, 2));
}

float Euclidian(cv::Point2f p1, cv::Point2f p2)
{
    return Euclidian(p1-p2);
}

template <class T>
std::vector<T> Mode(std::vector<T> data)
{
    std::map<T,unsigned> frequencyCount;
    for(size_t i = 0; i < data.size(); ++i)
        frequencyCount[data[i]]++;

    unsigned currentMax = 0;
    std::vector<T> mode;
    for(auto it = frequencyCount.cbegin(); it != frequencyCount.cend(); ++it )
    {
        if (it->second > currentMax)
        {
            mode.clear();
            mode.push_back(it->first);
            currentMax = it->second;
        }
        else if (it->second == currentMax)
        {
            mode.push_back(it->first);
        }
    }
    return mode;
}

cv::Point2f rotate(float angle, cv::Point2f point)
{
    float c, s;
    cv::Point2f outPoint;
    c = std::cos(2*pi*angle/360);
    s = std::sin(2*pi*angle/360);
    outPoint.x = c*point.x - s*point.y;
    outPoint.y = s*point.x + c*point.y;
    return outPoint;
}
void rotate(float angle, const CoordList &inPoints, CoordList &outPoints)
{
    outPoints.resize(inPoints.size());
    for (int k = 0; k < inPoints.size(); k++)
        outPoints[k] = rotate(angle, inPoints[k]);
}
    
