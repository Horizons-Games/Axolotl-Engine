#pragma once

#include <memory>
#include "ComponentLight.h"

class ResourceMesh;

struct AreaLightSphere
{
	float4 position;  	// xyz position+w radius
	float4 color; 		// rgb colour+alpha intensity
	float lightRadius;	// radius for attenuation
	float padding1;
	float2 padding2;
};

struct AreaLightTube
{
	float3 positionA;	
	float3 positionB;
	float4 color; 		// rgb colour+alpha intensity
	float lightRadius;	// radius for attenuation
	float padding1;
	float2 padding2;
};

class ComponentAreaLight : public ComponentLight
{
public:
	ComponentAreaLight();
	ComponentAreaLight(const ComponentAreaLight& componentAreaLight);
	ComponentAreaLight(AreaType areaType, GameObject* parent);
	ComponentAreaLight(const float3& color, float intensity, AreaType areaType);
	ComponentAreaLight(const float3& color, float intensity, GameObject* parent, AreaType areaType);
	~ComponentAreaLight();

	const AreaType GetAreaType();
	const float GetShapeRadius();
	const float GetLightRadius();

	void SetAreaType(AreaType newType);
	void SetShapeRadius(float newRadius);
	void SetLightRadius(float newRadius);

private:
	AreaType areaType;
	float shapeRadius;
	float lightRadius;
};

inline const AreaType ComponentAreaLight::GetAreaType()
{
	return areaType;
}

inline const float ComponentAreaLight::GetShapeRadius()
{
	return shapeRadius;
}

inline const float ComponentAreaLight::GetLightRadius()
{
	return lightRadius;
}

inline void ComponentAreaLight::SetAreaType(AreaType newType)
{
	areaType = newType;
}

inline void ComponentAreaLight::SetShapeRadius(float newRadius)
{
	shapeRadius = newRadius;
}

inline void ComponentAreaLight::SetLightRadius(float newRadius)
{
	lightRadius = newRadius;
}
