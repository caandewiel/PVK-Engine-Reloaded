#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outNormal;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 model;
    vec3 color;
} uniformBufferObject;

layout(push_constant) uniform constants
{
    mat4 data;
} transform;

void main() 
{
	gl_Position = uniformBufferObject.proj * uniformBufferObject.view * uniformBufferObject.model * vec4(vPosition, 1.0);
    // gl_Position = vec4(vPosition, 1.0);
	outColor = uniformBufferObject.color;
	outNormal = vNormal;
}