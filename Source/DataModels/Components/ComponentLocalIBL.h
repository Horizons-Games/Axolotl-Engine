#pragma once
#include "Components/ComponentLight.h"

#include "Geometry/AABB.h"
#include "Math/float4x4.h"

#include "GL/glew.h"

class Program;

#define RESOLUTION 512

struct LocalIBL
{
	uint64_t diffuse;						//0		//8
	uint64_t prefiltered;					//8		//8
	float4 position; //aligned like float4	//16	//16
	float4 minParallax;						//32	//16
	float4 maxParallax;						//48	//16
	float4x4 toLocal;						//64	//64
	float4 minInfluence;					//128	//16
	float4 maxInfluence;					//144	//16 --> 160
};

class ComponentLocalIBL : public ComponentLight
{
public:
	ComponentLocalIBL(GameObject* parent);
	~ComponentLocalIBL() override;

	void GenerateMaps();

	void Draw() const override;

	void OnTransformChanged() override;

	const uint64_t& GetHandleIrradiance();
	const GLuint GetIrradiance();
	const uint64_t& GetHandlePreFiltered();
	const GLuint GetPreFiltered();
	
	const AABB& GetParallaxAABB();
	void SetParallaxAABB(AABB& aabb);
	
	const float4x4 GetTransform();
	
	const AABB& GetInfluenceAABB();
	void SetInfluenceAABB(AABB& aabb);

	const float3 GetPosition();
	const Quat& GetRotation();

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

private:
	void RenderToCubeMap(unsigned int cubemapTex, Frustum& frustum, int resolution = RESOLUTION, int mipmapLevel = 0);
	void BindCameraToProgram(Program* program, Frustum& frustum, unsigned int uboCamera);

	GLuint frameBuffer;
	GLuint diffuse;
	GLuint preFiltered;
	
	int numMipMaps;

	AABB parallaxAABB;
	float3 originCenterParallax;

	AABB influenceAABB;
	float3 originCenterInfluence;

	uint64_t handleIrradiance;
	uint64_t handlePreFiltered;
};

inline const GLuint ComponentLocalIBL::GetIrradiance()
{
	return diffuse;
}

inline const GLuint ComponentLocalIBL::GetPreFiltered()
{
	return preFiltered;
}

inline const float3 ComponentLocalIBL::GetPosition()
{
	return parallaxAABB.CenterPoint();
}

inline const float4x4 ComponentLocalIBL::GetTransform()
{
	return float4x4(GetRotation(), GetPosition());
}

inline const AABB& ComponentLocalIBL::GetParallaxAABB()
{
	return parallaxAABB;
}

inline const AABB& ComponentLocalIBL::GetInfluenceAABB()
{
	return influenceAABB;
}