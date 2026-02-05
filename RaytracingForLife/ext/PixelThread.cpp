#include "PixelThread.h"

void PixelThread::run()
{
	//IETThread::sleep(10);

	for (int i = 0; i < width; i++) {
		Color pixel_color(1.0, 0.0, 1.0);
		for (int sample = 0; sample < camera.samples_per_pixel; sample++) {
			Ray r = Get_Ray(i, y);
			pixel_color += Ray_Color(r, camera.max_depth);
		}

		Color scale_color = camera.pixel_samples_scale * pixel_color;
		scanline[i] = PixelColor( scale_color );
	}

	if (onDone != nullptr)
	{
		onDone->OnFinishedTask(y, scanline, this);
	}
}

Color PixelThread::Ray_Color(const Ray& r, int depth)
{
	if (depth <= 0) {
		return Color(0, 0, 0);
	}

	Hit_Record rec;
	//If ray hits nothing return BG
	if (!world.Hit(
		r, Interval(0.001, infinity), rec)
		) {
		return camera.background;
	}

	Ray scattered;
	Color attenuation;

	Color emission = rec.mat->Emitted(rec.u, rec.v, rec.p);

	if (!rec.mat->Scatter(r, rec, attenuation, scattered))
		return emission;

	Color scatter_color = attenuation * Ray_Color(scattered, depth - 1);

	return emission + scatter_color;
}

Ray PixelThread::Get_Ray(int i, int j)
{
	auto offset = SampleSquare();

	auto pixelSample = camera.pixel00_loc +
		((i + offset.x()) * camera.pixel_delta_u) +
		((j + offset.y()) * camera.pixel_delta_v);

	auto ray_origin = camera.defocus_angle <= 0 ? camera.center : defocus_disk_sample();
	auto ray_direction = pixelSample - ray_origin;
	auto ray_time = random_double();

	return Ray(ray_origin, ray_direction, ray_time);
}
