#pragma once

#include "RTW_Image.h"
#include "Color.h"
#include "Perlin.h"

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

class ImageTexture : public Texture {
public:
	ImageTexture(const char* filename) : image(filename){}

	Color value(double u, double v, const Point3& p) const override {
		if (image.height() <= 0) return Color(0, 1, 1);

		u = Interval(0, 1).clamp(u);
		v = 1.0 - Interval(0, 1).clamp(v);

		auto i = int(u * image.width());
		auto j = int(v * image.height());
		auto pixel = image.pixel_data(i, j);

		auto color_scale = 1.0 / 255.0;
		return Color(color_scale * pixel[0],
						color_scale * pixel[1],
						color_scale * pixel[2]);
	}

private:
	RTW_Image image;
};

class NoiseTexture : public Texture {

public:
	NoiseTexture() : scale(1.0) {}
	NoiseTexture(double _scale) : scale(_scale){}

	Color value(double u, double v, const Point3& p) const override {
		return Color(1, 1, 1) * 0.5 * (1.0 + noise.Noise(scale * p));
	}

private:
	Perlin noise;
	double scale;

};