# Raytracing4Lyfe

## Branch check: compute shader support

Verified remote branches:
- `origin/main`
- `origin/compute_shader`
- `origin/copilot/check-compute-shader-support` (current)

Findings:
- `main` and `copilot/check-compute-shader-support`: OBJ loading exists, but no compute shader pipeline.
- `compute_shader`: compute shader infrastructure exists (GLFW/GLAD, compute shader dispatch, BVH GPU helpers), but the implemented compute path is sphere-based (`TestComputeShader_SphereSample`) and does not load OBJ meshes into the compute pipeline.

Conclusion: there is currently no branch with end-to-end **OBJ + compute shader** support.
