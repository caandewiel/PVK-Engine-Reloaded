#include "BinaryHelper.hpp"

#include <lz4.h>

namespace pvk::binary
{
std::vector<char> compress(std::vector<char> &&uncompressedData)
{
    const auto compressedBound = LZ4_compressBound(uncompressedData.size());

    std::vector<char> compressedData;
    compressedData.resize(compressedBound);
    int compressedSize = LZ4_compress_default(uncompressedData.data(),
                                              compressedData.data(),
                                              static_cast<int>(uncompressedData.size()),
                                              static_cast<int>(compressedBound));
    compressedData.resize(compressedSize);

    return compressedData;
}
} // namespace pvk::binary