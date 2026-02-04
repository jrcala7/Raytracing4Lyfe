#pragma once
#include "Vec3.h"

class Perlin {
public:
	Perlin() {
		for (int i = 0; i < point_count; i++) {
			//randfloat[i] = random_double();
			randVec[i] = unit_vector(Vec3::random(-1,1));
		}

		Generate_Perm(perm_x);
		Generate_Perm(perm_y);
		Generate_Perm(perm_z);
	}

	double Noise(const Point3& p) const {
		/*
		Old Implem
		auto i = (int(4 * p.x()) & 255);
		auto j = (int(4 * p.y()) & 255);
		auto k = (int(4 * p.z()) & 255);

		return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
		*/

		auto u = p.x() - std::floor(p.x());
		auto v = p.y() - std::floor(p.y());
		auto w = p.z() - std::floor(p.z());

		auto i = int(std::floor(p.x()));
		auto j = int(std::floor(p.y()));
		auto k = int(std::floor(p.z()));
		Vec3 c[2][2][2];

		for (int di = 0; di < 2; di++) {
			for (int dj = 0; dj < 2; dj++) {
				for (int dk = 0; dk < 2; dk++) {
					c[di][dj][dk] = randVec[
						perm_x[(i + di) & 255] ^
						perm_y[(j + dj) & 255] ^
						perm_z[(k + dk) & 255]
					];
				}
			}
		}

		return PerlinLerp(c, u, v, w);
	}

private:
	static const int point_count = 256;
	Vec3 randVec[point_count];

	double randfloat[point_count];

	int perm_x[point_count];
	int perm_y[point_count];
	int perm_z[point_count];

	static double HermSmooth(double val) {
		return val * val * (3 - 2 * val);
	}

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

	static double PerlinLerp(const Vec3 c[2][2][2], double u, double v, double w) {
		auto uu = HermSmooth(u);
		auto vv = HermSmooth(v);
		auto ww = HermSmooth(w);

		auto accum = 0.0;

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					Vec3 weight(u - i, v - j, w - k);
					accum +=
						(i * uu + (1 - i) * (1 - uu)) *
						(j * vv + (1 - j) * (1 - vv)) *
						(k * ww + (1 - k) * (1 - ww)) *
						dot(c[i][j][k], weight);
				}
			}
		}

		return accum;
	}

	static double trilinear_interp(double c[2][2][2], double u, double v, double w) {
		auto accum = 0.0;

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					accum +=
						(i * u + (1 - i) * (1 - u)) *
						(j * v + (1 - j) * (1 - v)) *
						(k * w + (1 - k) * (1 - w)) *
						c[i][j][k];
				}
			}
		}

		return accum;
	}
};