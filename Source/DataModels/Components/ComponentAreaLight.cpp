#include "ComponentAreaLight.h"

ComponentAreaLight::ComponentAreaLight() : ComponentLight(LightType::AREA, true)
{
}

ComponentAreaLight::ComponentAreaLight(const ComponentAreaLight& componentAreaLight) :
	ComponentLight(componentAreaLight), areaType(componentAreaLight.areaType)
{
}

ComponentAreaLight::ComponentAreaLight(GameObject* parent, AreaType areaType) :
	ComponentLight(LightType::AREA, parent, true)
{
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, true), areaType(areaType)
{
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, GameObject* parent, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, parent, true), areaType(areaType)
{
}

ComponentAreaLight::~ComponentAreaLight()
{
}
