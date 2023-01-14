#include "ComponentLight.h"

ComponentLight::ComponentLight(LightType type) : Component(ComponentType::LIGHT, true, nullptr)
{
	this->type = type;
};

ComponentLight::ComponentLight(LightType type, GameObject* gameObject) : 
	Component(ComponentType::LIGHT, true, gameObject)
{
	this->type = type;
}

ComponentLight::ComponentLight(LightType type, const float3& color, float intensity) :
	Component(ComponentType::LIGHT, true, nullptr)
{
	this->type = type;
	this->color = color;
	this->intensity = intensity;
}

ComponentLight::ComponentLight(LightType type, const float3& color, float intensity, GameObject* gameObject) :
	Component(ComponentType::LIGHT, true, gameObject)
{
	this->type = type;
	this->color = color;
	this->intensity = intensity;
}