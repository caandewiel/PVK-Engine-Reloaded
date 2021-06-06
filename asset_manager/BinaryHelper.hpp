#ifndef BINARYHELPER_HPP
#define BINARYHELPER_HPP

#include <vector>

namespace pvk::binary
{
template <typename T> std::vector<T> convertBinaryBlobToVector(std::vector<char> &&binaryBlob)
{
    std::vector<T> data;
    data.resize(binaryBlob.size() / sizeof(T));
    memcpy(data.data(), binaryBlob.data(), binaryBlob.size());

    return data;
}

template <typename T> std::vector<char> convertVectorToBinaryBlob(std::vector<T> &&data)
{
    std::vector<char> binaryBlob;
    binaryBlob.resize(data.size() * sizeof(T));
    memcpy(binaryBlob.data(), data.data(), binaryBlob.size());

    return binaryBlob;
}
std::vector<char> compress(std::vector<char> &&uncompressedData);
} // namespace pvk::binary

#endif // BINARYHELPER_HPP
