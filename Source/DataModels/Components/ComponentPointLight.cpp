#include "ComponentPointLight.h"
#include "ComponentTransform.h"

#include "FileSystem/Json.h"

#include "Modules/ModuleScene.h"

#ifndef ENGINE
#	include "Modules/ModuleDebugDraw.h"
#	include "Modules/ModuleEditor.h"

#	include "Windows/WindowDebug.h"
#endif // ENGINE

#include "Application.h"

#include "debugdraw.h"

ComponentPointLight::ComponentPointLight() : ComponentLight(LightType::POINT, true), radius(1.0f)
{
}

ComponentPointLight::ComponentPointLight(const ComponentPointLight& componentPointLight) :
	ComponentLight(componentPointLight), radius(componentPointLight.GetRadius())
{
}

ComponentPointLight::ComponentPointLight(GameObject* parent) :
	ComponentLight(LightType::POINT, parent, true), radius(1.0f)
{
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity) :
	ComponentLight(LightType::POINT, color, intensity, true), radius(radius)
{
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::POINT, color, intensity, parent, true), radius(radius)
{
}

ComponentPointLight::~ComponentPointLight()
{
}

void ComponentPointLight::Draw()
{
#ifndef ENGINE
	if (!App->editor->GetDebugOptions()->GetDrawPointLight())
	{
		return;
	}
#endif // ENGINE
	if (GetActive() && GetOwner() == App->scene->GetSelectedGameObject())
	{
		ComponentTransform* transform =
			static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));

		float3 position = transform->GetGlobalPosition();

		dd::sphere(position, dd::colors::White, radius);
	}
}

void ComponentPointLight::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;

	meta["color_light_X"] = (float) color.x;
	meta["color_light_Y"] = (float) color.y;
	meta["color_light_Z"] = (float) color.z;

	meta["intensity"] = (float) intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();
	meta["radius"] = (float) radius;
}

void ComponentPointLight::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];

	color.x = (float) meta["color_light_X"];
	color.y = (float) meta["color_light_Y"];
	color.z = (float) meta["color_light_Z"];

	intensity = (float) meta["intensity"];

	lightType = GetLightTypeByName(meta["lightType"]);
	radius = meta["radius"];
}