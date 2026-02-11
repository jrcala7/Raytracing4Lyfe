# CMakeList.txt : CMake project for RaytracingForLife, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Set directory of third party libraries
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty CACHE PATH "Base Directory of libs" FORCE)

project ("RaytracingForLife")

#set(OpenCV_DIR "I:/OpenCV/opencv/build")
#set(OpenCV_DIR "H:/Tools/OpenCV/opencv/build")
set(OpenCV_DIR "C:/Users/g411_jr/Tools/opencv/build")
find_package(OpenCV REQUIRED)

# Add source to this project's executable.
add_executable (RaytracingForLife
"stb_image.h" 
"tiny_obj_loader" 
"RaytracingForLife.cpp" 
"RaytracingForLife.h" 
"Utils.h" 
"rt_src/Vec3.h" 
"rt_src/Color.h" 
"rt_src/Ray.h" 
"rt_src/Hittable_List.h" 
"rt_src/Hittable.h" 
"rt_src/Sphere.h" 
"rt_src/RT_Weekend.h" 
"rt_src/Interval.h" 
"rt_src/Camera.h" 
"rt_src/Material.h" 
"rt_src/AABB.h" 
"rt_src/BVH.h" 
"rt_src/Texture.h" 
"rt_src/Checker_Texture.h" 
"rt_src/RTW_Image.h" 
"rt_src/RTW_Image.cpp" 
"rt_src/Perlin.h" 
"rt_src/Quad.h" 
"rt_src/Camera.cpp" 
"ext/IETThread.h" "ext/IETThread.cpp" 
"ext/PixelThread.h" "ext/PixelThread.cpp" 
"ext/RenderPool.h" "ext/RenderPool.cpp" 
"rt_src/Triangle.h" "rt_src/Model3D.h" "rt_src/Model3D.cpp" "rt_src/RayFuncs.h")

target_link_libraries(RaytracingForLife PRIVATE ${OpenCV_LIBS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RaytracingForLife PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
