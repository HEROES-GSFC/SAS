#include <opencv.hpp>
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
	std::cout << "Projection Size: " << (proj.size()).height << " by " << (proj.size()).width << "\n";
	cv::morphologyEx(proj, proj, cv::MORPH_BLACKHAT, kernel);
	cv::meanStdDev(image, mean, stddev);
/*	std::cout << "Mean: " << mean[0] << "\n";
	std::cout << "Std: " << stddev[0] << "\n";
*/	thresh = mean[0] + params.threshold*stddev[0];
	
	std::ofstream logfile;
  	logfile.open ("proj.txt");
	for (int k = 0; k < length; k++)
	{
  		logfile << proj.at<float>(k) << "\n";	
  	}
	logfile.close();
	
	
	min = 256*length;
	minIdx = 0;
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
						min = proj.at<float>(k);
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
		
		std::cout << sliceCols.start << " to " << sliceCols.end
		          << ", " << locs[k] << " Fiducial in row: ";
    	std::cout << locs[numLocs + k] << "\n";

	    cv::namedWindow( "Display window", CV_WINDOW_AUTOSIZE ); 
	    cv::imshow( "Display window", slice ); 
        cv::waitKey(0);

	}
	return nLocs;
}
