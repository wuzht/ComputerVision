#include "HoughCircle.h"


HoughCircle::HoughCircle(const char * inFileName, const char * outFileName, double sigma, double tlow, double thigh, double threshold, int windowSize)
{
	this->VERBOSE = false;
	if (VERBOSE) printf("#Circle performing canny.\n");
	Canny canny(inFileName, outFileName, sigma, tlow, thigh);
	this->originImg = canny.getOriginImg();
	this->imgEdge = canny.getEdgeImg();
	this->delta_x = canny.getDeltaX();
	this->delta_y = canny.getDeltaY();
	this->width = imgEdge.width();
	this->height = imgEdge.height();
	this->threshold = threshold;
	this->windowSize = windowSize;
	this->houghImg = CImg<int>(width, height, 1, 1, 0);

	if (VERBOSE) printf("#Circle center vote.\n");
	circleCenterVote();
	if (VERBOSE) printf("#Circle center detect.\n");
	circleCenterDetect();
	if (VERBOSE) printf("#Circle radius detect.\n");
	circleRadiusDetect();
	if (VERBOSE) printf("#Circle draw.\n");
	circleDraw();

	printf(inFileName);
	printf(" coins number: %d\n", circles.size());
	//originImg.display("oriImg");	
	originImg.save(outFileName);
}

HoughCircle::~HoughCircle()
{
}

void HoughCircle::circleCenterVote()
{
	cimg_forXY(imgEdge, x, y) {
		if (imgEdge(x, y) == EDGE) {
			// ����õ���ݶȷ���
			double tangent = 0.0;
			if (delta_x(x, y) == 0) tangent = 99;
			else tangent = (double)delta_y(x, y) / (double)delta_x(x, y);
			const double gradientDirectionTheta = atan(tangent);
			const double cosTheta = cos(gradientDirectionTheta);
			const double sinTheta = sin(gradientDirectionTheta);
			// Ӳ�����뾶
			const int maxRadius = width < height ? width / 2 : height / 2;
			// �ڻ���ͼƬ�У����յ�ǰ���ص���ݶȷ��򣬻�һ��2*maxRadius��ֱ��
			for (int itRadius = -maxRadius; itRadius <= maxRadius; itRadius++) {
				int x0 = (int)round(x + cosTheta * itRadius);
				int y0 = (int)round(y + sinTheta * itRadius);
				if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
					houghImg(x0, y0)++;
			}
		}
	}
}

// ����ͼƬ�����ֵ����
void HoughCircle::circleCenterDetect()
{
	for (int i = 0; i < houghImg.height(); i += windowSize / 2) {
		for (int j = 0; j < houghImg.width(); j += windowSize / 2) {
			// Ѱ�Ҿֲ����ֵ
			int localMax = 0;
			std::pair<int, int> localMaxCoord = std::make_pair(0, 0);
			int boxWidth = (j + windowSize > houghImg.width()) ? houghImg.width() : j + windowSize;
			int boxHeight = (i + windowSize > houghImg.height()) ? houghImg.height() : i + windowSize;
			for (int x = j; x < boxWidth; x++) {
				for (int y = i; y < boxHeight; y++) {
					if (houghImg(x, y) > localMax) {
						localMax = houghImg(x, y);
						localMaxCoord = std::make_pair(x, y);
					}
				}
			}
			// ȥ�����Ǿֲ����ֵ�ĵ�
			for (int x = j; x < boxWidth; x++) {
				for (int y = i; y < boxHeight; y++) {
					if (x != localMaxCoord.first || y != localMaxCoord.second)
						houghImg(x, y) = 0;
				}
			}
		}
	}
	
	// ����⵽�Ĳ����Լ���circles
	cimg_forXY(houghImg, x, y) {
		if (houghImg(x, y) != 0 && houghImg(x, y) > houghImg.max() * threshold) {
			circles.push_back(Circle(x, y, houghImg(x, y)));
		}
	}
	// ����
	std::sort(circles.begin(), circles.end(), [](const Circle &lVal, const Circle &rVal) {
		return lVal.votes > rVal.votes;
	});
	if (VERBOSE) printf("	circles size: %d\n", circles.size());
}

void HoughCircle::circleRadiusDetect()
{
	// Ӳ����С���뾶
	const int minR = 30, maxR = width < height ? width / 2 : height / 2;
	int *radiusVotes = new int[maxR - minR];
	for (auto &it : circles) {
		for (int i = 0; i < maxR - minR; i++)
			radiusVotes[i] = 0;
		// �԰뾶ͶƱ
		for (int x = it.a - maxR > 0 ? it.a - maxR : 0; x < it.a + maxR && x < width; x++) {
			for (int y = it.b - maxR > 0 ? it.b - maxR : 0; y < it.b + maxR && y < height; y++) {
				if (imgEdge(x, y) == EDGE) {
					int r = (int)round(sqrt((it.a - x) * (it.a - x) + (it.b - y) * (it.b - y)));
					if (r >= minR && r < maxR) {
						radiusVotes[r - minR]++;
					}
				}
			}
		}
		
		// �ҳ������ͶƱ���İ뾶
		int maxVote = 0, detectedRadius = 0;
		for (int i = 0; i < maxR - minR; i++) {
			if (radiusVotes[i] > maxVote) {
				maxVote = radiusVotes[i];
				detectedRadius = i + minR;
			}
		}
		it.radius = detectedRadius;
	}
	delete[]radiusVotes;
}

void HoughCircle::circleDraw()
{
	unsigned char blue[3] = { 0, 0, 255 };
	unsigned char green[3] = { 0, 255, 0 };
	for (auto it : circles) {
		// ��Բ�ģ���ɫ��
		originImg.draw_circle(it.a, it.b, 5, green);
		// ��Բ����ɫ��
		//for (int j = -2; j <= 2; j++)
		originImg.draw_circle(it.a, it.b, it.radius, blue, 1, 3);
		// ����Բ��ص����أ���ɫ��
		cimg_forXY(originImg, x, y) {
			if (imgEdge(x, y) == EDGE && originImg(x, y, 0) == 0 && originImg(x, y, 1) == 0 && originImg(x, y, 2) == 255) {
				originImg(x, y, 0) = 255;
				originImg(x, y, 2) = 0;
			}
		}
		if (VERBOSE) printf("	votes: %d\n", it.votes);
		if (VERBOSE) printf("	Detected radius: %d\n", it.radius);
	}
}