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
	float3 position; //aligned like float4	//16	//16
	float3 minParallax;						//32	//16
	float3 maxParallax;						//48	//16
	float4x4 toLocal;						//64	//64
	float3 minInfluence;					//128	//16
	float3 maxInfluence;					//144	//16 --> 160
};

class ComponentLocalIBL : public ComponentLight
{
public:
	ComponentLocalIBL(GameObject* parent);
	~ComponentLocalIBL() override;

	void Update();

	void Draw() const override;

	void OnTransformChanged() override;

	const uint64_t& GetHandleIrradiance();
	const uint64_t& GetHandlePreFiltered();
	
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
	void BindCameraToProgram(Program* program, Frustum& frustum);
	void RenderToCubeMap(unsigned int cubemapTex, Frustum& frustum, int resolution = RESOLUTION, int mipmapLevel = 0);

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