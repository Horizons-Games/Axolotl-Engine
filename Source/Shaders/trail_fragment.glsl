#version 460 

#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/srgba_functions.glsl"

in vec2 TexCoord;
in vec4 fragColor;

layout(location = 3) uniform int hasTexture;
layout(binding = 0)  uniform sampler2D diffuseTex;

out vec4 outColor;

void main()
{
	vec4 diffuse = texture(diffuseTex, TexCoord);
    diffuse = SRGBA(diffuse);


	vec4 color = hasTexture * diffuse * fragColor + (1 - hasTexture) * fragColor;

	color.rgb = pow(color.rgb, vec3(1.0/GAMMA));
	outColor = color;
}