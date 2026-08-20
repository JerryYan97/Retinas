#pragma once

#include <mutex>  // CL/opencl.hpp uses std::once_flag/call_once without including <mutex> itself

#include <CL/opencl.hpp>

#include <filesystem>
#include <string>

namespace cl_utils {

// Picks a device: prefers a GPU on any platform, falls back to any device
// type if no GPU is found. Throws std::runtime_error if nothing is available.
// If outPlatform is non-null, it's set to the platform the device came from.
cl::Device selectDevice(cl::Platform* outPlatform = nullptr);

// Directory containing the running executable. Kernel paths should be
// resolved against this rather than the current working directory, which
// varies depending on how the program is launched.
std::filesystem::path executableDir();

// Reads an entire file into a string. Throws std::runtime_error if the file
// can't be opened.
std::string readFile(const std::filesystem::path& path);

// Builds a program from source for a single device. On a build failure, the
// build log is printed to stderr before the cl::BuildError is rethrown.
cl::Program buildProgram(const cl::Context& context, const cl::Device& device,
                          const std::string& source);

}  // namespace cl_utils
