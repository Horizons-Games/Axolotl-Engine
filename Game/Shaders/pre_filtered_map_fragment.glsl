#version 460
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/ibl_functions.glsl"

uniform samplerCube environment;

in vec3 texcoords;

layout(location = 0) uniform float roughness;

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
		vec3 H = tangentSpace * hemisphereSampleGGX(rand_value[0], rand_value[1], roughness);
		vec3 L = reflect(-V, H);
		float NdotL = max(dot(N, L), 0.0);
		if (NdotL > 0)
		{
			color += texture(environment , L).rgb * NdotL;
			weight += NdotL;
		}
	}

	fragColor = vec4(color / weight, 1.0);
}