// RaytracingForLife.cpp : Defines the entry point for the application.
//

#include "RaytracingForLife.h"
#include "rt_src/Constant_Medium.h"

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
	camera.background = Color(0.7, 0.8, 1.0);

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
	camera.background = Color(0.7, 0.8, 1.0);

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
	cam.width = 100;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;
	cam.background = Color(0.7, 0.8, 1.0);

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
	cam.Render(world, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "SampleTexImg.png");

	cout << "SampleTexImg.png" << endl;
}

void QuadTest() {
	Hittable_List world;

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
	cam.Render(world, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "QuadTest.png");

	cout << "QuadTest.png" << endl;
}

void SimpleLight() {
	Hittable_List world;

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
	cam.Render(world, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "SampleLight.png");

	cout << "SampleLight.png" << endl;
}

void CornellBox() {
	Hittable_List world;

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
	cam.Render(world, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "CornellBox.png");

	cout << "CornellBox.png" << endl;
}

void CornellBox2() {
	Hittable_List world;

	auto red = make_shared<Lambertian>(Color(0.65, 0.05, 0.05));
	auto white = make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
	auto green = make_shared<Lambertian>(Color(0.12, 0.45, 0.15));
	auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

	// Cornell box sides
	world.Add(make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 0, 555), Vec3(0, 555, 0), green));
	world.Add(make_shared<Quad>(Point3(0, 0, 555), Vec3(0, 0, -555), Vec3(0, 555, 0), red));
	world.Add(make_shared<Quad>(Point3(0, 555, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
	world.Add(make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 0, -555), white));
	world.Add(make_shared<Quad>(Point3(555, 0, 555), Vec3(-555, 0, 0), Vec3(0, 555, 0), white));

	// Light
	world.Add(make_shared<Quad>(Point3(213, 554, 227), Vec3(130, 0, 0), Vec3(0, 0, 105), light));

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
	cam.Render(world, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "CornellBox2.png");

	cout << "CornellBox2.png" << endl;
}

void cornell_smoke() {
	Hittable_List world;

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
	cam.Render(world, image);

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
	cam.Render(world, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "RTNext_Weekend2.png");

	cout << "RTNext_Weekend2.png" << endl;
}

void TriTest() {
	Hittable_List world;

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
	/*
	world.Add(
		make_shared<Quad>(
			Point3(-3, -2, 5), Vec3(0, 0, -4), Vec3(0, 4, 0), l_mat
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
	*/

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

	cam.Render(world, image);

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, cam.width, cam.height, "RabbitTest.png");

	cout << "RabbitTest.png" << endl;
}

int main()
{
	//RT_WeekendFinalRender();
	//CheckerSphere();
	//SphereSample();
	//NoiseSample();
	//QuadTest();
	//SimpleLight();
	//CornellBox();
	CornellBox2();
	//TriTest();
	//cornell_smoke();
	//final_scene2(800, 10000, 40);
	return 0;
}
