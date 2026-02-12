#pragma once
#include "Hittable_List.h"
#include "ONB.h"

class PDF {
public:
	virtual ~PDF() {

	}

	virtual double value(const Vec3& dir) const = 0;
	virtual Vec3 Generate() const = 0;
};

class Sphere_PDF : public PDF {

public:
	Sphere_PDF() {

	}

	double value(const Vec3& dir) const override {
		return 1 / (4 * pi);
	}

	Vec3 Generate() const override {
		return random_unit_vector();
	}

};

class Cos_PDF : public PDF {

public:
	Cos_PDF(const Vec3& w) : uvw(w){}

	double value(const Vec3& dir) const override {
		auto cos_theta = dot(unit_vector(dir), uvw.w());
		return std::fmax(0, cos_theta / pi);
	}

	Vec3 Generate() const override {
		return uvw.Transform(random_cos_direction());
	}

private:
	ONB uvw;

};

class Hittable_PDF : public PDF {

public:
	Hittable_PDF(const Hittable& objs, const Point3& _origin) :
		objects(objs), origin(_origin) { }
	
	double value(const Vec3& dir) const override {
		return objects.PDF_Value(origin, dir);
	}

	Vec3 Generate() const override {
		return objects.Random(origin);
	}

private:
	const Hittable& objects;
	Point3 origin;

};