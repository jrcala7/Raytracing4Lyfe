#pragma once

#include "Triangle.h"
#include "Hittable_List.h"

#include <cstdlib>
#include <iostream>

#include "../tiny_obj_loader.h"

class Model3D {

public:

	void LoadModel(
		const char* model_filename, shared_ptr<Material> _mat, double uniformScale = 1.0
	)
	{
		modelTris = make_shared<Hittable_List>();
		mat = _mat;
		currentScale = uniformScale;

		if (TryLoad(model_filename)) {
			LoadTris();
			std::cout << "Done Loading model: " << model_filename << std::endl;
			std::cout << "Triangles: " << modelTris->objects.size() << std::endl;
		}
	}

	shared_ptr<Hittable_List> modelTris;

private:
	double currentScale = 1.0;
	shared_ptr<Material> mat;

	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> material;
	std::string error;

	void LoadTris() {
		for (int i = 0; i < shapes[0].mesh.indices.size(); i+= 3) {
			auto vertex1 = shapes[0].mesh.indices[i];
			auto vertex2 = shapes[0].mesh.indices[i + 1];
			auto vertex3 = shapes[0].mesh.indices[i + 2];

			auto i0 = vertex1.vertex_index;
			auto i1 = vertex2.vertex_index;
			auto i2 = vertex3.vertex_index;

			Point3 p1 = GetPointViaVertex(i0);
			Point3 p2 = GetPointViaVertex(i1);
			Point3 p3 = GetPointViaVertex(i2);

			auto triangle = make_shared<Triangle>(
				p1, p2, p3, mat
			);

			float u1 = 0, u2 = 0, u3 = 0;
			float v1 = 0, v2 = 0, v3 = 0;

			if (vertex1.texcoord_index >= 0) {
				GetUUV(vertex1.texcoord_index, u1, v1);
			}

			if (vertex2.texcoord_index >= 0) {
				GetUUV(vertex2.texcoord_index, u2, v2);
			}

			if (vertex3.texcoord_index >= 0) {
				GetUUV(vertex3.texcoord_index, u3, v3);
			}

			triangle->SetUV(
				u1, v1,
				u2, v2,
				u3, v3
			);

			modelTris->Add(
				triangle
			);
		}
	}

	void GetUUV(int texCoord, float& u, float& v) {
		u = attributes.texcoords[texCoord * 2];
		v = attributes.texcoords[texCoord * 2 + 1];
	}

	Point3 GetPointViaVertex(int i) {
		return Point3(
			attributes.vertices[i*3] * currentScale,
			attributes.vertices[i*3 +1] * currentScale,
			attributes.vertices[i*3 +2] * currentScale
		);
	}

	bool TryLoad(const char* model_filename) {
		auto filename = std::string(model_filename);

		std::string prefix = "";
		int fileLevel = 6;

		for (int i = 0; i < fileLevel; i++) {
			auto pathToLoad = prefix + "3D/" + filename;
			std::cout << "Trying to load model from " << pathToLoad << std::endl;

			if (Load(pathToLoad)) return true;

			prefix = prefix + "../";
		}

		std::cerr << "Failed to load model " << model_filename << " last error: " << error << std::endl;
		return false;
	}

	bool Load(std::string filename) {

		bool succ = tinyobj::LoadObj(
			&attributes,
			&shapes,
			&material,
			&error,
			filename.c_str()
		);

		if (!succ)
			std::cout << "Failed to load " << error << std::endl;

		return succ;
	}

};