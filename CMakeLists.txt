cmake_minimum_required(VERSION 3.20)
project(OpenCLHelloWorld CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- OpenCL headers (CL/cl.h, CL/opencl.hpp source of truth for versions) ---
FetchContent_Declare(
    OpenCLHeaders
    GIT_REPOSITORY https://github.com/KhronosGroup/OpenCL-Headers.git
    GIT_TAG        v2024.10.24
)

# --- ICD loader: builds the OpenCL.lib / libOpenCL we link against; at
#     runtime it dispatches to whatever vendor ICD is registered on the
#     machine (GPU driver, CPU runtime, etc.) ---
set(OPENCL_ICD_LOADER_BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    OpenCLICDLoader
    GIT_REPOSITORY https://github.com/KhronosGroup/OpenCL-ICD-Loader.git
    GIT_TAG        v2024.10.24
)

# --- C++ wrapper (CL/opencl.hpp) on top of the C API ---
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    OpenCLCLHPP
    GIT_REPOSITORY https://github.com/KhronosGroup/OpenCL-CLHPP.git
    GIT_TAG        v2024.10.24
)

# Headers must be available before the ICD loader configures, since the
# loader looks for an existing OpenCL::Headers target before falling back
# to find_package(OpenCLHeaders).
FetchContent_MakeAvailable(OpenCLHeaders OpenCLICDLoader OpenCLCLHPP)

add_executable(hello_opencl
    src/main.cpp
    src/cl_utils.cpp
)

target_link_libraries(hello_opencl PRIVATE OpenCL::OpenCL)
target_include_directories(hello_opencl PRIVATE ${openclclhpp_SOURCE_DIR}/include)

target_compile_definitions(hello_opencl PRIVATE
    CL_HPP_TARGET_OPENCL_VERSION=300
    CL_HPP_MINIMUM_OPENCL_VERSION=120
    CL_HPP_ENABLE_EXCEPTIONS
)

# Kernels are loaded from disk at runtime (see cl_utils::readFile), so make
# sure they sit next to the built executable.
add_custom_command(TARGET hello_opencl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/kernels
            $<TARGET_FILE_DIR:hello_opencl>/kernels
)
