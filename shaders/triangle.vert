#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 vUV;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUV;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} uniformBufferObject;

layout(push_constant) uniform Transform
{
    mat4 model;
} transform;

void main() 
{
	gl_Position = uniformBufferObject.proj * uniformBufferObject.view * transform.model * vec4(vPosition, 1.0);

	outNormal = vNormal;
    outUV = vUV;
}