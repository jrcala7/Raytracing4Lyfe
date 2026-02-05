#include "Camera.h"

//Camera Propertiesw
void Camera::Render(const Hittable& world, vector<PixelColor>& pixels) {
	Initialize();

	cout << "Rendering Image desu" << endl;
	pixels.resize(width * height);

	for (int j = 0; j < height; j++) {
		clog << "\rScanlines remaining " << height - j << ' ' << flush;
		for (int i = 0; i < width; i++) {
			Color pixel_color(0, 0, 0);

			for (int sample = 0; sample < samples_per_pixel; sample++) {
				Ray r = Get_Ray(i, j);
				pixel_color += Ray_Color(r, max_depth, world);
			}

			Color scale_color = pixel_samples_scale * pixel_color;

			PixelColor p(scale_color);
			pixels[j * width + i] = p;
		}
	}

	cout << "Done Rendering Image desu" << endl;
}

void Camera::Initialize() {
	height = int(width / aspectRatio);
	height = (height < 1) ? 1 : height;

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
}

Color Camera::Ray_Color(const Ray& r, int depth, const Hittable& world) const {

	if (depth <= 0) {
		return Color(0, 0, 0);
	}

	/*Old Implem
	Hit_Record rec;

	if (world.Hit(r, Interval(0.001, infinity), rec)) {
	Ray scattered;
	Color attenuation;

	if(rec.mat->Scatter(r, rec, attenuation, scattered)) {
	return attenuation *
	Ray_Color(
	scattered,
	depth - 1,
	world
	);
	}

	return Color(0, 0, 0);
	}

	Vec3 Unit_Dir = unit_vector(r.direction());
	auto a = 0.5 * (Unit_Dir.y() + 1.0);

	return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
	*/

	Hit_Record rec;
	//If ray hits nothing return BG
	if (!world.Hit(
		r, Interval(0.001, infinity), rec)
		) {
		return background;
	}

	Ray scattered;
	Color attenuation;

	Color emission = rec.mat->Emitted(rec.u, rec.v, rec.p);

	if (!rec.mat->Scatter(r, rec, attenuation, scattered))
		return emission;

	Color scatter_color = attenuation * Ray_Color(scattered, depth - 1, world);

	return emission + scatter_color;
}

Ray Camera::Get_Ray(int i, int j) const {
	auto offset = SampleSquare();

	auto pixelSample = pixel00_loc +
		((i + offset.x()) * pixel_delta_u) +
		((j + offset.y()) * pixel_delta_v);

	auto ray_origin = defocus_angle <= 0 ? center : defocus_disk_sample();
	auto ray_direction = pixelSample - ray_origin;
	auto ray_time = random_double();

	return Ray(ray_origin, ray_direction, ray_time);
}

Vec3 Camera::SampleSquare() const {
	return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
}

Point3 Camera::defocus_disk_sample() const {
	auto p = random_in_unit_disk();
	return center +
		(p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
}
