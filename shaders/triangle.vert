#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 3) in vec2 vUV;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outCameraPosition;
layout (location = 3) out vec3 outLocalPosition;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
} uniformBufferObject;

layout(push_constant) uniform Transform
{
    mat4 model;
    vec3 lightPosition;
} transform;

void main() 
{
	gl_Position = uniformBufferObject.proj * uniformBufferObject.view * transform.model * vec4(vPosition, 1.0);

    outLocalPosition = vec3(transform.model * vec4(vPosition, 1.0));
    outCameraPosition = uniformBufferObject.cameraPosition;
	outNormal = vNormal;
    outUV = vUV;
}