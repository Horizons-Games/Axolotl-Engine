#version 440

uniform sampler2D texDiffuse;

struct PointLight
{
	vec4 position; // xyz position+w radius
	vec4 color; // rgb colour+alpha intensity
};

layout(std140) uniform Ambient
{
	vec3 ambientValue;	//12	//0
};

layout(std140) uniform Directional
{
	vec3 directionalDir;  	//12	//0
	vec4 directionalColor;	//16	//12     // note: alpha parameter of colour is the intensity 
};

readonly layout(std430) buffer PointLights
{
	uint num_point;
	PointLight points[];
};

out vec4 color;

in vec2 uv0;

vec3 N = vec3(0.0f, 0.0f, 1.0f);

void main()
{	
	// Ambient light
	vec3 ambient = ambientValue * vec3(texture(texDiffuse, uv0));
	
	// Directional light
	//float dirIntensity = directionalColor.w;
	//float dirIntensity = 0.5f;
	//vec3 dirDiffuse = directionalColor.xyz;
	//vec3 dirDiffuse = vec3(1.0f, 1.0f, 0.0f);
	
	//vec3 Li = dirDiffuse * dirIntensity;
		
	//vec3 L = normalize(-(vec3(1.0f, 0.0f, 0.0f)));
	
	//float dotNL = max(0.0, dot(N, L));
	
	//vec3 Lo = Li * dotNL;
	
	vec3 result = ambient;
		
	color = vec4(result, 1.0);
}