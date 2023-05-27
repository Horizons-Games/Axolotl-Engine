#pragma once

#include "ComponentLight.h"

struct AreaLightSphere
{
	float4 position;  	// xyz position+w radius
	float4 color; 		// rgb colour+alpha intensity
	float attRadius;	// radius for attenuation
	float padding1;
	float2 padding2;
};

struct AreaLightTube
{
	float4 positionA;
	float4 positionB;
	float4 color; 		// rgb colour+alpha intensity
	float attRadius;	// radius for attenuation
	float padding3;
	float2 padding4;
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
	const float GetHeight();
	const float GetAttRadius();

	void SetAreaType(AreaType newType);
	void SetShapeRadius(float newRadius);
	void SetHeight(float newHeight);
	void SetLightRadius(float newRadius);

private:
	AreaType areaType;
	float shapeRadius;
	float height;
	float attRadius;
};

inline const AreaType ComponentAreaLight::GetAreaType()
{
	return areaType;
}

inline const float ComponentAreaLight::GetShapeRadius()
{
	return shapeRadius;
}

inline const float ComponentAreaLight::GetHeight()
{
	return height;
}

inline const float ComponentAreaLight::GetAttRadius()
{
	return attRadius;
}

inline void ComponentAreaLight::SetAreaType(AreaType newType)
{
	areaType = newType;
}

inline void ComponentAreaLight::SetShapeRadius(float newRadius)
{
	shapeRadius = newRadius;
}

inline void ComponentAreaLight::SetHeight(float newHeight)
{
	height = newHeight;
}

inline void ComponentAreaLight::SetLightRadius(float newRadius)
{
	attRadius = newRadius;
}
