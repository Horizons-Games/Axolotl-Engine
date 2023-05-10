#pragma once

#include "ComponentLight.h"

enum class AreaType
{
	SPHERE,
	TUBE,
	QUAD,
	DISC
};

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
	ComponentAreaLight(GameObject* parent, AreaType areaType);
	ComponentAreaLight(const float3& color, float intensity, AreaType areaType);
	ComponentAreaLight(const float3& color, float intensity, GameObject* parent, AreaType areaType);
	~ComponentAreaLight();

	const AreaType GetAreaType();
private:

	AreaType areaType;
};

inline const AreaType ComponentAreaLight::GetAreaType()
{
	return areaType;
}

