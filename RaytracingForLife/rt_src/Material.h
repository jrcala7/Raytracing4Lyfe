#pragma once

#include "Color.h"
#include "Hittable.h"
#include "ONB.h"
#include "Texture.h"

class Material {
public:
	virtual ~Material() = default;

	virtual bool Scatter(
		const Ray& r_in,
		const Hit_Record& rec,
		Color& attenuation,
		Ray& scattered,
		double& pdf
	) const {
		return false;
	}

	virtual Color Emitted(double u, double v, const Point3& p) const {
		return Color(0, 0, 0);
	}

	virtual double Scattering_PDF(const Ray& r_in, const Hit_Record& rec, const Ray& scattered) const{
		return 0;
	}
};

class Lambertian : public Material {

public :
	Lambertian(const Color& _albedo) : tex(make_shared<Solid_Color>(_albedo)) {};
	Lambertian(shared_ptr<Texture> _tex) : tex(_tex) {};

	bool Scatter(
		const Ray& r_in,
		const Hit_Record& rec,
		Color& attenuation,
		Ray& scattered,
		double& pdf
	) const override {
		ONB uvw = (rec.Normal);
		auto scatter_dir = uvw.Transform(random_cos_direction());
		
		if(scatter_dir.near_zero()) {
			scatter_dir = rec.Normal;
		}
		
		scattered = Ray(rec.p, scatter_dir, r_in.time());
		attenuation = tex->value(rec.u, rec.v, rec.p);
		pdf = dot(uvw.w(), scattered.direction() / pi);
		
		return true;
	}

	double Scattering_PDF(const Ray& r_in, const Hit_Record& rec, const Ray& scattered) const override {
		//auto cos_theta = dot(rec.Normal, unit_vector(scattered.direction()));
		//return cos_theta < 0 ? 0 : cos_theta / pi;

		return 1 / (2 * pi);
	}

private:
	shared_ptr<Texture> tex;

};

class Metal : public Material {

public:
	Metal(const Color& _albedo, double _fuzz) : 
		albedo(_albedo), fuzz(_fuzz < 1 ? _fuzz : 1) {};

	bool Scatter(
		const Ray& r_in,
		const Hit_Record& rec,
		Color& attenuation,
		Ray& scattered,
		double& pdf
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
		Ray& scattered,
		double& pdf
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

class DiffuseLight : public Material {
public:
	DiffuseLight(shared_ptr<Texture> _tex) : tex(_tex) {}

	DiffuseLight(const Color& emit) : tex(make_shared<Solid_Color>(emit)) {}

	Color Emitted(double u, double v, const Point3& p) const override {
		return tex->value(u, v, p);
	}

private:
	shared_ptr<Texture> tex;
};

class Isotropic : public Material {
public:
	Isotropic(const Color& albedo) : tex(make_shared<Solid_Color>(albedo)) {}
	Isotropic(shared_ptr<Texture> _tex) : tex(_tex) {}

	bool Scatter(
		const Ray& r_in,
		const Hit_Record& rec,
		Color& attenuation,
		Ray& scattered,
		double& pdf
	) const override {

		scattered = Ray(rec.p, random_unit_vector(), r_in.time());
		attenuation = tex->value(rec.u, rec.v, rec.p);
		pdf = 1 / (4 * pi);

		return true;
	}

	double Scattering_PDF(const Ray& r_in, const Hit_Record& rec, const Ray& scattered) const override {
		//auto cos_theta = dot(rec.Normal, unit_vector(scattered.direction()));
		//return cos_theta < 0 ? 0 : cos_theta / pi;

		return 1 / (4 * pi);
	}

private:
	shared_ptr<Texture> tex;
};