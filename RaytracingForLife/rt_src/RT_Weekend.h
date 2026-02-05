#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <random>

//usings
using std::make_shared;
using std::shared_ptr;

//consts
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.141592653589793285;

inline double degrees_to_radians(double deg) {
	return deg * pi / 180.0;
}

inline double random_double() {
	//return std::rand() / (RAND_MAX + 1.0);
	static thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_real_distribution<double> distribution(0.0, 1.0);
	return distribution(generator);
}

inline double random_double(double min, double max) {
	return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
	return int(random_double(min, max + 1));
}

/*
#include "Color.h"
#include "Interval.h"
#include "Vec3.h"
#include "Ray.h"
*/