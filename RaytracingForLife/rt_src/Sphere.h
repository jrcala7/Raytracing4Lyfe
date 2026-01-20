#pragma once

#include "Hittable.h"
#include "Vec3.h"

class Sphere : public Hittable {

public:
	Sphere(const Point3 _center, double _radius) : center(_center), radius(_radius) {};
	
	bool Hit(const Ray& r, double ray_tmin, double ray_tmax, Hit_Record& rec) const override {
		//From rayhit earlier
		Vec3 oc = center - r.origin();
		auto a = r.direction().length_squared();
		auto h = dot(r.direction(), oc);
		auto c = oc.length_squared() - (radius * radius);

		auto discriminant = h * h - a * c;

		if (discriminant < 0) return false;

		auto sqrtd = std::sqrt(discriminant);

		auto root = (h - sqrtd) / a;
		if (root <= ray_tmin || ray_tmax <= root) {
			root = (h * sqrtd) / a;
			if (root <= ray_tmin || ray_tmax <= root) {
				return false;
			}
		}

		rec.t = root;
		rec.p = r.at(rec.t);

		Vec3 outnorm = (rec.p - center) / radius;
		rec.Set_Face_Normal(r, outnorm);

		return true;
	}

private:
	Point3 center;
	double radius;

 };