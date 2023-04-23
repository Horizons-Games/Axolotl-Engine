#version 460
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/ibl_functions.glsl"

#define NUM_SAMPLES 4096
#define PI 3.1415926535897932384626433832795

in vec2 uv;
out vec4 fragColor;

vec3 hemisphereSampleGGX(float u1, float u2, float roughness )
{
	float phi = 2.0*PI*u1;
	float cos_theta = sqrt((1.0-u2)/(u2*(roughness*roughness-1)+1));
	float sin_theta = sqrt(1-cos_theta*cos_theta);
	// spherical to cartesian conversion
	vec3 dir;
	dir.x = cos(phi)*sin_theta;
	dir.y = sin(phi)*sin_theta;
	dir.z = cos_theta;
	return dir;
}

mat3 computeTangetSpace(in vec3 normal)
{
	vec3 up = abs(normal.y) > 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up = cross(normal, right);
	return mat3(right, up, normal);
}

float radicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 hammersley2D(uint i, uint N)
{
	return vec2(float(i)/float(N), radicalInverse_VdC(i));
}

float SmithVisibility(float NL, float NV, float roughness)
{
	return 0.5 / (NL * (NV * (1 - roughness) + roughness) + NV * (NL * (1 - roughness) + roughness));
}

void main()
{
	float NdotV = uv.x, roughness = uv.y;
	vec3 V = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);
	vec3 N = vec3(0.0, 0.0, 1.0);
	float fa = 0.0;
	float fb = 0.0;
	for (uint i = 0; i < NUM_SAMPLES; i++)
	{
		vec2 rand_value = hammersley2D(i, NUM_SAMPLES);
		vec3 H = hemisphereSampleGGX(rand_value[0], rand_value[1], roughness);
		vec3 L = reflect(-V, H);
		float NdotL = max(dot(N, L), 0.0);
		float NdotH = max(dot(N, H), 0.0);
		float VdotH = max(dot(V, H), 0.0);
		if (NdotL > 0.0)
		{
			float V_pdf = SmithVisibility(NdotL, NdotV, roughness) * VdotH * NdotL / NdotH;
			float Fc = pow(1.0 - VdotH, 5.0); // note: VdotH = LdotH
			fa += (1.0 - Fc) * V_pdf;
			fb += Fc * V_pdf;
		}
	}

	fragColor = vec4(4.0 * fa/float(NUM_SAMPLES), 4.0 * fb/float(NUM_SAMPLES), 1.0, 1.0);
}