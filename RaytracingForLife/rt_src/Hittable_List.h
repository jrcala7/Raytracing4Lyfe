#pragma once

#include "AABB.h"
#include "Hittable.h"

#include <memory>
#include <vector>

using namespace std;

class Hittable_List : public Hittable {

public:
	vector< shared_ptr<Hittable> > objects;

	Hittable_List() {};
	Hittable_List(shared_ptr<Hittable> obj) { Add(obj); }

	void Clear() { objects.clear(); }

	void Add(shared_ptr<Hittable> obj) {
		objects.push_back(obj);
		bbox = AABB(bbox, obj->bounding_box());
	}

	bool Hit(const Ray& ray, Interval ray_t, Hit_Record& rec) const override {

		Hit_Record temp;
		bool hit_anything = false;
		auto closest = ray_t.max;

		for (const auto& obj : objects) {
			if (obj->Hit(ray, Interval(ray_t.min, closest), temp)) {
				hit_anything = true;
				closest = temp.t;
				rec = temp;
			}
		}

		return hit_anything;
	}

	AABB bounding_box() const override {
		return bbox;
	}

	double PDF_Value(const Point3& origin, const Vec3& direction) const override {
		auto weight = 1.0 / objects.size();
		auto sum = 0.0;

		for (const auto& object : objects)
			sum += weight * object->PDF_Value(origin, direction);

		return sum;
	}

	Vec3 Random(const Point3& origin) const override {
		auto int_size = int(objects.size());
		return objects[random_int(0, int_size - 1)]->Random(origin);
	}


private:
	AABB bbox;
};