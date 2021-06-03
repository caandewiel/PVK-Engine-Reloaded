#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <exception>
namespace pvk
{
class PvkException : public std::exception
{
    [[nodiscard]] virtual constexpr const char *getPrefix() const = 0;
};
} // namespace pvk

#endif // EXCEPTION_HPP
