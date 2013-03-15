/* This code should encapsulate all the code necessary for generating solar aspect.
   It generates and stores all the variables needed for determining limb crossings,
   center, fiducials, etc, as well as a local copy of the current frame. 

   The idea would be to call "LoadFrame" once, at which point this module would
   reset all its values. Requests for data are made with the Get functions, and
   the necessary data is calculated and provided, usually as a CoordList or a
   cv::Point. All the functions doing real computation are private. If the desired
   data is already up to date when a Get function is called, then it won't be
   re-calculated.

   Remains to be added:
   -Configuration Set functions. Right now all the parameters are hard-coded in the
   constructor
   -Coordinate transforms
   -Catch bad center in FindPixelCenter
*/
#include "processing.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#define ID_TO_SCREEN(a) (a > 0 ? 45*a+6*a*(a+1) : 48*a-6*a*(a-1))

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
}

Aspect::~Aspect()
{

}

void Aspect::LoadFrame(cv::Mat inputFrame)
{
    inputFrame.copyTo(frame);
    frameSize = frame.size();
    limbCrossings.clear();
    centerValid = false;
    pixelFiducials.clear();
    fiducialsValid = false;

    fiducialIDs.clear();
    fiducialIDsValid = false;
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
    if(pixelCenter.x < 0 || pixelCenter.y < 0 ||
		       pixelCenter.x >= frameSize.width || 
		       pixelCenter.y >= frameSize.height)
    {
	limit = initialNumChords;

	rowStep = frameSize.height/limit;
	colStep = frameSize.width/limit;

	rowStart = rowStep/2;
	colStart = colStep/2;
    }
    //Otherwise, only use the solar subimage
    else
    {
	limit = chordsPerAxis;
	rowRange = GetSafeRange(pixelCenter.y - solarRadius,
				pixelCenter.y + solarRadius, 
				frameSize.height);

	colRange = GetSafeRange(pixelCenter.x - solarRadius,
				pixelCenter.x + solarRadius, 
				frameSize.width);

	rowStep = (rowRange.end - rowRange.start + 1)/limit;
	colStep = (colRange.end - colRange.start + 1)/limit;

	rowStart = rowRange.start + rowStep/2;
	colStart = colRange.start + colStep/2;
    }

    //Generate vectors of chord locations
    for (int k = 0; k < limit; k++)
    {
	rows.push_back(rowStart + k*rowStep);
	cols.push_back(colStart + k*colStep);
    }

    //Initialize
    pixelCenter = cv::Point2f(0.0,0.0);
    limbCrossings.clear();

    //For each dimension
    for (int dim = 0; dim < 2; dim++)
    {
	if (dim) K = rows.size();
	else K = cols.size();

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
		for (int l = 0; l < crossings.size(); l++)
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
	M = midpoints.size();
	for (int m = 0; m < M; m++)
	    mean += midpoints[m];
	mean = (float)mean/M;
	
	//Determine the std dev of the midpoints
	std = 0;
	for (int m = 0; m < M; m++)
	    std += pow(midpoints[m]-mean,2);
	std = sqrt(std/M);
	
	//Store the Center and RMS Error for this dimension
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

    //Add a test here for valid center
    
    centerValid = true;
}

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
    lastValue = chord.at<unsigned char>(0);
    for (int k = 1; k < K; k++)
    {
	thisValue = chord.at<unsigned char>(k);

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
    for (int k = 1; k < edges.size(); k++)
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
    if (edges.size() != 2)
    {
	return -1;
    }
    // if the pair isn't a rising edge followed by a falling edge, ignore the chord.
    else if(!(edges[0] > 0  && edges[1] < 0))
    {
	return -1;
    }

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
	    return -1;

	//compute fit to neighborhood
	x.clear(); y.clear();
	for (int l = min; l <= max; l++)
	{
	    x.push_back(l);
	    y.push_back((float) chord.at<unsigned char>(l));
	}
	GetLinearFit(x,y,fit);
	crossings.push_back(((float)chordThreshold - fit[0])/fit[1]);
    }
    return 0; 
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
		    for (int k = 0; k < pixelFiducials.size(); k++)
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

		    if (pixelFiducials.size() < numFiducials)
		    {
			pixelFiducials.add(n, m);
		    }
		    else
		    {
			minValue = kernelSize.width*kernelSize.height*256;
			minIndex = -1;
			for(int k = 0; k < numFiducials; k++)
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
    for (int k = 0; k < pixelFiducials.size(); k++)
    {
	//Get safe ranges for for the neighborhood around the fiducial
	rowRange = GetSafeRange(pixelFiducials[k].y - fiducialNeighborhood,
				pixelFiducials[k].y + fiducialNeighborhood,
				imageSize.height);

	colRange = GetSafeRange(pixelFiducials[k].x - fiducialNeighborhood,
				pixelFiducials[k].x + fiducialNeighborhood,
				imageSize.width);
	//Compute the centroid of the region around the local max
	//in the correlation image
	Cm = 0.0; Cn = 0.0; average = 0.0;
	for(int m = rowRange.start; m <= rowRange.end; m++)
	{
	    for(int n = colRange.start; n <= colRange.end; n++)
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

    fiducialsValid = true;
    return;
}

void Aspect::FindFiducialIDs()
{
    int K;
    float rowDiff, colDiff;
    IndexList rowPairs, colPairs;
    CoordList trash;
    K = pixelFiducials.size();
    fiducialIDs.clear();
    if(fiducialsValid == false)
	GetPixelFiducials(trash);
    for (int k = 0; k < K; k++)
    {
	fiducialIDs.push_back(cv::Point(-100, -100));
    }

    //Find fiducial pairs that are spaced correctly
    for (int k = 0; k < K; k++)
    {
	for (int l = k+1; l < K; l++)
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
    
    for (int k = 0; k < rowPairs.size(); k++)
    {
	rowDiff = pixelFiducials[rowPairs[k].y].y 
	    - pixelFiducials[rowPairs[k].x].y;
	for (int d = 0; d < mDistances.size(); d++)
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

    for (int k = 0; k < colPairs.size(); k++)
    {
	colDiff = pixelFiducials[colPairs[k].x].x 
	    - pixelFiducials[colPairs[k].y].x;
	for (int d = 0; d < nDistances.size(); d++)
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
}	
	    
cv::Point2f Aspect::PixelToScreen(cv::Point2f pixelPoint)
{
    std::vector<float> x, y, fit;
    cv::Point2f screenPoint;
    for (int dim = 0; dim < 2; dim++)
    {
	x.clear();
	y.clear();
	for (int k = 0; k < pixelFiducials.size(); k++)
	{
	    if(dim == 0)
	    {
		if(fiducialIDs[k].x < -10)
		    continue;
		x.push_back(pixelFiducials[k].x);
		y.push_back(ID_TO_SCREEN(fiducialIDs[k].x));
	    }
	    else
	    {
		if(fiducialIDs[k].y < -10)
		    continue;
		x.push_back(pixelFiducials[k].y);
		y.push_back(ID_TO_SCREEN(fiducialIDs[k].y));
	    }
	}
	GetLinearFit(x,y,fit);
	if(dim == 0)
	    screenPoint.x = fit[0] + fit[1]*pixelPoint.x;
	else
	    screenPoint.y = fit[0] + fit[1]*pixelPoint.y;
    }
    return screenPoint;
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
	    //	std::cout << temp.at<char>(cv::Point(n,m)) << " ";
	    //	std::cout << kernel.at<float>(cv::Point(n,m)) << " ";
	}
	//std::cout << "\n";		
    }
}

void GetLinearFit(const std::vector<float> &x, const std::vector<float> &y, std::vector<float> &fit)
{

    if (x.size() != y.size())
    {
	std::cout << "Error in GetLinearFit: x and y vectors should be same length\n";
	return;
    }

    float X, Y, XX, XY, D, slope, intercept;

    X = 0;
    Y = 0;
    XX = 0;
    XY = 0;

    for (int l = 0; l < x.size(); l++)
    {
	X += x[l];
	XX += x[l]*x[l];
	Y += y[l];
	XY += x[l]*y[l];
    }
    D = x.size()*XX -X*X;
    slope = (x.size()*XY - X*Y)/D;
    intercept = (Y*XX - XY*X)/D;
	
    fit.clear();
    fit.push_back(intercept);
    fit.push_back(slope);
}


cv::Range Aspect::GetSafeRange(int start, int stop, int size)
{
    cv::Range range;
    range.start = (start > 0) ? (start) : 0;
    range.end = (stop < size - 1) ? (stop) : (size - 1);
    return range;
}

void Aspect::GetPixelCenter(cv::Point2f &center)
{
    if (centerValid == false)
	FindPixelCenter();
    center = pixelCenter;
}

void Aspect::GetPixelError(cv::Point2f &error)
{
    if (centerValid == false)
	FindPixelCenter();
    error = pixelError;
}

void Aspect::GetPixelCrossings(CoordList& crossings)
{
    if (centerValid == false)
	FindPixelCenter();
    crossings.clear();
    for (int k = 0; k < limbCrossings.size(); k++)
	crossings.push_back(limbCrossings[k]);
    
    return;
}

void Aspect::GetPixelFiducials(CoordList& fiducials)
{
    cv::Range rowRange, colRange;
    cv::Mat solarImage;
    cv::Point offset;
    if(fiducialsValid == false)
    {
	if (centerValid == false)
	{  
	    FindPixelCenter();
	}
	rowRange = GetSafeRange(pixelCenter.y-solarRadius, pixelCenter.y+solarRadius, frameSize.height);
	colRange = GetSafeRange(pixelCenter.x-solarRadius, pixelCenter.x+solarRadius, frameSize.width);
	solarImage = frame(rowRange, colRange);
	offset = cv::Point(colRange.start, rowRange.start);
	FindPixelFiducials(solarImage, offset);
    }
    fiducials.clear();
    for (int k = 0; k < pixelFiducials.size(); k++)
	fiducials.push_back(pixelFiducials[k]);
    return;
}

void Aspect::GetFiducialIDs(IndexList& IDs)
{
    if(fiducialIDsValid == false)
	FindFiducialIDs();
    IDs.clear();
    for (int k = 0; k < fiducialIDs.size(); k++)
	IDs.push_back(fiducialIDs[k]);
    return;
}

void Aspect::GetScreenCenter(cv::Point2f &center)
{
    if(fiducialIDsValid == false)
	FindFiducialIDs();
    center = PixelToScreen(pixelCenter);
}
