#ifndef WIDGET_HPP
#define WIDGET_HPP

namespace pvk::ui
{
class Widget
{
public:
    virtual ~Widget() = default;
    virtual void draw() = 0;
};
} // namespace pvk::ui

#endif // WIDGET_HPP
