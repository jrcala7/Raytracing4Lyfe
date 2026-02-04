// RaytracingForLife.cpp : Defines the entry point for the application.
//

#include "RaytracingForLife.h"
#include "rt_src/Material.h"
#include "rt_src/BVH.h"
#include "rt_src/Checker_Texture.h"

using namespace std;

void RT_WeekendFinalRender() {
	// World
	Hittable_List world;

	auto checker = make_shared<Checker_Texture>(
		0.32,
		Color(0.2, 0.3, 0.1),
		Color(0.9, 0.9, 0.9)
	);
	world.Add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(checker)));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			Point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
			if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<Material> sphere_material;
				if (choose_mat < 0.8) {
					//Diffuse
					auto albedo = Color::random() * Color::random();
					sphere_material = make_shared<Lambertian>(albedo);

					auto off_center = center + Vec3(0, random_double(0, 0.5), 0);
					world.Add(make_shared<Sphere>(center, off_center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					//Metal
					auto albedo = Color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<Metal>(albedo, fuzz);
					world.Add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else {
					//Glass
					sphere_material = make_shared<Dielectric>(1.5);
					world.Add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<Dielectric>(1.5);
	world.Add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));
	
	auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));	
	world.Add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
	world.Add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

	world = Hittable_List(make_shared<BVH_Node>(world));

	vector<PixelColor> image;

	Camera camera;
	camera.aspectRatio = 16.0 / 9.0;
	camera.width = 400;
	camera.samples_per_pixel = 10;
	camera.max_depth = 20;

	camera.vfov = 20.0;
	camera.lookfrom = Point3(13, 2, 3);
	camera.lookat = Point3(0, 0, 0);
	camera.vup = Vec3(0, 1, 0);

	camera.defocus_angle = 0.6;
	camera.focus_dist = 10.0;

	camera.Render(world, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, camera.width, camera.height, "RT01_Final.png");

	cout << "RT01_Final.png" << endl;
}

void CheckerSphere() {
	Hittable_List world;

	auto checker = make_shared<Checker_Texture>(
		0.32,
		Color(0.2, 0.3, 0.1),
		Color(0.9, 0.9, 0.9)
	);

	world.Add(make_shared<Sphere>(Point3(0, -10, 0), 10, make_shared<Lambertian>(checker)));
	world.Add(make_shared<Sphere>(Point3(0, 10, 0), 10, make_shared<Lambertian>(checker)));

	vector<PixelColor> image;

	Camera camera;
	camera.aspectRatio = 16.0 / 9.0;
	camera.width = 400;
	camera.samples_per_pixel = 10;
	camera.max_depth = 50;

	camera.vfov = 20.0;
	camera.lookfrom = Point3(13, 2, 3);
	camera.lookat = Point3(0, 0, 0);
	camera.vup = Vec3(0, 1, 0);

	camera.defocus_angle = 0.;

	camera.Render(world, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, camera.width, camera.height, "RT02_Final.png");

	cout << "RT02_Final.png" << endl;
}

void SphereSample() {
	auto tex = make_shared<ImageTexture>("zukiss.png");
	auto surf = make_shared<Lambertian>(tex);
	auto globe = make_shared<Sphere>(Point3(0, 0, 0), 2.0, surf);

	Camera cam;
	
	cam.aspectRatio = 16.0 / 9.0;
	cam.width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.vfov = 20;
	cam.lookfrom = Point3(0, 0, 12);
	cam.lookat = Point3(0, 0, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;
	cam.Render(Hittable_List(globe), image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "SampleTexImg.png");

	cout << "SampleTexImg.png" << endl;
}

void NoiseSample() {
	Hittable_List world;

	auto pertex = make_shared<NoiseTexture>();
	world.Add(
		make_shared<Sphere>(
			Point3(0, -1000, 0), 1000,
			make_shared<Lambertian>(pertex)
		)
	);

	world.Add(
		make_shared<Sphere>(
			Point3(0, 2, 0), 2,
			make_shared<Lambertian>(pertex)
		)
	);

	Camera cam;

	cam.aspectRatio = 16.0 / 9.0;
	cam.width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.vfov = 20;
	cam.lookfrom = Point3(13, 2, 3);
	cam.lookat = Point3(0, 0, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;
	cam.Render(world, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "SampleTexImg.png");

	cout << "SampleTexImg.png" << endl;
}

int main()
{
	//RT_WeekendFinalRender();
	//CheckerSphere();
	//SphereSample();
	NoiseSample();
	return 0;
}
