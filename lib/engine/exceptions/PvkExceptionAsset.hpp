#ifndef PVKEXCEPTIONASSET_HPP
#define PVKEXCEPTIONASSET_HPP

#include "PvkException.hpp"

#include <fmt/format.h>
#include <string>

namespace pvk
{
class PvkExceptionAsset : public PvkException
{
public:
    explicit PvkExceptionAsset(std::string &&message) : m_message(std::move(message))
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
        return "[Asset]";
    }

    std::string m_message;
};
} // namespace pvk

#endif // PVKEXCEPTIONASSET_HPP