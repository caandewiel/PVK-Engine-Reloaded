#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <GLFW/glfw3.h>
#include <memory>
#include <string>

namespace pvk::vulkan
{
class Window
{
public:
    Window(uint32_t width, uint32_t height, const std::string &title);
    [[nodiscard]] GLFWwindow *getWindow() const;
    GLFWwindow *getWindow();

private:
    GLFWwindow *m_window;
};

std::unique_ptr<Window> create(uint32_t width, uint32_t height, const std::string &title);
} // namespace pvk::vulkan
#endif // __WINDOW_H__