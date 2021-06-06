#ifndef EXCEPTIONGRAPHICS_HPP
#define EXCEPTIONGRAPHICS_HPP

#include "PvkException.hpp"

#include <string>

#include <fmt/format.h>

namespace pvk
{
class PvkExceptionGraphics : public PvkException
{
public:
    explicit PvkExceptionGraphics(std::string &&message) : m_message(std::move(message))
    {
        m_message = fmt::format("{} {}", getPrefix(), m_message);
    }

    [[nodiscard]] const char *what() const noexcept override
    {
        return m_message.c_str();
    }

private:
    [[nodiscard]] constexpr const char *getPrefix() const override
    {
        return "[Graphics]";
    }

    std::string m_message;
};
} // namespace pvk

#endif // EXCEPTIONGRAPHICS_HPP
