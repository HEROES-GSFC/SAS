#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>
#include <fiducials.hpp>
#include <iostream>
#include <fstream>

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

int matchFindFiducials(cv::InputArray _image, cv::InputArray _kernel, int threshold, cv::Point* locs, int numLocs)
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
						locs[locIdx] = cv::Point(n,m);
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
							locs[minIdx] = cv::Point(n,m);
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
