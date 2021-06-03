#include "io.hpp"

namespace pvk::io
{
std::vector<uint32_t> readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file.");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    std::vector<uint32_t> convertedBuffer(buffer.size() / sizeof(uint32_t));
    memcpy(convertedBuffer.data(), buffer.data(), buffer.size());

    return convertedBuffer;
}

} // namespace pvk::io
