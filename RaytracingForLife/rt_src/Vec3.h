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

		bool near_zero() const {
		
			auto s = 1e-8;
			return (fabs(e[0]) < s) && 
				(fabs(e[1]) < s) && 
				(fabs(e[2]) < s);
		
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

inline Vec3 random_in_unit_disk() {
	while (true) {
		auto p = Vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.length_squared() < 1) {
			return p;
		}
	}
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

inline Vec3 reflect(const Vec3& v, const Vec3& n) {
	return v - 2 * dot(v, n) * n;
}

inline Vec3 refract(
	const Vec3& uv,
	const Vec3& n,
	double etai_over_etat
){
	auto cos_theta = fmin(dot(-uv, n), 1.0);
	Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vec3 r_out_parallel = -sqrt(fabs(
		1.0 - r_out_perp.length_squared()
	)) * n;

	return r_out_perp + r_out_parallel;
}