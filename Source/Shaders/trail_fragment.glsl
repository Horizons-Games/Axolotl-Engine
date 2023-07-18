#version 460 

#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/srgba_functions.glsl"

layout(binding = 0)  uniform sampler2D diffuseTex;
layout(location = 3) uniform int hasTexture;

in vec2 TexCoord;
in vec4 fragColor;

out vec4 outColor;

void main()
{
	vec4 diffuse = texture(diffuseTex, TexCoord);
    diffuse = SRGBA(diffuse);


	vec4 color = hasTexture * diffuse * fragColor + (1 - hasTexture) * fragColor;

	color.rgb = pow(color.rgb, vec3(GAMMA));
	outColor = color;
}