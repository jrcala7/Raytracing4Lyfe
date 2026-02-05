#pragma once
#include "IETThread.h"
#include "../rt_src/Ray.h"
#include "../rt_src/Color.h"
#include "../rt_src/Material.h"
#include "../rt_src/Hittable.h"
#include "../Utils.h"

struct CameraProperties {
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

	Point3 center;
	Point3 pixel00_loc;
	double pixel_samples_scale;

	Vec3 pixel_delta_u;
	Vec3 pixel_delta_v;
	Vec3 u, v, w;

	Vec3 defocus_disk_u;
	Vec3 defocus_disk_v;
};

class IFinishedTask {
public:
	virtual void OnFinishedTask(int id, PixelColor pixelOut) = 0;
};

class PixelThread : public IETThread {
public:
	PixelThread(int _x, int _y, int width,
		CameraProperties prop, const Hittable& _world) :
		x(_x), y(_y),
		camera(prop), world(_world), id (_y * width + _x){

	}

	void AssignCallback(IFinishedTask* callback) {
		onDone = callback;
	}

	void Sample() {
		std::cout << "Sample " << camera.height << std::endl;
	}

private:
	void run() override;
	IFinishedTask* onDone;

	int x = 0;
	int y = 0;

	int id = 0;

	const Hittable& world;
	CameraProperties camera;

	Color Ray_Color(const Ray& r, int depth);
	Ray Get_Ray(int i, int j);

	Vec3 SampleSquare() const {
		return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
	}

	Point3 defocus_disk_sample() const {
		auto p = random_in_unit_disk();
		return camera.center +
			(p.x() * camera.defocus_disk_u) + (p.y() * camera.defocus_disk_v);
	}
};