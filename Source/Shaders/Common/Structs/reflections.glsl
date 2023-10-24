#ifndef _REFLECTIONS_INCLUDE_
#define _REFLECTIONS_INCLUDE_

struct LocalIBL 
{
	samplerCube diffuse;					//0		//8
	samplerCube prefiltered;				//8		//8
	vec4 position;							//16	//16
	vec4 minParallax;						//32	//16
	vec4 maxParallax;						//48	//16
	mat4 toLocal;							//64	//64
	vec4 minInfluence;						//128	//16
	vec4 maxInfluence;						//144	//16 --> 160
};

struct PlanarReflection
{
	mat4 toLocal;						//0		//64
	mat4 viewProj;						//64	//64
	vec4 minInfluence;					//128	//16
	vec4 maxInfluence;					//144	//16
	sampler2D reflection;				//160	//8 
	int numMipMaps;						//168	//4
	float distortionAmount;				//172	//4
	vec4 planeNormal;					//176	//16 -->192
};

#endif