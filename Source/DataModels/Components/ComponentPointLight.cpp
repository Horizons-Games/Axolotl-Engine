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
#ifdef ENGINE
	if (!App->IsOnPlayMode() && IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject())
	{
		ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();

		float3 position = transform->GetGlobalPosition();

		dd::sphere(position, dd::colors::White, radius);
	}
#else
	if (!App->GetModule<ModuleEditor>()->GetDebugOptions()->GetDrawPointLight())
	{
		return;
	}
	if (IsEnabled())
	{
		const ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();

		float3 position = transform->GetGlobalPosition();

		dd::sphere(position, dd::colors::White, radius);
	}
#endif // ENGINE
}

void ComponentPointLight::SignalEnable()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (currentScene)
	{
		currentScene->UpdateScenePointLights();
		currentScene->RenderPointLights();
	}
}

void ComponentPointLight::SignalDisable()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (currentScene)
	{
		currentScene->UpdateScenePointLights();
		currentScene->RenderPointLights();
	}
}

void ComponentPointLight::InternalSave(Json& meta)
{
	meta["color_light_X"] = (float) color.x;
	meta["color_light_Y"] = (float) color.y;
	meta["color_light_Z"] = (float) color.z;

	meta["intensity"] = (float) intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();
	meta["radius"] = (float) radius;
}

void ComponentPointLight::InternalLoad(const Json& meta)
{
	color.x = (float) meta["color_light_X"];
	color.y = (float) meta["color_light_Y"];
	color.z = (float) meta["color_light_Z"];

	intensity = (float) meta["intensity"];

	lightType = GetLightTypeByName(meta["lightType"]);
	radius = meta["radius"];
}