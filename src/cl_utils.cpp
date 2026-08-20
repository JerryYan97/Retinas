#include "cl_utils.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace cl_utils {

cl::Device selectDevice(cl::Platform* outPlatform) {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) {
        throw std::runtime_error("No OpenCL platforms found on this system.");
    }

    for (cl::Platform& platform : platforms) {
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        if (!devices.empty()) {
            if (outPlatform) *outPlatform = platform;
            return devices.front();
        }
    }

    for (cl::Platform& platform : platforms) {
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
        if (!devices.empty()) {
            if (outPlatform) *outPlatform = platform;
            return devices.front();
        }
    }

    throw std::runtime_error("No OpenCL devices found on any platform.");
}

std::filesystem::path executableDir() {
#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (len == 0 || len == MAX_PATH) {
        throw std::runtime_error("Could not determine the executable's path.");
    }
    return std::filesystem::path(buffer, buffer + len).parent_path();
#else
#error "executableDir() is not implemented for this platform"
#endif
}

std::string readFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file: " + path.string());
    }
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

cl::Program buildProgram(const cl::Context& context, const cl::Device& device,
                          const std::string& source) {
    cl::Program program(context, source);
    try {
        program.build({device});
    } catch (const cl::BuildError& err) {
        std::cerr << "OpenCL build error:\n";
        for (const auto& log : err.getBuildLog()) {
            std::cerr << log.second << '\n';
        }
        throw;
    }
    return program;
}

}  // namespace cl_utils
