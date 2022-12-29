#include "ComponentLight.h"



ComponentLight::ComponentLight(LightType type) : Component(Type::LIGHT, true, nullptr)
{
	this->type = type;
};

ComponentLight::ComponentLight(LightType type, const float3& color, float intensity) :
	Component(Type::LIGHT, true, nullptr)
{
	this->type = type;
	this->color = color;
	this->intensity = intensity;
}

void ComponentLight::Draw() {
	
}