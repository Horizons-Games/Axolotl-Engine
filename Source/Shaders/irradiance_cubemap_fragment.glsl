#version 460
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/ibl_functions.glsl"

in vec3 texcoords;
uniform samplerCube environment;

out vec4 fragColor;

void main()
{
	vec3 normal = normalize(texcoords);
	mat3 tangentSpace = computeTangetSpace(normal);

	vec3 irradiance = vec3(0.0);
	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		vec2 rand_value = hammersley2D(i, NUM_SAMPLES);
		vec3 sample_dir = tangentSpace * hemisphereSample(rand_value[0], rand_value[1]);
		irradiance += texture(environment, sample_dir).rgb;
	}

	fragColor = vec4(irradiance/float(NUM_SAMPLES), 1.0);
}