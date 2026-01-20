// RaytracingForLife.cpp : Defines the entry point for the application.
//

#include "RaytracingForLife.h"

using namespace std;

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
				double(i) / (width-1),
				double(j) / (height-1),
				0
			);

			PixelColor p(pColor);
			image[j * width + i] = p;
		}
	}

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, width, height, "NewTest.png");
}

double Hit_Sphere(const Point3& center, double radius, const Ray& r) {
	Vec3 oc = center - r.origin();
	auto a = r.direction().length_squared();
	auto h = dot(r.direction(), oc);
	auto c = oc.length_squared() - (radius * radius);

	auto discriminant = h * h - a * c;

	if (discriminant < 0) {
		return -1.0;
	}
	else {
		return (h - std::sqrt(discriminant)) / (a);
	}
}

Color Ray_Color(const Ray& r) {
	auto t = Hit_Sphere(Point3(0, 0, -1), 0.5, r);
	
	if (t > 0.0) {
		Vec3 N = unit_vector(r.at(t) - Vec3(0, 0, -1));
		return 0.5 * Color(N.x() + 1, N.y() + 1, N.z() + 1);
	}

	Vec3 Unit_Dir = unit_vector(r.direction());
	auto a = 0.5 * (Unit_Dir.y() + 1.0);

	return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

int main()
{
	//Image
	//Viewport is righthanded
	auto aspectRatio = 16.0 / 9.0;
	int width = 480;

	int height = int(width / aspectRatio);
	height = (height < 1) ? 1 : height;

	//Camera
	auto focal_len = 1.0;
	auto viewport_height = 2.0;
	auto viewport_width = viewport_height * (double(width) / height);
	auto camera_center = Point3(0, 0, 0);

	//Calculate Horizontal and down viewport edge
	auto viewport_u = Vec3(viewport_width, 0, 0);
	auto viewport_v = Vec3(0, -viewport_height, 0);

	//Calculate horizontal and vertical delta vecs
	auto pixel_delta_u = viewport_u / width;
	auto pixel_delta_v = viewport_v / height;

	//Calculate loc of upper left pixel
	auto viewport_upper_left = camera_center
		- Vec3(0, 0, focal_len) - viewport_u / 2 - viewport_v / 2;
	auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	cout << "Printing Output Image Desu" << endl;

	vector<PixelColor> image;
	image.resize(width * height);

	for (int j = 0; j < height; j++) {
		clog << "\rScanlines remaining " << height - j << ' ' << flush;
		for (int i = 0; i < width; i++) {
			
			auto pixelCenter = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
			auto ray_dir = pixelCenter - camera_center;
			
			Ray r(camera_center, ray_dir);
			Color pColor = Ray_Color(r);

			PixelColor p(pColor);
			image[j * width + i] = p;
		}
	}

	auto sdArr = ConvertToDoubleVector(image);

	SaveImage(sdArr, width, height, "NewTest2.png");

	cout << "Done Printing new test";

	//PrintSampleImage(image);

	//vector<double> doubleImage = ConvertToDoubleVector(image);
	//SaveImage(doubleImage, 256, 256, "saved_output.png");



	return 0;
}
