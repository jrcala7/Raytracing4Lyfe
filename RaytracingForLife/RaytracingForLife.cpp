// RaytracingForLife.cpp : Defines the entry point for the application.
//

#include "RaytracingForLife.h"

using namespace std;

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
	camera.samples_per_pixel = 100;
	camera.max_depth = 50;

	camera.Render(hittables, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, camera.width, camera.height, "CameraTest.png");

	cout << "Done Printing Camera Test";

	return 0;
}
