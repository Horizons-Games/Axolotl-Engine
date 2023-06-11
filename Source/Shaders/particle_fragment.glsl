#version 460 

in vec2 fragUv0;
in vec3 fragPos;
in vec4 fragColor;

layout(location = 3) uniform int hasTexture;
layout(binding = 0)  uniform sampler2D diffuseTex;

out vec4 outColor;

void main()
{
	vec4 diffuse = texture(diffuseTex, fragUv0);
	vec4 color = hasTexture * diffuse * fragColor + (1 - hasTexture) * fragColor;

	vec4 gammaCorrection = vec4(2.2);

	color.rgb = pow(color.rgb, vec3(1.0/gammaCorrection));
	outColor = color;
}