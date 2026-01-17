#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "rt_src/Color.h"

using namespace std;

class PixelColor {
	public:
		double r;
		double g;
		double b;

		PixelColor() : r(0), g(0), b(0) {};
		PixelColor(double _r, double _g, double _b) : r(_r), g(_g), b(_b) {};
		PixelColor(Vec3 v) : r(v.x()), g(v.y()), b(v.z()) {};
};

/// <summary>
/// Prints sample PPM image
/// </summary>
/// <param name="image"></param>
static void PrintSampleImage(vector<PixelColor>& image) {
	int imgWidth = 256;
	int imgHeight = 256;

	image.resize(imgWidth * imgHeight);

	cout << "Image Width: " << imgWidth << ", Image Height: " << imgHeight << endl;

	ofstream outFile("output.ppm");
	if (!outFile || !outFile.is_open()) {
		cerr << "Error creating file!" << endl;
		return;
	}

	outFile << "P3\n" << imgWidth << ' ' << imgHeight << "\n255\n";

	for (int j = 0; j < imgHeight; j++) {
		for (int i = 0; i < imgWidth; i++) {
			auto r = double(i) / (imgWidth - 1);
			auto g = double(j) / (imgHeight - 1);
			auto b = 0.0;

			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);

			outFile << ir << ' ' << ig << ' ' << ib << '\n';
			PixelColor pixel = { r, g, b };
			image[j * imgWidth + i] = pixel;
		}
	}

	outFile.close();
	cout << "Image file 'output.ppm' created successfully." << endl;
}

/// <summary>
/// Converts from Pixel RGB to OpenCV BGR
/// </summary>
/// <param name="image"></param>
/// <returns></returns>
static vector<double> ConvertToDoubleVector(const vector<PixelColor> image) {
	vector<double> doubleImage;
	doubleImage.reserve(image.size() * 3); // 3 channels per pixel
	for (const auto& pixel : image) {
		//OpenCV uses BGR order
		doubleImage.push_back(pixel.b);
		doubleImage.push_back(pixel.g);
		doubleImage.push_back(pixel.r);

	}
	return doubleImage;
}

/// <summary>
/// Saves image from BGR vector format to png
/// </summary>
/// <param name="image"></param>
/// <param name="imgWidth"></param>
/// <param name="imgHeight"></param>
/// <param name="filename"></param>
static void SaveImage(const vector<double>& image, int imgWidth, int imgHeight, const string& filename) {
	// 1. Wrap the vector into a 3-channel double-precision Mat
	// Ensure the vector size is exactly height * width * 3
	cv::Mat doubleMat(imgHeight, imgWidth, CV_64FC3, (void*)image.data());

	// 2. Convert to 8-bit (standard PNG)
	// If your doubles are in range [0, 1], scale by 255.
	// If they are already [0, 255], use a scale factor of 1.
	cv::Mat eightBitMat;
	doubleMat.convertTo(eightBitMat, CV_8UC3, 255.0);

	// 3. Save as PNG
	cv::imwrite(filename, eightBitMat);
}