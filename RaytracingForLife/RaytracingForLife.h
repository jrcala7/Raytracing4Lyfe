// RaytracingForLife.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include <opencv2/opencv.hpp>
#include "Utils.h"

#include "rt_src/RT_Weekend.h"
#include "rt_src/Hittable.h"
#include "rt_src/Hittable_List.h"
#include "rt_src/Sphere.h"
#include "rt_src/Camera.h"

void PrintTestImage() {
	int width = 256;
	int height = 256;

	vector<PixelColor> image;
	image.resize(width * height);

	cout << "Printing Test Image" << endl;

	for (int j = 0; j < height; j++) {
		clog << "\rScanlines remaining " << height - j << ' ' << flush;
		for (int i = 0; i < width; i++) {
			auto pColor = Color(
				double(i) / (width - 1),
				double(j) / (height - 1),
				0
			);

			PixelColor p(pColor);
			image[j * width + i] = p;
		}
	}

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, width, height, "NewTest.png");
}
