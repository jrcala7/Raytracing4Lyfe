#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

//usings
using std::make_shared;
using std::shared_ptr;

//consts
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.141592653589793285;

inline double degrees_to_radians(double deg) {
	return deg * pi / 180.0;
}

#include "Color.h"
#include "Interval.h"
#include "Ray.h"
#include "Vec3.h"