#pragma once

#include "Hittable.h"
#include "Vec3.h"

class Triangle : public Hittable {

	struct UVCoord {
		float u=0;
		float v=0;
	};


public:
	Triangle(Point3 _v0, Point3 _v1, Point3 _v2, shared_ptr<Material> _mat) :
		p0(_v0), p1(_v1), p2(_v2), mat(_mat) {

		SetBBox();
	};

	void SetUV(float _u0, float _v0,
				float _u1, float _v1,
				float _u2, float _v2) {

		uv0.u = _u0;
		uv0.v = _v0;

		uv1.u = _u1;
		uv1.v = _v1;

		uv2.u = _u2;
		uv2.v = _v2;
	}

	bool Hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override {
	
		Vec3 e1 = p1 - p0;
		Vec3 e2 = p2 - p0;

		Vec3 h = cross(r.direction(), e2);
		auto a = dot(e1, h);

		//Ray is parrallel
		if (a > -1e-8 && a < 1e-8)
			return false;

		double f = 1.0 / a;
		auto s = r.origin() - p0;
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

			float w = 1.0 - u - v;

			rec.u =
				(w * uv0.u) + 
				(u * uv1.u) + 
				(v * uv2.u);

			rec.v = 
				(w * uv0.v) +
				(u * uv1.v) +
				(v * uv2.v);

			return true;
		}

		return false;
	
	}

	AABB bounding_box() const override {
		return bbox;
	}

private:

	void SetBBox() {
		double minX = fmin(
			fmin(p0.x(), p1.x()), p2.x()
		);
		double minY = fmin(
			fmin(p0.y(), p1.y()), p2.y()
		);
		double minZ = fmin(
			fmin(p0.z(), p1.z()), p2.z()
		);

		double maxX = fmax(
			fmax(p0.x(), p1.x()), p2.x()
		);
		double maxY = fmax(
			fmax(p0.y(), p1.y()), p2.y()
		);
		double maxZ = fmax(
			fmax(p0.z(), p1.z()), p2.z()
		);

		bbox = AABB(
			Point3(minX, minY, minZ),
			Point3(maxX, maxY, maxZ)
		);
	}

	AABB bbox;
	
	Point3 p0, p1, p2;
	UVCoord uv0, uv1, uv2;

	shared_ptr<Material> mat;

};