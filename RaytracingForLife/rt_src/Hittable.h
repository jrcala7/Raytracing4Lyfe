#pragma once

#include "RT_Weekend.h"
#include "Ray.h"	

class Material;

class Hit_Record {
public:
	Point3 p;
	Vec3 Normal;
	double t;

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
};