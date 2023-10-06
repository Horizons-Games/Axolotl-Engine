#pragma once
#include "Components/ComponentLight.h"

#include "Geometry/AABB.h"
#include "Math/float4x4.h"

#include "GL/glew.h"

class Program;

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
	ComponentLocalIBL(const ComponentLocalIBL& componentLocalIBL);
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

	const float3& GetPosition();
	const Quat& GetRotation();

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	void SignalEnable() override;
	void SignalDisable() override;

private:

	void Initialize();
	void CreateCubemap();
	void RenderToCubeMap(unsigned int cubemapTex, Program* usedProgram, int resolution, int mipmapLevel = 0);
	void CreateVAO();

	GLuint frameBuffer;
	GLuint depth;

	GLuint cubemap;
	GLuint diffuse;
	GLuint preFiltered;
	
	GLuint cubeVAO;
	GLuint cubeVBO;
	
	int numMipMaps;

	AABB parallaxAABB;
	float3 originCenterParallax;

	AABB influenceAABB;
	float3 originCenterInfluence;

	uint64_t handleIrradiance;
	uint64_t handlePreFiltered;

	float3 initialParallaxOffset;
	float3 initialInfluenceOffset;

	bool first;
};

inline const GLuint ComponentLocalIBL::GetIrradiance()
{
	return diffuse;
}

inline const GLuint ComponentLocalIBL::GetPreFiltered()
{
	return preFiltered;
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