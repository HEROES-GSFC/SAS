#include "processing.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

int chordCenter(const unsigned char* image, int M, int N, int chords, int thresh, double* center)
{
    int loc = 0;
    //CRAZINESS!
    int total[2] = {0};
    center[0] = 0; //will contain center in the X direction
    center[1] = 0; //will contain center in the Y direction
    center[2] = 0; //will contain the number of row chords
    center[3] = 0; //will contain the number of column chords
    center[4] = 0; //will contain sample standard deviation in the X direction
    center[5] = 0; //will contain sample standard deviation in the Y direction
    double temp;
    for (int l = 0; l < chords; l++)
    {
	loc = (int) ((float) l*M/chords + M/(2*chords));
	//std::cout << "Trying row: " << loc << "\n";
	if ((temp = chord(image, thresh, 4, loc, M, N, 0)) >= 0)
	{
	    total[0]++;
	    center[0] += temp;
	    center[4] += temp*temp;
	    //std::cout << "Row: " << loc << ", value: " << temp << std::endl;
	}
		
	loc = (int) ((float) l*N/chords + N/(2*chords));
	//std::cout << "Trying col: " << loc << "\n";
	if ((temp = chord(image, thresh, 2, loc, M, N, 1)) >=0)
	{
	    total[1]++;
	    center[1] += temp;
	    center[5] += temp*temp;
	    //std::cout << "Col: " << loc << ", value: " << temp << std::endl;
	}
    }
    if(!total[0] || !total[1])
    {
	std::cout << "failed to find any chords\n";
	center[0] = -1; center[1] = -1;
    }
    else
    {
	center[2] = total[0];
	center[3] = total[1];
	center[0] = center[0]/total[0];
	center[1] = center[1]/total[1];
	center[4] = sqrt((center[4]-center[2]*center[0]*center[0])/(center[2]-1));
	center[5] = sqrt((center[5]-center[3]*center[1]*center[1])/(center[3]-1));
	//std::cout << "  Chords found: " << total[0] << " rows, " << total[1] << " columns" << std::endl;
    }
	
    return 0;
}

double chord(const unsigned char* image, int thresh, int width, int loc, int M, int N, bool mode)
{
    std::vector<bool> edge_dir;
    std::vector< std::vector<int> > idx;
    std::vector< std::vector<char> > edge;
    int cur;
    int last = -1;
    int K;
    int min, max;
    int x, xx, y, xy, Num;
    double D, slope, intercept, center;
	
    if (mode) K = M;
    else K = N;
    for (int k = 0; k < K; k++)
    {
	if (mode) cur = image[N*k + loc];
	else cur = image[N*loc + k];
		
	if (last < thresh && cur >= thresh)
	{
	    //std::cout << "    Rising Edge Found (" << loc << "): " << k << "\n";
	    edge_dir.push_back(0);
	    if ((k-width) < 0) min = 0;
	    else min = k-width;
			
	    if ((k+width) > K) max = K;
	    else max = k+width;
			
	    idx.resize(idx.size() + 1);
	    edge.resize(edge.size() +1);
	    for (int e = min; e < max; e++)
	    {
		idx.back().push_back(e);
		if (mode) edge.back().push_back(image[N*e + loc]);
		else edge.back().push_back(image[N*loc + e]);
	    }
	    k = idx.back().back();
	}
	else if(last >= thresh && cur < thresh)
	{
	    //std::cout << "    Falling Edge Found (" << loc << "): " << k-1 << "\n";
	    edge_dir.push_back(1);
	    if ((k-width-1) < 0)	min = 0;
	    else min = k-width-1;
			
	    if ((k+width-1) > K) max = K;
	    else max = k+width-1;
			
	    idx.resize(idx.size() + 1);
	    edge.resize(edge.size() +1);
	    for (int e = min; e < max; e++)
	    {
		idx.back().push_back(e);
		if (mode) edge.back().push_back(image[N*e + loc]);
		else edge.back().push_back(image[N*loc + e]);
	    }
	    k = idx.back().back();
	}
	if (mode) last = image[N*k + loc];
	else last = image[N*loc + k];
    }
	
    if (edge_dir.size() != 2)
    {
	//std::cout << "Wrong number of limbs: "<< edge_dir.size() <<"\n";
	//for (unsigned int k = 0; k < edge_dir.size(); k++)
	//	std::cout << "Edge Loc: " << (int) idx[k][width] << "\n";
	return -1;
    }
    else if( edge_dir[0] != 0 && edge_dir[1] != 1)
    {
	//std::cout << "Wrong limb direction\n";
	//std::cout << "Edge Dir: ";
	//for (unsigned int k = 0; k < edge_dir.size(); k ++)
	//	std::cout << edge_dir[k] << " ";
	//std:: cout << "\n";
	return -1;
    }
    else
    {
	//std::cout << "Performing Linear Fits\n";
	center = 0;
	for (int k = 0; k < 2; k++)
	{
	    Num = idx[k].size();
	    x = 0;
	    y = 0;
	    xx = 0;
	    xy = 0;
	    //std::cout << "idx length: " << idx[k].size() << "\n";
	    //std::cout << "edge length: " << edge[k].size() << "\n";
	    for (int e = 0; e < Num; e++)
	    {
		x += idx[k][e];
		xx += idx[k][e]*idx[k][e];
		y += edge[k][e];
		xy += edge[k][e]* idx[k][e];
	    }
	    D = Num*xx -x*x;
	    slope = (double) (Num*xy - x*y)/D;
	    intercept = (double) (y*xx - xy*x)/D;
	    center += .5*(thresh - intercept)/slope;
	}
    }
    return center;
}

int morphPeakFind(cv::Mat image, morphParams params, int* locs, int numLocs)
{
    int thresh, length;
    int locIdx = 0;
    cv::Size imSize = image.size();
    cv::Scalar mean, stddev;
    cv::Mat proj, kernel;
	
    int minIdx;
    float min;
	
    if (params.dim)
    {
	length = imSize.height;
	kernel = cv::Mat(params.tophatWidth,1, CV_32FC1, 1);
    }
    else
    {
	length = imSize.width;
	kernel = cv::Mat(1, params.tophatWidth,CV_32FC1, 1);
    }
	
    cv::reduce(image, proj, params.dim, CV_REDUCE_SUM, CV_32F);
//	std::cout << "Projection Size: " << (proj.size()).height << " by " << (proj.size()).width << "\n";
    cv::morphologyEx(proj, proj, cv::MORPH_BLACKHAT, kernel);
    cv::meanStdDev(image, mean, stddev);
/*	std::cout << "Mean: " << mean[0] << "\n";
	std::cout << "Std: " << stddev[0] << "\n";
*/	thresh = mean[0] + params.threshold*stddev[0];
	
/*	std::ofstream logfile;
  	logfile.open ("proj.txt");
	for (int k = 0; k < length; k++)
	{
	logfile << proj.at<float>(k) << "\n";	
  	}
	logfile.close();
*/
	
    for(int k = 1; k < length-1; k++)
    {
	if (proj.at<float>(k) > thresh)
	{
	    if ((proj.at<float>(k) > proj.at<float>(k+1)) & 
		(proj.at<float>(k) > proj.at<float>(k-1)))
	    {
		if (locIdx < numLocs)
		{
		    locs[locIdx] = k;
		    locIdx++;
		}
		else
		{
		    min = 256*length;
		    minIdx = -1;
		    for(int m = 0; m < numLocs; m++)
		    {
			if (proj.at<float>(locs[m]) < min)
			{
			    minIdx = m;
			    min = proj.at<float>(locs[m]);
			}	
		    }
		    if (proj.at<float>(k) > min)
		    {
			locs[minIdx] = k;
		    }
		}
	    }
	}
    }

    return locIdx;	
}

int morphFindFiducials(cv::Mat image, morphParams rowParams, morphParams colParams, 
		       int fidWidth, int* locs, int numLocs)
{
    int nLocs;
    int temp = 0;
    cv::Size imSize;
    cv::Range sliceCols;
    cv::Mat slice;
	
    imSize = image.size();
    nLocs = morphPeakFind(image, rowParams, &locs[0], numLocs);
    for	(int k = 0; k < nLocs; k++)
    {
	sliceCols.end = (locs[k] + fidWidth/2 < imSize.width) ? (locs[k] + fidWidth/2) : (imSize.width-1);
	sliceCols.start = (locs[k] - fidWidth/2 > 0) ? (locs[k] - fidWidth/2) : 0;
	slice = image.colRange(sliceCols);

	morphPeakFind(slice, colParams, &temp, 1);
	locs[numLocs + k] = temp;
/*		
		std::cout << sliceCols.start << " to " << sliceCols.end
		<< ", " << locs[k] << " Fiducial in row: ";
		std::cout << locs[numLocs + k] << "\n";

		cv::namedWindow( "Display window", CV_WINDOW_AUTOSIZE ); 
		cv::imshow( "Display window", slice ); 
		cv::waitKey(0);
*/
    }
    return nLocs;
}

int matchFindFiducials(cv::InputArray _image, cv::InputArray _kernel, int threshold, cv::Point2f* locs, int numLocs)
{
    cv::Scalar mean, stddev;
    cv::Size imSize, kerSize;
    cv::Mat detect;
    int locIdx = 0;
    float thresh = 0;
    int minIdx;
//	double dMin, dMax;
    float min, curVal;

    cv::Mat image = _image.getMat();
    cv::Mat kernel = _kernel.getMat();

    imSize = image.size();
    kerSize = kernel.size();



    cv::filter2D(image, detect, CV_32FC1, kernel, cv::Point(-1,-1));
    cv::normalize(detect,detect,0,1,cv::NORM_MINMAX);
	
    cv::meanStdDev(detect, mean, stddev);
/*	std::cout << "Mean: " << mean[0] << "\n";
	std::cout << "Std: " << stddev[0] << "\n";
	
	cv::minMaxLoc(detect, &dMin, &dMax, NULL, NULL);
	std::cout << "Min: " << dMin << "\n";
	std::cout << "Max: " << dMax << "\n";
	cv::namedWindow( "Display window", CV_WINDOW_AUTOSIZE ); 
	cv::imshow( "Display window", detect ); 
	cv::waitKey(0);
*/
    thresh = mean[0] + threshold*stddev[0];
	
    for (int m = 1; m < imSize.height-1; m++)
    {
	for (int n = 1; n < imSize.width-1; n++)
	{	 
	    curVal = detect.at<float>(m,n);
	    if(curVal > thresh)
	    {
//				std::cout << m << " " << n << "\n";
		if((curVal > detect.at<float>(m,n+1)) &
		   (curVal > detect.at<float>(m,n-1)) &
		   (curVal > detect.at<float>(m+1,n)) &
		   (curVal > detect.at<float>(m-1,n)))
		{
		    if (locIdx < numLocs)
		    {
			locs[locIdx] = cv::Point2f(n,m);
			locIdx++;
		    }
		    else
		    {
			min = kerSize.width*kerSize.height*256;
			minIdx = -1;
			for(int k = 0; k < numLocs; k++)
			{
			    if (detect.at<float>(locs[k]) < min)
			    {
				minIdx = k;
				min = detect.at<float>(locs[k]);
			    }	
			}
			if (curVal > min)
			{
			    locs[minIdx] = cv::Point2f(n,m);
			}
		    }
		}
	    }
	}
    }
    return locIdx;
	
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
