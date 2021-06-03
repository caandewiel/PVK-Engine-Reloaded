#include <GLFW/glfw3.h>
#include <memory>
#include <string>

#include "Window.hpp"

namespace pvk::vulkan
{
Window::Window(uint32_t width, uint32_t height, const std::string &title)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

GLFWwindow *Window::getWindow() const
{
    return m_window;
}

GLFWwindow *Window::getWindow()
{
    return m_window;
}

std::unique_ptr<Window> create(uint32_t width, uint32_t height, const std::string &title)
{
    return std::make_unique<Window>(width, height, title);
}
} // namespace pvk::vulkan