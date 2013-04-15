/* This code should encapsulate all the code necessary for generating solar aspect.
   center, fiducials, etc, as well as a local copy of the current frame. 

   The idea would be to call "LoadFrame" once, at which point this module would
   reset all its values. Next, the "Run" function computes all the data products possible.
   Requests for data are made with the Get functions, and the necessary data is provided if
   its available, otherwise an error code is returned. Most data is stored as either CoordList or a
   cv::Point. All the functions doing real computation are private, other than "Run."
*/
#include "processing.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cmath>

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
        return "Pixel centers have too large a variance";
        
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
    frameMin = 255;
    frameMax = 0;
    
    initialNumChords = 20;
    chordsPerAxis = 5;
    chordThreshold = .25;
    solarRadius = 105;
    limbWidth = 2;
    fiducialLength = 15;
    fiducialWidth = 2; 
    fiducialThreshold = 5;
    fiducialNeighborhood = 2;
    numFiducials = 10;

    fiducialSpacing = 15.5;
    fiducialSpacingTol = 1.5;
    pixelCenter = cv::Point2f(-1.0, -1.0);
    pixelError = cv::Point2f(0.0, 0.0);
    
    matchKernel(kernel);
    kernelSize = kernel.size();

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
            inputFrame.copyTo(frame);
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
        else if(max - min < 64)
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
            std::isnan(pixelCenter.x) || std::isnan(pixelCenter.y))
        {
            //std::cout << "Aspect: Center Out-of-bounds:" << pixelCenter << std::endl;
            pixelCenter = cv::Point2f(-1,-1);
            state = CENTER_OUT_OF_BOUNDS;
            return state;
        }
/*        else if (pixelError.x > 1.5*solarRadius || pixelError.x < .5*solarRadius|| 
                 std::isnan(pixelError.x))
*/
        else if (pixelError.x > 50 || pixelError.y > 50 || 
                 std::isnan(pixelError.x) || std::isnan(pixelError.y))
        {
            //std::cout << "Aspect: Center Error greater than 50 pixels: " << pixelError << std::endl;
            pixelCenter = cv::Point2f(-1,-1);
            state = CENTER_ERROR_LARGE;
            return state;
        }
        
        //Find solar subImage
        //std::cout << "Aspect: Finding solar subimage" << std::endl;
        rowRange = SafeRange(pixelCenter.y-solarRadius, pixelCenter.y+solarRadius, frameSize.height);
        colRange = SafeRange(pixelCenter.x-solarRadius, pixelCenter.x+solarRadius, frameSize.width);
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
    case FIDUCIAL_THRESHOLD:
        return fiducialThreshold;
    case FIDUCIAL_SPACING:
        return fiducialSpacing;
    case FIDUCIAL_SPACING_TOL:
        return fiducialSpacingTol;
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
    case FIDUCIAL_NEIGHBORHOOD:
        return fiducialNeighborhood;
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
    case FIDUCIAL_THRESHOLD:
        fiducialThreshold = value;
        break;
    case FIDUCIAL_SPACING:
        fiducialSpacing = value;
        break;
    case FIDUCIAL_SPACING_TOL:
        fiducialSpacingTol = value;
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
    case FIDUCIAL_NEIGHBORHOOD:
        fiducialNeighborhood = value;
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

int Aspect::FindLimbCrossings(cv::Mat chord, std::vector<float> &crossings)
{
    std::vector<int> edges;
    std::vector<float> x, y, fit;
    unsigned char thisValue, lastValue, pixelThreshold;
    int K = chord.total();
    int edgeSpread;
    int edge, min, max;
    int N;

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

    //if we still have anything other than a single edge pair, ignore the chord
    if ( edges.size() != 2)
    {
        return -1;
    }
    // if the pair isn't a rising edge followed by a falling edge, ignore the chord.
    else if(!(edges[0] > 0  && edges[1] < 0))
    {
        return -1;
    }
    else
    {
        // at this point we're reasonably certain we've found a valid chord

        // for each edge, perform a fit to find the limb crossing
        crossings.clear();
        for (int k = 0; k < 2; k++)
        {
            //take a neighborhood around the edge
            edge = abs(edges[k]);
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
            crossings.push_back((threshold - fit[0])/fit[1]);
            slopes.push_back(fabs(fit[1]));
        }
    }
    return 0; 
}

void Aspect::FindPixelCenter()
{
    std::vector<int> rows, cols;
    std::vector<float> crossings, midpoints;
    int rowStart, colStart, rowStep, colStep, limit, K, M;
    cv::Range rowRange, colRange;
    float mean, std;

    rows.clear();
    cols.clear();

    //Determine new row and column locations for chords
    //If the past center was invalid, search the whole frame
    if(pixelCenter.x < 0 || pixelCenter.x >= frameSize.width ||
       pixelCenter.y < 0 || pixelCenter.y >= frameSize.height ||
       std::isnan(pixelCenter.x) || std::isnan(pixelCenter.y))
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

        rowStep = (rowRange.end - rowRange.start + 1)/limit;
        colStep = (colRange.end - colRange.start + 1)/limit;

        rowStart = rowRange.start + rowStep/2;
        colStart = colRange.start + colStep/2;
    }

    //Generate vectors of chord locations
    //std::cout << "Aspect: Generating chord location list" << std::endl;
    for (int k = 0; k < limit; k++)
    {
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

        //Find the midpoints of the chords.
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
            else
            {
                //Save the crossings
                for (unsigned int l = 0; l < crossings.size(); l++)
                {
                    if (dim) limbCrossings.add(crossings[l], rows[k]);
                    else limbCrossings.add(cols[k], crossings[l]);
                }
                //Compute and store the midpoint
                midpoints.push_back((crossings[0] + crossings[1])/2);
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
}

void Aspect::FindPixelFiducials(cv::Mat image, cv::Point offset)
{
    cv::Scalar mean, stddev;
    cv::Size imageSize;
    cv::Mat correlation, nbhd;
    cv::Range rowRange, colRange;
    float threshold, thisValue, thatValue, minValue;
    double Cm, Cn, average;
    int minIndex;
    bool redundant;

    pixelFiducials.clear();
    imageSize = image.size();

    cv::filter2D(image, correlation, CV_32FC1, kernel, cv::Point(-1,-1));
    cv::normalize(correlation,correlation,0,1,cv::NORM_MINMAX);
        
    cv::meanStdDev(correlation, mean, stddev);

    threshold = mean[0] + fiducialThreshold*stddev[0];
        
    for (int m = 1; m < imageSize.height-1; m++)
    {
        for (int n = 1; n < imageSize.width-1; n++)
        {        
            thisValue = correlation.at<float>(m,n);
            if(thisValue > threshold)
            {
                if((thisValue > correlation.at<float>(m, n+1)) &
                   (thisValue > correlation.at<float>(m, n- 1)) &
                   (thisValue > correlation.at<float>(m+1, n)) &
                   (thisValue > correlation.at<float>(m-1, n)))
                {
                    redundant = false;
                    for (unsigned int k = 0; k <  pixelFiducials.size(); k++)
                    {
                        if (abs(pixelFiducials[k].y - m) < fiducialLength &&
                            abs(pixelFiducials[k].x - n) < fiducialLength)
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
                        minValue = kernelSize.width*kernelSize.height*256;
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
        rowRange = SafeRange(pixelFiducials[k].y - fiducialNeighborhood,
                             pixelFiducials[k].y + fiducialNeighborhood,
                             imageSize.height);

        colRange = SafeRange(pixelFiducials[k].x - fiducialNeighborhood,
                             pixelFiducials[k].x + fiducialNeighborhood,
                             imageSize.width);
        //Compute the centroid of the region around the local max
        //in the correlation image
        Cm = 0.0; Cn = 0.0; average = 0.0;
        for (int m = rowRange.start; m <= rowRange.end; m++)
        {
            for (int n = colRange.start; n <= colRange.end; n++)
            {
                thisValue = correlation.at<float>(m,n);
                Cm += m*thisValue;
                Cn += n*thisValue;
                average += thisValue;
            }
        }

        //Set the fiducials to the centroid location, plus an offset
        //to convert from the solar subimage to the original frame
        pixelFiducials[k].y = (float) (Cm/average + (double) offset.y);
        pixelFiducials[k].x = (float) (Cn/average + (double) offset.x);
    }
    return;
}

void Aspect::FindFiducialIDs()
{
    unsigned int d, k, l, K;
    float rowDiff, colDiff;
    IndexList rowPairs, colPairs;
    CoordList trash;
    
    K = pixelFiducials.size();
    fiducialIDs.clear();
    fiducialIDs.resize(K);
    
    std::vector<std::vector<int> > rowVotes, colVotes;
    rowVotes.resize(K); colVotes.resize(K);
    
    std::vector<int> modes;

    //Find fiducial pairs that are spaced correctly
    //std::cout << "Aspect: Find valid fiducial pairs" << std::endl;
    //std::cout << "Aspect: Searching through " << K << " Fiducials" << std::endl;
    for (k = 0; k < K; k++)
    {
        for (l = k+1; l < K; l++)
        {
            rowDiff = pixelFiducials[k].y - pixelFiducials[l].y;
            if (fabs(rowDiff) > (float) fiducialSpacing - fiducialSpacingTol &&
                fabs(rowDiff) < (float) fiducialSpacing + fiducialSpacingTol)
                colPairs.push_back(cv::Point(k,l));

            colDiff = pixelFiducials[k].x - pixelFiducials[l].x;
            if (fabs(colDiff) > (float) fiducialSpacing - fiducialSpacingTol &&
                fabs(colDiff) < (float) fiducialSpacing + fiducialSpacingTol)
                rowPairs.push_back(cv::Point(k,l));
        }
    }
    
    //std::cout << "Aspect: Compute intra-pair distances for row pairs." << std::endl;
    for (k = 0; k <  rowPairs.size(); k++)
    {
        rowDiff = pixelFiducials[rowPairs[k].y].y 
            - pixelFiducials[rowPairs[k].x].y;

        for (d = 0; d < mDistances.size(); d++)
        {
            if (fabs(fabs(rowDiff) - mDistances[d]) < fiducialSpacingTol)
            {
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

    //std::cout << "Aspect: Compute intra-pair distances for col pairs." << std::endl;
    for (k = 0; k <  colPairs.size(); k++)
    {
        colDiff = pixelFiducials[colPairs[k].x].x 
            - pixelFiducials[colPairs[k].y].x;
        for (d = 0; d <  nDistances.size(); d++)
        {
            if (fabs(fabs(colDiff) - nDistances[d]) < fiducialSpacingTol)
            {
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
        rowDiff = pixelFiducials[rowPairs[k].y].y 
            - pixelFiducials[rowPairs[k].x].y;

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
        colDiff = pixelFiducials[colPairs[k].x].x 
            - pixelFiducials[colPairs[k].y].x;

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
    range.end = (stop < size - 1) ? (stop) : (size - 1);
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

void CircleFit(const std::vector<float> &x, const std::vector<float> &y, int method, Circle& fit)
{
    if (x.size() != y.size())
    {
        std::cout << "CircleFit: Vector lengths don't match." << std::endl;
        return;
    }
    CoordList points;
    for (unsigned int k = 0; k < x.size(); k++)
        points.add(x[k], y[k]);
    return CircleFit(points, method, fit);
}

void CircleFit(const CoordList& points, int method, Circle& fit)
{
    switch(method)
    {
    case 0:
        return BullockCircleFit(points, fit);
    case 1:
        return CoopeCircleFit(points, fit);
    default:
        std::cout << "Unrecognized circle fitting method" << std::endl;
        return;
    }
}

void BullockCircleFit(const CoordList& inPoints, Circle& fit)
{
    CoordList points;
    cv::Point2f offset = Mean(inPoints);
    for (unsigned int k = 0; k < inPoints.size(); k++)
        points.push_back((inPoints[k] - offset));

    float Sxx, Sxy, Syy, Sxxx, Sxxy, Sxyy, Syyy;
    float xx, xy, yy;
    cv::Mat A, B, X;
    cv::Point2f center;
    float radius;

    CoordList biasVectors;
    cv::Point biasVector;
    float bias;
    
    Sxx = Sxy = Syy = Sxxx = Sxxy = Sxyy = Syyy = 0;
    
    for (unsigned int k = 0; k < inPoints.size(); k++)
    {
        xx = std::pow(points[k].x,2);
        xy = points[k].x*points[k].y;
        yy = std::pow(points[k].y,2);
        
        Sxx += xx;
        Sxy += xy;
        Syy += yy;

        Sxxx += xx*points[k].x;
        Sxxy += xx*points[k].y;
        Sxyy += yy*points[k].x;
        Syyy += yy*points[k].y;
    }
    float a[2][2] = {{Sxx, Sxy}, {Sxy, Syy}};
    float b[2][1] = {{-(Sxxx + Sxyy)/2}, {-(Sxxy + Syyy)/2}};
    A = cv::Mat(2, 2, CV_32F, a);
    B = cv::Mat(2, 1, CV_32F, b);

    cv::solve((A.t()*A), (A.t()*B), X, cv::DECOMP_CHOLESKY);
    

    center.x = X.at<float>(0);
    center.y = X.at<float>(1);
    radius = sqrt(pow(center.x,2) + pow(center.y,2) + (Sxx + Syy)/(float) points.size());
    center = center + offset;
    //Adjust for bias from unbalanced points
    for (unsigned int k = 0; k < inPoints.size(); k++)
    {
        biasVector = center - inPoints[k];
        bias = Euclidian(biasVector);
        biasVectors.push_back((bias > radius ? (bias - radius)/bias : (radius - bias)/radius) * biasVector);
        
    }
    center = center - Mean(biasVectors);

    fit[0] = center.x;
    fit[1] = center.y;
    fit[2] = radius;
    return;
}

void CoopeCircleFit(const CoordList& points, Circle& fit)
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
        CoopeCircleFit(CookPoints, fit);
    }

    if (CookPoints.size() <= 4)
    {
        fit[0] = -1;
        fit[1] = -1;
        fit[2] = -1;
    }
    else
    {
        fit[0] = center.x;
        fit[1] = center.y;
        fit[2] = radius;
    }
    return;
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

void matchKernel(cv::OutputArray _kernel)
{
    cv::Mat temp;
    temp = cv::imread("./Mask.png",0);
        
    _kernel.create(temp.size(), CV_32FC1);
    cv::Mat kernel = _kernel.getMat();
    cv::Size kerSize = kernel.size();
    for (int m = 0; m < kerSize.height; m++)
    {
        for (int n = 0; n < kerSize.width; n++)
        {
            if(temp.at<unsigned char>(cv::Point(n,m)) == 0x80)
            {
                kernel.at<float>(cv::Point(n,m)) = 0.0;
            }
            else
            {
                if(temp.at<unsigned char>(cv::Point(n,m)) > 0x80)
                {
                    kernel.at<float>(cv::Point(n,m)) = 1.0;
                }
                else
                {
                    kernel.at<float>(cv::Point(n,m)) = -1.0;
                }
            }
        }       
    }
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
