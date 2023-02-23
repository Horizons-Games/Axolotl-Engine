#include "ComponentSpotLight.h"
#include "ComponentTransform.h"
#include "ComponentPointLight.h"

#include "Application.h"

#include "../Modules/ModuleScene.h"

#include "Scene/Scene.h"

#include "FileSystem/Json.h"

#include "debugdraw.h"

ComponentSpotLight::ComponentSpotLight() : ComponentLight(LightType::SPOT, true)
{
}

ComponentSpotLight::ComponentSpotLight(GameObject* parent) :
	ComponentLight(LightType::SPOT, parent, true)
{
}

ComponentSpotLight::ComponentSpotLight(float radius, float innerAngle, float outerAngle, 
									   const float3& color, float intensity) :
	ComponentLight(LightType::SPOT, color, intensity, true)
{
	this->radius = radius;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
}

ComponentSpotLight::ComponentSpotLight(float radius, float innerAngle, float outerAngle, 
									   const float3& color, float intensity,
									   GameObject* parent) :
	ComponentLight(LightType::SPOT, color, intensity, parent, true)
{
	this->radius = radius;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
}

void ComponentSpotLight::Draw()
{
	if (GetActive())
	{
		ComponentTransform* transform =
			static_cast<ComponentTransform*>(GetOwner()
				->GetComponent(ComponentType::TRANSFORM));

		float3 position = transform->GetGlobalPosition();
		float3 forward = transform->GetGlobalForward().Normalized();

		dd::cone(position, forward * radius, dd::colors::White, outerAngle * radius , 0.0f);
		dd::cone(position, forward * radius, dd::colors::Yellow, innerAngle * radius, 0.0f);
	}
}

void ComponentSpotLight::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	meta["color_light_X"] = (float)color.x;
	meta["color_light_Y"] = (float)color.y;
	meta["color_light_Z"] = (float)color.z;

	meta["intensity"] = (float)intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();
	meta["radius"] = (float)radius;
	meta["innerAngle"] = (float)innerAngle;
	meta["outerAngle"] = (float)outerAngle;
}

void ComponentSpotLight::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

	color.x = (float)meta["color_light_X"];
	color.y = (float)meta["color_light_Y"];
	color.z = (float)meta["color_light_Z"];

	intensity = (float)meta["intensity"];

	lightType = GetLightTypeByName(meta["lightType"]);

	radius = (float)meta["radius"];
	innerAngle = (float)meta["innerAngle"];
	outerAngle = (float)meta["outerAngle"];
}