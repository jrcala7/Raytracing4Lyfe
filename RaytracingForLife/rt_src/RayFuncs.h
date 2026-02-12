#pragma once

#include "Ray.h"
#include "PDF.h"
#include "Color.h"
#include "Material.h"
#include "Hittable.h"
#include "../Utils.h"

/// <summary>
/// Separated the ray functions so I can parallelize these ez later
/// </summary>

inline Vec3 Sample_Square_Strat(int s_i, int s_j, double recip_sqrt) {
	auto x = ((s_i + random_double()) * recip_sqrt) - 0.5;
	auto y = ((s_j + random_double()) * recip_sqrt) - 0.5;

	return Vec3(x, y, 0);
}

inline Vec3 SampleSquareFunc() {
	return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
}

inline Point3 defocus_disk_sampleFunc(Vec3 defocus_disk_u, Vec3 defocus_disk_v, Point3 center) {
	auto p = random_in_unit_disk();
	return center +
		(p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
}

inline Ray GetRayFunc(int i, int j,
	int s_i, int s_j, double recip_sqrt,
	Point3 pixel00_loc,
	Vec3 pixel_delta_u, Vec3 pixel_delta_v,
	double defocus_angle,
	Point3 center,
	Vec3 defocus_disk_u, Vec3 defocus_disk_v) {

	auto offset = Sample_Square_Strat(s_i, s_j, recip_sqrt);

	auto pixelSample = pixel00_loc +
		((i + offset.x()) * pixel_delta_u) +
		((j + offset.y()) * pixel_delta_v);

	auto ray_origin = defocus_angle <= 0 ? center : defocus_disk_sampleFunc(defocus_disk_u, defocus_disk_v, center);
	auto ray_direction = pixelSample - ray_origin;
	auto ray_time = random_double();

	return Ray(ray_origin, ray_direction, ray_time);

}

inline Color Ray_ColorFunc(const Ray& r, int depth, const Hittable& world,
	const Hittable& lights, 
	Color background) {
	if (depth <= 0) {
		return Color(0, 0, 0);
	}

	Hit_Record rec;
	//If ray hits nothing return BG
	if (!world.Hit(
		r, Interval(0.001, infinity), rec)
		) {
		return background;
	}

	Ray scattered;
	Color attenuation;
	double pdf_val;

	Color emission = rec.mat->Emitted(r, rec, rec.u, rec.v, rec.p);

	if (!rec.mat->Scatter(r, rec, attenuation, scattered, pdf_val))
		return emission;

	Hittable_PDF lightPDF(lights, rec.p);

	scattered = Ray(rec.p, lightPDF.Generate(), r.time());
	pdf_val = lightPDF.value(scattered.direction());

	double scatter_pdf = rec.mat->Scattering_PDF(r, rec, scattered);
	
	Color sample_color = Ray_ColorFunc(scattered, depth - 1, world, lights, background);
	Color scatter_color = (attenuation * scatter_pdf * sample_color) / pdf_val;

	return emission + scatter_color;
}