/* This code should encapsulate all the code necessary for generating solar aspect.
   center, fiducials, etc, as well as a local copy of the current frame. 

   The idea would be to call "LoadFrame" once, at which point this module would
   reset all its values. Next, the "Run" function computes all the data products possible.
   Requests for data are made with the Get functions, and the necessary data is provided if
   its available, otherwise an error code is returned. Most data is stored as either CoordList or a
   cv::Point. All the functions doing real computation are private, other than "Run."

   Todo:
   -Check condition number in GetLinear

*/
#include "processing.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

cv::Point2f fiducialIDtoScreen(cv::Point2i id) {
    cv::Point2f result;

    result.x = 6 * ((id.x >= 0 ? 45*id.x+3*id.x*(id.x-1) : 48*id.x-3*id.x*(id.x+1)) - 15*id.y);
    result.y = 6 * ((id.y >= 0 ? 45*id.y+3*id.y*(id.y-1) : 48*id.y-3*id.y*(id.y+1)) + 15*id.x);

    return result;
}

Aspect::Aspect()
{
    initialNumChords = 20;
    chordsPerAxis = 5;
    chordThreshold = 50;
    solarRadius = 105;
    limbWidth = 3;
    fiducialTolerance = 2;
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
}

Aspect::~Aspect()
{

}

int Aspect::LoadFrame(cv::Mat inputFrame)
{
    frameProcessed == false;
    //std::cout << "Aspect: Loading Frame" << std::endl;
    if(inputFrame.empty())
    {
	std::cout << "Aspect: Tried to load empty frame" << std::endl;
	frameValid = false;
	return 1;
    }
    else
    {
	cv::Size inputSize = inputFrame.size();
	if (inputSize.width == 0 || inputSize.height == 0)
	{
	    std::cout << "Aspect: Tried to load a frame with dimension 0" << std::endl;
	    frameValid = false;
	    return 1;
	}
	else
	{
	    inputFrame.copyTo(frame);
	    frameSize = frame.size();

	    frameValid = true;
	    return 0;
	}
    }
    return -1;
}

int Aspect::Run()
{
    cv::Range rowRange, colRange;
    cv::Mat solarImage;
    cv::Size solarSize;
    cv::Point offset;

    limbCrossings.clear();
    crossingsValid = false;
    
    centerValid = false;
    
    pixelFiducials.clear();
    fiducialsValid = false;

    fiducialIDs.clear();
    fiducialIDsValid = false;
    
    mapping.clear();
    mapping.resize(4);
    conditionNumbers.clear();
    conditionNumbers.resize(2);
    mappingValid = false;

    frameProcessed = false;

    if (frameValid == false)
    {
	std::cout << "Aspect: Frame is empty." << std::endl;
	return 1;
    }
    else
    {
	//std::cout << "Aspect: Finding Center" << std::endl;
	FindPixelCenter();
	if (limbCrossings.size() == 0)
	{
	    std::cout << "Aspect: No Limb Crossings." << std::endl;
	    return 1;
	}
	else if (limbCrossings.size() < 4)
	{
	    std::cout << "Aspect: Too Few Limb Crossings." << std::endl;
	    return 1;
	}
	else
	{
	    crossingsValid = true;
	}


	if (pixelCenter.x < 0 || pixelCenter.x >= frameSize.width ||
	    pixelCenter.y < 0 || pixelCenter.y >= frameSize.height ||
	    std::isnan(pixelCenter.x) || std::isnan(pixelCenter.y))
	{
	  std::cout << "Aspect: Center Out-of-bounds:" << pixelCenter << std::endl;
	    pixelCenter = cv::Point2f(-1,-1);
	    return 1;
	}
	else if (pixelError.x > 50 || pixelError.y > 50 ||
		 std::isnan(pixelError.x) || std::isnan(pixelError.y))
	  {
	    std::cout << "Aspect: Center Error greater than 50 pixels: " << pixelError << std::endl;
	    pixelCenter = cv::Point2f(-1,-1);
	    return 1;
	  }
	else
	{
	    centerValid = true;
	}
	
	//Find solar subImage
	//std::cout << "Aspect: Finding solar subimage" << std::endl;
	rowRange = SafeRange(pixelCenter.y-solarRadius, pixelCenter.y+solarRadius, frameSize.height);
	colRange = SafeRange(pixelCenter.x-solarRadius, pixelCenter.x+solarRadius, frameSize.width);
	solarImage = frame(rowRange, colRange);
	if (solarImage.empty())
	{
	    std::cout << "Aspect: Solar Image too empty." << std::endl;
	    return 1;  
	}
	else
	{
	    solarSize = solarImage.size();
	}

	if (solarSize.width < (int) fiducialSpacing + 2*fiducialLength || 
	    solarSize.height < (int) fiducialSpacing + 2*fiducialLength)
        {
	    std::cout << "Aspect: Solar Image too small." << std::endl;
	    return 1;
	}

	//Define offset for converting subimage locations to frame locations
	offset = cv::Point(colRange.start, rowRange.start);
        if (offset.x < 0 || offset.x >= (frameSize.width - solarSize.width + 1) ||
		offset.y < 0 || offset.y >= (frameSize.height - solarSize.height + 1))
	{
	    std::cout << "Aspect: Solar Image Offset out of bounds." << std::endl;
	    return 1;
	}
	    
	//Find fiducials
	//std::cout << "Aspect: Finding Fiducials" << std::endl;
	   FindPixelFiducials(solarImage, offset);
	if (pixelFiducials.size() == 0)
	{
	    std::cout << "Aspect: No Fiducials found" << std::endl;
	    return 1;
	}
	else if (pixelFiducials.size() < 3)
	{
	    std::cout << "Aspect: Too Few Fiducials" << std::endl;
	    return 1;
	}
	else
	{
	    fiducialsValid = true;
	}

	//Find fiducial IDs
	//std::cout << "Aspect: Finding fiducial IDs" << std::endl;
	FindFiducialIDs();
	if (fiducialIDs.size() == 0)
	{
	    std::cout << "Aspect: No Valid IDs" << std::endl;
	    return 1;
	}
	else
	{
	    fiducialIDsValid = true;
	}
	
	//std::cout << "Aspect: Finding Mapping" << std::endl;
	FindMapping();
	if (/*ILL CONDITIONED*/ false)
	{
	    std::cout << "Aspect: Mapping is ill-conditioned." << std::endl;
	    return 1;
	}
	else
	{
	    mappingValid = true;
	}
    }
    frameProcessed = true;
    return 0;
}


/****************************************************

Data product Get functions

***************************************************/

int Aspect::GetPixelCrossings(CoordList& crossings)
{
    if (crossingsValid == true && frameProcessed == true)
    {
	crossings.clear();
	for (unsigned int k = 0; k <  limbCrossings.size(); k++)
	    crossings.push_back(limbCrossings[k]);
	return 0;
    }
    else return -1;
}

int Aspect::GetPixelCenter(cv::Point2f &center)
{
    if (centerValid == true && frameProcessed == true)
    {
	center = pixelCenter;
	return 0;
    }
    else return -1;
}

int Aspect::GetPixelError(cv::Point2f &error)
{
    if (centerValid == true && frameProcessed == true)
    {
	error = pixelError;
	return 0;
    }
    else return -1;
}

int Aspect::GetPixelFiducials(CoordList& fiducials)
{
    if (fiducialsValid == true && frameProcessed == true)
    {
	fiducials.clear();
	for (unsigned int k = 0; k < pixelFiducials.size(); k++)
	    fiducials.push_back(pixelFiducials[k]);
	return 0;
    }
    else return -1;
	
}

int Aspect::GetFiducialIDs(IndexList& IDs)
{
    if (fiducialIDsValid == true && frameProcessed == true)
    {
	IDs.clear();
	for (unsigned int k = 0; k <  fiducialIDs.size(); k++)
	    IDs.push_back(fiducialIDs[k]);
	return 0;
    }
    else return -1;
}

int Aspect::GetMapping(std::vector<float>& map)
{
    if(mappingValid == true && frameProcessed == true)
    {
	map.clear();
	for (unsigned int k = 0; k < mapping.size(); k++)
	    map.push_back(mapping[k]);
	return 0;
    }
    else return -1;
}

int Aspect::GetScreenCenter(cv::Point2f &center)
{
    
    if(centerValid == true && mappingValid == true && frameProcessed == true)
    {
	center = PixelToScreen(pixelCenter);
	return 0;
    }
    else return -1;
}


/*************************************************************************

Aspect Parameter Set/Get Functions

*************************************************************************/

float Aspect::GetFloat(FloatParameter variable)
{
    switch(variable)
    {
    case FiducialSpacing:
	return fiducialSpacing;
    case FiducialSpacingTol:
	return fiducialSpacingTol;
    default:
	return 0;
    }
}

int Aspect::GetInteger(IntParameter variable)
{
    switch(variable)
    {
    case InitialNumChords:
	return initialNumChords;
    case ChordsPerAxis:
	return chordsPerAxis;
    case LimbWidth:
	return limbWidth; 
    case FiducialTolerance:
	return fiducialTolerance;
    case SolarRadius:
	return solarRadius;
    case FiducialLength:
	return fiducialLength;
    case FiducialWidth:
	return fiducialWidth;
    case FiducialThreshold:
	return fiducialThreshold;
    case FiducialNeighborhood:
	return fiducialNeighborhood;
    case NumFiducials:
	return numFiducials;
    default:
	return 0;
    }
}

void Aspect::SetFloat(FloatParameter variable, float value)
{
    switch(variable)
    {
    case FiducialSpacing:
	fiducialSpacing = value;
	break;
    case FiducialSpacingTol:
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
    case InitialNumChords:
	initialNumChords = value;
	break;
    case ChordsPerAxis:
	chordsPerAxis = value;
	break;
    case LimbWidth:
	limbWidth = value;
	break;
    case FiducialTolerance:
	fiducialTolerance = value;
	break;
    case SolarRadius:
	solarRadius = value;
	break;
    case FiducialLength:
	fiducialLength = value;
	break;
    case FiducialWidth:
	fiducialWidth = value;
	break;
    case FiducialThreshold:
	fiducialThreshold = value;
	break;
    case FiducialNeighborhood:
	fiducialNeighborhood = value;
	break;
    case NumFiducials:
	numFiducials = value;
	break;
    default:
	return ;
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
    int thisValue, lastValue;
    int K = chord.total();
    int edgeSpread;
    int edge, min, max;
    int N;
	
    //for each pixel, check if the pixel lies on a potential limb
    lastValue = (int) chord.at<unsigned char>(0);
    for (int k = 1; k < K; k++)
    {
	thisValue = (int) chord.at<unsigned char>(k);

	//check for a rising edge, save the index above the threshold
	if (lastValue <= chordThreshold && thisValue > chordThreshold)
	{
	    edges.push_back(k);
	}
	//check for a falling edge
	else if(lastValue > chordThreshold && thisValue <= chordThreshold)
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
	//positive if the region is below the chordThreshold
	edgeSpread = edges[k] + edges[k-1];

	//if the pair is along a fiducial
	if(abs(edgeSpread - fiducialLength) <= fiducialTolerance || 
	   // or across a fiducial
	   abs(edgeSpread - fiducialWidth) <= fiducialTolerance ||
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
	    crossings.push_back(((float)chordThreshold - fit[0])/fit[1]);
	}
    }
    return 0; 
}

void Aspect::FindPixelCenter()
{
    std::vector<int> rows, cols;
    std::vector<float> crossings, midpoints;
    int rowStart, colStart, rowStep, colStep, limit;
    unsigned int K, M, m, k, l, dim;
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
	//std::cout << "Aspect: Finding new center" << std::endl;
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
    for (k = 0; k < limit; k++)
    {
	rows.push_back(rowStart + k*rowStep);
	cols.push_back(colStart + k*colStep);
    }

    //Initialize
    pixelCenter = cv::Point2f(0,0);
    limbCrossings.clear();

    //For each dimension
    for (dim = 0; dim < 2; dim++)
    {
	//if (dim) std::cout << "Aspect: Searching Rows" << std::endl;
	//else std::cout << "Aspect: Searching Cols" << std::endl;

	if (dim) K =  rows.size();
	else K =  cols.size();

	//Find the midpoints of the chords.
	//For each chord
	midpoints.clear();
	for (k = 0; k < K; k++)
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
		for (l = 0; l <  crossings.size(); l++)
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
	for (m = 0; m < M; m++)
	    mean += midpoints[m];
	mean = (float)mean/M;
	
	//Determine the std dev of the midpoints
	std = 0;
	for (m = 0; m < M; m++)
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

		    if ( pixelFiducials.size() < numFiducials)
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


    rowPairs.clear();
      colPairs.clear();
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
		  fiducialIDs[rowPairs[k].x].y = d-7;
		    fiducialIDs[rowPairs[k].y].y = d+1-7;
		}
		else
		{
		    fiducialIDs[rowPairs[k].x].y = d+1-7;
		    fiducialIDs[rowPairs[k].y].y = d-7;
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
		    fiducialIDs[colPairs[k].x].x = d-7;
		    fiducialIDs[colPairs[k].y].x = d+1-7;
		}
		else
		{
		    fiducialIDs[colPairs[k].x].x = d+1-7;
		    fiducialIDs[colPairs[k].y].x = d-7;
		}
	    }
	}
    }
    fiducialIDsValid = true;
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
    mappingValid = true;
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
    
    cv::solve(A,B,X,cv::DECOMP_CHOLESKY);
    
    fit.clear();
    fit.resize(2);
    fit[0] = X.at<float>(1); //intercept
    fit[1] = X.at<float>(0); //slope
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
