#pragma once

#include "RT_Weekend.h"

class Interval {
public:
	double min, max;

	Interval() : min(+infinity), max(-infinity) {}
	Interval(double _min, double _max) : min(_min), max(_max) {}

	double size() const {
		return max - min;
	}

	bool contains(double x) const {
		return min <= x && x <= max;
	}

	bool surrounds(double x) const {
		return min < x && x < max;
	}

	static const Interval empty, universe;
};

const Interval Interval::empty = Interval(infinity, -infinity);
const Interval Interval::universe = Interval(-infinity, infinity);