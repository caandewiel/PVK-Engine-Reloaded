#ifndef RENDERSTAGEUI_HPP
#define RENDERSTAGEUI_HPP

#include "../../external/imgui/imgui.h"
#include "../engine/RenderStageBase.hpp"
#include "../vulkan/CommandBuffer.hpp"
#include "LogWidget.hpp"
#include "OverlayWidget.hpp"
#include <absl/container/flat_hash_map.h>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan.hpp>

namespace pvk::ui
{
constexpr uint32_t defaultDescriptorCount = 1000;

class RenderStageUI : public engine::RenderStageBase
{
public:
    RenderStageUI(const vulkan::CommandPool &commandPool, const vulkan::RenderPass &renderPass);
    ~RenderStageUI() override;

    void render(const command_buffer::CommandBuffer &commandBuffer) const override;
    void log(const std::string &text);
    void registerWidget(const std::string &identifier, std::unique_ptr<Widget> &&widget);

    [[nodiscard]] LogWidget &getLogger() const;
    [[nodiscard]] OverlayWidget &getOverlayWidget() const;

private:
    ImGuiContext *m_context;
    absl::flat_hash_map<std::string, std::unique_ptr<Widget>> m_widgets;
    std::unique_ptr<LogWidget> m_logWidget;
    std::unique_ptr<OverlayWidget> m_overlayWidget;

    // template <typename T> const T &getWidget(const std::string &identifier)
    // {
    //     try
    //     {
    //         m_widgets.at(identifier);
    //     }
    //     catch (const std::out_of_range &exception)
    //     {
    //         pvk::engine::log("Could not find widget ")
    //     }
    // }
};
} // namespace pvk::ui

#endif // RENDERSTAGEUI_HPP
