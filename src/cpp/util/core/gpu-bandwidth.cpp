// @LDFLAGS -lOpenCL

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <vector>

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 300
#include <CL/opencl.hpp>

#include <xph/die.hpp>
#include <xph/exec_info.hpp>

DEFINE_EXEC_INFO();

using data_t = float;
const constexpr std::size_t data_length = 1'000'000'000;
const constexpr std::size_t data_size = data_length * sizeof(data_t);
const constexpr std::size_t iteration_count = 100;

int main()
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    xph::die_if(platforms.empty(), "No OpenCL platforms found.");

    const cl::Platform& platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

    xph::die_if(devices.empty(), "No GPUs found.");

    const cl::Device& device = devices.front();

    const cl::Context context(device);
    const cl::CommandQueue queue(context, device);

    std::vector<data_t> data(data_length);
    const cl::Buffer buffer(context, CL_MEM_READ_WRITE, data_size);

    std::cerr << "Initialising data..." << std::flush;

    for (std::size_t i = 0; i < data_length; ++i)
        data[i] = static_cast<data_t>(i);

    std::cerr << " OK\nPerforming write test..." << std::flush;

    const auto write_start_time = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iteration_count; ++i)
        queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, data_size, data.data());
    const auto write_end_time = std::chrono::high_resolution_clock::now();

    std::cerr << " OK\nPerforming read test..." << std::flush;

    const auto read_start_time = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < iteration_count; ++i)
        queue.enqueueReadBuffer(buffer, CL_TRUE, 0, data_size, data.data());
    const auto read_end_time = std::chrono::high_resolution_clock::now();

    std::cerr << " OK" << std::endl;

    const double total_write_time =
        std::chrono::duration<double>(write_end_time - write_start_time).count();
    const double total_read_time =
        std::chrono::duration<double>(read_end_time - read_start_time).count();

    const double average_write_time = total_write_time / iteration_count;
    const double average_read_time = total_read_time / iteration_count;
    const double data_size_MB = static_cast<double>(data_size) / (1024.0 * 1024.0);

    const double write_bandwidth_MBps = data_size_MB / average_write_time;
    const double read_bandwidth_MBps = data_size_MB / average_read_time;

    std::cout << "Average Write Bandwidth: " << write_bandwidth_MBps << " MB/s\n"
              << "Average Read Bandwidth: " << read_bandwidth_MBps << " MB/s" << std::endl;

    return EXIT_SUCCESS;
}
