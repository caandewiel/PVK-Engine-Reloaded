#ifndef TREEWIDGET_HPP
#define TREEWIDGET_HPP

#include "Widget.hpp"

#include "../geometry/Object.hpp"

namespace pvk::ui
{
struct Node {
    std::string content;
    std::vector<std::unique_ptr<Node>> children;
    const char * icon;
};

class TreeWidget : public Widget
{
public:
    void draw() override;
    void setContent(const geometry::Object &object);

private:
    void drawNode(const Node &node);

    std::unique_ptr<Node> addNode(const geometry::Node &node);

    std::unique_ptr<Node> m_root;
};
} // namespace pvk::ui

#endif // TREEWIDGET_HPP
