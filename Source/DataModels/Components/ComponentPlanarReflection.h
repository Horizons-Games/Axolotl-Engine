#pragma once

#include "ComponentLight.h"

#include "Geometry/AABB.h"
#include "Geometry/Frustum.h"

#include "GL/glew.h"

class Program;
class UtilBlur;

struct PlanarReflection
{
	float4x4 toLocal;						//0		//64
	float4x4 viewProj;						//64	//64
	float4 minInfluence;					//128	//16
	float4 maxInfluence;					//144	//16
	uint64_t reflection;					//160	//8 
	float4 padding1;						//168	//16
	float2 padding2;						//184	//8	 -->192
};

class ComponentPlanarReflection : public ComponentLight
{
public:

	ComponentPlanarReflection(GameObject* parent);
	~ComponentPlanarReflection();

	void Draw() const override;

	void OnTransformChanged() override;

	void InitBuffer();
	void Update();
	void UpdateReflection();

	const AABB& GetInfluenceAABB();
	void SetInfluenceAABB(AABB& aabb);

	const float3& GetScale() const;
	void ScaleInfluenceAABB(float3& scaling);
	
	const uint64_t& GetHandleReflection();

	const float4x4 GetViewProj() const;
	const float4x4 GetTransform();

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

private:
	const float3& GetPosition();
	const Quat& GetRotation();

	GLuint frameBuffer;
	GLuint depth;
	GLuint reflectionTex;

	uint64_t handle = 0;

	AABB influenceAABB;

	Frustum* frustum;

	float3 planeNormal;

	float3 originScaling;
	float3 scale;

	UtilBlur* utilBlur;
};

inline const AABB& ComponentPlanarReflection::GetInfluenceAABB()
{
	return influenceAABB;
}

inline void ComponentPlanarReflection::SetInfluenceAABB(AABB& aabb)
{
	influenceAABB = aabb;
}

inline const float3& ComponentPlanarReflection::GetScale() const
{
	return scale;
}

inline const float4x4 ComponentPlanarReflection::GetViewProj() const
{
	return frustum->ProjectionMatrix() * frustum->ViewMatrix();
}

inline const float4x4 ComponentPlanarReflection::GetTransform()
{
	return float4x4(GetRotation(), GetPosition());
}