#version 460 

in vec2 fragUv0;
in vec2 fragUv1;
in vec3 fragPos;
in vec4 fragColor;
in float blendFactor;
in flat int blending;

layout(location = 3) uniform int hasTexture;
layout(binding = 0)  uniform sampler2D diffuseTex;

out vec4 outColor;

void main()
{
	vec4 diffuse0 = texture(diffuseTex, fragUv0);
	vec4 diffuse = diffuse0;

	if (blending > 0)
	{
		vec4 diffuse1 = texture(diffuseTex, fragUv1);
		diffuse = mix(diffuse0, diffuse1, blendFactor);
	}

	vec4 color = hasTexture * diffuse * fragColor + (1 - hasTexture) * fragColor;
	vec4 gammaCorrection = vec4(2.2);

	color.rgb = pow(color.rgb, vec3(1.0/gammaCorrection));
	outColor = color;
}