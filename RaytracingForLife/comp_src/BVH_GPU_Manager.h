#pragma once

#include "BVH_Compute.h"
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

/**
 * GPU BVH integration helper.
 * Manages SSBO creation, data upload, and binding for compute shaders.
 */

class BVH_GPU_Manager {
public:
	GLuint bvh_ssbo = 0;
	GLuint sphere_ssbo = 0;
	GLuint material_ssbo = 0;
	GLuint texture_id = 0;

	comp_bvh::BVH_GPU bvh_data;

	~BVH_GPU_Manager() {
		cleanup();
	}

	void cleanup() {
		if (bvh_ssbo) glDeleteBuffers(1, &bvh_ssbo);
		if (sphere_ssbo) glDeleteBuffers(1, &sphere_ssbo);
		if (material_ssbo) glDeleteBuffers(1, &material_ssbo);
		if (texture_id) glDeleteTextures(1, &texture_id);

		bvh_ssbo = sphere_ssbo = material_ssbo = texture_id = 0;
	}

	/**
	 * Upload BVH to GPU SSBO (binding point 1)
	 */
	void upload_bvh(const comp_bvh::BVH_GPU& bvh) {
		bvh_data = bvh;

		if (bvh_ssbo == 0) {
			glGenBuffers(1, &bvh_ssbo);
		}

		glBindBuffer(GL_COPY_WRITE_BUFFER, bvh_ssbo);
		glBufferData(GL_COPY_WRITE_BUFFER, bvh.get_ssbo_size(), bvh.get_ssbo_data(), GL_DYNAMIC_READ);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}

	/**
	 * Upload sphere data to GPU SSBO (binding point 2)
	 */
	void upload_spheres(const std::vector<glm::vec4>& sphere_data) {
		if (sphere_ssbo == 0) {
			glGenBuffers(1, &sphere_ssbo);
		}

		glBindBuffer(GL_COPY_WRITE_BUFFER, sphere_ssbo);
		glBufferData(GL_COPY_WRITE_BUFFER, sphere_data.size() * sizeof(glm::vec4), sphere_data.data(), GL_DYNAMIC_READ);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}

	/**
	 * Upload material data to GPU SSBO (binding point 3)
	 */
	void upload_materials(const std::vector<glm::vec4>& material_data) {
		if (material_ssbo == 0) {
			glGenBuffers(1, &material_ssbo);
		}

		glBindBuffer(GL_COPY_WRITE_BUFFER, material_ssbo);
		glBufferData(GL_COPY_WRITE_BUFFER, material_data.size() * sizeof(glm::vec4), material_data.data(), GL_DYNAMIC_READ);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}

	/**
	 * Upload texture image to GPU (binding point 4)
	 * data: RGBA image data (4 bytes per pixel)
	 * width, height: image dimensions
	 */
	void upload_texture(const unsigned char* data, int width, int height) {
		if (texture_id == 0) {
			glGenTextures(1, &texture_id);
		}

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	/**
	 * Bind SSBOs and texture to compute program
	 */
	void bind_to_program(GLuint program) {
		if (bvh_ssbo) {
			GLuint bvh_block = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "BVH_Nodes");
			if (bvh_block != GL_INVALID_INDEX) {
				glShaderStorageBlockBinding(program, bvh_block, 1);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bvh_ssbo);
			}
		}

		if (sphere_ssbo) {
			GLuint sphere_block = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "Spheres");
			if (sphere_block != GL_INVALID_INDEX) {
				glShaderStorageBlockBinding(program, sphere_block, 2);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, sphere_ssbo);
			}
		}

		if (material_ssbo) {
			GLuint material_block = glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "MaterialData");
			if (material_block != GL_INVALID_INDEX) {
				glShaderStorageBlockBinding(program, material_block, 3);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, material_ssbo);
			}
		}

		if (texture_id) {
			glActiveTexture(GL_TEXTURE0 + 4);
			glBindTexture(GL_TEXTURE_2D, texture_id);
			GLint texture_loc = glGetUniformLocation(program, "texture_sampler");
			if (texture_loc != -1) {
				glUniform1i(texture_loc, 4);
			}
		}
	}

	uint32_t get_root_index() const {
		return bvh_data.root_index;
	}

	uint32_t get_node_count() const {
		return bvh_data.get_node_count();
	}
};
