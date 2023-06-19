#include "ComponentSpotLight.h"
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

ComponentSpotLight::ComponentSpotLight() :
	ComponentLight(LightType::SPOT, true),
	radius(1.0f),
	innerAngle(2.0f),
	outerAngle(2.5f)
{
}

ComponentSpotLight::ComponentSpotLight(const ComponentSpotLight& componentSpotLight) :
	ComponentLight(componentSpotLight),
	radius(componentSpotLight.GetRadius()),
	innerAngle(componentSpotLight.GetInnerAngle()),
	outerAngle(componentSpotLight.GetOuterAngle())
{
}

ComponentSpotLight::ComponentSpotLight(GameObject* parent) :
	ComponentLight(LightType::SPOT, parent, true),
	radius(1.0f),
	innerAngle(2.0f),
	outerAngle(2.5f)
{
}

ComponentSpotLight::ComponentSpotLight(
	float radius, float innerAngle, float outerAngle, const float3& color, float intensity) :
	ComponentLight(LightType::SPOT, color, intensity, true),
	radius(radius),
	innerAngle(innerAngle),
	outerAngle(outerAngle)
{
}

ComponentSpotLight::ComponentSpotLight(
	float radius, float innerAngle, float outerAngle, const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::SPOT, color, intensity, parent, true),
	radius(radius),
	innerAngle(innerAngle),
	outerAngle(outerAngle)
{
}

ComponentSpotLight::~ComponentSpotLight()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (currentScene)
	{
		currentScene->UpdateSceneSpotLights();
		currentScene->RenderSpotLights();
	}
}

void ComponentSpotLight::Draw() const
{
#ifdef ENGINE
	if (IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject())
	{
		ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();

		float3 position = transform->GetGlobalPosition();
		float3 forward = transform->GetGlobalForward().Normalized();

		dd::cone(position, forward * radius, dd::colors::White, outerAngle * radius, 0.0f);
		dd::cone(position, forward * radius, dd::colors::Yellow, innerAngle * radius, 0.0f);
	}
#else
	if (!App->GetModule<ModuleEditor>()->GetDebugOptions()->GetDrawSpotLight())
	{
		return;
	}
	if (IsEnabled())
	{
		const ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();

		float3 position = transform->GetGlobalPosition();
		float3 forward = transform->GetGlobalForward().Normalized();

		dd::cone(position, forward * radius, dd::colors::White, outerAngle * radius, 0.0f);
		dd::cone(position, forward * radius, dd::colors::Yellow, innerAngle * radius, 0.0f);
	}
#endif // ENGINE
}

void ComponentSpotLight::SignalEnable()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (currentScene)
	{
		currentScene->UpdateSceneSpotLights();
		currentScene->RenderSpotLights();
	}
}

void ComponentSpotLight::SignalDisable()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (currentScene)
	{
		currentScene->UpdateSceneSpotLights();
		currentScene->RenderSpotLights();
	}
}

void ComponentSpotLight::InternalSave(Json& meta)
{
	meta["color_light_X"] = (float) color.x;
	meta["color_light_Y"] = (float) color.y;
	meta["color_light_Z"] = (float) color.z;

	meta["intensity"] = (float) intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();
	meta["radius"] = (float) radius;
	meta["innerAngle"] = (float) innerAngle;
	meta["outerAngle"] = (float) outerAngle;
}

void ComponentSpotLight::InternalLoad(const Json& meta)
{
	color.x = (float) meta["color_light_X"];
	color.y = (float) meta["color_light_Y"];
	color.z = (float) meta["color_light_Z"];

	intensity = (float) meta["intensity"];

	lightType = GetLightTypeByName(meta["lightType"]);

	radius = (float) meta["radius"];
	innerAngle = (float) meta["innerAngle"];
	outerAngle = (float) meta["outerAngle"];
}