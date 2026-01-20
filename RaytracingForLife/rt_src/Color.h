#pragma once

#include "Vec3.h"
#include "Interval.h"
#include <iostream>

using Color = Vec3;

inline double linear_to_gamma(double lin) {
	if (lin > 0) {
		return std::sqrt(lin);
	}

	return 0.0;
}

void Write_Color(std::ostream& out, const Color& pixelColor) {
	auto r = pixelColor.x();
	auto g = pixelColor.y();
	auto b = pixelColor.z();

	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	static const Interval intensity(0.0, 0.999);
	int rbyte = int(255.999 * intensity.clamp(r));
	int gbyte = int(255.999 * intensity.clamp(g));
	int bbyte = int(255.999 * intensity.clamp(b));

	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}