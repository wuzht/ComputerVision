#pragma once
#include "CImg.h"
#include "Canny.h"
#include <vector>
#include <algorithm>
#define PI 3.1415926535

class HoughCircle
{
public:
	HoughCircle(const char *inFileName, const char *outFileName, double sigma, double tlow, double thigh, double threshold = 0.5, int windowSize = 100);
	~HoughCircle();

	void circleCenterVote();
	void circleCenterDetect();
	void circleRadiusDetect();
	void circleDraw();
private:
	struct Circle
	{
		int a;	// 圆心坐标为(a, b)
		int b;	// 圆心坐标为(a, b)
		int votes;	// 票数
		int radius;	// (半径, 票数)
		Circle(int aInput, int bInput, int vInput) : a(aInput), b(bInput), votes(vInput), radius(0) {}
	};

	bool VERBOSE;
	int width;	// 原图的宽
	int height;	// 原图的高

	int windowSize;					// 滑动窗口大小，用来对houghImg非最大值抑制
	double threshold;				// threshold取值范围(0, 1), 最大票数threshold倍以下票数的圆心将被去掉
	std::vector<Circle> circles;

	CImg<unsigned char> originImg;	// 原图
	CImg<unsigned char> imgEdge;	// 边缘图
	CImg<short int> delta_x;		// x方向上的梯度
	CImg<short int> delta_y;		// y方向上的梯度
	CImg<int> houghImg;				// 圆心的投票图像
};

