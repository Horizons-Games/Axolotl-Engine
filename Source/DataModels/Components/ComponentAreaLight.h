#pragma once

#include "ComponentLight.h"

struct AreaLightSphere
{
	float4 position;  	// xyz position+w radius
	float4 color; 		// rgb colour+alpha intensity
};

struct AreaLightTube
{
	float3 positionA;	
	float3 positionB;
	float4 color; 		// rgb colour+alpha intensity
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
	const float GetRadius();

	void SetAreaType(AreaType newType);
	void SetRadius(float newRadius);

private:

	AreaType areaType;
	float radius;
};

inline const AreaType ComponentAreaLight::GetAreaType()
{
	return areaType;
}

inline const float ComponentAreaLight::GetRadius()
{
	return radius;
}

inline void ComponentAreaLight::SetAreaType(AreaType newType)
{
	areaType = newType;
}

inline void ComponentAreaLight::SetRadius(float newRadius)
{
	radius = newRadius;
}

