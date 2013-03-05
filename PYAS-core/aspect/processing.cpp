#include "processing.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

Aspect::Aspect()
{
    initialNumChords = 20;
    chordsPerAxis = 5;
    chordThreshold = 70;
    solarRadius = 105;
    limbWidth = 3;
    fiducialTolerance = 3;
    fiducialLength = 15;
    fiducialWidth = 2; 
    fiducialThreshold = 5;
    fiducialNeighborhood = 1;
    numFiducials = 10;
    pixelCenter = cv::Point2f(-1.0, -1.0);
    pixelError = cv::Point2f(0.0, 0.0);
}

Aspect::~Aspect()
{

}

void Aspect::LoadFrame(cv::Mat inputFrame)
{
    inputFrame.copyTo(frame);
    frameSize = frame.size();
    centerValid = false;
    fiducialsValid = false;
}

void Aspect::FindPixelCenter()
{
    std::vector<int> rows, cols;
    std::vector<float> crossings, midpoints;
    int rowStep, rowStart, colStep, colStart, limit, K, M;
    float mean, std;

    rows.clear();
    cols.clear();

    //SANITIZE
    if(pixelCenter.x < 0 || pixelCenter.y < 0)
    {
	limit = initialNumChords;

	rowStep = frameSize.height/limit;
	colStep = frameSize.width/limit;

	rowStart = rowStep/2;
	colStart = colStep/2;
    }
    else
    {
	limit = chordsPerAxis;
	rowStep = (2*solarRadius + 1)/limit;
	colStep = (2*solarRadius + 1)/limit;

	rowStart = pixelCenter.y - solarRadius + rowStep/2;
	colStart = pixelCenter.x - solarRadius + colStep/2;
    }

    for (int k = 0; k < limit; k++)
    {
	rows.push_back(rowStart + k*rowStep);
	cols.push_back(colStart + k*colStep);
    }

    pixelCenter = cv::Point2f(0.0,0.0);
    limbCrossings.clear();

    for (int dim = 0; dim < 2; dim++)
    {
	if (dim) K = rows.size();
	else K = cols.size();

	midpoints.clear();
	for (int k = 0; k < K; k++)
	{
	    crossings.clear();
	    if (dim) EvaluateChord(frame.row(rows[k]), crossings);
	    else EvaluateChord(frame.col(cols[k]), crossings);
	    
	    if (crossings.size() != 2) continue;
	    else
	    {
		for (int l = 0; l < crossings.size(); l++)
		{
		    if (dim) limbCrossings.add(crossings[l], rows[k]);
		    else limbCrossings.add(cols[k], crossings[l]);
		}
		midpoints.push_back((crossings[0] + crossings[1])/2);
	    }
	}

		mean = 0;
	M = midpoints.size();
	for (int m = 0; m < M; m++)
	    mean += midpoints[m];
	mean = (float)mean/M;
	
	std = 0;
	for (int m = 0; m < M; m++)
	    std += pow(midpoints[m]-mean,2);
	std = sqrt(std/M);
	
	std::cout << mean << " " << std << "\n";

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
    centerValid = true;
}

void Aspect::FindLimbCrossings(std::vector<int> rows, std::vector<int> cols)
{
    limbCrossings.clear();
    std::vector<float> crossings;
    //Search rows
    for (int m = 0; m < rows.size(); m++)
    {
	crossings.clear();
	EvaluateChord(frame.row(rows[m]), crossings);
	for (int k = 0; k < crossings.size(); k++)
	    limbCrossings.add(crossings[k], rows[m]);
    }
    //Search columns
    for (int n = 0; n < cols.size(); n++)
    {
	crossings.clear();
	EvaluateChord(frame.col(cols[n]), crossings);
	for (int k = 0; k < crossings.size(); k++)
	    limbCrossings.add(cols[n], crossings[k]);
    }	
    return;
}

int Aspect::EvaluateChord(cv::Mat chord, std::vector<float> &crossings)
{
    std::vector<int> edges;
    int thisValue, lastValue;
    int K = chord.total();
    int edgeSpread;

    int edge, min, max;
    int x, xx, y, xy, N;
    float D, slope, intercept;
	
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

	//compute the fit to the neighborhood
	x = 0;
	y = 0;
	xx = 0;
	xy = 0;

	for (int l = min; l <= max; l++)
	{
	    x += l;
	    xx += l*l;
	    y += chord.at<unsigned char>(l);
	    xy += l*chord.at<unsigned char>(l);
	}
	D = N*xx -x*x;
	slope = (float) (N*xy - x*y)/D;
	intercept = (float) (y*xx - xy*x)/D;
	
	//push the crossing into the output vector
	crossings.push_back(((float)chordThreshold - intercept)/slope);
    }
    return 0; 
}

void Aspect::FindPixelFiducials()
{
    cv::Scalar mean, stddev;
    cv::Mat correlation, nbhd, A, B, X;
    cv::Point2f fiducialOffset;
    cv::Range rowRange, colRange;
    float threshold, thisValue, minValue, D;
    int minIndex, M, N, index, row, col;
    
    pixelFiducials.clear();

    matchKernel(kernel);
    kernelSize = kernel.size();
    
    rowRange = GetSafeRange(pixelCenter.y-solarRadius, pixelCenter.y+solarRadius, frameSize.height);
    colRange = GetSafeRange(pixelCenter.x-solarRadius, pixelCenter.x+solarRadius, frameSize.width);
    solarImage = frame(rowRange, colRange);
    solarSize = solarImage.size();
    fiducialOffset = cv::Point(colRange.start, rowRange.start);
    
    cv::filter2D(solarImage, correlation, CV_32FC1, kernel, cv::Point(-1,-1));
    cv::normalize(correlation,correlation,0,1,cv::NORM_MINMAX);
	
    cv::meanStdDev(correlation, mean, stddev);

    threshold = mean[0] + fiducialThreshold*stddev[0];
	
    for (int m = 1; m < solarSize.height-1; m++)
    {
	for (int n = 1; n < solarSize.width-1; n++)
	{	 
	    thisValue = correlation.at<float>(m,n);
	    if(thisValue > threshold)
	    {
		if((thisValue > correlation.at<float>(m, n+1)) &
		   (thisValue > correlation.at<float>(m, n- 1)) &
		   (thisValue > correlation.at<float>(m+1, n)) &
		   (thisValue > correlation.at<float>(m-1, n)))
		{
		    if (pixelFiducials.size() < numFiducials)
		    {
			pixelFiducials.push_back(cv::Point2f(n, m));
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
    for (int k = 0; k < pixelFiducials.size(); k++)
    {
	rowRange = GetSafeRange(pixelFiducials[k].y - fiducialNeighborhood,
				pixelFiducials[k].y + fiducialNeighborhood,
				solarSize.height);

	colRange = GetSafeRange(pixelFiducials[k].x - fiducialNeighborhood,
				pixelFiducials[k].x + fiducialNeighborhood,
				solarSize.width);
	M = rowRange.end - rowRange.start + 1;
	N = colRange.end - colRange.start + 1;
	A.create(M*N,6, CV_32FC1);
	B.create(M*N,1, CV_32FC1);
	X.create(6,1, CV_32FC1);
	for(int m = 0; m < M; m++)
	{
	    for(int n = 0; n < N; n++)
	    {
		index = M*m+n;
		row = rowRange.start + m;
		col = colRange.start + n;
		A.at<float>(index, 0) = (float) m*m;
		A.at<float>(index, 1) = (float) m*n;
		A.at<float>(index, 2) = (float) n*n;
		A.at<float>(index, 3) = (float) m;
		A.at<float>(index, 4) = (float) n;
		A.at<float>(index, 5) = (float) 1.0;
		B.at<float>(index) = correlation.at<float>(row,col);
	    }
	}
	X = (A.t()*A).inv()*A.t()*B;
//	cv::solve(A.t()*A, A.t()*B, X, cv::DECOMP_CHOLESKY);
	D = (4.0*X.at<float>(0)*X.at<float>(2) - pow(X.at<float>(1),2));
	pixelFiducials[k].y = (  X.at<float>(1)*X.at<float>(4) -
			       2*X.at<float>(2)*X.at<float>(3)  )/D +
	    rowRange.start;
	pixelFiducials[k].x = (  X.at<float>(1)*X.at<float>(3) - 
			       2*X.at<float>(0)*X.at<float>(4)  )/D +
	    colRange.start;
    }

    //Adjust positions to original image pixels
    for (int k = 0; k < pixelFiducials.size(); k++)
    {
	pixelFiducials[k].x += (float) fiducialOffset.x;
	pixelFiducials[k].y += (float) fiducialOffset.y;
    }
    fiducialsValid = true;
    return;
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
    if(fiducialsValid == false)
	FindPixelFiducials();
    fiducials.clear();
    for (int k = 0; k < pixelFiducials.size(); k++)
	fiducials.push_back(pixelFiducials[k]);
    return;
}
