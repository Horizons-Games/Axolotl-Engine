#include "ComponentSpotLight.h"
#include "ComponentTransform.h"

#include "debugdraw.h"

ComponentSpotLight::ComponentSpotLight() : ComponentLight(LightType::SPOT)
{
}

ComponentSpotLight::ComponentSpotLight(GameObject* parent) : ComponentLight(LightType::SPOT, parent)
{
}

ComponentSpotLight::ComponentSpotLight(float radius, float innerAngle, float outerAntgle, 
									   const float3& color, float intensity) :
	ComponentLight(LightType::SPOT, color, intensity)
{
	this->radius = radius;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
}

ComponentSpotLight::ComponentSpotLight(float radius, float innerAngle, float outerAntgle, 
									   const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::SPOT, color, intensity, parent)
{
	this->radius = radius;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
}

void ComponentSpotLight::Display()
{
}

void ComponentSpotLight::Draw()
{
	if (this->GetActive())
	{
		ComponentTransform* transform = (ComponentTransform*)this->GetOwner()->GetComponent(ComponentType::TRANSFORM);

		float3 position = transform->GetPosition();
		float3 forward = transform->GetGlobalForward();

		dd::cone(position, forward * radius, dd::colors::White, outerAngle, 0.0f);
		dd::cone(position, forward * radius, dd::colors::Yellow, innerAngle, 0.0f);
	}
}