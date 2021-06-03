#include "OverlayWidget.hpp"

namespace pvk::ui
{

OverlayWidget::OverlayWidget(const std::string &title) : m_title(title)
{
}

void OverlayWidget::setContent(const std::string &content)
{
    m_content = content;
}

void OverlayWidget::draw()
{
    ImGui::SetNextWindowBgAlpha(opacity); // Transparent background
    auto io = ImGui::GetIO();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 workArea = viewport->WorkPos;
    ImVec2 windowPosition;
    ImVec2 windowPositionPivot;
    windowPosition.x = workArea.x + padding;
    windowPosition.y = workArea.y + padding;
    windowPositionPivot.x = 0.0F;
    windowPositionPivot.y = 0.0F;
    ImGui::SetNextWindowPos(windowPosition, ImGuiCond_Always, windowPositionPivot);
    window_flags |= ImGuiWindowFlags_NoMove;

    if (ImGui::Begin(m_title.c_str()))
    {
        ImGui::Text("%s", m_content.c_str());
        // ImGui::Separator();
    }
    ImGui::End();
}

} // namespace pvk::ui
