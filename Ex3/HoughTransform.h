#pragma once
#include "CImg.h"
#include "Canny.h"
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <math.h>
#define PI 3.1415926535

class HoughTransform
{
public:
	HoughTransform(const char *inFileName, const char *outFileName, double sigma, double tlow, double thigh);
	~HoughTransform();

	void lineVote();
	void lineDetect();
	void lineDraw();		// 画边缘直线（蓝色）
	void lineDrawPoint();	// 画半径为20的角点，并画相关边缘点（红色）

private:
	struct Line
	{
		int rho;
		int theta;
		int votes;
		Line(int rInput, int tInput, int vInput) : rho(rInput), theta(tInput), votes(vInput) {}
	};

	bool VERBOSE;
	std::vector<double> mySin;
	std::vector<double> myCos;
	int windowSize;	// 滑动窗口大小，用来对houghImg非最大值抑制
	int pointNum;
	int width;		// 原图的宽
	int height;		// 原图的高
	int rhoRange;
	int thetaRange;

	std::vector<Line> lines;					// 检测到的直线参数对(rho, theta)
	std::vector<std::pair<int, int>> points;	// 检测到的角点坐标(x, y)

	CImg<unsigned char> imgEdge;	// canny后的边缘图
	CImg<int> houghImg;				// 霍夫图
	CImg<unsigned char> outImage;	// 输出图像

};

