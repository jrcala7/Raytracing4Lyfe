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
		
		scattered = Ray(rec.p, scatter_dir);
		attenuation = albedo;
		
		return true;
	}

private:
	Color albedo;

};

class Metal : public Material {

public:
	Metal(const Color& _albedo) : albedo(_albedo) {};

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

		scattered = Ray(rec.p, refl);
		attenuation = albedo;

		return true;
	}

private:
	Color albedo;

};