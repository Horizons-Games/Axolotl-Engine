#version 460
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/ibl_functions.glsl"

#define EPSILON 1e-5

uniform samplerCube environment;

in vec3 texcoords;

layout(location = 4) uniform float roughness;
layout(location = 5) uniform float environmentResolution;

out vec4 fragColor;

void main() 
{
	vec3 R = normalize(texcoords);
	vec3 N = R, V = R;
	vec3 color = vec3(0.0);
	float weight = 0.0f;
	mat3 tangentSpace = computeTangetSpace(N);
	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		vec2 rand_value = hammersley2D(i, NUM_SAMPLES);
		vec3 H = normalize(tangentSpace * hemisphereSampleGGX(rand_value[0], rand_value[1], roughness));
		vec3 L = reflect(-V, H);
		float NdotL = max(dot(N, L), EPSILON);
		if (NdotL > EPSILON)
		{
			float NdotH = max(dot(N, H), EPSILON);
			float VdotH = max(dot(V, H), EPSILON);

			float D = GGXNormalDistribution(NdotH, roughness);
			float pdf = (D * NdotH / (4.0 * VdotH)) + 0.0001;

			float saTexel  = 4.0 * PI / (6.0 * environmentResolution * environmentResolution);
			float saSample = 1.0 / (float(NUM_SAMPLES) * pdf + 0.0001);

			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

			color += textureLod(environment, L, mipLevel).rgb * NdotL;
			weight += NdotL;
		}
	}

	fragColor = vec4(color / weight, 1.0);
}