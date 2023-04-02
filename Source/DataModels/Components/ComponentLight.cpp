#include "ComponentLight.h"

ComponentLight::ComponentLight(const bool active, GameObject* owner) :
	Component(ComponentType::LIGHT, active, owner, true),
	color(float3(1.0f, 1.0f, 1.0f)),
	intensity(1.0f)
{
}

ComponentLight::ComponentLight(LightType type, bool canBeRemoved) :
	Component(ComponentType::LIGHT, true, nullptr, canBeRemoved),
	color(float3(1.0f, 1.0f, 1.0f)),
	intensity(1.0f),
	lightType(type)
{
}

ComponentLight::ComponentLight(LightType type, GameObject* gameObject, bool canBeRemoved) :
	Component(ComponentType::LIGHT, true, gameObject, canBeRemoved),
	color(float3(1.0f, 1.0f, 1.0f)),
	intensity(1.0f),
	lightType(type)
{
}

ComponentLight::ComponentLight(LightType type, const float3& color, float intensity, bool canBeRemoved) :
	Component(ComponentType::LIGHT, true, nullptr, canBeRemoved),
	color(color),
	intensity(intensity),
	lightType(type)
{
}

ComponentLight::ComponentLight(
	LightType type, const float3& color, float intensity, GameObject* gameObject, bool canBeRemoved) :
	Component(ComponentType::LIGHT, true, gameObject, canBeRemoved),
	color(color),
	intensity(intensity),
	lightType(type)
{
}

ComponentLight::ComponentLight(const ComponentLight& componentLight) :
	Component(componentLight),
	color(componentLight.GetColor()),
	intensity(componentLight.GetIntensity()),
	lightType(componentLight.GetLightType())
{
}

ComponentLight::~ComponentLight()
{
}