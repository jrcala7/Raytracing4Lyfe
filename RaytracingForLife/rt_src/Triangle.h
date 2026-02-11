#pragma once

#include "Hittable.h"
#include "Vec3.h"

class Triangle : public Hittable {

public:
	Triangle(Point3 _v0, Point3 _v1, Point3 _v2, shared_ptr<Material> _mat) :
		v0(_v0), v1(_v1), v2(_v2), mat(_mat) {
	};

	bool Hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override {
	
		Vec3 e1 = v1 - v0;
		Vec3 e2 = v2 - v0;

		Vec3 h = cross(r.direction(), e2);
		auto a = dot(e1, h);

		//Ray is parrallel
		if (a > -1e-8 && a < 1e-8)
			return false;

		double f = 1.0 / a;
		auto s = r.origin() - v0;
		auto u = f * dot(s, h);

		if (u < 0.0 || u > 1.0) return false;

		Vec3 q = cross(s, e1);
		auto v = f * dot(r.direction(), q);

		if (v < 0.0 || u + v > 1.0) return false;

		auto t = f * dot(e2, q);

		if (ray_t.contains(t)) {
			rec.t = t;
			rec.p = r.at(t);
			Vec3 norm = unit_vector(cross(e1, e2));
			rec.Set_Face_Normal(r, norm);
			rec.mat = mat;

			return true;
		}

		return false;
	
	}

	AABB bounding_box() const override {
		return bbox;
	}

private:
	AABB bbox;
	Point3 v0, v1, v2;
	shared_ptr<Material> mat;

};