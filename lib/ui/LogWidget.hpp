#ifndef LOGWIDGET_HPP
#define LOGWIDGET_HPP

#include "Widget.hpp"

#include <string>

#include <imgui.h>

namespace pvk::ui
{
class LogWidget : public Widget
{
public:
    void clear();
    void addLog(const char *text, ...);
    void draw() override;

private:
    ImGuiTextBuffer m_buffer;
    bool m_scrollToBottom;
};
} // namespace pvk::ui

#endif // LOGWIDGET_HPP
