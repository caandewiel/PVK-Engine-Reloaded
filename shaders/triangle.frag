#version 450

// Based on: https://learnopengl.com/PBR/Theory

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inCameraPosition;
layout(location = 3) in vec3 inLocalPosition;

layout(set = 1, binding = 0) uniform Material
{
    vec3 albedo;
    float metallicness;
    float roughness;
}
material;

layout(set = 1, binding = 1) uniform sampler2D baseColorTexture;
layout(set = 1, binding = 2) uniform sampler2D normalTexture;
layout(set = 1, binding = 3) uniform sampler2D metallicRoughnessTexture;

layout(push_constant) uniform Transform
{
    mat4 model;
    vec3 lightPosition;
}
transform;

// output write
layout(location = 0) out vec4 outFragColor;

#define PI 3.1415926535897932384626433832795

float DGGX(float NoH, float a)
{
    float a2 = a * a;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * f * f);
}

float GeometrySchlickGGX(float NoV, float a)
{
    float r = (a + 1.0);
    float r2 = r * r;
    float k = r2 / 8.0;
    float denom = NoV * (1.0 - k) + k;
    return NoV / denom;
}

float GeometrySmith(float NoV, float NoL, float a)
{
    float gl = GeometrySchlickGGX(NoL, a);
    float gv = GeometrySchlickGGX(NoV, a);
    return gl * gv;
}

vec3 FresnelSchlick(float VoH, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - VoH, 5.0);
}

vec3 lightColors[4] = vec3[](vec3(1.0, 1.0, 1.0), vec3(0.7, 0.2, 0.0), vec3(1.0, 1.0, 1.0), vec3(0.0, 0.3, 0.9));
vec3 lightPositions[4] = vec3[](vec3(0.0, 1.0, 1.0), vec3(2.0, 1.0, 1.0), vec3(0.0, 2.0, 0.0), vec3(2.0, 2.0, 0.0));

void main()
{
    float roughness = texture(metallicRoughnessTexture, inUV).g;
    float metalness = texture(metallicRoughnessTexture, inUV).b;
    vec3 baseColor = pow(texture(baseColorTexture, inUV).xyz, vec3(2.2));

    // vec3 baseColor = material.albedo;
    // float roughness = material.roughness;
    // float metalness = material.metallicness;
    vec3 F0 = mix(vec3(0.04), baseColor, metalness);
    vec3 Lo = vec3(0.0);

    for (int ii = 0; ii < 4; ++ii)
    {
        vec3 lightColor = lightColors[ii];

        vec3 lightPosition = 60 * lightPositions[ii];

        vec3 N = normalize(inNormal * texture(normalTexture, inUV).xyz);
        // vec3 N = normalize(inNormal);
        vec3 V = normalize(inCameraPosition - inLocalPosition);
        vec3 L = normalize(lightPosition - inLocalPosition);
        vec3 H = normalize(V + L);

        float NoL = clamp(dot(N, L), 0.0, 1.0);
        float NoV = clamp(dot(N, V), 0.0, 1.0);
        float NoH = clamp(dot(N, H), 0.0, 1.0);
        float VoH = clamp(dot(V, H), 0.0, 1.0);
        float LoH = clamp(dot(L, H), 0.0, 1.0);

        float distance = length(lightPosition - inLocalPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = 10000 * lightColor * attenuation * NoL;

        vec3 F = FresnelSchlick(VoH, F0);
        float D = DGGX(NoH, roughness);
        float G = GeometrySmith(NoV, NoL, roughness);

        vec3 specularBRDF = D * F * G / (4.0 * NoL * NoV + 0.001);
        vec3 kD = (1.0 - F) * (1.0 - metalness);

        Lo += (kD * baseColor / PI + specularBRDF) * radiance * NoL;
    }

    vec3 ambient = vec3(0.0) * baseColor;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    outFragColor = vec4(color, 1.0);
}