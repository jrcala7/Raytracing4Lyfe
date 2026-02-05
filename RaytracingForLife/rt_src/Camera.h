#pragma once

#include "Hittable.h"
#include "Material.h"
#include "../Utils.h"
#include <vector>


using namespace std;

class Camera {

public:
	double aspectRatio = 16.0 / 9.0;
	int width = 480;
	int height = 0;
	int samples_per_pixel = 10;
	int max_depth = 10;
	Color background;

	double vfov = 90.0; //Vertical FOV
	Point3 lookfrom = Point3(0, 0, 0);
	Point3 lookat = Point3(0, 0, -1);
	Vec3 vup = Vec3(0, 1, 0);

	double defocus_angle = 0;
	double focus_dist = 10;

	//Camera Propertiesw
	void Render(const Hittable& world, vector<PixelColor>& pixels);;

private:
	Point3 center;
	Point3 pixel00_loc;
	double pixel_samples_scale;

	Vec3 pixel_delta_u;
	Vec3 pixel_delta_v;
	Vec3 u, v, w;

	Vec3 defocus_disk_u;
	Vec3 defocus_disk_v;

	void Initialize();

	Color Ray_Color(const Ray& r, int depth, const Hittable& world) const;

	Ray Get_Ray(int i, int j) const;

	Vec3 SampleSquare() const;

	Point3 defocus_disk_sample() const;
};