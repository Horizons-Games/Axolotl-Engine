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
	int numMipMaps;							//168	//4
	float distortionAmount;					//172	//4
	float4 planeNormal;						//176	//16 -->192
};

class ComponentPlanarReflection : public ComponentLight
{
public:

	ComponentPlanarReflection(GameObject* parent);
	ComponentPlanarReflection(const ComponentPlanarReflection& copyComponent);
	~ComponentPlanarReflection();

	void Draw() const override;

	void OnTransformChanged() override;

	void InitBuffer();
	void Update();
	void UpdateReflection();

	const AABB& GetInfluenceAABB() const;
	void SetInfluenceAABB(AABB& aabb);

	const float3& GetScale() const;
	void ScaleInfluenceAABB(float3& scaling);
	
	const uint64_t& GetHandleReflection();

	const float4x4 GetViewProjMatrix() const;
	const float4x4 GetTransform() const;
	
	const int GetNumMipMaps() const;

	const float GetDistortionAmount() const;
	void SetDistortionAmount(float newAmount);
	
	const float3 GetPlaneNormal() const;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	void SignalEnable() override;
	void SignalDisable() override;

private:
	void BlurReflection();

	const float3& GetPosition() const;
	const Quat& GetRotation() const;

	GLuint frameBuffer;
	GLuint depth;
	GLuint reflectionTex;

	uint64_t handle;

	AABB influenceAABB;

	float3 planeNormal;

	int numMipMaps;

	float distortionAmount;

	Frustum* frustum;
	std::vector<UtilBlur*> utilsBlur;
	float3 originScaling;
	float3 scale;
};

inline const AABB& ComponentPlanarReflection::GetInfluenceAABB() const
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

inline const float4x4 ComponentPlanarReflection::GetViewProjMatrix() const
{
	return frustum->ViewProjMatrix();
}

inline const float4x4 ComponentPlanarReflection::GetTransform() const
{
	return float4x4(GetRotation(), GetPosition());
}

inline const int ComponentPlanarReflection::GetNumMipMaps() const
{
	return numMipMaps;
}

inline const float ComponentPlanarReflection::GetDistortionAmount() const
{
	return distortionAmount;
}

inline void ComponentPlanarReflection::SetDistortionAmount(float newAmount)
{
	distortionAmount = newAmount;
}

inline const float3 ComponentPlanarReflection::GetPlaneNormal() const
{
	return planeNormal;
}
