#pragma once
#include "Components/Component.h"
#include "Auxiliar/Generics/Drawable.h"

#include "Geometry/AABB.h"

#include "GL/glew.h"

class Program;

#define RESOLUTION 512

struct LocalIBL
{
	uint64_t irradiance;					//0		//8
	uint64_t prefiltered;					//8		//8
	float3 position; //aligned like float4	//16	//16
	float3 minParallax;						//32	//16
	float3 maxParallax;						//48	//16
	float4x4 toLocal;						//64	//64 --> 128
};

class ComponentLocalIBL : public Component, public Drawable
{
public:
	ComponentLocalIBL(bool active, GameObject* owner);
	~ComponentLocalIBL() override;

	void Update();

	void Draw() const override;

	const uint64_t& GetHandleIrradiance();
	const uint64_t& GetHandlePreFiltered();
	const float3& GetPosition();
	const Quat& GetRotation();
	const float4x4& GetTransform();
	const AABB& GetAABB();

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

private:
	void BindCameraToProgram(Program* program, Frustum& frustum);
	void RenderToCubeMap(unsigned int cubemapTex, Frustum& frustum, int resolution = RESOLUTION, int mipmapLevel = 0);

	GLuint frameBuffer;
	GLuint irradiance;
	GLuint preFiltered;
	
	int numMipMaps;

	AABB aabb;

	uint64_t handleIrradiance;
	uint64_t handlePreFiltered;
};