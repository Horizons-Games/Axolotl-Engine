#version 440 core
out vec4 FragColor;
in vec2 uv0;
layout(binding=0) uniform sampler2D mytexture;
uniform vec3 spriteColor;
void main()
{
	vec4 texColor = vec4(1.0,1.0,1.0,1.0) * texture2D(mytexture, uv0);
	 if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
};