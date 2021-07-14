#version 450

// shader input
layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inUV;

layout(set = 1, binding = 0) uniform Material
{
    vec3 color;
}
material;

layout(set = 2, binding = 0) uniform sampler2D texSampler;

// output write
layout(location = 0) out vec4 outFragColor;

void main()
{
    outFragColor = 2.0F * texture(texSampler, inUV);
}