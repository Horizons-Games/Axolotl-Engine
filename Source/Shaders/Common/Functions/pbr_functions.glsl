#ifndef _PBR_FUNCTIONS_INCLUDE_
#define _PBR_FUNCTIONS_INCLUDE_

#define PI 3.1415926535897932384626433832795
#define EPSILON 1e-5

vec3 GetAmbientLight(in vec3 normal, in vec3 R, float NdotV, float roughness, in vec3 diffuseColor, in vec3 specularColor,
samplerCube diffuse_IBL, samplerCube prefiltered_IBL, sampler2D environmentBRDF, int numLevels_IBL)
{
    vec3 irradiance = texture(diffuse_IBL, normal).rgb;
    vec3 radiance = textureLod(prefiltered_IBL, R, roughness * numLevels_IBL).rgb;
    vec2 fab = texture(environmentBRDF, vec2(NdotV, roughness)).rg;
    vec3 diffuse = (diffuseColor * (1 - specularColor));
    return diffuse * irradiance + radiance * (specularColor * fab.x + fab.y);
}

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
    vec3 orthoTangent = normalize(tangent - max(dot(tangent, normal),EPSILON) * normal);
    vec3 bitangent = cross(orthoTangent, normal);
    return mat3(tangent, bitangent, normal); //TBN
}

vec3 fresnelSchlick(vec3 F0, float dotLH)
{
    return F0 + (1.0 - F0) * pow(1.0 - dotLH, 5.0);
}

float smithVisibility(float dotNL, float dotNV, float roughness)
{
    return 0.5/(dotNL * (dotNV * (1 - roughness) + roughness) + dotNV * (dotNL * (1 - roughness) + roughness));
}

float GGXNormalDistribution(float dotNH, float roughness)
{
    float squareRoughness = roughness*roughness;
    float squareNH = dotNH*dotNH;

    return squareRoughness/max((PI * pow(squareNH * (squareRoughness - 1.0) + 1.0, 2)), EPSILON);
}

#endif