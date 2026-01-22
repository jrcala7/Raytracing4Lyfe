#pragma once

#include "Color.h"
#include "Hittable.h"

class Material {
public:
	virtual ~Material() = default;

	virtual bool Scatter(
		const Ray& r_in,
		const Hit_Record& rec,
		Color& attenuation,
		Ray& scattered
	) const {
		return false;
	}
};

class Lambertian : public Material {

public :
	Lambertian(const Color& _albedo) : albedo(_albedo) {};

	bool Scatter(
		const Ray& r_in,
		const Hit_Record& rec,
		Color& attenuation,
		Ray& scattered
	) const override {
		
		auto scatter_dir = rec.Normal + random_unit_vector();
		
		if(scatter_dir.near_zero()) {
			scatter_dir = rec.Normal;
		}
		
		scattered = Ray(rec.p, scatter_dir, r_in.time());
		attenuation = albedo;
		
		return true;
	}

private:
	Color albedo;

};

class Metal : public Material {

public:
	Metal(const Color& _albedo, double _fuzz) : 
		albedo(_albedo), fuzz(_fuzz < 1 ? _fuzz : 1) {};

	bool Scatter(
		const Ray& r_in,
		const Hit_Record& rec,
		Color& attenuation,
		Ray& scattered
	) const override {

		Vec3 refl = reflect(
			r_in.direction(),
			rec.Normal
		);

		refl = unit_vector(refl) + 
			(fuzz * random_unit_vector());

		scattered = Ray(rec.p, refl, r_in.time());
		attenuation = albedo;

		return (dot(scattered.direction(), rec.Normal) > 0);
	}

private:
	Color albedo;
	//rand reflection
	double fuzz;
};

class Dielectric : public Material {

public:
	Dielectric(double _refraction_index) : 
		refraction_index(_refraction_index) {};

	bool Scatter(
		const Ray& r_in,
		const Hit_Record& rec,
		Color& attenuation,
		Ray& scattered
	) const override {

		attenuation = Color(1.0, 1.0, 1.0);
		double ri = rec.front_face ?
			(1.0 / refraction_index) : refraction_index;

		Vec3 unit_dir = unit_vector(r_in.direction());

		double cos_theta = fmin(
			dot(-unit_dir, rec.Normal), 1.0
		);

		double sin_thta = sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = ri * sin_thta > 1.0;
		Vec3 refracted;

		if (cannot_refract || 
			reflectance(cos_theta, ri) > random_double()) {
			refracted = reflect(unit_dir, rec.Normal);
		}
		else {
			refracted = refract(unit_dir, rec.Normal, ri);
		}

		scattered = Ray(rec.p, refracted, r_in.time());
		return true;
	}

private:
	//Refractive index in a vacuum
	double refraction_index;

	static double reflectance(double cos, double ref_idx) {
		//Shlicks approximation
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;

		return r0 + (1 - r0) * pow((1 - cos), 5);
	}

};