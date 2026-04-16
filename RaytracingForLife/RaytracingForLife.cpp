// RaytracingForLife.cpp : Defines the entry point for the application.
//

#include "RaytracingForLife.h"
#include "rt_src/Constant_Medium.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "rt_src/ShaderUtils.h"
#include "rt_src/GPUComputeRenderer.h"
#include "comp_src/BVH_Compute.h"
#include "comp_src/BVH_GPU_Manager.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

using namespace std;

void RT_WeekendFinalRender() {
	// World
	Hittable_List world;
	//Lights
	Hittable_List lights;

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
	camera.background = Color(0.7, 0.8, 1.0);

	camera.vfov = 20.0;
	camera.lookfrom = Point3(13, 2, 3);
	camera.lookat = Point3(0, 0, 0);
	camera.vup = Vec3(0, 1, 0);

	camera.defocus_angle = 0.6;
	camera.focus_dist = 10.0;

	camera.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, camera.width, camera.height, "RT01_Final.png");

	cout << "RT01_Final.png" << endl;
}

void CheckerSphere() {
	Hittable_List world;
	//Lights
	Hittable_List lights;

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
	camera.background = Color(0.7, 0.8, 1.0);

	camera.vfov = 20.0;
	camera.lookfrom = Point3(13, 2, 3);
	camera.lookat = Point3(0, 0, 0);
	camera.vup = Vec3(0, 1, 0);

	camera.defocus_angle = 0.;

	camera.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, camera.width, camera.height, "RT02_Final.png");

	cout << "RT02_Final.png" << endl;
}

void SphereSample() {
	auto tex = make_shared<ImageTexture>("zukiss.png");
	auto surf = make_shared<Lambertian>(tex);
	auto globe = make_shared<Sphere>(Point3(0, 0, 0), 2.0, surf);

	//Lights
	Hittable_List lights;

	auto empty_material = shared_ptr<Material>();
	lights.Add(
		make_shared<Quad>(Point3(0, 554, 0), Vec3(-130, 0, 0), Vec3(0, 0, -105), empty_material));

	Camera cam;
	
	cam.aspectRatio = 16.0 / 9.0;
	cam.width = 1600;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.background = Color(0.7, 0.8, 1.0);

	cam.vfov = 20;
	cam.lookfrom = Point3(0, 0, 12);
	cam.lookat = Point3(0, 0, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;
	cam.Render(Hittable_List(globe), lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "SampleTexImg.png");

	cout << "SampleTexImg.png " << cam.width << "x" << cam.height << "y" << endl;
}

void NoiseSample() {
	Hittable_List world;
	//Lights
	Hittable_List lights;

	auto pertex = make_shared<NoiseTexture>(4.0);
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
	cam.background = Color(0.7, 0.8, 1.0);

	cam.vfov = 20;
	cam.lookfrom = Point3(13, 2, 3);
	cam.lookat = Point3(0, 0, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;
	cam.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "SampleTexImg.png");

	cout << "SampleTexImg.png" << endl;
}

void QuadTest() {
	Hittable_List world;
	//Lights
	Hittable_List lights;

	//Colors
	auto l_mat = make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
	auto b_mat = make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
	auto r_mat = make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
	auto u_mat = make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
	auto d_mat = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

	//Quads
	world.Add(
		make_shared<Quad>(
			Point3(-3, -2, 5), Vec3(0,0,-4), Vec3(0, 4, 0), l_mat
		)
	);

	world.Add(
		make_shared<Quad>(
			Point3(-2, -2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), b_mat
		)
	);

	world.Add(
		make_shared<Quad>(
			Point3(3, -2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), r_mat
		)
	);

	world.Add(
		make_shared<Quad>(
			Point3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), u_mat
		)
	);

	world.Add(
		make_shared<Quad>(
			Point3(-2, -3, 5), Vec3(4, 0, 0), Vec3(0, 0, -4), d_mat
		)
	);

	Camera cam;

	cam.aspectRatio = 1.0;
	cam.width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.background = Color(0.7, 0.8, 1.0);

	cam.vfov = 80;
	cam.lookfrom = Point3(0, 0, 9);
	cam.lookat = Point3(0, 0, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;
	cam.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "QuadTest.png");

	cout << "QuadTest.png" << endl;
}

void SimpleLight() {
	Hittable_List world;
	//Lights
	Hittable_List lights;

	auto pertex = make_shared<NoiseTexture>(4.0);
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


	auto diffLight = make_shared<DiffuseLight>(Color(4,4,4));
	world.Add(
		make_shared<Sphere>(Point3(0, 7, 0), 2, diffLight)
	);
	world.Add(
		make_shared<Quad>(Point3(3, 1, -2), Vec3(2,0,0), Vec3(0,2,0), diffLight)
	);

	Camera cam;

	cam.aspectRatio = 16.0 / 9.0;
	cam.width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.background = Color(0.0, 0.0, 0.0);

	cam.vfov = 20;
	cam.lookfrom = Point3(26, 3, 6);
	cam.lookat = Point3(0, 2, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;
	cam.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "SampleLight.png");

	cout << "SampleLight.png" << endl;
}

void CornellBox() {
	Hittable_List world;
	//Lights
	Hittable_List lights;

	auto red = make_shared<Lambertian>(Color(0.65, 0.05, 0.05));
	auto white = make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
	auto green = make_shared<Lambertian>(Color(0.12, 0.45, 0.15));
	auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

	world.Add(
		make_shared<Quad>(
			Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green
		)
	);

	world.Add(
		make_shared<Quad>(
			Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red
		)
	);

	world.Add(
		make_shared<Quad>(
			Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light
		)
	);

	world.Add(
		make_shared<Quad>(
			Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white
		)
	);

	world.Add(
		make_shared<Quad>(
			Point3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), white
		)
	);

	world.Add(
		make_shared<Quad>(
			Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white
		)
	);

	shared_ptr<Hittable> box1 = Box(
		Point3(0, 0, 0),
		Point3(165, 330, 165),
		white
	);
	box1 = make_shared<Rot_Y>(box1, 15);
	box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));
	world.Add(box1);

	shared_ptr<Hittable> box2 = Box(
		Point3(0, 0, 0),
		Point3(165, 165, 165),
		white
	);
	box2 = make_shared<Rot_Y>(box2, -18);
	box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));
	world.Add(box2);

	Camera cam;

	cam.aspectRatio = 1.0;
	cam.width = 600;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.background = Color(0.0, 0.0, 0.0);

	cam.vfov = 40;
	cam.lookfrom = Point3(278, 278, -800);
	cam.lookat = Point3(278, 278, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;
	cam.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "CornellBox.png");

	cout << "CornellBox.png" << endl;
}

void CornellBox2() {
	Hittable_List world;

	auto red = make_shared<Lambertian>(Color(0.65, 0.05, 0.05));
	auto white = make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
	auto green = make_shared<Lambertian>(Color(0.12, 0.45, 0.15));
	auto blue = make_shared<Lambertian>(Color(0.68, 0.78, 0.81));
	auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

	// Cornell box sides
	world.Add(make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 0, 555), Vec3(0, 555, 0), green));
	world.Add(make_shared<Quad>(Point3(0, 0, 555), Vec3(0, 0, -555), Vec3(0, 555, 0), red));
	world.Add(make_shared<Quad>(Point3(0, 555, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
	world.Add(make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 0, -555), white));
	world.Add(make_shared<Quad>(Point3(555, 0, 555), Vec3(-555, 0, 0), Vec3(0, 555, 0), white));

	// Light
	world.Add(make_shared<Quad>(Point3(213, 554, 227), Vec3(130, 0, 0), Vec3(0, 0, 105), light));

	shared_ptr<Material> aluminum = make_shared<Metal>(Color(0.8, 0.85, 0.88), 0.0);
	shared_ptr<Hittable> box1 = Box(
		Point3(0, 0, 0),
		Point3(165, 330, 165),
		white
	);
	box1 = make_shared<Rot_Y>(box1, 15);
	box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));
	world.Add(box1);

	shared_ptr<Hittable> box2 = Box(
		Point3(0, 0, 0),
		Point3(165, 165, 165),
		white
	);
	box2 = make_shared<Rot_Y>(box2, -18);
	box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));
	world.Add(box2);

	Model3D cube;

	cube.LoadModel("bunny.obj", blue, 800.0);
	cube.ModelTranslate(300, 200, 200);

	//////Tris
	world.Add(
		cube.modelTris
	);

	//Light itself
	Hittable_List lights;
	auto empty_material = shared_ptr<Material>();
	lights.Add(
		make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), empty_material));

	Camera cam;

	cam.aspectRatio = 1.0;
	cam.width = 1200;
	cam.samples_per_pixel = 5000;
	cam.max_depth = 40;
	cam.background = Color(0.0, 0.0, 0.0);

	cam.vfov = 40;
	cam.lookfrom = Point3(278, 278, -800);
	cam.lookat = Point3(278, 278, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;
	
	world = Hittable_List(make_shared<BVH_Node>(world));

	cam.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "CornellBox2.png");

	cout << "CornellBox2.png" << endl;
}

void cornell_smoke() {
	Hittable_List world;
	//Lights
	Hittable_List lights;

	auto red = make_shared<Lambertian>(Color(.65, .05, .05));
	auto white = make_shared<Lambertian>(Color(.73, .73, .73));
	auto green = make_shared<Lambertian>(Color(.12, .45, .15));
	auto light = make_shared<DiffuseLight>(Color(7, 7, 7));

	world.Add(make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
	world.Add(make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
	world.Add(make_shared<Quad>(Point3(113, 554, 127), Vec3(330, 0, 0), Vec3(0, 0, 305), light));
	world.Add(make_shared<Quad>(Point3(0, 555, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
	world.Add(make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
	world.Add(make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

	shared_ptr<Hittable> box1 = Box(Point3(0, 0, 0), Point3(165, 330, 165), white);
	box1 = make_shared<Rot_Y>(box1, 15);
	box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));

	shared_ptr<Hittable> box2 = Box(Point3(0, 0, 0), Point3(165, 165, 165), white);
	box2 = make_shared<Rot_Y>(box2, -18);
	box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));

	world.Add(make_shared<Constant_Medium>(box1, 0.01, Color(0, 0, 0)));
	world.Add(make_shared<Constant_Medium>(box2, 0.01, Color(1, 1, 1)));

	Camera cam;

	cam.aspectRatio = 1.0;
	cam.width = 600;
	cam.samples_per_pixel = 10;
	cam.max_depth = 50;
	cam.background = Color(0, 0, 0);

	cam.vfov = 40;
	cam.lookfrom = Point3(278, 278, -800);
	cam.lookat = Point3(278, 278, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;
	cam.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "Cornnel Smoke.png");

	cout << "Cornnel Smoke.png" << endl;
}

void final_scene2(int image_width, int samples_per_pixel, int max_depth) {
	Hittable_List boxes1;
	auto ground = make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

	int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) {
		for (int j = 0; j < boxes_per_side; j++) {
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = random_double(1, 101);
			auto z1 = z0 + w;

			boxes1.Add(Box(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
		}
	}

	Hittable_List world;
	//Lights
	Hittable_List lights;

	auto empty_material = shared_ptr<Material>();
	lights.Add(
		make_shared<Quad>(Point3(0, 554, 0), Vec3(-130, 0, 0), Vec3(0, 0, -105), empty_material));

	world.Add(make_shared<BVH_Node>(boxes1));

	auto light = make_shared<DiffuseLight>(Color(7, 7, 7));
	world.Add(make_shared<Quad>(Point3(123, 554, 147), Vec3(300, 0, 0), Vec3(0, 0, 265), light));

	auto center1 = Point3(400, 400, 200);
	auto center2 = center1 + Vec3(30, 0, 0);
	auto sphere_material = make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
	world.Add(make_shared<Sphere>(center1, center2, 50, sphere_material));

	world.Add(make_shared<Sphere>(Point3(260, 150, 45), 50, make_shared<Dielectric>(1.5)));
	world.Add(make_shared<Sphere>(
		Point3(0, 150, 145), 50, make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)
	));

	auto boundary = make_shared<Sphere>(Point3(360, 150, 145), 70, make_shared<Dielectric>(1.5));
	world.Add(boundary);
	world.Add(make_shared<Constant_Medium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
	boundary = make_shared<Sphere>(Point3(0, 0, 0), 5000, make_shared<Dielectric>(1.5));
	world.Add(make_shared<Constant_Medium>(boundary, .0001, Color(1, 1, 1)));

	auto emat = make_shared<Lambertian>(make_shared<ImageTexture>("zukiss.png"));
	world.Add(make_shared<Sphere>(Point3(400, 200, 400), 100, emat));
	auto pertext = make_shared<NoiseTexture>(0.2);
	world.Add(make_shared<Sphere>(Point3(220, 280, 300), 80, make_shared<Lambertian>(pertext)));

	Hittable_List boxes2;
	auto white = make_shared<Lambertian>(Color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.Add(make_shared<Sphere>(Point3::random(0, 165), 10, white));
	}

	world.Add(make_shared<Translate>(
		make_shared<Rot_Y>(
			make_shared<BVH_Node>(boxes2), 15),
		Vec3(-100, 270, 395)
	)
	);

	Camera cam;

	cam.aspectRatio = 1.0;
	cam.width = image_width;
	cam.samples_per_pixel = samples_per_pixel;
	cam.max_depth = max_depth;
	cam.background = Color(0, 0, 0);

	cam.vfov = 40;
	cam.lookfrom = Point3(478, 278, -600);
	cam.lookat = Point3(278, 278, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;
	cam.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "RTNext_Weekend2.png");

	cout << "RTNext_Weekend2.png" << endl;
}

void TriTest() {
	Hittable_List world;
	//Lights
	Hittable_List lights;

	//Colors
	auto l_mat = make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
	auto b_mat = make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
	auto r_mat = make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
	auto u_mat = make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
	auto d_mat = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

	auto tex = make_shared<ImageTexture>("zukiss.png");
	auto surf = make_shared<Lambertian>(tex);

	Model3D cube;
	
	cube.LoadModel("myCube.obj", surf, 2.0);

	//Tris
	world.Add(
		cube.modelTris
	);

	Camera cam;

	cam.aspectRatio = 1.0;
	cam.width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.background = Color(0.7, 0.8, 1.0);

	cam.vfov = 80;
	cam.lookfrom = Point3(0, 0, 9);
	cam.lookat = Point3(0, 0, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;

	world = Hittable_List(make_shared<BVH_Node>(world));

	cam.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "RabbitTest.png");

	cout << "RabbitTest.png" << endl;
}

void RabbitScene() {
	Hittable_List world;
	//Lights
	Hittable_List lights;

	//Colors
	auto l_mat = make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
	auto b_mat = make_shared<Lambertian>(Color(0.4, 0.8, 0.4));
	auto r_mat = make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
	auto u_mat = make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
	auto d_mat = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

	auto tex = make_shared<ImageTexture>("zukiss.png");
	auto surf = make_shared<Lambertian>(tex);

	Model3D cube;

	cube.LoadModel("bunny.obj", b_mat, 45.0);
	

	//Tris
	world.Add(
		cube.modelTris
	);

	auto material1 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
	//auto material1 = make_shared<Dielectric>(1.5);
	world.Add(make_shared<Sphere>(Point3(0, -40, 0), 40, material1));

	auto white = make_shared<Lambertian>(Color(.73, .73, .73));
	auto box1 = make_shared<Sphere>(Point3(0, -40, 0), 45, material1);
	world.Add(make_shared<Constant_Medium>(box1, 0.01, Color(0, 0, 0)));

	auto empty_material = shared_ptr<Material>();
	lights.Add(
		make_shared<Quad>(Point3(0, 554, 0), Vec3(-130, 0, 0), Vec3(0, 0, -105), empty_material));

	Camera cam;

	cam.aspectRatio = 1.0;
	cam.width = 1200;
	cam.samples_per_pixel = 5;
	cam.max_depth = 50;
	cam.background = Color(0.4, 0.4, 1.0);

	cam.vfov = 80;
	cam.lookfrom = Point3(0, 10, 9);
	cam.lookat = Point3(-1.5, 4, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;

	world = Hittable_List(make_shared<BVH_Node>(world));

	cam.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "RabbitScene.png");

	cout << "RabbitScene.png" << endl;
}

void SwordScene() {
	Hittable_List world;
	//Lights
	Hittable_List lights;

	//Colors
	auto l_mat = make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
	auto b_mat = make_shared<Lambertian>(Color(0.4, 0.8, 0.4));
	auto r_mat = make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
	auto u_mat = make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
	auto d_mat = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

	auto tex = make_shared<ImageTexture>("partenza.jpg");
	auto surf = make_shared<Lambertian>(tex);

	Model3D cube;

	cube.LoadModel("djSword.obj", surf, 0.63);
	cube.ModelTranslate(0, -22, -5);

	//Tris
	world.Add(
		cube.modelTris
	);



	auto material1 = make_shared<Dielectric>(1.5);
	auto bloo = make_shared<Lambertian>(Color(0.2, 0.2, 0.8));
	auto red = make_shared<Lambertian>(Color(0.8, 0.2, 0.2));
	auto gray = make_shared<Lambertian>(Color(0.2, 0.2, 0.2));
	auto yerro = make_shared<Lambertian>(Color(1.0, 0.9, 0.5));

	auto white = make_shared<Lambertian>(Color(0.8, 0.8, 0.8));

	shared_ptr<Material> aluminum = make_shared<Metal>(Color(0.8, 0.85, 0.88), 0.0);
	world.Add(make_shared<Sphere>(Point3(-30, 25, 20), 20.0, aluminum));
	world.Add(make_shared<Sphere>(Point3(-10, -15, 40), 30.0, red));
	world.Add(make_shared<Sphere>(Point3(-120, -5, -10), 100.0, bloo));
	world.Add(make_shared<Sphere>(Point3(0, -2500, -0), 2000, gray));
	world.Add(make_shared<Sphere>(Point3(0, 2300, -0), 2000, yerro));

	world.Add(make_shared<Sphere>(Point3(180, -5, -10), 150.0, white));

	//auto material1 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
	////auto material1 = make_shared<Dielectric>(1.5);
	//world.Add(make_shared<Sphere>(Point3(0, -40, 0), 40, material1));

	//auto white = make_shared<Lambertian>(Color(.73, .73, .73));
	//auto box1 = make_shared<Sphere>(Point3(0, -40, 0), 45, material1);
	//world.Add(make_shared<Constant_Medium>(box1, 0.01, Color(0, 0, 0)));

	auto empty_material = shared_ptr<Material>();
	lights.Add(
		make_shared<Quad>(Point3(0, 554, 0), Vec3(-130, 0, 0), Vec3(0, 0, -105), empty_material));

	Camera cam;

	cam.aspectRatio = 1.0;
	cam.width = 600;
	cam.samples_per_pixel = 100;
	cam.max_depth = 40;
	cam.background = Color(0.4, 0.4, 1.0);

	cam.vfov = 80;
	cam.lookfrom = Point3(15, 3, 0);
	cam.lookat = Point3(0, 2, 0);
	cam.vup = Vec3(0, 1, 0);

	cam.defocus_angle = 0;

	vector<PixelColor> image;

	world = Hittable_List(make_shared<BVH_Node>(world));

	cam.Render(world, lights, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "RabbitScene.png");

	cout << "RabbitScene.png" << endl;
}

int TestComputeShader() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "FAILED: GLFW could not initialize\n";
		return -1;
	}
	std::cout << "SUCCESS: GLFW initialized\n";

	// Set OpenGL context version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Compute Shader Test", NULL, NULL);
	if (!window) {
		std::cerr << "FAILED: Could not create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "FAILED: GLAD could not load OpenGL functions\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	std::cout << "SUCCESS: GLAD loaded OpenGL\n";
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	// Create compute renderer
	GLuint width = 800;
	GLuint height = 600;
	ComputeRenderer renderer(width, height);

	// Load compute shader
	// Construct the shader path relative to the executable location
	//std::string shaderPath = "C:/Users/g411_jr/Repos/Raytracing4Lyfe/RaytracingForLife/rt_src/shaders/sample_gradient.comp";
	//std::string shaderPath = "C:/Users/g411_jr/Repos/Raytracing4Lyfe/RaytracingForLife/rt_src/shaders/checkerboard.comp";
	std::string shaderPath = "C:/Users/g411_jr/Repos/Raytracing4Lyfe/RaytracingForLife/rt_src/shaders/raytrace_simple.comp";
	GLuint program = ShaderUtils::LoadComputeShader(shaderPath);
	if (program == 0) {
		std::cerr << "FAILED: Could not load compute shader from: " << shaderPath << "\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	std::cout << "SUCCESS: Compute shader loaded\n";

	// Initialize image
	if (!renderer.LoadShaderProgram(program)) {
		std::cerr << "FAILED: Could not load shader program\n";
		glDeleteProgram(program);
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	if (!renderer.InitializeImage()) {
		std::cerr << "FAILED: Could not initialize image\n";
		glDeleteProgram(program);
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	std::cout << "SUCCESS: Image initialized (" << width << "x" << height << ")\n";

	// Dispatch compute shader
	if (!renderer.Dispatch()) {
		std::cerr << "FAILED: Compute shader dispatch failed\n";
		glDeleteProgram(program);
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	std::cout << "SUCCESS: Compute shader dispatched\n";

	// Read pixels back
	std::vector<float> pixels;
	if (!renderer.ReadImage(pixels)) {
		std::cerr << "FAILED: Could not read image from GPU\n";
		glDeleteProgram(program);
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	std::cout << "SUCCESS: Read " << pixels.size() << " pixels from GPU\n";

	// Verify some pixels were written
	bool hasNonZero = false;
	for (float val : pixels) {
		if (val > 0.0f) {
			hasNonZero = true;
			break;
		}
	}

	if (!hasNonZero) {
		std::cerr << "WARNING: All pixels are zero\n";
	} else {
		std::cout << "SUCCESS: Pixels contain non-zero values\n";
	}

	// Convert float RGBA pixels to BGR format for OpenCV
	std::vector<double> bgrImage;
	bgrImage.reserve(width * height * 3);

	for (size_t i = 0; i < pixels.size(); i += 4) {
		// RGBA from GPU -> BGR for OpenCV
		double r = std::max(0.0, std::min(1.0, (double)pixels[i]));
		double g = std::max(0.0, std::min(1.0, (double)pixels[i + 1]));
		double b = std::max(0.0, std::min(1.0, (double)pixels[i + 2]));
		// Skip alpha channel

		bgrImage.push_back(b);
		bgrImage.push_back(g);
		bgrImage.push_back(r);
	}

	SaveImage(bgrImage, width, height, "ComputeShader_Gradient.png");
	std::cout << "SUCCESS: Saved to ComputeShader_Gradient.png\n";

	// Cleanup
	glDeleteProgram(program);
	glfwDestroyWindow(window);
	glfwTerminate();

	std::cout << "\nCompute shader test completed successfully!\n";
	return 0;
}

int TestGLFW() {

	// Check GLFW
	if (!glfwInit()) {
		std::cerr << "FAILED: GLFW could not initialize\n";
		return -1;
	}
	std::cout << "SUCCESS: GLFW initialized\n";

	GLFWwindow* window = glfwCreateWindow(800, 600, "Verify", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Check GLAD & OpenGL
	// This loads the OpenGL function pointers via GLFW's function loader
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "FAILED: GLAD could not load OpenGL functions\n";
		return -1;
	}

	std::cout << "SUCCESS: GLAD loaded OpenGL\n";
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	glfwTerminate();
	return 0;

}

int TestComputeShader_SphereSample() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "FAILED: GLFW could not initialize\n";
		return -1;
	}
	std::cout << "SUCCESS: GLFW initialized\n";

	// Set OpenGL context version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Sphere Sample - Compute Shader", NULL, NULL);
	if (!window) {
		std::cerr << "FAILED: Could not create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "FAILED: GLAD could not load OpenGL functions\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	std::cout << "SUCCESS: GLAD loaded OpenGL\n";
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	// ===== Camera parameters (matching SphereSample) =====
	float aspectRatio = 16.0f / 9.0f;
	float vfov = 20.0f;  // vertical field of view in degrees
	float focus_dist = 1.0f;  // default focus distance
	glm::vec3 camera_origin(0.0f, 0.0f, 12.0f);
	glm::vec3 camera_lookat(0.0f, 0.0f, 0.0f);

	// Image dimensions
	GLuint width = 1600;
	GLuint height = static_cast<GLuint>(width / aspectRatio);
	float aspect_ratio = (float)width / (float)height;

	// Calculate viewport height using vfov (matching Camera::Initialize)
	float theta = glm::radians(vfov);
	float h = glm::tan(theta / 2.0f);
	float viewport_height = 2.0f * h * focus_dist;
	float viewport_width = viewport_height * aspect_ratio;

	std::cout << "SUCCESS: Camera vfov=" << vfov << "°, viewport_height=" << viewport_height 
		<< ", viewport_width=" << viewport_width << "\n";

	// ===== Step 1: Create scene geometry =====
	// Create a single sphere (matching SphereSample's globe)
	glm::vec3 sphere_center(0.0f, 0.0f, 0.0f);
	float sphere_radius = 2.0f;

	// Create BVH primitives - single sphere bounds
	std::vector<comp_bvh::BVH_Builder::Primitive> primitives;
	primitives.push_back({
		sphere_center - glm::vec3(sphere_radius),
		sphere_center + glm::vec3(sphere_radius),
		0  // object_id
	});

	// ===== Step 2: Build GPU BVH =====
	comp_bvh::BVH_Builder builder(primitives);
	comp_bvh::BVH_GPU bvh = builder.build();
	std::cout << "SUCCESS: BVH built with " << bvh.get_node_count() << " nodes\n";

	// ===== Step 3: Create sphere geometry for GPU =====
	// Sphere data: center (vec3) + radius (float)
	std::vector<glm::vec4> sphere_data;
	sphere_data.push_back(glm::vec4(sphere_center, sphere_radius));

	// ===== Step 4: Create material data =====
	// Simple material: just store a color for now
	std::vector<glm::vec4> material_data;
	material_data.push_back(glm::vec4(0.5f, 0.5f, 0.8f, 1.0f));  // Matching SphereSample's Lambertian(Color(0.5, 0.5, 0.8))

	// ===== Step 4b: Load texture image =====
	int tex_width, tex_height, tex_channels;
	const char* texture_filename = "C:\\Users\\g411_jr\\Repos\\Raytracing4Lyfe\\RaytracingForLife\\images\\zukiss.png";
	stbi_set_flip_vertically_on_load(true);  // Flip vertically to match OpenGL texture coordinates
	unsigned char* texture_data = stbi_load(texture_filename, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
	stbi_set_flip_vertically_on_load(false);

	if (!texture_data) {
		std::cerr << "FAILED: Could not load texture: " << texture_filename << "\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	std::cout << "SUCCESS: Loaded texture " << texture_filename << " (" << tex_width << "x" << tex_height << ", " << tex_channels << " channels)\n";

	// ===== Step 5: Create compute renderer =====
	ComputeRenderer renderer(width, height);

	// Load compute shader
	std::string shaderPath = "C:/Users/g411_jr/Repos/Raytracing4Lyfe/RaytracingForLife/comp_src/shaders/bvh_traverse.comp";
	GLuint program = ShaderUtils::LoadComputeShader(shaderPath);
	if (program == 0) {
		std::cerr << "FAILED: Could not load compute shader from: " << shaderPath << "\n";
		stbi_image_free(texture_data);
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	std::cout << "SUCCESS: Compute shader loaded\n";

	// Initialize image
	if (!renderer.LoadShaderProgram(program)) {
		std::cerr << "FAILED: Could not load shader program\n";
		glDeleteProgram(program);
		stbi_image_free(texture_data);
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	if (!renderer.InitializeImage()) {
		std::cerr << "FAILED: Could not initialize image\n";
		glDeleteProgram(program);
		stbi_image_free(texture_data);
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	std::cout << "SUCCESS: Image initialized (" << width << "x" << height << ")\n";

	// ===== Step 6: Upload BVH, scene data, and texture to GPU =====
	BVH_GPU_Manager bvh_manager;
	bvh_manager.upload_bvh(bvh);
	bvh_manager.upload_spheres(sphere_data);
	bvh_manager.upload_materials(material_data);
	bvh_manager.upload_texture(texture_data, tex_width, tex_height);
	bvh_manager.bind_to_program(program);

	std::cout << "SUCCESS: BVH, scene data, and texture uploaded to GPU\n";

	// ===== Step 7: Set shader uniforms =====
	glUseProgram(program);

	// Camera parameters (matching SphereSample)
	glm::vec3 camera_dir = glm::normalize(camera_lookat - camera_origin);
	glm::vec3 camera_vup(0.0f, 1.0f, 0.0f);  // Standard up vector

	// Compute camera basis vectors (matching Camera::Initialize)
	glm::vec3 w = glm::normalize(camera_origin - camera_lookat);  // backwards
	glm::vec3 u = glm::normalize(glm::cross(camera_vup, w));       // right
	glm::vec3 v = glm::cross(w, u);                                 // up

	// Compute viewport basis vectors
	glm::vec3 viewport_u = viewport_width * u;
	glm::vec3 viewport_v = viewport_height * (-v);  // negative v for top-to-bottom

	// Compute pixel delta vectors
	glm::vec3 pixel_delta_u = viewport_u / float(width);
	glm::vec3 pixel_delta_v = viewport_v / float(height);

	// Compute upper left pixel location
	glm::vec3 viewport_upper_left = camera_origin - (focus_dist * w) - viewport_u / 2.0f - viewport_v / 2.0f;
	glm::vec3 pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

	// Set uniforms
	GLint bvh_root_loc = glGetUniformLocation(program, "bvh_root");
	GLint sphere_count_loc = glGetUniformLocation(program, "sphere_count");
	GLint camera_origin_loc = glGetUniformLocation(program, "camera_origin");
	GLint pixel00_loc_loc = glGetUniformLocation(program, "pixel00_loc");
	GLint pixel_delta_u_loc = glGetUniformLocation(program, "pixel_delta_u");
	GLint pixel_delta_v_loc = glGetUniformLocation(program, "pixel_delta_v");
	GLint use_texture_loc = glGetUniformLocation(program, "use_texture");

	if (bvh_root_loc != -1) glUniform1ui(bvh_root_loc, bvh_manager.get_root_index());
	if (sphere_count_loc != -1) glUniform1ui(sphere_count_loc, 1);  // 1 sphere
	if (camera_origin_loc != -1) glUniform3fv(camera_origin_loc, 1, glm::value_ptr(camera_origin));
	if (pixel00_loc_loc != -1) glUniform3fv(pixel00_loc_loc, 1, glm::value_ptr(pixel00_loc));
	if (pixel_delta_u_loc != -1) glUniform3fv(pixel_delta_u_loc, 1, glm::value_ptr(pixel_delta_u));
	if (pixel_delta_v_loc != -1) glUniform3fv(pixel_delta_v_loc, 1, glm::value_ptr(pixel_delta_v));
	if (use_texture_loc != -1) glUniform1i(use_texture_loc, 1);  // Enable texture sampling

	std::cout << "SUCCESS: Shader uniforms set\n"
		<< "  pixel00_loc: (" << pixel00_loc.x << ", " << pixel00_loc.y << ", " << pixel00_loc.z << ")\n"
		<< "  pixel_delta_u: (" << pixel_delta_u.x << ", " << pixel_delta_u.y << ", " << pixel_delta_u.z << ")\n"
		<< "  pixel_delta_v: (" << pixel_delta_v.x << ", " << pixel_delta_v.y << ", " << pixel_delta_v.z << ")\n";

	// Dispatch compute shader
	if (!renderer.Dispatch()) {
		std::cerr << "FAILED: Compute shader dispatch failed\n";
		glDeleteProgram(program);
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	std::cout << "SUCCESS: Compute shader dispatched\n";

	// ===== Step 8: Read pixels back =====
	std::vector<float> pixels;
	if (!renderer.ReadImage(pixels)) {
		std::cerr << "FAILED: Could not read image from GPU\n";
		glDeleteProgram(program);
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	std::cout << "SUCCESS: Read " << pixels.size() << " pixels from GPU\n";

	// Verify some pixels were written
	bool hasNonZero = false;
	for (float val : pixels) {
		if (val > 0.0f) {
			hasNonZero = true;
			break;
		}
	}

	if (!hasNonZero) {
		std::cerr << "WARNING: All pixels are zero\n";
	} else {
		std::cout << "SUCCESS: Pixels contain non-zero values\n";
	}

	// ===== Step 9: Convert and save image =====
	// Convert float RGBA pixels to BGR format for OpenCV
	std::vector<double> bgrImage;
	bgrImage.reserve(width * height * 3);

	for (size_t i = 0; i < pixels.size(); i += 4) {
		// RGBA from GPU -> BGR for OpenCV
		double r = std::max(0.0, std::min(1.0, (double)pixels[i]));
		double g = std::max(0.0, std::min(1.0, (double)pixels[i + 1]));
		double b = std::max(0.0, std::min(1.0, (double)pixels[i + 2]));
		// Skip alpha channel

		bgrImage.push_back(b);
		bgrImage.push_back(g);
		bgrImage.push_back(r);
	}

	SaveImage(bgrImage, width, height, "ComputeShader_SphereSample.png");
	std::cout << "SUCCESS: Saved to ComputeShader_SphereSample.png\n";

	// Cleanup
	stbi_image_free(texture_data);
	glDeleteProgram(program);
	glfwDestroyWindow(window);
	glfwTerminate();

	std::cout << "\nCompute shader SphereSample test completed successfully!\n";
	return 0;
}

int main()
{
	//RT_WeekendFinalRender();
	//CheckerSphere();
	SphereSample();
	//NoiseSample();
	//QuadTest();
	//SimpleLight();
	//CornellBox();
	//CornellBox2();
	//TriTest();
	//cornell_smoke();
	//final_scene2(1200, 10000, 50);
	//RabbitScene();
	//SwordScene();

	//int ret = TestComputeShader();
	//int ret = TestComputeShader_SphereSample();

	return 1;
}
