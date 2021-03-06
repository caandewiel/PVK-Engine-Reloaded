#include "TreeWidget.hpp"

#include "../../external/FontAwesome/IconsFontAwesome4.h"

#include <memory>

#include <fmt/format.h>

#include <imgui.h>

namespace pvk::ui
{
void TreeWidget::draw()
{
    ImGui::Begin("Object Hierarchy");

    if (m_root != nullptr)
    {
        drawNode(*m_root);
    }

    ImGui::End();
}

void TreeWidget::setContent(const geometry::Object &object)
{
    m_root = addNode(object.getRootNode());
}

void TreeWidget::drawNode(const Node &node)
{
    if (ImGui::TreeNode(fmt::format("{} {}", node.icon, node.content.c_str()).c_str()))
    {
        for (const auto &child : node.children)
        {
            drawNode(*child);
        }

        ImGui::TreePop();
    }
}

std::unique_ptr<Node> TreeWidget::addNode(const geometry::Node &node)
{
    auto currentNode = std::make_unique<Node>();
    currentNode->content = node.getName();
    currentNode->icon = node.hasMesh() ? ICON_FA_GLOBE : ICON_FA_CODE_FORK;

    for (const auto &child : node.getChildren())
    {
        currentNode->children.emplace_back(addNode(*child.lock()));
    }

    return currentNode;
}
} // namespace pvk::ui
