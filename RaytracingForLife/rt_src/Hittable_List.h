#pragma once

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
	}

	bool Hit(const Ray& ray, double ray_tmin, double ray_tmax, Hit_Record& rec) const override {

		Hit_Record temp;
		bool hit_anything = false;
		auto closest = ray_tmax;

		for (const auto& obj : objects) {
			if (obj->Hit(ray, ray_tmin, closest, temp)) {
				hit_anything = true;
				closest = temp.t;
				rec = temp;
			}
		}

		return hit_anything;
	}
};