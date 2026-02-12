#pragma once

#include "Hittable.h"
#include "Hittable_List.h"

class Quad : public Hittable {
public:
	Quad(const Point3& _q, const Vec3& _u, const Vec3& _v, shared_ptr<Material> _mat) :
		q(_q), u(_u), v(_v), mat(_mat)
	{
		auto n = cross(u, v);
		normal = unit_vector(n);
		d = dot(normal, q);
		w = n / dot(n, n);

		area = n.length();

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

	double PDF_Value(const Point3& origin, const Vec3& direction) const override {
		Hit_Record rec;

		if (!this->Hit(Ray(origin, direction),
			Interval(0.001, infinity),
			rec)) {
			return 0;
		}

		auto dist_sq = rec.t * rec.t * direction.length_squared();
		auto cosine = std::fabs(dot(direction, rec.Normal) / direction.length());

		return dist_sq / (cosine * area);
	}

	Vec3 Random(const Point3& origin) const override {
		auto p = q + (random_double() * u) + (random_double() * v);
		return p - origin;
	}

private:
	Point3 q;
	Vec3 u, v;
	Vec3 w;
	shared_ptr<Material> mat;
	AABB bbox;
	Vec3 normal;
	double d;
	double area;
};

inline shared_ptr<Hittable_List> Box(const Point3& a, const Point3& b, shared_ptr<Material> mat) {
	auto sides = make_shared<Hittable_List>();

	auto min = Point3(
		std::fmin(a.x(), b.x()),
		std::fmin(a.y(), b.y()),
		std::fmin(a.z(), b.z())
	);

	auto max = Point3(
		std::fmax(a.x(), b.x()),
		std::fmax(a.y(), b.y()),
		std::fmax(a.z(), b.z())
	);

	auto dx = Vec3(max.x() - min.x(), 0, 0);
	auto dy = Vec3( 0, max.y() - min.y(), 0);
	auto dz = Vec3( 0, 0, max.z() - min.z());

	//F
	sides->Add(
		make_shared<Quad>(
			Point3(min.x(), min.y(), max.z()), dx, dy, mat
		)
	);
	//R
	sides->Add(
		make_shared<Quad>(
			Point3(max.x(), min.y(), max.z()), -dz, dy, mat
		)
	);
	//B
	sides->Add(
		make_shared<Quad>(
			Point3(max.x(), min.y(), min.z()), -dx, dy, mat
		)
	);
	//L
	sides->Add(
		make_shared<Quad>(
			Point3(min.x(), min.y(), min.z()), dz, dy, mat
		)
	);
	//T
	sides->Add(
		make_shared<Quad>(
			Point3(min.x(), max.y(), max.z()), dx, -dz, mat
		)
	);
	//Bot
	sides->Add(
		make_shared<Quad>(
			Point3(min.x(), min.y(), min.z()), dx, dz, mat
		)
	);

	return sides;
}