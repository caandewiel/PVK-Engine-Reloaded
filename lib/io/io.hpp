#ifndef IO_HPP
#define IO_HPP

#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace pvk::io
{
std::vector<uint32_t> readFile(const std::string &filename);
} // namespace pvk::io

#endif // IO_HPP
