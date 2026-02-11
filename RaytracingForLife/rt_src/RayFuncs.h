#pragma once

#include "Ray.h"
#include "Color.h"
#include "Material.h"
#include "Hittable.h"
#include "../Utils.h"

/// <summary>
/// Separated the ray functions so I can parallelize these ez later
/// </summary>

inline Vec3 SampleSquareFunc() {
	return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
}

inline Point3 defocus_disk_sampleFunc(Vec3 defocus_disk_u, Vec3 defocus_disk_v, Point3 center) {
	auto p = random_in_unit_disk();
	return center +
		(p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
}

inline Ray GetRayFunc(int i, int j,
	Point3 pixel00_loc,
	Vec3 pixel_delta_u, Vec3 pixel_delta_v,
	double defocus_angle,
	Point3 center,
	Vec3 defocus_disk_u, Vec3 defocus_disk_v) {

	auto offset = SampleSquareFunc();

	auto pixelSample = pixel00_loc +
		((i + offset.x()) * pixel_delta_u) +
		((j + offset.y()) * pixel_delta_v);

	auto ray_origin = defocus_angle <= 0 ? center : defocus_disk_sampleFunc(defocus_disk_u, defocus_disk_v, center);
	auto ray_direction = pixelSample - ray_origin;
	auto ray_time = random_double();

	return Ray(ray_origin, ray_direction, ray_time);

}

inline Color Ray_ColorFunc(const Ray& r, int depth, const Hittable& world,
	Color background) {
	if (depth <= 0) {
		return Color(0, 0, 0);
	}

	/*Old Implem
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
	*/

	Hit_Record rec;
	//If ray hits nothing return BG
	if (!world.Hit(
		r, Interval(0.001, infinity), rec)
		) {
		return background;
	}

	Ray scattered;
	Color attenuation;

	Color emission = rec.mat->Emitted(rec.u, rec.v, rec.p);

	if (!rec.mat->Scatter(r, rec, attenuation, scattered))
		return emission;

	Color scatter_color = attenuation * Ray_ColorFunc(scattered, depth - 1, world, background);

	return emission + scatter_color;
}