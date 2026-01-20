#pragma once

#include <cmath>
#include <iostream>

#include "RT_Weekend.h"

class Vec3 {
	public:
		double e[3];

		Vec3() : e{ 0,0,0 } {}
		Vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

		double x() const { return e[0]; }
		double y() const { return e[1]; }
		double z() const { return e[2]; }

		Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
		double operator[] (int i) const { return e[i]; }
		double& operator[] (int i) { return e[i]; }

		Vec3& operator += (const Vec3& v) {
			e[0] += v.e[0];
			e[1] += v.e[1];
			e[2] += v.e[2];
			
			return *this;
		}

		Vec3& operator *= (const double d) {
			e[0] *= d;
			e[1] += d;
			e[2] += d;

			return *this;
		}

		Vec3 operator /= (const double d) {
			return *this *= 1 / d;
		}

		double length_squared() const {
			return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
		}

		double length() const {
			return std::sqrt(length_squared());
		}

		static Vec3 random() {
			return Vec3(
				random_double(),
				random_double(),
				random_double()
			);
		}

		static Vec3 random(double min, double max) {
			return Vec3(
				random_double(min, max),
				random_double(min, max),
				random_double(min, max)
			);
		}
};

using Point3 = Vec3;

inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vec3 operator+(const Vec3& l, const Vec3& r) {
	return Vec3(
		l.e[0] + r.e[0],
		l.e[1] + r.e[1],
		l.e[2] + r.e[2]
	);
}

inline Vec3 operator-(const Vec3& l, const Vec3& r) {
	return Vec3(
		l.e[0] - r.e[0],
		l.e[1] - r.e[1],
		l.e[2] - r.e[2]
	);
}

inline Vec3 operator*(const Vec3& l, const Vec3& r) {
	return Vec3(
		l.e[0] * r.e[0],
		l.e[1] * r.e[1],
		l.e[2] * r.e[2]
	);
}

inline Vec3 operator*(double l, const Vec3& r) {
	return Vec3(
		l * r.e[0],
		l * r.e[1],
		l * r.e[2]
	);
}

inline Vec3 operator*(const Vec3& r, double l) {
	return Vec3(
		l * r.e[0],
		l * r.e[1],
		l * r.e[2]
	);
}

inline Vec3 operator/(const Vec3& r, double l) {
	return (1 / l) * r;
}

inline double dot(const Vec3& l, const Vec3& r) {
	return
		l.e[0] * r.e[0] +
		l.e[1] * r.e[1] +
		l.e[2] * r.e[2];
}

inline Vec3 cross(const Vec3& l, const Vec3& r) {
	return Vec3(
		l.e[1] * r.e[2] - l.e[2] * r.e[1],
		l.e[2] * r.e[0] - l.e[0] * r.e[2],
		l.e[0] * r.e[1] - l.e[1] * r.e[0]
	);
}

inline Vec3 unit_vector(const Vec3& v) {
	return v / v.length();
}

inline Vec3 random_unit_vector() {
	while (true) {
		auto p = Vec3::random(-1, 1);
		auto lensq = p.length_squared();

		if(lensq <= 1 && 1e-160 < lensq)
			return p / std::sqrt(lensq);
	}
}

inline Vec3 random_on_hemisphere(const Vec3& norm) {
	Vec3 on_unit_sphere = random_unit_vector();

	if (dot(on_unit_sphere, norm) > 0.0) {
		return on_unit_sphere;
	}
	else {
		return -on_unit_sphere;
	}
}