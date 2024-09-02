
#include "vulkantogo.h"

#include <iostream>


int main() {

    vktg::StartUp();

    std::cout << "StartUp\n";
    std::cout << vktg::Instance() << '\n';
    std::cout << vktg::Window() << '\n';
    std::cout << vktg::Surface() << '\n';
    std::cout << vktg::Gpu() << '\n';
    std::cout << vktg::Device() << '\n';

    std::cout << "Buffer\n";
    vktg::Buffer buffer = vktg::CreateBuffer( 256, vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eGpuOnly);

    std::cout << buffer.buffer << '\n';
    std::cout << buffer.bufferSize << '\n';
    std::cout << vk::to_string( buffer.bufferUsage) << '\n';
    std::cout << vma::to_string(buffer.memoryUsage) << '\n';
    std::cout << buffer.allocation << '\n';

    vktg::ShutDown();

    return 0;
}