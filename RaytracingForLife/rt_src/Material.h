#pragma once

#include "Color.h"
#include "Hittable.h"

class Material {
public:
	virtual ~Material() = default;

	virtual bool Scatter(
		const Ray& r_in,
		const Hit_Record& rec,
		Color& attenuation,
		Ray& scattered
	) const {
		return false;
	}
};