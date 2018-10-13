#pragma once
#include "CImg.h"

class ImageProcess
{
public:
	ImageProcess(const char* filePath);
	~ImageProcess();
	void Display() const;

	// set the white region to red, the black region to green
	void ChangeColor();

	// draw by cimg_forXY
	void DrawBlueCircle();
	void DrawYellowCircle();
	void DrawBlueLine();

	// draw by the CImg functions
	void DrawBlueCircleCImg();
	void DrawYellowCircleCImg();
	void DrawBlueLineCImg();

	void Save(const char* filePath) const;
private:
	cimg_library::CImg<unsigned char> srcImg;

	// return the distance of two points
	double Distance(double x0, double y0, double x1, double y1);
};