#ifndef _LIGHTS_INCLUDE_
#define _LIGHTS_INCLUDE_

struct Light 
{
    vec3 position;
    vec3 color;
};

struct PointLight
{
	vec4 position;  	//16 //16	// xyz position+w radius
	vec4 color; 		//16 //16   // rgb colour+alpha intensity
};

struct SpotLight
{
	vec4 position;  	//16 //0	// xyz position+w radius
	vec4 color; 		//16 //16	// rgb colour+alpha intensity
	vec3 aim;			//12 //32
	float innerAngle;	//4  //44
	float outerAngle;	//4  //48   --> 52 
};

struct AreaLightSphere
{
    vec4 position;  	//16 //16   // xyz position+w radius
	vec4 color; 		//16 //32   // rgb colour+alpha intensity
	float lightRadius;	//4	 //48
	float padding1;		//4  //52
	vec2 pading2;		//8  //56   -->64
};

struct AreaLightTube
{
	vec3 positionA;     //16 //16
	vec3 positionB;     //16 //32
	vec4 color; 		//16 //48   // rgb colour+alpha intensity
	float lightRadius;	//4	 //64
	float padding1;		//4  //68
	vec2 pading2;		//8  //72   -->80
};

#endif