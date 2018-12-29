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
	void lineDraw();		// ����Եֱ�ߣ���ɫ��
	void lineDrawPoint();	// ���뾶Ϊ20�Ľǵ㣬������ر�Ե�㣨��ɫ��

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
	int windowSize;	// �������ڴ�С��������houghImg�����ֵ����
	int pointNum;
	int width;		// ԭͼ�Ŀ�
	int height;		// ԭͼ�ĸ�
	int rhoRange;
	int thetaRange;

	std::vector<Line> lines;					// ��⵽��ֱ�߲�����(rho, theta)
	std::vector<std::pair<int, int>> points;	// ��⵽�Ľǵ�����(x, y)

	CImg<unsigned char> imgEdge;	// canny��ı�Եͼ
	CImg<int> houghImg;				// ����ͼ
	CImg<unsigned char> outImage;	// ���ͼ��

};

