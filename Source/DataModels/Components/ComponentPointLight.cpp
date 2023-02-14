#include "ComponentPointLight.h"
#include "ComponentTransform.h"
#include "ComponentSpotLight.h"

#include "Application.h"

#include "../Modules/ModuleScene.h"

#include "Scene/Scene.h"

#include "FileSystem/Json.h"

#include "debugdraw.h"

ComponentPointLight::ComponentPointLight() : ComponentLight(LightType::POINT, true) 
{
}

ComponentPointLight::ComponentPointLight(GameObject* parent) :
	ComponentLight(LightType::SPOT, parent, true)
{
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity) :
	ComponentLight(LightType::POINT, color, intensity, true)
{
	this->radius = radius;
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity,
											GameObject* parent) :
	ComponentLight(LightType::POINT, color, intensity, parent, true)
{
	this->radius = radius;
}

ComponentPointLight::~ComponentPointLight()
{
}

void ComponentPointLight::Draw()
{
	if (this->GetActive())
	{
		ComponentTransform* transform =
			static_cast<ComponentTransform*>(this->GetOwner()
				->GetComponent(ComponentType::TRANSFORM));

		float3 position = transform->GetGlobalPosition();

		dd::sphere(position, dd::colors::White, radius);
	}
}

void ComponentPointLight::SaveOptions(Json& meta)
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
	
}

void ComponentPointLight::LoadOptions(Json& meta)
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
	radius = meta["radius"];
}