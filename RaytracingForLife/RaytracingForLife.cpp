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
	// World
	Hittable_List hittables;
	hittables.Add(
		make_shared<Sphere>(Point3(0, 0, -1), 0.5)
	);

	hittables.Add(
		make_shared<Sphere>(Point3(0, -100.5, -1), 100)
	);

	vector<PixelColor> image;

	Camera camera;
	camera.aspectRatio = 16.0 / 9.0;
	camera.width = 400;

	camera.Render(hittables, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, camera.width, camera.height, "CameraTest.png");

	cout << "Done Printing Camera Test";

	return 0;
}
