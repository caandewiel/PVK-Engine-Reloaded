#version 450

//shader input
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inNormal;

//output write
layout (location = 0) out vec4 outFragColor;

void main() 
{
	outFragColor = vec4(inNormal, 1.0f);
}