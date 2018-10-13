#include "ImageProcess.h"
#include <iostream>

int main() {
	ImageProcess imageProcess("1.bmp");
	int option = 0;
	std::cout << "0 - draw by cimg_forXY\n1 - draw by the CImg functions\noption: ";
	std::cin >> option;
	imageProcess.Display();					// Step 1
	imageProcess.ChangeColor();				// Step 2
	if (option == 0) {
		imageProcess.DrawBlueCircle();		// Step 3
		imageProcess.DrawYellowCircle();	// Step 4
		imageProcess.DrawBlueLine();		// Step 5
	}
	else {
		imageProcess.DrawBlueCircleCImg();	// Step 3
		imageProcess.DrawYellowCircleCImg();// Step 4
		imageProcess.DrawBlueLineCImg();	// Step 5
	}
	imageProcess.Display();
	imageProcess.Save("2.bmp");				// Step 6
	return 0;
}