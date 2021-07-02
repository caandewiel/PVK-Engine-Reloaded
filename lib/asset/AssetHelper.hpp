#ifndef ASSETHELPER_HPP
#define ASSETHELPER_HPP

#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <memory>

#include <json.hpp>

namespace pvk::asset
{
template <typename T> std::vector<T> convertBinaryToVector(std::vector<char> &&binaryBlob)
{
    std::vector<T> data;
    data.resize(binaryBlob.size() / sizeof(T));
    memcpy(data.data(), binaryBlob.data(), binaryBlob.size());

    return data;
}

template <typename T> std::vector<char> convertVectorToBinary(std::vector<T> &&data)
{
    std::vector<char> binaryBlob;
    binaryBlob.resize(data.size() * sizeof(T));
    memcpy(binaryBlob.data(), data.data(), binaryBlob.size());

    return binaryBlob;
}

template <typename T> struct isVector : public std::false_type
{
};

template <typename T, typename A> struct isVector<std::vector<T, A>> : public std::true_type
{
};

template <typename T> T readFromInputFileStream(std::ifstream &stream)
{
    if constexpr (isVector<T>::value == true)
    {
        throw std::runtime_error(
            "In order to read a vector from a file stream, you need to provide the size of the buffer.");
    }

    T result{};

    stream.read(reinterpret_cast<char *>(&result), sizeof(T));

    if constexpr (std::is_same_v<T, uint32_t>) {
        result = ntohl(result);
    }

    return result;
}

template <typename T> T readFromInputFileStream(std::ifstream &stream, size_t size)
{
    if constexpr (isVector<T>::value == false)
    {
        return readFromInputFileStream<T>(stream);
    }

    T result{};

    result.resize(size / sizeof(typename decltype(result)::value_type));
    stream.read(result.data(), size);

    return result;
}

nlohmann::json readJsonFromInputFileStream(std::ifstream &stream, size_t size);

nlohmann::json readMessagePackFromInputFileStream(std::ifstream &stream, size_t size);

std::vector<char> compress(std::vector<char> &&uncompressedData);

std::vector<char> uncompress(std::vector<char> &&compressedData, size_t uncompressedSize);
} // namespace pvk::asset

#endif // ASSETHELPER_HPP
