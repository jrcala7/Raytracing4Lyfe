#pragma once

#include "Vec3.h"
#include "Interval.h"
#include <iostream>

using Color = Vec3;

void Write_Color(std::ostream& out, const Color& pixelColor) {
	auto r = pixelColor.x();
	auto g = pixelColor.y();
	auto b = pixelColor.z();

	static const Interval intensity(0.0, 0.999);
	int rbyte = int(255.999 * intensity.clamp(r));
	int gbyte = int(255.999 * intensity.clamp(g));
	int bbyte = int(255.999 * intensity.clamp(b));

	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}