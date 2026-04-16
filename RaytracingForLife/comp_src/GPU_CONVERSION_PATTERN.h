#pragma once

/**
 * Quick Reference: CPU Function → GPU Compute Shader Conversion Pattern
 * 
 * This file documents the pattern used to convert SphereSample from CPU to GPU.
 * Use this as a template for converting other CPU raytracing functions.
 */

/*
================================================================================
PATTERN: CPU Raytracing Function → GPU Compute Shader
================================================================================

STEP 1: Scene Setup (CPU)
────────────────────────────────────────────────────────────────────────────
• Create geometric primitives (spheres, meshes, etc.)
• Compute bounding boxes for each primitive
• Convert to BVH_Builder::Primitive format
• Example:

	std::vector<comp_bvh::BVH_Builder::Primitive> primitives;
	for (auto& sphere : scene.spheres) {
		primitives.push_back({
			sphere.center - glm::vec3(sphere.radius),
			sphere.center + glm::vec3(sphere.radius),
			sphere.id
		});
	}

STEP 2: Build GPU BVH (CPU)
────────────────────────────────────────────────────────────────────────────
• Create BVH_Builder with primitives
• Call build() to create flattened GPU structure
• Example:

	comp_bvh::BVH_Builder builder(primitives);
	comp_bvh::BVH_GPU bvh = builder.build();

STEP 3: Prepare GPU Data (CPU)
────────────────────────────────────────────────────────────────────────────
• Pack sphere/mesh data into vec4 arrays (no pointers!)
• Pack material data (colors, properties)
• Example:

	std::vector<glm::vec4> sphere_data;
	for (auto& sphere : scene.spheres) {
		sphere_data.push_back(glm::vec4(sphere.center, sphere.radius));
	}

STEP 4: Upload to GPU (CPU)
────────────────────────────────────────────────────────────────────────────
• Create BVH_GPU_Manager
• Upload BVH, geometry, materials
• Bind to compute program
• Example:

	BVH_GPU_Manager manager;
	manager.upload_bvh(bvh);
	manager.upload_spheres(sphere_data);
	manager.upload_materials(material_data);
	manager.bind_to_program(compute_program);

STEP 5: Set Shader Uniforms (CPU)
────────────────────────────────────────────────────────────────────────────
• Set camera parameters
• Set BVH root and counts
• Example:

	glUseProgram(program);
	glUniform1ui(glGetUniformLocation(program, "bvh_root"), bvh_manager.get_root_index());
	glUniform1ui(glGetUniformLocation(program, "sphere_count"), 1);
	glUniform3fv(glGetUniformLocation(program, "camera_origin"), 1, glm::value_ptr(camera_pos));

STEP 6: Dispatch Compute Shader (CPU)
────────────────────────────────────────────────────────────────────────────
• Calculate dispatch grid (image_width/8, image_height/8, 1) for 8x8 work groups
• Example:

	GLuint width = 800, height = 600;
	glDispatchCompute((width + 7) / 8, (height + 7) / 8, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

STEP 7: Ray Tracing in Compute Shader (GLSL)
────────────────────────────────────────────────────────────────────────────
• For each pixel, generate a ray from camera
• Call BVH traversal function
• Compute shading
• Store result in output image
• Example GLSL:

	void main() {
		ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
		vec2 uv = vec2(pixel) / vec2(imageSize(img_output));

		Ray ray = generate_camera_ray(uv);
		HitRecord hit = bvh_trace(ray, 0.001, 1e30);

		vec4 color;
		if (hit.hit) {
			color = compute_shading(hit);
		} else {
			color = vec4(background_gradient(uv), 1.0);
		}

		imageStore(img_output, pixel, color);
	}

STEP 8: BVH Traversal in Shader (GLSL)
────────────────────────────────────────────────────────────────────────────
• Use stack-based iterative traversal (no recursion!)
• Test AABB bounds
• Intersect primitives
• Track closest hit
• Example GLSL:

	HitRecord bvh_trace(Ray r, float t_min, float t_max) {
		HitRecord closest;
		uint stack[32];
		uint sp = 0;
		stack[sp++] = bvh_root;

		while (sp > 0) {
			uint idx = stack[--sp];
			BVH_Node node = nodes[idx];

			if (!aabb_hit(node.bounds_min, node.bounds_max, r, t_min, closest.t))
				continue;

			if (node.object_id != 0xFFFFFFFFu) {
				// Leaf - test sphere
				HitRecord hit;
				if (sphere_hit(spheres[node.object_id], r, t_min, closest.t, hit)) {
					if (hit.t < closest.t)
						closest = hit;
				}
			} else {
				// Internal - push children
				if (node.right_child != 0xFFFFFFFFu)
					stack[sp++] = node.right_child;
				if (node.left_child != 0xFFFFFFFFu)
					stack[sp++] = node.left_child;
			}
		}
		return closest;
	}

STEP 9: Read Back & Save (CPU)
────────────────────────────────────────────────────────────────────────────
• Use glGetTexImage to read pixels from GPU
• Convert RGBA float to BGR double (OpenCV format)
• Save PNG using SaveImage()
• Example:

	std::vector<float> pixels;
	glBindTexture(GL_TEXTURE_2D, output_texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels.data());

	// Convert to BGR...
	SaveImage(bgr_image, width, height, "output.png");

================================================================================
KEY DIFFERENCES: CPU vs GPU
================================================================================

CPU (Original)                          GPU (New)
───────────────────────────────────────────────────────────────────────────
Path tracing loop (N iterations)    → Single compute dispatch
Recursive ray traversal              → Iterative stack-based traversal
Pointer-based BVH                    → Flat linear BVH array
Double precision (64-bit)            → Float precision (32-bit)
Per-pixel sampling over time         → Per-pixel single sample (extensible)
Sequential execution (cores × threads) → Parallel GPU execution (thousands)
CPU memory                            → GPU VRAM (bandwidth advantage)

================================================================================
GLSL SHADER STRUCTURE TEMPLATE
================================================================================

#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

// Define GPU structures matching C++ layouts (std430)
struct BVH_Node {
	vec3 bounds_min;
	float pad0;
	vec3 bounds_max;
	float pad1;
	uint left_child;
	uint right_child;
	uint object_id;
	uint pad2;
};

struct Sphere {
	vec3 center;
	float radius;
};

// SSBOs with std430 layout
layout(std430, binding = 1) readonly buffer BVH_Nodes {
	BVH_Node nodes[];
};

layout(std430, binding = 2) readonly buffer Spheres {
	Sphere spheres[];
};

// Uniforms for camera and scene parameters
uniform uint bvh_root;
uniform uint sphere_count;
uniform vec3 camera_origin;
uniform vec3 camera_dir;
uniform float aspect_ratio;

// Implement ray generation, BVH traversal, shading, etc.
// See bvh_traverse.comp for full example

void main() {
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	// ... render pixel at (pixel.x, pixel.y)
	imageStore(img_output, pixel, color);
}

================================================================================
C++ INTEGRATION TEMPLATE
================================================================================

// In RaytracingForLife.cpp or your main file:

#include "comp_src/BVH_Compute.h"
#include "comp_src/BVH_GPU_Manager.h"

int GPU_RenderScene_Template() {
	// Initialize OpenGL/GLFW (existing pattern)
	// Create scene geometry
	std::vector<comp_bvh::BVH_Builder::Primitive> prims;

	// Build GPU BVH
	comp_bvh::BVH_Builder builder(prims);
	auto bvh = builder.build();

	// Create GPU resources
	BVH_GPU_Manager mgr;
	mgr.upload_bvh(bvh);

	// Load and dispatch compute shader
	GLuint program = ShaderUtils::LoadComputeShader(shader_path);
	mgr.bind_to_program(program);

	// Set uniforms
	glUseProgram(program);
	glUniform1ui(glGetUniformLocation(program, "bvh_root"), mgr.get_root_index());
	// ... set other uniforms

	// Dispatch
	ComputeRenderer renderer(width, height);
	renderer.LoadShaderProgram(program);
	renderer.InitializeImage();
	renderer.Dispatch();

	// Read back & save
	std::vector<float> pixels;
	renderer.ReadImage(pixels);
	// Convert and save...

	return 0;
}

================================================================================
CHECKLIST FOR NEW GPU CONVERSIONS
================================================================================

Scene Setup
□ Define all geometric primitives with bounds
□ Create BVH_Builder::Primitive array
□ Verify bounds are correct and aligned

GPU Data Preparation
□ Pack geometry into flat vec4 arrays (no indirection)
□ Pack materials into vec4 arrays
□ Ensure all data is POD (no pointers)

BVH Building
□ Create BVH_Builder with primitives
□ Call build() and verify node count
□ Check root index

GPU Upload
□ Create BVH_GPU_Manager
□ Upload BVH, geometry, materials
□ Bind to compute program
□ Verify SSBO bindings match shader

Shader Development
□ Define matching struct layouts (std430)
□ Implement ray generation from camera
□ Implement BVH traversal (stack-based!)
□ Implement intersection tests
□ Implement shading
□ Test with simple scenes first

Integration & Testing
□ Set shader uniforms before dispatch
□ Dispatch with correct grid size
□ Read back pixels
□ Verify non-zero pixels
□ Convert to output format
□ Save and verify image

Performance
□ Profile SSBO access patterns
□ Check for warp divergence
□ Optimize traversal order (front-to-back)
□ Consider temporal coherence

================================================================================
USEFUL REFERENCES
================================================================================

• CPU BVH: rt_src/BVH.h
• GPU BVH: comp_src/BVH_Compute.h
• SSBO Manager: comp_src/BVH_GPU_Manager.h
• Example Shader: comp_src/shaders/bvh_traverse.comp
• Full Example: RaytracingForLife.cpp::TestComputeShader_SphereSample()
• GLSL std430: https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#std430_layout_rules

================================================================================
*/
