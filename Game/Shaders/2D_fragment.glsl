#version 440 core
out vec4 FragColor;
in vec2 uv0;
layout(binding=0) uniform sampler2D mytexture;
uniform int hasDiffuse;
uniform vec4 spriteColor;
void main()
{
	vec4 texColor = spriteColor * (texture2D(mytexture, uv0) + 1 - hasDiffuse);
	 if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
};