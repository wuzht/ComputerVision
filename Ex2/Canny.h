#pragma once
#include "CImg.h"
#include <math.h>
#include <vector>
using namespace cimg_library;
#define PI 3.1415926535
#define BOOSTBLURFACTOR 90.0
#define NOEDGE 0
#define POSSIBLE_EDGE 128
#define EDGE 255
#define PREDICTED_EDGE 192

#define MARK_0 100
#define MARK_IGNORE 102

class Canny
{
public:
	Canny(const char *inFileName, const char *outFileName, double sigma, double tlow, double thigh);
	~Canny();

	// To perform canny edge detection.
	void canny();

	// Blur an image with a gaussian filter.
	void gaussian_smooth();

	// Create a one dimensional gaussian kernel.
	void make_gaussian_kernel(double **kernel, int *windowsize);
	/**
	*  Compute the first derivative of the image in both the x any y
	*  directions. The differential filters that are used are:
	*                                          -1
	*         dx =  -1 0 +1     and       dy =  0
	*                                          +1
	*/
	void derivative_x_y();
	/**
	*  Compute the magnitude of the gradient. This is the square root of
	*  the sum of the squared derivative values.
	*/
	void magnitude_x_y();
	/**
	*  This routine applies non-maximal suppression to the magnitude of
	*  the gradient image.
	*/
	void non_max_supp();
	/**
	*  This routine finds edges that are above some high threshhold or
	*  are connected to a high pixel by a path of pixels greater than a low
	*/
	void apply_hysteresis();
	/**
	*  This procedure edges is a recursive routine that traces edgs along
	*  all paths whose magnitude values remain above some specifyable lower
	*/
	void follow_edges(int curCol, int curRow, int lowthreshold);
	
	// link the adjacent edges first, and then remove the short edges whose length is less than 20
	void linkAdjacentEdges_RemoveShortEdges();
private:
	bool VERBOSE;
	const char *outFileName;		// Name of the output edge image.
	int rows;
	int cols;
	int width;
	int height;
	/*  sigma = The standard deviation of the gaussian smoothing filter. */
	double sigma;
	/*  tlow  = Specifies the low value to use in hysteresis. This is a
	*			fraction (0-1) of the computed high threshold edge strength value.*/
	double tlow;
	/*  thigh = Specifies the high value to use in hysteresis.This fraction(0 - 1)
	*           specifies the percentage point in a histogram of the gradient of
	*           the magnitude.Magnitude values of zero are not counted in the histogram.*/
	double thigh;
	CImg<unsigned char> image;		// The input image.
	CImg<short int> smoothedim;		// The image after gaussian smoothing.
	CImg<short int> delta_x;		// The first derivative image, x - direction.
	CImg<short int> delta_y;		// The first derivative image, y - direction.
	CImg<short int> magnitude;		// The magnitude of the gadient image.
	CImg<unsigned char> nms;		// Points that are local maximal magnitude. An image.
	CImg<unsigned char> edge;		// The edge image
	CImg<unsigned char> linkedEdge;	// The linked image
	CImg<unsigned char> result;		// The output reult image.

	void recursive_removeShortEdges(CImg<unsigned char> &marked, int curX, int curY, int &edgeLength, std::vector<std::pair<int, int> > &pixels);
};