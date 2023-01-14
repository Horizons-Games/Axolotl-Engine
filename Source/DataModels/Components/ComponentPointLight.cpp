#include "ComponentPointLight.h"
#include "ComponentTransform.h"

#include "debugdraw.h"

ComponentPointLight::ComponentPointLight() : ComponentLight(LightType::POINT) 
{
}

ComponentPointLight::ComponentPointLight(GameObject* parent) : ComponentLight(LightType::SPOT, parent)
{
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity) :
	ComponentLight(LightType::POINT, color, intensity)
{
	this->radius = radius;
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::POINT, color, intensity, parent)
{
	this->radius = radius;
}

void ComponentPointLight::Draw()
{
	if (this->GetActive())
	{
		ComponentTransform* transform = (ComponentTransform*)this->GetOwner()->GetComponent(ComponentType::TRANSFORM);

		float3 position = transform->GetPosition();

		dd::sphere(position, dd::colors::White, radius);
	}
}

void ComponentPointLight::Display()
{
}