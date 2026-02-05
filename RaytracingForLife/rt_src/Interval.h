#pragma once

#include "RT_Weekend.h"

class Interval {
public:
	double min, max;

	Interval() : min(+infinity), max(-infinity) {}
	Interval(double _min, double _max) : min(_min), max(_max) {}
	Interval(const Interval& a, const Interval& b) {
	
		min = fmin(a.min, b.min);
		max = fmax(a.max, b.max);
	
	}

	double size() const {
		return max - min;
	}

	bool contains(double x) const {
		return min <= x && x <= max;
	}

	bool surrounds(double x) const {
		return min < x && x < max;
	}

	double clamp(double x) const {
		if (x < min) return min;
		if (x > max) return max;

		return x;
	}

	Interval expand(double delta) const {
		return Interval(min - delta, max + delta);
	}

	static const Interval empty, universe;
};

inline const Interval Interval::empty = Interval(infinity, -infinity);
inline const Interval Interval::universe = Interval(-infinity, infinity);

inline Interval operator+ (const Interval& v, double disp) {
	return Interval(
		v.min + disp, v.max + disp
	);
}

inline Interval operator+ (double disp, const Interval& v) {
	return Interval(
		v.min + disp, v.max + disp
	);
}