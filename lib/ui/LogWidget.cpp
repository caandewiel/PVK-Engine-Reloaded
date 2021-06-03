#include "LogWidget.hpp"
#include "imgui.h"
#include <cstdarg>

namespace pvk::ui
{

void LogWidget::clear()
{
    m_buffer.clear();
}

void LogWidget::addLog(const char *text, ...)
{
    va_list args;
    va_start(args, text);
    m_buffer.appendfv(text, args);
    va_end(args);
    m_scrollToBottom = true;
}

void LogWidget::draw()
{
    ImGui::Begin("PVK Log");
    ImGui::TextUnformatted(m_buffer.begin());
    if (m_scrollToBottom)
    {
        ImGui::SetScrollHereY(1.0F);
    }
    m_scrollToBottom = false;
    ImGui::End();
}

} // namespace pvk::ui
