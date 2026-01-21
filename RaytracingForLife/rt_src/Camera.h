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

	//Camera Propertiesw
	void Render(const Hittable& world, vector<PixelColor>& pixels) {
		Initialize();

		cout << "Rendering Image desu" << endl;
		pixels.resize(width * height);

		for (int j = 0; j < height; j++) {
			clog << "\rScanlines remaining " << height - j << ' ' << flush;
			for (int i = 0; i < width; i++) {
				Color pixel_color(0, 0, 0);
				
				for (int sample = 0; sample < samples_per_pixel; sample++) {
					Ray r = Get_Ray(i, j);
					pixel_color += Ray_Color(r, max_depth, world);
				}

				Color scale_color = pixel_samples_scale * pixel_color;

				PixelColor p(scale_color);
				pixels[j * width + i] = p;
			}
		}

		cout << "Done Rendering Image desu" << endl;
	};

private:
	Point3 center;
	Point3 pixel00_loc;
	double pixel_samples_scale;

	Vec3 pixel_delta_u;
	Vec3 pixel_delta_v;

	void Initialize() {
		height = int(width / aspectRatio);
		height = (height < 1) ? 1 : height;

		pixel_samples_scale = 1.0 / samples_per_pixel;

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

	Color Ray_Color(const Ray& r, int depth, const Hittable& world) const {
		
		if(depth <= 0) {
			return Color(0, 0, 0);
		}

		Hit_Record rec;

		if (world.Hit(r, Interval(0.001, infinity), rec)) {
			Ray scattered;
			Color attenuation;

			if(rec.mat->Scatter(r, rec, attenuation, scattered)) {
				return attenuation * 
					Ray_Color(
						scattered,
						depth - 1,
						world
					);
			}
			
			return Color(0, 0, 0);
		}

		Vec3 Unit_Dir = unit_vector(r.direction());
		auto a = 0.5 * (Unit_Dir.y() + 1.0);

		return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
	}

	Ray Get_Ray(int i, int j) const {
		auto offset = SampleSquare();

		auto pixelSample = pixel00_loc +
			((i + offset.x()) * pixel_delta_u) +
			((j + offset.y()) * pixel_delta_v);

		auto ray_origin = center;
		auto ray_direction = pixelSample - ray_origin;

		return Ray(ray_origin, ray_direction);
	}

	Vec3 SampleSquare() const {
		return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
	}
};