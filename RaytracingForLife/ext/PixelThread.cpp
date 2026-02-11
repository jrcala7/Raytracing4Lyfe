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
	return Ray_ColorFunc(r, depth, world, camera.background);
}

Ray PixelThread::Get_Ray(int i, int j)
{
	return GetRayFunc(
		i, j,
		camera.pixel00_loc,
		camera.pixel_delta_u, camera.pixel_delta_v,
		camera.defocus_angle,
		camera.center,
		camera.defocus_disk_u, camera.defocus_disk_v
	);
}
