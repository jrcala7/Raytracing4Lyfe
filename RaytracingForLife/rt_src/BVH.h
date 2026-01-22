#pragma once

#include "AABB.h"
#include "Hittable.h"
#include "Hittable_List.h"

#include <algorithm>

class BVH_Node : public Hittable {

public:
	BVH_Node(Hittable_List list) : BVH_Node(list.objects, 0, list.objects.size()) {};
	BVH_Node(vector<shared_ptr<Hittable>>& objs, size_t start, size_t end) {

		bbox = AABB::empty;

		for (size_t obj_index = start; obj_index < end; obj_index++) {
			bbox = AABB(bbox, objs[obj_index]->bounding_box());
		}

		int axis = bbox.longest_axis();

		auto comparator = (axis == 0) ? box_x_compare
						: (axis == 1) ? box_y_compare
						: box_z_compare;

		size_t object_span = end - start;

		if (object_span == 1) {
			left = right = objs[start];
		}
		else if (object_span == 2) {
			left = objs[start];
			right = objs[start + 1];
		}
		else {
			sort(std::begin(objs) + start, begin(objs) + end, comparator);

			auto mid = start + object_span / 2;
			left = make_shared<BVH_Node>(objs, start, mid);
			right = make_shared<BVH_Node>(objs, mid, end);
		}
	}

	bool Hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override {
		if (!bbox.hit(r, ray_t)) {
			return false;
		}

		bool hit_left = left->Hit(r, ray_t, rec);
		bool hit_right = right->Hit(r, Interval(ray_t.min,
			hit_left ? rec.t : ray_t.max),
			rec);

		return hit_left || hit_right;
	}

	AABB bounding_box() const override {
		return bbox;
	}

private:
	shared_ptr<Hittable> left;
	shared_ptr<Hittable> right;

	AABB bbox;

	static bool box_compare(
		const shared_ptr<Hittable> a,
		const shared_ptr<Hittable> b,
		int axis
	){
		auto a_axis_interval = a->bounding_box().axis_interval(axis);
		auto b_axis_interval = b->bounding_box().axis_interval(axis);

		return a_axis_interval.min < b_axis_interval.min;
	}

	static bool box_x_compare(
		const shared_ptr<Hittable> a,
		const shared_ptr<Hittable> b) {

		return box_compare(a, b, 0);
	}

	static bool box_y_compare(
		const shared_ptr<Hittable> a,
		const shared_ptr<Hittable> b) {

		return box_compare(a, b, 1);
	}

	static bool box_z_compare(
		const shared_ptr<Hittable> a,
		const shared_ptr<Hittable> b) {

		return box_compare(a, b, 2);
	}
};
