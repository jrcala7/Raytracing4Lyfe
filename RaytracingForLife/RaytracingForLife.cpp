// RaytracingForLife.cpp : Defines the entry point for the application.
//

#include "RaytracingForLife.h"
#include "rt_src/Material.h"

using namespace std;

int main()
{
	// World
	Hittable_List hittables;

	auto mat_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
	auto mat_center = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));

	auto mat_left = make_shared<Metal>(Color(0.8, 0.8, 0.8));
	auto mat_right = make_shared<Metal>(Color(0.8, 0.6, 0.2));

	hittables.
		Add(make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, mat_ground));

	hittables.
		Add(make_shared<Sphere>(Point3(0.0, 0.0, -1.2), 0.5, mat_center));

	hittables.
		Add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, mat_left));

	hittables.
		Add(make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, mat_right));

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
