#include "HoughTransform.h"

HoughTransform::HoughTransform(const char *inFileName, const char *outFileName, double sigma, double tlow, double thigh)
{
	printf("#Processing %s\n", inFileName);
	this->VERBOSE = false;
	if (VERBOSE) printf("#Line performing canny.\n");
	Canny canny(inFileName, outFileName, sigma, tlow, thigh);
	this->imgEdge = canny.getEdgeImg();

	this->windowSize = 60;
	this->pointNum = 4;
	this->width = imgEdge.width();
	this->height = imgEdge.height();
	this->rhoRange = std::sqrt((width / 2) * (width / 2) + (height / 2) * (height / 2));
	this->thetaRange = 360;
	this->houghImg = CImg<int>(rhoRange, thetaRange, 1, 1, 0);
	this->outImage = CImg<unsigned char>(width, height, 1, 3, NOEDGE);

	cimg_forXY(imgEdge, x, y) {
		if (imgEdge(x, y) == EDGE) {
			outImage(x, y, 0) = outImage(x, y, 1) = outImage(x, y, 2) = EDGE;
		}
	}

	for (int theta = 0; theta < thetaRange; theta++) {
		mySin.push_back(std::sin(theta * PI / 180));
		myCos.push_back(std::cos(theta * PI / 180));
	}

	if (VERBOSE) printf("#Line vote.\n");
	lineVote();
	if (VERBOSE) printf("#Line detect.\n");
	lineDetect();
	if (VERBOSE) printf("#Line draw.\n");
	lineDraw();
	if (VERBOSE) printf("#Line draw point.\n");
	lineDrawPoint();
	
	outImage.save(outFileName);
	printf("#Finish %s\n", inFileName);
}

HoughTransform::~HoughTransform()
{
}

void HoughTransform::lineVote()
{
	cimg_forXY(imgEdge, x, y) {
		if (imgEdge(x, y) == EDGE) {
			int x0 = x - width / 2, y0 = height / 2 - y;
			for (int theta = 0; theta < thetaRange; theta++) {
				int rho = x0 * myCos[theta] + y0 * mySin[theta];
				if (rho >= 0 && rho < rhoRange) {
					houghImg(rho, theta)++;
				}
			}
		}
	}
}

void HoughTransform::lineDetect()
{
	for (int i = 0; i < houghImg.height(); i += windowSize / 2) {
		for (int j = 0; j < houghImg.width(); j += windowSize / 2) {
			// 寻找局部最大值
			int localMax = 0;
			int boxWidth = (j + windowSize > houghImg.width()) ? houghImg.width() : j + windowSize;
			int boxHeight = (i + windowSize > houghImg.height()) ? houghImg.height() : i + windowSize;
			for (int x = j; x < boxWidth; x++) {
				for (int y = i; y < boxHeight; y++) {
					if (houghImg(x, y) > localMax)
						localMax = houghImg(x, y);
				}
			}
			// 去掉不是局部最大值的点
			for (int x = j; x < boxWidth; x++) {
				for (int y = i; y < boxHeight; y++) {
					if (houghImg(x, y) < localMax)
						houghImg(x, y) = 0;
				}
			}
		}
	}
	// 将检测到的参数对加入lines
	cimg_forXY(houghImg, x, y) {
		if (houghImg(x, y) != 0) {
			lines.push_back(Line(x, y, houghImg(x, y)));
		}
	}
	// 排序
	std::sort(lines.begin(), lines.end(), [](const Line &lVal, const Line &rVal) {
		return lVal.votes > rVal.votes;
	});
	if (VERBOSE) printf("	lines size: %d\n", lines.size());
}

void HoughTransform::lineDraw()
{
	int count = 0, lineIndex = pointNum;
	for (int i = 0; i < pointNum && lineIndex < 10;) {
		// 要求前4条线两两近似垂直或平行，否则从后面替换
		int perpOrParaNum = 0;
		for (int j = 0; j < pointNum; j++) {
			if (i == j) continue;
			if (fabs(cos((lines[i].theta - lines[j].theta) * PI / 180)) < 0.2 ||
				fabs(sin((lines[i].theta - lines[j].theta) * PI / 180)) < 0.2) {
				perpOrParaNum++;
				break;
			}		
		}
		if (perpOrParaNum == 0) {
			lines[i] = lines[lineIndex];
			lineIndex++;
		}
		else
			i++;
	}
	if (VERBOSE) printf("	lineIndex: %d\n", lineIndex);

	// 画线（蓝色）
	for (int i = 0; i < pointNum; i++) {
		int rho = lines[i].rho;
		int theta = lines[i].theta;
		if (VERBOSE) printf("	Vote: %d\n", lines[i].votes);
		if (VERBOSE) printf("	Args: %d %d\n", rho, theta);
		printf("[Line parametric equation %d] %d = (%lf)x + (%lf)y\n", i, rho, myCos[theta], mySin[theta]);

		cimg_forXY(outImage, x, y) {
			int x0 = x - width / 2, y0 = height / 2 - y;
			if (rho == (int)(x0 * myCos[theta] + y0 * mySin[theta])) {
				if (outImage(x, y, 2) == 255 && outImage(x, y, 0) == 0 && outImage(x, y, 1) == 0) {
					points.push_back(std::pair<int, int>(x, y));
					if (VERBOSE) printf("	Point: %d %d\n", x, y);
				}	
				outImage(x, y, 0) = outImage(x, y, 1) = 0;
				outImage(x, y, 2) = 255;
			}
		}
	}
}

void HoughTransform::lineDrawPoint()
{
	// 画半径为20的角点（红色）
	unsigned char red[3] = { 255, 0, 0 };
	for (auto it : points) {
		outImage.draw_circle(it.first, it.second, 20, red);
	}
	// 画相关边缘点（红色）
	cimg_forXY(outImage, x, y) {
		if (imgEdge(x, y) == EDGE && outImage(x, y, 0) == 0 && outImage(x, y, 1) == 0 && outImage(x, y, 2) == 255) {
			outImage(x, y, 0) = 255;
			outImage(x, y, 2) = 0;
		}
	}
}