#ifndef _LOCAL_IBL_INCLUDE_
#define _LOCAL_IBL_INCLUDE_

struct LocalIBL 
{
	samplerCube diffuse;					//0		//8
	samplerCube prefiltered;				//8		//8
	vec4 position; //aligned like float4	//16	//16
	vec4 minParallax;						//32	//16
	vec4 maxParallax;						//48	//16
	mat4 toLocal;							//64	//64
	vec4 minInfluence;						//128	//16
	vec4 maxInfluence;						//144	//16 --> 160
};

#endif