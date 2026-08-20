#include "cl_utils.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int main() {
    try {
        cl::Platform platform;
        cl::Device device = cl_utils::selectDevice(&platform);

        std::cout << "Platform: " << platform.getInfo<CL_PLATFORM_NAME>() << '\n';
        std::cout << "Device:   " << device.getInfo<CL_DEVICE_NAME>() << '\n';

        cl::Context context(device);
        cl::CommandQueue queue(context, device);

        std::filesystem::path kernelPath = cl_utils::executableDir() / "kernels" / "hello.cl";
        std::string kernelSource = cl_utils::readFile(kernelPath);
        cl::Program program = cl_utils::buildProgram(context, device, kernelSource);

        // Each character shifted down by one; the kernel shifts it back up.
        std::string message = "Hello, World!";
        for (char& c : message) {
            c -= 1;
        }

        cl::Buffer buffer(context, CL_MEM_READ_WRITE, message.size());
        queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, message.size(), message.data());

        cl::Kernel kernel(program, "shift_chars");
        kernel.setArg(0, buffer);
        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(message.size()));

        std::vector<char> result(message.size());
        queue.enqueueReadBuffer(buffer, CL_TRUE, 0, result.size(), result.data());

        std::cout << std::string(result.begin(), result.end()) << '\n';
    } catch (const cl::Error& err) {
        std::cerr << "OpenCL error: " << err.what() << " (" << err.err() << ")\n";
        return 1;
    } catch (const std::exception& err) {
        std::cerr << "Error: " << err.what() << '\n';
        return 1;
    }

    return 0;
}
