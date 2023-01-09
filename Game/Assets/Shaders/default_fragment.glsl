#version 440

uniform sampler2D texDiffuse;

layout(std140) uniform Ambient
{
	vec3 ambientValue;		//12	//0
};

layout(std140) uniform Directional
{
	vec3 directionalDir;  	//12	//0
	vec4 directionalColor;	//16	//16     // note: alpha parameter of colour is the intensity 
};

out vec4 color;

in vec2 uv0;

void main()
{		
	color = texture2D(texDiffuse, uv0);
}