#include "ComponentLight.h"

#include "imgui.h"

ComponentLight::ComponentLight(const bool active, GameObject* owner)
	: Component(ComponentType::LIGHT, active, owner, true)
{
}

ComponentLight::ComponentLight(LightType type) : Component(ComponentType::LIGHT, true, nullptr, true)
{
	this->lightType = type;
};

ComponentLight::ComponentLight(LightType type, GameObject* gameObject) : 
	Component(ComponentType::LIGHT, true, gameObject, true)
{
	this->lightType = type;
}

ComponentLight::ComponentLight(LightType type, const float3& color, float intensity) :
	Component(ComponentType::LIGHT, true, nullptr, true)
{
	this->lightType = type;
	this->color = color;
	this->intensity = intensity;
}

ComponentLight::ComponentLight(LightType type, const float3& color, float intensity, GameObject* gameObject) :
	Component(ComponentType::LIGHT, true, gameObject, true)
{
	this->lightType = type;
	this->color = color;
	this->intensity = intensity;
	
}

void ComponentLight::Display()
{
	ImGui::Text("LIGHT COMPONENT");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
}