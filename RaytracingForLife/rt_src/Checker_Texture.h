#pragma once

#include "Texture.h"

class Checker_Texture : public Texture {

public:
	Checker_Texture(double scale, shared_ptr<Texture> _even, shared_ptr<Texture> _odd) :
		inv_scale(1.0 / scale),
		even(_even),
		odd(_odd) {};

	Checker_Texture(double scale, const Color& c1, const Color& c2) :
		Checker_Texture(scale, make_shared<Solid_Color>(c1), make_shared<Solid_Color>(c2)) {};

	Color value(double u, double v, const Point3& p) const override {
		auto x = int(floor(inv_scale * p.x()));
		auto y = int(floor(inv_scale * p.y()));
		auto z = int(floor(inv_scale * p.z()));

		bool isEven = (x + y + z) % 2 == 0;

		return isEven ? even->value(u, v, p) : odd->value(u, v, p);
	}

private:
	double inv_scale;
	shared_ptr<Texture> even;
	shared_ptr<Texture> odd;

};