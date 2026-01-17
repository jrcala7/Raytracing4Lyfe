// RaytracingForLife.cpp : Defines the entry point for the application.
//

#include "RaytracingForLife.h"

using namespace std;

void PrintTestImage() {
	int width = 256;
	int height = 256;

	vector<PixelColor> image;
	image.resize(width * height);

	cout << "Printing Test Image" << endl;

	for (int j = 0; j < height; j++) {
		clog << "\rScanlines remaining " << height - j << ' ' << flush;
		for (int i = 0; i < width; i++) {
			auto pColor = Color(
				double(i) / (width-1),
				double(j) / (height-1),
				0
			);

			PixelColor p(pColor);
			image[j * width + i] = p;
		}
	}

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, width, height, "NewTest.png");
}

int main()
{
	vector<PixelColor> image;
	PrintSampleImage(image);

	vector<double> doubleImage = ConvertToDoubleVector(image);
	SaveImage(doubleImage, 256, 256, "saved_output.png");

	PrintTestImage();

	return 0;
}
