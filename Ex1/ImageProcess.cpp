#include "ImageProcess.h"

ImageProcess::ImageProcess(const char* filePath)
{
	srcImg.load_bmp(filePath);
}

ImageProcess::~ImageProcess()
{
}

void ImageProcess::Display() const
{
	srcImg.display();
}

// set the white region to red, the black region to green
void ImageProcess::ChangeColor()
{
	cimg_forXY(srcImg, x, y) {
		if (srcImg(x, y, 0) != 0 && srcImg(x, y, 0) != 102 && srcImg(x, y, 1) != 102 && srcImg(x, y, 2) != 102)
		{
			srcImg(x, y, 0) = 255;
			srcImg(x, y, 1) = srcImg(x, y, 2) = 0;
		}
		if (srcImg(x, y, 0) == 0 && srcImg(x, y, 1) == 0 && srcImg(x, y, 2) == 0)
		{
			srcImg(x, y, 1) = 255;
			srcImg(x, y, 0) = srcImg(x, y, 2) = 0;
		}
	}
}

// draw by cimg_forXY
void ImageProcess::DrawBlueCircle()
{
	cimg_forXY(srcImg, x, y) {
		if (Distance(x, y, 50, 50) <= 30) {
			srcImg(x, y, 2) = 255;
			srcImg(x, y, 0) = srcImg(x, y, 1) = 0;
		}
	}
}

void ImageProcess::DrawYellowCircle()
{
	cimg_forXY(srcImg, x, y) {
		if (Distance(x, y, 50, 50) <= 3) {
			srcImg(x, y, 2) = 0;
			srcImg(x, y, 0) = srcImg(x, y, 1) = 255;
		}
	}
}

void ImageProcess::DrawBlueLine()
{
	const double PI = 3.1415926535;
	cimg_forXY(srcImg, x, y) {
		if (x <= 100 * std::cos(35 * PI / 180)) {
			double temp = y - x * std::tan(35 * PI / 180);
			if (temp > -0.5 && temp < 0.5) {
				srcImg(x, y, 2) = 255;
				srcImg(x, y, 0) = srcImg(x, y, 1) = 0;
			}
		}
	}
}

// draw by the CImg functions
void ImageProcess::DrawBlueCircleCImg()
{
	const unsigned char blue[3] = { 0, 0, 255 };
	srcImg.draw_circle(50, 50, 30, blue);
}

void ImageProcess::DrawYellowCircleCImg()
{
	const unsigned char yellow[3] = { 255, 255, 0 };
	srcImg.draw_circle(50, 50, 3, yellow);
}

void ImageProcess::DrawBlueLineCImg()
{
	const double PI = 3.1415926535;
	const unsigned char blue[3] = { 0, 0, 255 };
	srcImg.draw_line(0, 0, 100 * std::cos(35 * PI / 180), 100 * std::sin(35 * PI / 180), blue);
}

void ImageProcess::Save(const char* filePath) const
{
	srcImg.save(filePath);
}

// return the distance of two points
double ImageProcess::Distance(double x0, double y0, double x1, double y1)
{
	return std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
}
