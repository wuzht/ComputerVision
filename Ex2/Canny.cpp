#include "Canny.h"

Canny::Canny(const char *inFileName, const char *outFileName, double sigma, double tlow, double thigh)
{
	this->VERBOSE = false;
	CImg<unsigned char> originImage(inFileName);
	this->outFileName = outFileName;
	this->rows = originImage.height();
	this->cols = originImage.width();
	this->height = this->rows;
	this->width = this->cols;
	this->sigma = sigma;
	this->tlow = tlow;
	this->thigh = thigh;
	this->image = CImg<unsigned char>(cols, rows, 1, 1, 0);
	this->smoothedim = CImg<short int>(cols, rows, 1, 1, 0);
	this->delta_x = CImg<short int>(cols, rows, 1, 1, 0);
	this->delta_y = CImg<short int>(cols, rows, 1, 1, 0);
	this->magnitude = CImg<short int>(cols, rows, 1, 1, 0);
	this->nms = CImg<unsigned char>(cols, rows, 1, 1, 0);
	this->edge = CImg<unsigned char>(cols, rows, 1, 1, 0);
	this->linkedEdge = CImg<unsigned char>(cols, rows, 1, 1, 0);
	this->result = CImg<unsigned char>(cols, rows, 1, 1, 0);

	// 转化为灰度图像
	cimg_forXY(image, x, y) {
		image(x, y) = originImage(x, y, 0) * 0.299 
					+ originImage(x, y, 1) * 0.587
					+ originImage(x, y, 2) * 0.114; 
	}
	
	canny();

	result.save(this->outFileName);
}


Canny::~Canny()
{
}

void Canny::canny()
{
	if (VERBOSE) printf("Smoothing the image using a gaussian kernel.\n");
	gaussian_smooth();

	if (VERBOSE) printf("Computing the X and Y first derivatives.\n");
	derivative_x_y();

	if (VERBOSE) printf("Computing the magnitude of the gradient.\n");
	magnitude_x_y();

	if (VERBOSE) printf("Doing the non-maximal suppression.\n");
	non_max_supp();

	if (VERBOSE) printf("Doing hysteresis thresholding.\n");
	apply_hysteresis();

	if (VERBOSE) printf("Doing linkAdjacentEdges and RemoveShortEdges.\n");
	linkAdjacentEdges_RemoveShortEdges();
}

void Canny::gaussian_smooth()
{
	int	windowsize;      /* Dimension of the gaussian kernel. */
	int	center;          /* Half of the windowsize. */
	double *tempim;      /* Buffer for separable filter gaussian smoothing. */
	double *kernel;		 /* A one dimensional gaussian kernel. */
	double dot;			 /* Dot product summing variable. */
	double sum;			 /* Sum of the kernel weights variable. */
	/****************************************************************************
	* Create a 1-dimensional gaussian smoothing kernel.
    ****************************************************************************/
	if (VERBOSE) printf("   Computing the gaussian smoothing kernel.\n");
	make_gaussian_kernel(&kernel, &windowsize);
	center = windowsize / 2;
	/****************************************************************************
	* Allocate a temporary buffer image and the smoothed image.
	****************************************************************************/
	if ((tempim = (double *)calloc(rows*cols, sizeof(double))) == NULL) {
		fprintf(stderr, "Error allocating the buffer image.\n");
		exit(1);
	}
	/****************************************************************************
	* Blur in the x - direction.
	****************************************************************************/
	if (VERBOSE) printf("   Bluring the image in the X-direction.\n");
	for (int r = 0; r<rows; r++) {
		for (int c = 0; c<cols; c++) {
			dot = 0.0;
			sum = 0.0;
			for (int cc = (-center); cc <= center; cc++) {
				if (((c + cc) >= 0) && ((c + cc) < cols)) {
					dot += (double)image(c + cc, r) * kernel[center + cc];
					sum += kernel[center + cc];
				}
			}
			tempim[r*cols + c] = dot / sum;
		}
	}
	/****************************************************************************
	* Blur in the y - direction.
	****************************************************************************/
	if (VERBOSE) printf("   Bluring the image in the Y-direction.\n");
	for (int c = 0; c<cols; c++) {
		for (int r = 0; r<rows; r++) {
			sum = 0.0;
			dot = 0.0;
			for (int rr = (-center); rr <= center; rr++) {
				if (((r + rr) >= 0) && ((r + rr) < rows)) {
					dot += tempim[(r + rr)*cols + c] * kernel[center + rr];
					sum += kernel[center + rr];
				}
			}
			smoothedim(c, r) = (short int)(dot*BOOSTBLURFACTOR / sum + 0.5);
		}
	}

	free(tempim);
	free(kernel);
}

void Canny::make_gaussian_kernel(double ** kernel, int * windowsize)
{
	int center;
	double sum = 0.0;

	*windowsize = 1 + 2 * ceil(2.5 * sigma);
	center = (*windowsize) / 2;

	if (VERBOSE) printf("      The kernel has %d elements.\n", *windowsize);
	
	if ((*kernel = (double *)calloc((*windowsize), sizeof(double))) == NULL) {
		fprintf(stderr, "Error callocing the gaussian kernel array.\n");
		exit(1);
	}
	for (int i = 0; i < (*windowsize); i++) {
		double x = (double)(i - center);
		//fx = pow(2.71828, -0.5*x*x / (sigma*sigma)) / (sigma * sqrt(6.2831853));
		double fx = exp(-0.5 * x * x / (sigma * sigma)) / (sigma * sqrt(2 * PI));
		(*kernel)[i] = fx;
		sum += fx;
	}
	for (int i = 0; i < (*windowsize); i++)
		(*kernel)[i] /= sum;
	if (VERBOSE) {
		printf("The filter coefficients are:\n");
		for (int i = 0; i<(*windowsize); i++)
			printf("kernel[%d] = %f\n", i, (*kernel)[i]);
	}
}

void Canny::derivative_x_y()
{
	/****************************************************************************
	* Compute the x-derivative. Adjust the derivative at the borders to avoid
	* losing pixels.
	****************************************************************************/
	if (VERBOSE) printf("   Computing the X-direction derivative.\n");
	for (int r = 0; r < rows; r++) {
		int pos = 0;
		delta_x(pos, r) = smoothedim(pos + 1, r) - smoothedim(pos, r);
		pos++;
		for (int c = 1; c < cols - 1; c++, pos++) {
			delta_x(pos, r) = smoothedim(pos + 1, r) - smoothedim(pos - 1, r);
		}
		delta_x(pos, r) = smoothedim(pos, r) - smoothedim(pos - 1, r);
	}

	/****************************************************************************
	* Compute the y-derivative. Adjust the derivative at the borders to avoid
	* losing pixels.
	****************************************************************************/
	if (VERBOSE) printf("   Computing the Y-direction derivative.\n");
	for (int c = 0; c < cols; c++) {
		int pos = 0;
		delta_y(c, pos) = smoothedim(c, pos + 1) - smoothedim(c, pos);
		pos++;
		for (int r = 1; r < rows - 1; r++, pos++) {
			delta_y(c, pos) = smoothedim(c, pos + 1) - smoothedim(c, pos - 1);
		}
		delta_y(c, pos) = smoothedim(c, pos) - smoothedim(c, pos - 1);
	}
}

void Canny::magnitude_x_y()
{
	int pos = 0;
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++, pos++) {
			int sq1 = (int)delta_x(c, r) * (int)delta_x(c, r);
			int sq2 = (int)delta_y(c, r) * (int)delta_y(c, r);
			magnitude(c, r) = (short)(0.5 + std::sqrt((double)sq1 + (double)sq2));
		}
	}
}

void Canny::non_max_supp()
{
	short z1, z2;
	short m00, gx, gy;
	double mag1, mag2, xperp, yperp;
	/****************************************************************************
	* Zero the edges of the result image. (done)
	****************************************************************************/

	/****************************************************************************
	* Suppress non-maximum points.
	****************************************************************************/
	for (int r = 1; r < rows - 2; r++) {
		for (int c = 1; c < cols - 2; c++) {
			m00 = magnitude(c, r);
			gx = delta_x(c, r);
			gy = delta_y(c, r);
			if (m00 == 0) {
				nms(c, r) = (unsigned char)NOEDGE;
			}
			
			xperp = -gx / ((double)m00);
			yperp = gy / ((double)m00);
			

			if (gx >= 0) {
				if (gy >= 0) {
					if (gx >= gy) {
						/* 111 */
						/* Left point */
						z1 = magnitude(c - 1, r);
						z2 = magnitude(c - 1, r - 1);
						mag1 = (m00 - z1) * xperp + (z2 - z1) * yperp;

						/* Right point */
						z1 = magnitude(c + 1, r);
						z2 = magnitude(c + 1, r + 1);
						mag2 = (m00 - z1) * xperp + (z2 - z1) * yperp;
					}
					else {
						/* 110 */
						/* Left point */
						z1 = magnitude(c, r - 1);
						z2 = magnitude(c - 1, r - 1);
						mag1 = (z1 - z2) * xperp + (z1 - m00) * yperp;

						/* Right point */
						z1 = magnitude(c, r + 1);
						z2 = magnitude(c + 1, r + 1);
						mag2 = (z1 - z2) * xperp + (z1 - m00) * yperp;
					}
				}
				else {	/* gy < 0 */
					if (gx >= -gy) {
						/* 101 */
						/* Left point */
						z1 = magnitude(c - 1, r);
						z2 = magnitude(c - 1, r + 1);
						mag1 = (m00 - z1) * xperp + (z1 - z2) * yperp;

						/* Right point */
						z1 = magnitude(c + 1, r);
						z2 = magnitude(c + 1, r - 1);
						mag2 = (m00 - z1) * xperp + (z1 - z2) * yperp;
					}
					else {
						/* 100 */
						/* Left point */
						z1 = magnitude(c, r + 1);
						z2 = magnitude(c - 1, r + 1);
						mag1 = (z1 - z2) * xperp + (m00 - z1) * yperp;

						/* Right point */
						z1 = magnitude(c, r - 1);
						z2 = magnitude(c + 1, r - 1);
						mag2 = (z1 - z2) * xperp + (m00 - z1) * yperp;
					}
				}
			}
			else {	/* gx < 0 */
				if (gy >= 0) {
					if (-gx >= gy) {
						/* 011 */
						/* Left point */
						z1 = magnitude(c + 1, r);
						z2 = magnitude(c + 1, r - 1);
						mag1 = (z1 - m00) * xperp + (z2 - z1) * yperp;

						/* Right point */
						z1 = magnitude(c - 1, r);
						z2 = magnitude(c - 1, r + 1);
						mag2 = (z1 - m00) * xperp + (z2 - z1) * yperp;
					}
					else {
						/* 010 */
						/* Left point */
						z1 = magnitude(c, r - 1);
						z2 = magnitude(c + 1, r - 1);
						mag1 = (z2 - z1) * xperp + (z1 - m00) * yperp;

						/* Right point */
						z1 = magnitude(c, r + 1);
						z2 = magnitude(c - 1, r + 1);
						mag2 = (z2 - z1) * xperp + (z1 - m00) * yperp;
					}
				}
				else {
					if (-gx > gy) {
						/* 001 */
						/* Left point */
						z1 = magnitude(c + 1, r);
						z2 = magnitude(c + 1, r + 1);
						mag1 = (z1 - m00) * xperp + (z1 - z2) * yperp;

						/* Right point */
						z1 = magnitude(c - 1, r);
						z2 = magnitude(c - 1, r - 1);
						mag2 = (z1 - m00) * xperp + (z1 - z2) * yperp;
					}
					else {
						/* 000 */
						/* Left point */
						z1 = magnitude(c, r + 1);
						z2 = magnitude(c + 1, r + 1);
						mag1 = (z2 - z1) * xperp + (m00 - z1) * yperp;

						/* Right point */
						z1 = magnitude(c, r - 1);
						z2 = magnitude(c - 1, r - 1);
						mag2 = (z2 - z1) * xperp + (m00 - z1) * yperp;
					}
				}
			}
			/* Now determine if the current point is a maximum point */
			if (mag1 > 0.0 || mag2 > 0.0) {
				nms(c, r) = (unsigned char)NOEDGE;
			}
			else {
				if (mag2 == 0.0)
					nms(c, r) = (unsigned char)NOEDGE;
				else
					nms(c, r) = (unsigned char)POSSIBLE_EDGE;
			}
		}
	}
	
}

void Canny::apply_hysteresis()
{
	int numedges = 0, lowcount = 0, highcount = 0, 
		lowthreshold = 0, highthreshold = 0,
		hist[32768] = {0};
	short int maximum_mag = 0, sumpix = 0;
	/****************************************************************************
	* Initialize the edge map to possible edges everywhere the non-maximal
	* suppression suggested there could be an edge except for the border. At
	* the border we say there can not be an edge because it makes the
	* follow_edges algorithm more efficient to not worry about tracking an
	* edge off the side of the image.
	****************************************************************************/
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			if (nms(c, r) == POSSIBLE_EDGE)
				edge(c, r) = POSSIBLE_EDGE;
			else
				edge(c, r) = NOEDGE;
		}
	}
	for (int r = 0; r < rows; r++) {
		edge(0, r) = NOEDGE;
		edge(cols - 1, r) = NOEDGE;
	}
	for (int c = 0; c < cols; c++) {
		edge(c, 0) = NOEDGE;
		edge(c, rows - 1) = NOEDGE;
	}
	/****************************************************************************
	* Compute the histogram of the magnitude image. Then use the histogram to
	* compute hysteresis thresholds.
	****************************************************************************/
	for (int r = 0; r < 32768; r++) 
		hist[r] = 0;
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			if (edge(c, r) == POSSIBLE_EDGE)
				hist[magnitude(c, r)]++;
		}
	}
	/****************************************************************************
	* Compute the number of pixels that passed the nonmaximal suppression.
	****************************************************************************/
	for (int r = 1; r < 32768; r++) {
		if (hist[r] != 0) maximum_mag = r;
		numedges += hist[r];
	}
	
	highcount = (int)(numedges * thigh + 0.5);
	/****************************************************************************
	* Compute the high threshold value as the (100 * thigh) percentage point
	* in the magnitude of the gradient histogram of all the pixels that passes
	* non-maximal suppression. Then calculate the low threshold as a fraction
	* of the computed high threshold value. John Canny said in his paper
	* "A Computational Approach to Edge Detection" that "The ratio of the
	* high to low threshold in the implementation is in the range two or three
	* to one." That means that in terms of this implementation, we should
	* choose tlow ~= 0.5 or 0.33333.
	****************************************************************************/
	int r = 1;
	numedges = hist[1];
	while ((r < (maximum_mag - 1)) && (numedges < highcount)) {
		r++;
		numedges += hist[r];
	}
	highthreshold = r;
	lowthreshold = (int)(highthreshold * tlow + 0.5);
	if (VERBOSE) {
		printf("The input low and high fractions of %f and %f computed to\n",
			tlow, thigh);
		printf("magnitude of the gradient threshold values of: %d %d\n",
			lowthreshold, highthreshold);
	}
	/****************************************************************************
	* This loop looks for pixels above the highthreshold to locate edges and
	* then calls follow_edges to continue the edge.
	****************************************************************************/
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			if ((edge(c, r) == POSSIBLE_EDGE) && (magnitude(c, r) >= highthreshold)) {
				edge(c, r) = EDGE;
				follow_edges(c, r, lowthreshold);
			}
		}
	}
	/****************************************************************************
	* Set all the remaining possible edges to non-edges.
	****************************************************************************/
	for (int r = 0; r< rows; r++) {
		for (int c = 0; c < cols; c++) 
			if (edge(c, r) != EDGE) 
				edge(c, r) = NOEDGE;
	}
}

void Canny::follow_edges(int curCol, int curRow, int lowthreshold)
{
	int x[8] = { 1,1,0,-1,-1,-1,0,1 },
		y[8] = { 0,1,1,1,0,-1,-1,-1 };
	for (int i = 0; i < 8; i++) {
		int tempCol = curCol + x[i];
		int tempRow = curRow - y[i];

		if (edge(tempCol, tempRow) == POSSIBLE_EDGE && magnitude(tempCol, tempRow) > lowthreshold) {
			edge(tempCol, tempRow) = (unsigned char)EDGE;
			follow_edges(tempCol, tempRow, lowthreshold);
		}
	}
}

void Canny::linkAdjacentEdges_RemoveShortEdges()
{
	/* Link Adjacent Edges */
	for (int x = 1; x < width - 2; x++) {
		for (int y = 1; y < height - 2; y++) {
			if (edge(x, y) == EDGE) {
				linkedEdge(x, y) = EDGE;
				double tangent = 0;
				if (delta_x(x, y) == 0)
					tangent = 99;
				else
					tangent = (double)delta_y(x, y) / (double)delta_x(x, y);
				
				int angle = 0;
				if (tangent > -0.4142 && tangent <= 0.4142)
					angle = 0;
				else if (tangent > 0.4142 && tangent < 2.4142)
					angle = 1;
				else if (abs(tangent) >= 2.4142)
					angle = 2;
				else
					angle = 3;

				int tempX[8] = { 0, 0, 1, -1, 1, -1, 1, -1 };
				int tempY[8] = { -1, 1, 1, -1, 0, 0, -1, 1 };
				int x1 = x + tempX[angle * 2], y1 = y + tempY[angle * 2];
				int x2 = x + tempX[angle * 2 + 1], y2 = y + tempY[angle * 2 + 1];
				if (linkedEdge(x1, y1) == PREDICTED_EDGE)
					linkedEdge(x1, y1) = EDGE;
				else if (nms(x1, y1) == POSSIBLE_EDGE && linkedEdge(x1, y1) != EDGE)
					linkedEdge(x1, y1) = PREDICTED_EDGE;

				if (linkedEdge(x2, y2) == PREDICTED_EDGE)
					linkedEdge(x2, y2) = EDGE;
				else if (nms(x2, y2) == POSSIBLE_EDGE && linkedEdge(x2, y2) != EDGE)
					linkedEdge(x2, y2) = PREDICTED_EDGE;
			}
		}
	}
	cimg_forXY(linkedEdge, x, y) {
		if (linkedEdge(x, y) != EDGE)
			linkedEdge(x, y) = (unsigned char)NOEDGE;
	}

	/* Remove Short Edges */
	result = CImg<unsigned char>(linkedEdge);
	CImg<unsigned char> marked(width, height, 1, 1, 0);
	for (int x = 1; x < width - 2; x++) {
		for (int y = 1; y < height - 2; y++) {
			if (result(x, y) == EDGE && marked(x, y) != MARK_IGNORE) {
				int edgeLength = 1;
				std::vector<std::pair<int, int> > pixels;
				pixels.push_back(std::pair<int, int>(x, y));
				recursive_removeShortEdges(marked, x, y, edgeLength, pixels);

				if (edgeLength >= 20) {
					for (auto it : pixels) {
						marked(it.first, it.second) = MARK_IGNORE;
					}
				}
				else {
					for (auto it : pixels) {
						result(it.first, it.second) = NOEDGE;
					}
				}
			}
		}
	}
}

void Canny::recursive_removeShortEdges(CImg<unsigned char> &marked, int curX, int curY, int &edgeLength, std::vector<std::pair<int, int> > &pixels)
{
	if (edgeLength >= 20)
		return;
	int x[8] = { 1,1,0,-1,-1,-1,0,1 },
		y[8] = { 0,1,1,1,0,-1,-1,-1 };
	for (int i = 0; i < 8; i++) {
		int tempX = curX + x[i];
		int tempY = curY - y[i];
		// 防止下标越界
		if (!(tempX >= 0 && tempX < width && tempY >= 0 && tempY < height))
			continue;
		if (marked(tempX, tempY) == MARK_IGNORE) {
			edgeLength = 100;
			return;
		}
		if (result(tempX, tempY) == EDGE && marked(tempX, tempY) != MARK_0) {
			edgeLength++;
			marked(tempX, tempY) = MARK_0;
			pixels.push_back(std::pair<int, int>(tempX, tempY));
			recursive_removeShortEdges(marked, tempX, tempY, edgeLength, pixels);
		}
	}
}
