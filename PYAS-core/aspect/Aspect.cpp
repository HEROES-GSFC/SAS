#include "Aspect.hpp"

void Aspect::FindLimbCrossings()
{
}

void EvaluateChord(cv::Mat chord)
{
     m
    std::vector<bool> edge_dir;
    std::vector< std::vector<int> > idx;
    std::vector< std::vector<char> > edge;
    int cur;
    int last = -1;
    int K;
    int min, max;
    int x, xx, y, xy, Num;
    double D, slope, intercept, center;
	

    //for each pixel
    //if current value is above threshold, last is below
    //push positive index onto edge stack
    //else if current value is below threshold, previous is above
    //push negative index onto edge stack
    for (int k
	
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

            if (mode == 0) { //looking at rows
              limbs.add((thresh - intercept)/slope, loc);
            } else {
              limbs.add(loc, (thresh - intercept)/slope);
            }

	}
    }
    return center; 
}
