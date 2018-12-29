#include "Canny.h"

int main() {
	printf("Processing...\n");
	// 默认参数
	Canny("test_Data/lena.bmp", "out_Data/lenaCanny.bmp", 1.5, 0.3, 0.7);
	Canny("test_Data/bigben.bmp", "out_Data/bigbenCanny.bmp", 0.8, 0.3, 0.7);
	Canny("test_Data/stpietro.bmp", "out_Data/stpietroCanny.bmp", 0.8, 0.3, 0.7);
	Canny("test_Data/twows.bmp", "out_Data/twowsCanny.bmp", 0.8, 0.3, 0.7);

	// 测试sigma对图像的影响
	double sigma = 0.5;
	Canny("test_Data/lena.bmp", "out_Data/sigma/lenaCanny_sigma0.5.bmp", sigma, 0.3, 0.7);
	Canny("test_Data/bigben.bmp", "out_Data/sigma/bigbenCanny_sigma0.5.bmp", sigma, 0.3, 0.7);
	Canny("test_Data/stpietro.bmp", "out_Data/sigma/stpietroCanny_sigma0.5.bmp", sigma, 0.3, 0.7);
	Canny("test_Data/twows.bmp", "out_Data/sigma/twowsCanny_sigma0.5.bmp", sigma, 0.3, 0.7);
	sigma = 2;
	Canny("test_Data/lena.bmp", "out_Data/sigma/lenaCanny_sigma2.bmp", sigma, 0.3, 0.7);
	Canny("test_Data/bigben.bmp", "out_Data/sigma/bigbenCanny_sigma2.bmp", sigma, 0.3, 0.7);
	Canny("test_Data/stpietro.bmp", "out_Data/sigma/stpietroCanny_sigma2.bmp", sigma, 0.3, 0.7);
	Canny("test_Data/twows.bmp", "out_Data/sigma/twowsCanny_sigma2.bmp", sigma, 0.3, 0.7);

	// 测试tlow对图像的影响
	double tlow = 0.2;
	Canny("test_Data/lena.bmp", "out_Data/tlow/lenaCanny_tlow0.2.bmp", 1.5, tlow, 0.7);
	Canny("test_Data/bigben.bmp", "out_Data/tlow/bigbenCanny_tlow0.2.bmp", 0.8, tlow, 0.7);
	Canny("test_Data/stpietro.bmp", "out_Data/tlow/stpietroCanny_tlow0.2.bmp", 0.8, tlow, 0.7);
	Canny("test_Data/twows.bmp", "out_Data/tlow/twowsCanny_tlow0.2.bmp", 0.8, tlow, 0.7);
	tlow = 0.4;
	Canny("test_Data/lena.bmp", "out_Data/tlow/lenaCanny_tlow0.4.bmp", 1.5, tlow, 0.7);
	Canny("test_Data/bigben.bmp", "out_Data/tlow/bigbenCanny_tlow0.4.bmp", 0.8, tlow, 0.7);
	Canny("test_Data/stpietro.bmp", "out_Data/tlow/stpietroCanny_tlow0.4.bmp", 0.8, tlow, 0.7);
	Canny("test_Data/twows.bmp", "out_Data/tlow/twowsCanny_tlow0.4.bmp", 0.8, tlow, 0.7);

	// 测试thigh对图像的影响
	double thigh = 0.6;
	Canny("test_Data/lena.bmp", "out_Data/thigh/lenaCanny_thigh0.6.bmp", 1.5, 0.3, thigh);
	Canny("test_Data/bigben.bmp", "out_Data/thigh/bigbenCanny_thigh0.6.bmp", 0.8, 0.3, thigh);
	Canny("test_Data/stpietro.bmp", "out_Data/thigh/stpietroCanny_thigh0.6.bmp", 0.8, 0.3, thigh);
	Canny("test_Data/twows.bmp", "out_Data/thigh/twowsCanny_thigh0.6.bmp", 0.8, 0.3, thigh);
	thigh = 0.8;
	Canny("test_Data/lena.bmp", "out_Data/thigh/lenaCanny_thigh0.8.bmp", 1.5, 0.3, thigh);
	Canny("test_Data/bigben.bmp", "out_Data/thigh/bigbenCanny_thigh0.8.bmp", 0.8, 0.3, thigh);
	Canny("test_Data/stpietro.bmp", "out_Data/thigh/stpietroCanny_thigh0.8.bmp", 0.8, 0.3, thigh);
	Canny("test_Data/twows.bmp", "out_Data/thigh/twowsCanny_thigh0.8.bmp", 0.8, 0.3, thigh);

	printf("Complete!\n");
	system("pause");
	return 0;
}