#include "ComponentLight.h"

ComponentLight::ComponentLight(const bool active, GameObject* owner)
	: Component(ComponentType::LIGHT, active, owner)
{
}

ComponentLight::ComponentLight(LightType type) : Component(ComponentType::LIGHT, true, nullptr)
{
	this->type = type;
};

ComponentLight::ComponentLight(LightType type, const float3& color, float intensity) :
	Component(ComponentType::LIGHT, true, nullptr)
{
	this->type = type;
	this->color = color;
	this->intensity = intensity;
}

void ComponentLight::Draw() 
{
	
}