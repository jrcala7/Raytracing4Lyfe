#pragma once

/**
 * Quick reference and usage examples for GPU BVH
 * 
 * Minimal example:
 * 
 * 1. Create primitives (bounding boxes + object IDs):
 *    std::vector<comp_bvh::BVH_Builder::Primitive> prims;
 *    prims.push_back({ min, max, object_id });
 * 
 * 2. Build BVH:
 *    comp_bvh::BVH_Builder builder(prims);
 *    comp_bvh::BVH_GPU bvh = builder.build();
 * 
 * 3. Upload to GPU:
 *    BVH_GPU_Manager mgr;
 *    mgr.upload_bvh(bvh);
 *    mgr.bind_to_program(compute_shader);
 * 
 * 4. In compute shader:
 *    HitRecord hit = bvh_trace(ray, t_min, t_max);
 */

// Include all GPU BVH components
#include "BVH_Compute.h"
#include "BVH_GPU_Manager.h"

// Import main namespace for convenience
using namespace comp_bvh;
