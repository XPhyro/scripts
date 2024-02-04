// @LDFLAGS -lOpenCL

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <vector>

#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/opencl.hpp>

#include <xph/die.hpp>
#include <xph/exec_info.hpp>

DEFINE_EXEC_INFO()

const constexpr std::size_t data_length = 1000000uz;
using data_type = float;
const constexpr std::size_t data_size = data_length * sizeof(data_type);

int main(void)
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    xph::die_if(platforms.empty(), "no OpenCL platforms found");

    const cl::Platform platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

    xph::die_if(devices.empty(), "no GPUs found");

    const cl::Device device = devices.front();

    const cl::Context context(device);
    const cl::CommandQueue queue(context, device);

    std::vector<data_type> data(data_length, 0.0f);
    const cl::Buffer buffer(context, CL_MEM_READ_WRITE, data_size);

    for (std::size_t i = 0; i < data_length; ++i)
        data[i] = i * 1.0f;

    std::cerr << "Initializing data..." << std::endl;

    queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, data_size, data.data());

    const auto begin_time = std::chrono::high_resolution_clock::now();
    queue.enqueueReadBuffer(buffer, CL_TRUE, 0, data_size, data.data());
    const auto end_time = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> elapsed_time = end_time - begin_time;

    std::cerr << "Data read back from device." << std::endl;

    const double data_size_MB = data_size / (1024.0 * 1024.0);
    const double bandwidth_MBps = data_size_MB / elapsed_time.count();

    std::cout << bandwidth_MBps << " MB/s" << std::endl;

    return EXIT_SUCCESS;
}
