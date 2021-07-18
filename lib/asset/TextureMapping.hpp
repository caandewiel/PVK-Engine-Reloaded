#ifndef __TEXTUREMAPPING_H__
#define __TEXTUREMAPPING_H__

#include <map>

namespace pvk::asset
{
enum TextureType
{
    BASE_COLOR,
    NORMAL,
    AMBIENT_OCCLUSION,
    METALLIC_ROUGHNESS,
    EMISSION,
    SPECULAR,
};

// Based on: assimp/code/AssetLib/glTF2/glTF2Importer.cpp
const std::map<uint8_t, const char *> textureTypeMappingSerialize = {
    {1, "BASE_COLOR"},
    {2, "SPECULAR"},
    {6, "NORMAL"},
    {10, "AMBIENT_OCCLUSION"},
    {18, "METALLIC_ROUGHNESS"},
    {4, "EMISSION"},
};

const std::map<const char *, TextureType> textureTypeMappingDeserialize = {
    {"BASE_COLOR", BASE_COLOR},
    {"NORMAL", NORMAL},
    {"AMBIENT_OCCLUSION", AMBIENT_OCCLUSION},
    {"METALLIC_ROUGHNESS", METALLIC_ROUGHNESS},
    {"EMISSION", EMISSION},
    {"SPECULAR", SPECULAR},
};
} // namespace pvk::asset

#endif // __TEXTUREMAPPING_H__