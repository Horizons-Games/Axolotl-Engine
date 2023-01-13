#version 440

uniform sampler2D texDiffuse;

struct PointLight
{
	vec4 position;  	//16 //0	// xyz position+w radius
	vec4 color; 		//16 //16   // rgb colour+alpha intensity
};

struct SpotLight
{
	vec4 position;  	//16 //0	// xyz position+w radius
	vec4 color; 		//16 //16	// rgb colour+alpha intensity
	vec3 aim;			//16 //32
	float innerAngle;	//4  //48
	float outerAngle;	//4  //52
};

layout(std140) uniform Ambient
{
	vec3 ambientValue;	//12	//0
};

layout(std140) uniform Directional
{
	vec3 directionalDir;  	//12	//0
	vec4 directionalColor;	//16	//16     // note: alpha parameter of colour is the intensity 
};

readonly layout(std430) buffer PointLights
{
	uint num_point;			//4		//0
	PointLight points[]; 	//32	//16
};

readonly layout(std430) buffer SpotLights
{
	uint num_spot;
	SpotLight spots[];
};

out vec4 color;

in vec2 uv0;

void main()
{	
    vec3 ambient = ambientValue * vec3(texture(texDiffuse, uv0));
	
	vec3 result = ambient;
		
	color = vec4(result, 1.0);
}