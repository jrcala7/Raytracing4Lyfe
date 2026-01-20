# CMakeList.txt : CMake project for RaytracingForLife, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

include(FetchContent)

# Set directory of third party libraries
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty CACHE PATH "Base Directory of libs" FORCE)

project ("RaytracingForLife")

#set(OpenCV_DIR "I:/OpenCV/opencv/build")
set(OpenCV_DIR "H:/Tools/OpenCV/opencv/build")
find_package(OpenCV REQUIRED)

# Add source to this project's executable.
add_executable (RaytracingForLife
"RaytracingForLife.cpp" 
"RaytracingForLife.h" 
"Utils.h" 
"rt_src/Vec3.h" 
"rt_src/Color.h" 
"rt_src/Ray.h" 
"rt_src/Hittable_List.h" 
"rt_src/Hittable.h" 
"rt_src/Sphere.h" "rt_src/RT_Weekend.h")

target_link_libraries(RaytracingForLife PRIVATE ${OpenCV_LIBS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RaytracingForLife PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
