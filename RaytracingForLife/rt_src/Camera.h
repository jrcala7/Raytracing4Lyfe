#pragma once

#include "Hittable.h"
#include "../Utils.h"
#include <vector>


using namespace std;

class Camera {

public:
	double aspectRatio = 16.0 / 9.0;
	int width = 480;
	int height = 0;

	//Camera Propertiesw
	void Render(const Hittable& world, vector<PixelColor>& pixels) {
		Initialize();

		cout << "Rendering Image desu" << endl;
		pixels.resize(width * height);

		for (int j = 0; j < height; j++) {
			clog << "\rScanlines remaining " << height - j << ' ' << flush;
			for (int i = 0; i < width; i++) {

				auto pixelCenter = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
				auto ray_dir = pixelCenter - center;

				Ray r(center, ray_dir);
				Color pColor = Ray_Color(r, world);

				PixelColor p(pColor);
				pixels[j * width + i] = p;
			}
		}

		cout << "Done Rendering Image desu" << endl;
	};

private:
	Point3 center;
	Point3 pixel00_loc;

	Vec3 pixel_delta_u;
	Vec3 pixel_delta_v;

	void Initialize() {
		height = int(width / aspectRatio);
		height = (height < 1) ? 1 : height;

		center = Point3(0, 0, 0);

		//Viewport dimensions
		//View port is righthanded
		auto focal_len = 1.0;
		auto viewport_height = 2.0;
		auto viewport_width = viewport_height * (double(width) / height);
	
		//Calculate Horizontal and down viewport edge
		auto viewport_u = Vec3(viewport_width, 0, 0);
		auto viewport_v = Vec3(0, -viewport_height, 0);

		//Calculate horizontal and vertical delta vecs
		pixel_delta_u = viewport_u / width;
		pixel_delta_v = viewport_v / height;

		//Calculate loc of upper left pixel
		auto viewport_upper_left = center
			- Vec3(0, 0, focal_len) - viewport_u / 2 - viewport_v / 2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
	}

	Color Ray_Color(const Ray& r, const Hittable& world) const {
		Hit_Record rec;

		if (world.Hit(r, Interval(0, infinity), rec)) {
			return 0.5 * (rec.Normal + Color(1, 1, 1));
		}

		Vec3 Unit_Dir = unit_vector(r.direction());
		auto a = 0.5 * (Unit_Dir.y() + 1.0);

		return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
	}
};