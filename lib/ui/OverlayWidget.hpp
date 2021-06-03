#ifndef OVERLAYWIDGET_HPP
#define OVERLAYWIDGET_HPP

#include "../../external/imgui/imgui.h"
#include "Widget.hpp"
#include <string>

namespace pvk::ui
{
class OverlayWidget : public Widget
{
public:
    OverlayWidget(const std::string &title);

    void setContent(const std::string &content);
    void draw() override;

private:
    static constexpr float opacity = 0.35F;
    static constexpr float padding = 10.0F;

    std::string m_title;
    std::string m_content;
};
} // namespace pvk::ui

#endif // OVERLAYWIDGET_HPP
