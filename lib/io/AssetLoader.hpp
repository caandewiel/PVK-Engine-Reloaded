#ifndef __ASSETLOADER_H__
#define __ASSETLOADER_H__

#include <string>
#include <vector>

namespace pvk::io {
    std::pair<std::vector<char>, std::vector<char>> loadVertexBinary(const std::string &path);
}

#endif // __ASSETLOADER_H__