#pragma once

#include "Hittable.h"

class Quad : public Hittable {
public:
	Quad(const Point3& _q, const Vec3& _u, const Vec3& _v, shared_ptr<Material> _mat) :
		q(_q), u(_u), v(_v), mat(_mat)
	{
		auto n = cross(u, v);
		normal = unit_vector(n);
		d = dot(normal, q);
		w = n / dot(n, n);

		Set_Bounding_Box();
	}

	virtual void Set_Bounding_Box() {
		auto diagonal1 = AABB(q, q + u + v);
		auto diagonal2 = AABB(q + u, q + v);

		bbox = AABB(diagonal1, diagonal2);
	}

	bool Hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override {
		auto denom = dot(normal, r.direction());
		
		//If Parallel
		if (std::fabs(denom) < 1e-8) return false;
		
		//If t is outside ray interval
		auto t = (d - dot(normal, r.origin())) / denom;
		if (!ray_t.contains(t)) return false;

		auto intersection = r.at(t);
		Vec3 planar_hit_vec = intersection - q;
		auto alpha = dot(w, cross(planar_hit_vec, v));
		auto beta = dot(w, cross(u, planar_hit_vec));

		//If its within the planar shape
		if (!In_Interior(alpha, beta, rec)) return false;

		rec.t = t;
		rec.p = intersection;
		rec.mat = mat;
		rec.Set_Face_Normal(r, normal);

		return true;
	}

	AABB bounding_box() const override {
		return bbox;
	}

	virtual bool In_Interior(double a, double b, Hit_Record& rec) const {
		Interval unit_interval = Interval(0, 1);

		if (!unit_interval.contains(a) || !unit_interval.contains(b)) return false;

		rec.u = a;
		rec.v = b;
		return true;
	}

private:
	Point3 q;
	Vec3 u, v;
	Vec3 w;
	shared_ptr<Material> mat;
	AABB bbox;
	Vec3 normal;
	double d;
};