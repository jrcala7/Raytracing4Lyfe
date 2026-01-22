#pragma once

#include "Hittable.h"
#include "Vec3.h"

class Sphere : public Hittable {

public:
	Sphere(const Point3 _center, double _radius, shared_ptr<Material> _mat) : 
		center(_center, Vec3(0,0,0)), radius(fmax(0, _radius)), mat(_mat) {

		auto rvec = Vec3(radius, radius, radius);
		bbox = AABB(_center - rvec, _center + rvec);
	
	};

	Sphere(const Point3 _center, const Point3 _off_center, double _radius, shared_ptr<Material> _mat) :
		center(_center, _off_center - _center), radius(fmax(0, _radius)), mat(_mat) {

		auto rvec = Vec3(radius, radius, radius);

		AABB box0 = AABB(center.at(0) - rvec, center.at(0) + rvec);
		AABB box1 = AABB(center.at(1) - rvec, center.at(1) + rvec);

		//bbox = AABB(box0, box1);
	};
	
	bool Hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override {
		Point3 curr_center = center.at(r.time());
		//From rayhit earlier
		Vec3 oc = curr_center - r.origin();
		auto a = r.direction().length_squared();
		auto h = dot(r.direction(), oc);
		auto c = oc.length_squared() - (radius * radius);

		auto discriminant = h * h - a * c;

		if (discriminant < 0) return false;

		auto sqrtd = std::sqrt(discriminant);

		auto root = (h - sqrtd) / a;
		if (!ray_t.surrounds(root)) {
			root = (h + sqrtd) / a;
			if (!ray_t.surrounds(root)) {
				return false;
			}
		}

		rec.t = root;
		rec.p = r.at(rec.t);

		Vec3 outnorm = (rec.p - curr_center) / radius;
		rec.Set_Face_Normal(r, outnorm);
		rec.mat = mat;

		return true;
	}

	AABB bounding_box() const override {
		return bbox;
	}

private:
	Ray center;
	double radius;
	shared_ptr <Material> mat;
	AABB bbox;

 };