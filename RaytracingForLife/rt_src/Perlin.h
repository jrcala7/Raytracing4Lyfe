#pragma once
#include "Vec3.h"

class Perlin {
public:
	Perlin() {
		for (int i = 0; i < point_count; i++) {
			randfloat[i] = random_double();
		}

		Generate_Perm(perm_x);
		Generate_Perm(perm_y);
		Generate_Perm(perm_z);
	}

	double Noise(const Point3& p) const {
		auto i = (int(4 * p.x()) & 255);
		auto j = (int(4 * p.y()) & 255);
		auto k = (int(4 * p.z()) & 255);

		return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
	}

private:
	static const int point_count = 256;
	double randfloat[point_count];

	int perm_x[point_count];
	int perm_y[point_count];
	int perm_z[point_count];

	static void Generate_Perm(int* p) {
		for (int i = 0; i < point_count; i++) {
			p[i] = i;
		}

		Permutate(p, point_count);
	}

	static void Permutate(int* p, int n) {
		for (int i = n - 1; i > 0; i--) {
			int target = random_int(0, i);
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
	}

};