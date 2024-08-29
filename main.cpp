
#include "vulkantogo.h"

#include <iostream>


int main() {

    vktg::StartUp();

    std::cout << vktg::Instance() << '\n';
    std::cout << vktg::Window() << '\n';
    std::cout << vktg::Surface() << '\n';
    std::cout << vktg::Gpu() << '\n';
    std::cout << vktg::Device() << '\n';

    vktg::ShutDown();

    return 0;
}