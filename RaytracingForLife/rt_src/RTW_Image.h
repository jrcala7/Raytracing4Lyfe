#pragma once

#include <cstdlib>
#include <iostream>
#include "../stb_image.h"

class RTW_Image {
public:
	RTW_Image() {}

	RTW_Image(const char* image_filename) {
		auto filename = std::string(image_filename);
		auto imagedir = getenv("RTW_IMAGES");

		if (imagedir && Load(std::string(imagedir) + "/" + image_filename)) return;
		if (Load(filename)) return;

		std::string prefix = "";
		int fileLevel = 6;

		for (int i = 0; i < fileLevel; i++) {
			auto pathToLoad = prefix + "images/" + filename;

			std::cout << "Loading image from " << pathToLoad << std::endl;
			if (Load(pathToLoad)) return;

			prefix = prefix + "../";
		}
		
		std::cerr << "ERROR LOADING IMAGE " << image_filename << std::endl;
	}

	~RTW_Image() {
		delete[] bdata;
		stbi_image_free(fdata);
	}

	bool Load(const std::string& filename) {
		auto n = bytes_per_pixel;
		fdata = stbi_loadf(
			filename.c_str(),
			&image_width,
			&image_hieght,
			&n,
			bytes_per_pixel
		);

		if (fdata == nullptr) return false;

		byte_per_scanline = image_width * bytes_per_pixel;
		convert_to_bytes();
		
		return true;
	}

	int width() const { return (fdata == nullptr) ? 0 : image_width; }
	int height() const { return (fdata == nullptr) ? 0 : image_hieght; }

	const unsigned char* pixel_data(int x, int y) const {
		static unsigned char magenta[] = { 255, 0, 255 };
		if (bdata == nullptr) return magenta;

		x = clamp(x, 0, image_width);
		y = clamp(y, 0, image_hieght);

		return bdata + y * byte_per_scanline + x * bytes_per_pixel;
	}

private:
	const int bytes_per_pixel = 3;
	float* fdata = nullptr;
	unsigned char* bdata = nullptr;

	int image_width = 0;
	int image_hieght = 0;
	int byte_per_scanline = 0;

	static int clamp(int x, int low, int high) {
		if (x < low) return low;
		if (x < high) return x;
		return high - 1;
	}

	static unsigned char float_to_byte(float val) {
		if (val <= 0.0) return 0;
		if (1.0 <= val) return 255;

		return static_cast<unsigned char>(256.0 * val);
	}

	void convert_to_bytes() {
		int total_bytes = image_width * image_hieght * bytes_per_pixel;
		bdata = new unsigned char[total_bytes];

		auto *bptr = bdata;
		auto* fptr = fdata;

		for (auto i = 0; i < total_bytes; i++, fptr++, bptr++) {
			*bptr = float_to_byte(*fptr);
		}
	}
};