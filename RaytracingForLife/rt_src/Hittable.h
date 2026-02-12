#pragma once

#include "RT_Weekend.h"
#include "Ray.h"	
#include "AABB.h"

class Material;

class Hit_Record {
public:
	Point3 p;
	Vec3 Normal;
	double t;
	double u, v;

	shared_ptr<Material> mat;

	bool front_face;
	void Set_Face_Normal(const Ray& r, const Vec3& out_normal) {
		// Set the hit record normal vector

		front_face = dot(r.direction(), out_normal) < 0;
		Normal = front_face ? out_normal : -out_normal; // <-- this is a unit vector
	}
};

class Hittable {
public:
	virtual ~Hittable() = default;
	virtual bool Hit(const Ray& r, Interval ray_t, Hit_Record& rec) const = 0;

	virtual AABB bounding_box() const = 0;

	virtual double PDF_Value(const Point3& origin, const Vec3& direction) const {
		return 0.0;
	}

	virtual Vec3 Random(const Point3& origin) const {
		return Vec3(1, 0, 0);
	}

};

class Translate : public Hittable {
public:
	Translate(shared_ptr<Hittable> obj, const Vec3& _offset) :
		object(obj), offset(_offset) {

		bbox = object->bounding_box() + offset;
	}

	bool Hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override {
		Ray offsetR(r.origin() - offset, r.direction(), r.time());

		if (!object->Hit(offsetR, ray_t, rec)) return false;

		rec.p += offset;
		
		return true;
	}

	AABB bounding_box() const override { return bbox; }

private:
	shared_ptr<Hittable> object;
	Vec3 offset;
	AABB bbox;
};

class Rot_Y : public Hittable {
public:
	Rot_Y(shared_ptr<Hittable> obj, double ang) : object(obj) {
		auto radians = degrees_to_radians(ang);

		sin_theta = sin(radians);
		cos_theta = cos(radians);
		bbox = object->bounding_box();

		Point3 min(infinity, infinity, infinity);
		Point3 max(-infinity, -infinity, -infinity);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
					auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
					auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

					auto newX = cos_theta * x + sin_theta * z;
					auto newZ = -sin_theta * x + cos_theta * z;

					Vec3 tester(newX, y, newZ);

					for (int c = 0; c < 3; c++) {
						min[c] = fmin(min[c], tester[c]);
						max[c] = fmax(max[c], tester[c]);
					}
				}
			}
		}

		bbox = AABB(min, max);
	}

	bool Hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override {
		auto origin = Point3(
			(cos_theta * r.origin().x()) - (sin_theta * r.origin().z()),
			r.origin().y(),
			(sin_theta * r.origin().x()) + (cos_theta * r.origin().z())
		);

		auto dir = Point3(
			(cos_theta * r.direction().x()) - (sin_theta * r.direction().z()),
			r.direction().y(),
			(sin_theta * r.direction().x()) + (cos_theta * r.direction().z())
		);

		Ray rotated_r(origin, dir, r.time());

		if (!object->Hit(rotated_r, ray_t, rec)) return false;

		rec.p = Point3(
			(cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
			rec.p.y(),
			(-sin_theta * rec.p.x()) + (cos_theta * rec.p.z())
		);

		rec.Normal = Point3(
			(cos_theta * rec.Normal.x()) + (sin_theta * rec.Normal.z()),
			rec.Normal.y(),
			(-sin_theta * rec.Normal.x()) + (cos_theta * rec.Normal.z())
		);

		return true;
	}

	AABB bounding_box() const override { return bbox; }

private:
	shared_ptr<Hittable> object;
	double sin_theta;
	double cos_theta;
	AABB bbox;
};