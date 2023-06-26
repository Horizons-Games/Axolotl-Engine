#version 440 core
out vec4 FragColor;
in vec2 uv0;
layout(binding=0) uniform sampler2D mytexture;
uniform int hasDiffuse;
uniform int direction;
uniform vec4 spriteColor;
uniform float renderPercentage;
void main()
{
	vec2 center = vec2(0.5,0.5);
	vec2 initPos = vec2(0.5,0);
	switch(direction)
	{
		case 0:
			if(uv0.x >= renderPercentage)
				discard;
			break;
		case 1:
			if(1 - uv0.x >= renderPercentage)
				discard;
			break;
		case 2:
			if(uv0.y >= renderPercentage)
				discard;
			break;
		case 3:
			if(1 - uv0.y >= renderPercentage)
				discard;
			break;
		case 4:
		{
			float angle = acos(dot(normalize(initPos-center), normalize(center - uv0))) / 1.8;
			if(1 - uv0.x < center.x)
			{
				angle = 3.6 - angle;
			}
			if(angle/3.6 <= 1 - renderPercentage)
				discard;
			break;
		}
		case 5:
		{
			float angle = acos(dot(normalize(initPos-center), normalize(center - uv0))) / 1.8;
			if(uv0.x < center.x)
			{
				angle = 3.6 - angle;
			}
			if(angle/3.6 <= 1 - renderPercentage)
				discard;
			break;
		}
		case 6:
		{
			initPos = vec2(0.5,1);
			float angle = acos(dot(normalize(initPos-center), normalize(center - uv0)));
			if(angle/3.6 >= renderPercentage)
				discard;
			break;
		}
		case 7:
		{
			float angle = acos(dot(normalize(initPos-center), normalize(center - uv0)));
			if(angle/3.6 >= renderPercentage)
				discard;
			break;
		}
	}
	vec4 texColor = spriteColor * (texture2D(mytexture, uv0) + 1 - hasDiffuse);
	if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
};