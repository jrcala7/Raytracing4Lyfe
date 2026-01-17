// RaytracingForLife.cpp : Defines the entry point for the application.
//

#include "RaytracingForLife.h"

using namespace std;

int main()
{
	vector<PixelColor> image;
	PrintSampleImage(image);

	vector<double> doubleImage = ConvertToDoubleVector(image);
	SaveImage(doubleImage, 256, 256, "saved_output.png");

	return 0;
}
