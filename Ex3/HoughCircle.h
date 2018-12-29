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
		int a;	// Բ������Ϊ(a, b)
		int b;	// Բ������Ϊ(a, b)
		int votes;	// Ʊ��
		int radius;	// (�뾶, Ʊ��)
		Circle(int aInput, int bInput, int vInput) : a(aInput), b(bInput), votes(vInput), radius(0) {}
	};

	bool VERBOSE;
	int width;	// ԭͼ�Ŀ�
	int height;	// ԭͼ�ĸ�

	int windowSize;					// �������ڴ�С��������houghImg�����ֵ����
	double threshold;				// thresholdȡֵ��Χ(0, 1), ���Ʊ��threshold������Ʊ����Բ�Ľ���ȥ��
	std::vector<Circle> circles;

	CImg<unsigned char> originImg;	// ԭͼ
	CImg<unsigned char> imgEdge;	// ��Եͼ
	CImg<short int> delta_x;		// x�����ϵ��ݶ�
	CImg<short int> delta_y;		// y�����ϵ��ݶ�
	CImg<int> houghImg;				// Բ�ĵ�ͶƱͼ��
};

