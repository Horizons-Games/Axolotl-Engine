#version 460
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/ibl_functions.glsl"

#define EPSILON 1e-5

in vec2 uv;
out vec4 fragColor;

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
		float NdotL = max(dot(N, L), EPSILON);
		float NdotH = max(dot(N, H), EPSILON);
		float VdotH = max(dot(V, H), EPSILON);
		if (NdotL > EPSILON)
		{
			float V_pdf = SmithVisibility(NdotL, NdotV, roughness) * VdotH * NdotL / NdotH;
			float Fc = pow(1.0 - VdotH, 5.0); // note: VdotH = LdotH
			fa += (1.0 - Fc) * V_pdf;
			fb += Fc * V_pdf;
		}
	}

	fragColor = vec4(4.0 * fa/float(NUM_SAMPLES), 4.0 * fb/float(NUM_SAMPLES), 1.0, 1.0);
}