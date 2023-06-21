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

//Area Lights

float GGXNDAreaLight(float dotNH, float roughness, float alpha, float alphaPrime)
{
    float alpha2 = max(alpha * alpha, EPSILON);
    float alphaPrime2 =  max(alphaPrime * alphaPrime, EPSILON);
    
    return (alpha2 * alphaPrime2) / pow( dotNH * dotNH * ( alpha2 - 1.0 ) + 1.0, 2.0);
}

vec3 ClosestRayToLine(vec3 pos, vec3 a, vec3 b, vec3 r)
{
    vec3 PA = a - pos;
    vec3 AB = b - a;
    float dotABDir = dot(AB, r);
    float num = dot(r, PA) * dotABDir - dot(AB, PA);
    float denom = dot(AB, AB) - dotABDir * dotABDir;
    float t = clamp(num/denom, 0.0f, 1.0f);

    return PA + AB * t;
}

vec3 ClosestPointToLine(vec3 pos, vec3 a, vec3 b, float radius)
{
    vec3 ab = (b - a);
	float len = length(ab);
	ab = ab / len;
	vec3 pointInLine = a + ab * clamp(dot(pos - a, ab), 0.0, len);

	return pointInLine + normalize(pos - pointInLine) * min(radius, distance(pointInLine, pos));
}

vec3 ClosestRayToSphere(float radius, vec3 l, vec3 r)
{
    vec3 centerToRay = dot(l, r) * r - l;
    return l + centerToRay * min(radius/length(centerToRay),1.0);
}

vec3 BisectionIntersection(vec3 pos, vec3 a, vec3 b)
{
	float distA = length(a - pos);
	float distB = length(b - pos);
	vec3 AB = b - a;

	float x = distA / (distB + distA);

	return a + AB * x;
}

#endif