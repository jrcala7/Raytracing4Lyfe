#pragma once

#include "Color.h"

class Texture {
public:
	virtual ~Texture() = default;

	virtual Color value(double u, double v, const Point3& p) const = 0;
};

class Solid_Color : public Texture {

public:
	Solid_Color(const Color& _albedo) : albedo(_albedo) {};

	Solid_Color(double red, double green, double blue) : Solid_Color(Color(red, green, blue)) {};

	Color value(double u, double v, const Point3& p) const override {
		return albedo;
	}

private:
	Color albedo;
};