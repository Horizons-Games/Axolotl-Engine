#include "ComponentPointLight.h"
#include "ComponentTransform.h"

#include "FileSystem/Json.h"

#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#ifndef ENGINE
	#include "Modules/ModuleDebugDraw.h"
	#include "Modules/ModuleEditor.h"

	#include "Windows/WindowDebug.h"
#endif // ENGINE

#include "Application.h"

#include "debugdraw.h"

ComponentPointLight::ComponentPointLight() : ComponentLight(LightType::POINT, true), radius(1.0f)
{
}

ComponentPointLight::ComponentPointLight(const ComponentPointLight& componentPointLight) :
	ComponentLight(componentPointLight),
	radius(componentPointLight.GetRadius())
{
}

ComponentPointLight::ComponentPointLight(GameObject* parent) :
	ComponentLight(LightType::POINT, parent, true),
	radius(1.0f)
{
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity) :
	ComponentLight(LightType::POINT, color, intensity, true),
	radius(radius)
{
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::POINT, color, intensity, parent, true),
	radius(radius)
{
}

ComponentPointLight::~ComponentPointLight()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (currentScene)
	{
		currentScene->UpdateScenePointLights();
		currentScene->RenderPointLights();
	}
}

void ComponentPointLight::Draw() const
{
#ifndef ENGINE
	if (!App->GetModule<ModuleEditor>()->GetDebugOptions()->GetDrawPointLight())
	{
		return;
	}
	if (IsEnabled())
	{
		const ComponentTransform* transform = owner->GetComponent<ComponentTransform>();

		float3 position = transform->GetGlobalPosition();

		dd::sphere(position, dd::colors::White, radius);
	}
#else
	if (IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject())
	{
		ComponentTransform* transform = owner->GetComponent<ComponentTransform>();

		float3 position = transform->GetGlobalPosition();

		dd::sphere(position, dd::colors::White, radius);
	}
#endif // ENGINE
}

void ComponentPointLight::Enable()
{
	Component::Enable();

	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (currentScene)
	{
		currentScene->UpdateScenePointLights();
		currentScene->RenderPointLights();
	}
}

void ComponentPointLight::Disable()
{
	Component::Disable();

	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (currentScene)
	{
		currentScene->UpdateScenePointLights();
		currentScene->RenderPointLights();
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