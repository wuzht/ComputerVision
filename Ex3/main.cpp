#include "HoughTransform.h"
#include "HoughCircle.h"
#include "Canny.h"

int main() {
	printf("Processing...\n");
	// A4Ö½¼ì²â
	HoughTransform("Dataset1/1.bmp", "Dataset1_out/1Hough.bmp", 5, 0.05, 0.995);
	HoughTransform("Dataset1/2.bmp", "Dataset1_out/2Hough.bmp", 5, 0.2, 0.995);
	HoughTransform("Dataset1/3.bmp", "Dataset1_out/3Hough.bmp", 5, 0.2, 0.995);
	HoughTransform("Dataset1/4.bmp", "Dataset1_out/4Hough.bmp", 5, 0.2, 0.995);
	HoughTransform("Dataset1/5.bmp", "Dataset1_out/5Hough.bmp", 5, 0.2, 0.995);
	HoughTransform("Dataset1/6.bmp", "Dataset1_out/6Hough.bmp", 5, 0.2, 0.995);

	// Ó²±Ò¼ì²â
	HoughCircle("Dataset2/1.bmp", "Dataset2_out/1out.bmp", 2, 0.1, 0.95, 0.5);
	HoughCircle("Dataset2/2.bmp", "Dataset2_out/2out.bmp", 2.3, 0.2, 0.97, 0.4, 350);
	HoughCircle("Dataset2/3.bmp", "Dataset2_out/3out.bmp", 2, 0.01, 0.995, 0.5);
	HoughCircle("Dataset2/4.bmp", "Dataset2_out/4out.bmp", 2, 0.2, 0.975, 0.5, 350);
	HoughCircle("Dataset2/5.bmp", "Dataset2_out/5out.bmp", 3, 0.05, 0.975, 0.5, 1000);
	HoughCircle("Dataset2/6.bmp", "Dataset2_out/6out.bmp", 1.5, 0.05, 0.975, 0.5);
	printf("Complete!\n");
	system("pause");
	return 0;
}