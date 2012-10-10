#include <iostream>
#include <vector>
#include <math.h>
#include <chord.hpp>

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

