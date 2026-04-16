#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstring>
#include <numeric>

/**
 * GPU-ready BVH data structures designed for compute shader traversal.
 * 
 * This is a port of rt_src/BVH.h, converted from a recursive tree structure
 * to a flattened linear representation suitable for SSBO binding and iterative
 * traversal in GLSL compute shaders.
 */

namespace comp_bvh {

// Simple AABB representation - GPU friendly
struct AABB_GPU {
	glm::vec3 min;
	float pad0;  // padding for alignment
	glm::vec3 max;
	float pad1;  // padding for alignment

	AABB_GPU() : min(0.0f), max(0.0f) {}
	AABB_GPU(const glm::vec3& mn, const glm::vec3& mx) : min(mn), max(mx) {}

	// Check if ray intersects this AABB
	// This logic mirrors the CPU version but is GPU-efficient
	bool hit(const glm::vec3& ray_origin, const glm::vec3& ray_inv_dir, float t_min, float t_max) const {
		for (int axis = 0; axis < 3; axis++) {
			float t0 = (min[axis] - ray_origin[axis]) * ray_inv_dir[axis];
			float t1 = (max[axis] - ray_origin[axis]) * ray_inv_dir[axis];

			if (t0 > t1) std::swap(t0, t1);

			t_min = std::max(t_min, t0);
			t_max = std::min(t_max, t1);

			if (t_max < t_min) return false;
		}
		return true;
	}

	// Get the longest axis (0=x, 1=y, 2=z)
	int longest_axis() const {
		glm::vec3 extent = max - min;
		if (extent.x > extent.y && extent.x > extent.z) return 0;
		if (extent.y > extent.z) return 1;
		return 2;
	}
};

// Single BVH node in the flattened tree
// Designed for std430 layout (16-byte alignment)
struct BVH_Node_GPU {
	AABB_GPU bounds;              // 32 bytes (2x vec4-aligned)
	uint32_t left_child_index;    // 4 bytes
	uint32_t right_child_index;   // 4 bytes
	uint32_t object_index;        // 4 bytes (index into object list, 0xFFFFFFFF = internal node)
	uint32_t pad;                 // 4 bytes (padding to 64 bytes total)

	BVH_Node_GPU() 
		: left_child_index(0xFFFFFFFF), 
		  right_child_index(0xFFFFFFFF), 
		  object_index(0xFFFFFFFF),
		  pad(0) {}

	// Check if this is a leaf node (contains an object)
	bool is_leaf() const {
		return object_index != 0xFFFFFFFF;
	}
};

// Complete BVH structure ready for GPU consumption
class BVH_GPU {
public:
	std::vector<BVH_Node_GPU> nodes;
	uint32_t root_index = 0;

	BVH_GPU() = default;

	// Get the size in bytes for SSBO buffer allocation
	size_t get_ssbo_size() const {
		return nodes.size() * sizeof(BVH_Node_GPU);
	}

	// Get raw pointer for buffer upload
	const void* get_ssbo_data() const {
		return nodes.data();
	}

	// Get number of nodes
	uint32_t get_node_count() const {
		return static_cast<uint32_t>(nodes.size());
	}
};

/**
 * CPU-side helper to build a GPU BVH from scene objects.
 * This is called during scene setup; the resulting structure is uploaded to GPU.
 * 
 * Usage:
 *   BVH_GPU bvh_gpu;
 *   BVH_Builder builder(objects); // objects are CPU-side primitive descriptions
 *   bvh_gpu = builder.build();
 */
class BVH_Builder {
public:
	struct Primitive {
		glm::vec3 min;
		glm::vec3 max;
		uint32_t object_id;  // application-specific ID for shader lookup
	};

	explicit BVH_Builder(const std::vector<Primitive>& prims) : primitives(prims) {}

	BVH_GPU build() {
		if (primitives.empty()) {
			return BVH_GPU();
		}

		BVH_GPU result;
		std::vector<uint32_t> indices(primitives.size());
		std::iota(indices.begin(), indices.end(), 0);

		result.root_index = build_recursive(indices, 0, primitives.size(), result.nodes);

		return result;
	}

private:
	const std::vector<Primitive>& primitives;

	uint32_t build_recursive(
		std::vector<uint32_t>& indices,
		size_t start,
		size_t end,
		std::vector<BVH_Node_GPU>& output_nodes
	) {
		if (start >= end) {
			return 0xFFFFFFFF;  // Invalid index
		}

		// Create a node for this range
		uint32_t node_index = static_cast<uint32_t>(output_nodes.size());
		BVH_Node_GPU node;

		// Compute bounding box
		node.bounds.min = glm::vec3(1e30f);
		node.bounds.max = glm::vec3(-1e30f);

		for (size_t i = start; i < end; i++) {
			const Primitive& prim = primitives[indices[i]];
			node.bounds.min = glm::min(node.bounds.min, prim.min);
			node.bounds.max = glm::max(node.bounds.max, prim.max);
		}

		size_t span = end - start;

		if (span == 1) {
			// Leaf node
			node.object_index = indices[start];
			node.left_child_index = 0xFFFFFFFF;
			node.right_child_index = 0xFFFFFFFF;
			output_nodes.push_back(node);
			return node_index;
		}

		// Internal node - partition primitives
		int axis = node.bounds.longest_axis();

		auto comparator = [this, axis](uint32_t a, uint32_t b) {
			float center_a = (primitives[a].min[axis] + primitives[a].max[axis]) * 0.5f;
			float center_b = (primitives[b].min[axis] + primitives[b].max[axis]) * 0.5f;
			return center_a < center_b;
		};

		std::sort(indices.begin() + start, indices.begin() + end, comparator);

		size_t mid = start + span / 2;

		output_nodes.push_back(node);  // Add placeholder

		node.left_child_index = build_recursive(indices, start, mid, output_nodes);
		node.right_child_index = build_recursive(indices, mid, end, output_nodes);

		// Update the node with child indices
		output_nodes[node_index].left_child_index = node.left_child_index;
		output_nodes[node_index].right_child_index = node.right_child_index;

		return node_index;
	}
};

}  // namespace comp_bvh
