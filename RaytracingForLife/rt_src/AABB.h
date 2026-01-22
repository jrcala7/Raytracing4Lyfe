#pragma once

#include "Ray.h"
#include "Vec3.h"
#include "Interval.h"

class AABB {

public:
	Interval x, y, z;

	AABB() {};

	AABB(const Interval& _x, const Interval& _y, const  Interval& _z) : x(_x), y(_y), z(_z) {}

	AABB(const Point3& a, const Point3& b) {
		x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
		y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
		z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
	}

	AABB(const AABB& a, const AABB& b) {
		x = Interval(a.x, b.x);
		y = Interval(a.y, b.y);
		z = Interval(a.z, b.z);
	}

	const Interval& axis_interval(int n) const {
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	bool hit(const Ray& r, Interval ray_t) const {
		const Point3& orig = r.origin();
		const Point3& dir = r.direction();

		for (int axis = 0; axis < 3; axis++) {
		
			const Interval& ax = axis_interval(axis);
			const double adinv = 1.0 / dir[axis];

			auto t0 = (ax.min - orig[axis]) * adinv;
			auto t1 = (ax.max - orig[axis]) * adinv;

			if (t0 < t1) {
				if (t0 > ray_t.min) ray_t.min = t0;
				if (t1 < ray_t.max) ray_t.max = t1;
			}
			else {
				if (t1 > ray_t.min) ray_t.min = t1;
				if (t0 < ray_t.max) ray_t.max = t0;
			}

			if (ray_t.max <= ray_t.min) {
				return false;
			}
		}

		return true;
	}

	int longest_axis() const {
		if (x.size() > y.size()) {
			return x.size() > z.size() ? 0 : 2;
		}
		else {
			return y.size() > z.size() ? 1 : 2;
		}
	}

	static const AABB empty, universe;
};

const AABB AABB::empty = AABB(
	Interval::empty,
	Interval::empty,
	Interval::empty
);

const AABB AABB::universe = AABB(
	Interval::universe,
	Interval::universe,
	Interval::universe
);
