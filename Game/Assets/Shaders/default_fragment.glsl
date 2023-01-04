#version 440

uniform sampler2D diffuse;

struct PointLight
{
	vec4 position; // xyz position+w radius
	vec4 color; // rgb colour+alpha intensity
};

layout(std140) uniform Ambient
{
	vec3 value;	//16	//0
};

layout(std140) uniform Directional
{
	vec4 directional_dir;
	vec4 directional_color;	// note: alpha parameter of colour is the intensity
};

readonly layout(std430) buffer PointLights
{
	uint num_point;
	PointLight points[];
};

out vec4 color;

in vec2 uv0;

void main()
{	
	//vec3 ambient = Ambient.value * vec3(texture(diffuse, uv0));
	vec3 ambient = vec3(0.091f, 0.091f, 0.091f) * vec3(texture(diffuse, uv0));
	
	vec3 result = ambient;
		
	color = vec4(result, 1.0);
}