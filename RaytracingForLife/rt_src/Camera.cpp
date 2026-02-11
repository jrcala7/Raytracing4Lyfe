#include "Camera.h"

//Camera Propertiesw
void Camera::Render(const Hittable& world, vector<PixelColor>& pixels) {
	Initialize();

	cout << "Rendering Image desu" << endl;
	
	auto start = std::chrono::steady_clock::now();

	//RenderOld(world, pixels);

	RenderThreaded(world, pixels);

	//for (int j = 0; j < height; j++) {
	//	//clog << "\rScanlines remaining " << height - j << ' ' << flush;
	//	for (int i = 0; i < width; i++) {
	//		int index = j * width + i;
	//		pixels[index] = renderer.outputPixels[index];
	//	}
	//}
	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	cout << "Done Rendering Image desu " << duration.count() << "ms" << endl;
}

void Camera::Initialize() {
	height = int(width / aspectRatio);
	height = (height < 1) ? 1 : height;

	sqrt_spp = int(std::sqrt(samples_per_pixel));
	recip_sqrt_spp = 1.0 / sqrt_spp;

	pixel_samples_scale = 1.0 / samples_per_pixel;

	center = lookfrom;

	//Viewport dimensions
	//View port is righthanded
	auto theta = degrees_to_radians(vfov);
	auto h = tan(theta / 2);
	auto viewport_height = 2.0 * h * focus_dist;
	auto viewport_width = viewport_height * (double(width) / height);

	w = unit_vector(lookfrom - lookat);
	u = unit_vector(cross(vup, w));
	v = cross(w, u);

	//Calculate Horizontal and down viewport edge
	auto viewport_u = viewport_width * u;
	auto viewport_v = viewport_height * -v;

	//Calculate horizontal and vertical delta vecs
	pixel_delta_u = viewport_u / width;
	pixel_delta_v = viewport_v / height;

	//Calculate loc of upper left pixel
	auto viewport_upper_left = center
		- (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
	pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle) / 2);
	defocus_disk_u = u * defocus_radius;
	defocus_disk_v = v * defocus_radius;

	renderer.InitPool(height, width);
}

Color Camera::Ray_Color(const Ray& r, int depth, const Hittable& world) const {
	return Ray_ColorFunc(r, depth, world, background);
}

Ray Camera::Get_Ray(int i, int j, int si, int sj) const {
	return GetRayFunc(
		i, j,
		si, sj, recip_sqrt_spp,
		pixel00_loc,
		pixel_delta_u, pixel_delta_v,
		defocus_angle,
		center,
		defocus_disk_u, defocus_disk_v
	);
}

void Camera::RenderOld(const Hittable& world, vector<PixelColor>& pixels)
{
	pixels.resize(width * height);
	for (int j = 0; j < height; j++) {
		clog << "\rScanlines remaining " << height - j << ' ' << flush;
		for (int i = 0; i < width; i++) {
			Color pixel_color(0, 0, 0);

			PixelThread* t = new PixelThread(
				i, j, width,
				ToCameraProperties(),
				world
			);

			for (int sj = 0; sj < sqrt_spp; sj++) {
				for (int si = 0; si < sqrt_spp; si++) {
					Ray r = Get_Ray(i, j, si, sj);
					pixel_color += Ray_Color(r, max_depth, world);
				}
			}

			Color scale_color = pixel_samples_scale * pixel_color;

			PixelColor p(scale_color);
			pixels[j * width + i] = p;
		}
	}

}

void Camera::RenderThreaded(const Hittable& world, vector<PixelColor>& pixels)
{
	pixels.resize(width * height);

	for (int j = 0; j < height; j++) {
		Color pixel_color(0, 0, 0);

		PixelThread* t = new PixelThread(
			0, j, width,
			ToCameraProperties(),
			world
		);

		renderer.AddPixel(t);
	}

	renderer.StartRendering();

	renderer.Wait();

	std::shared_lock<std::shared_mutex> lock(renderer.currPixelMutex);
	pixels = renderer.outputPixels;
}
