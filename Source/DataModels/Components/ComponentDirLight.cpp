#include "StdAfx.h"

#include "ComponentDirLight.h"

#include "ComponentTransform.h"

#include "Modules/ModuleScene.h"

#include "Scene/Scene.h"

#ifndef ENGINE
	#include "Modules/ModuleDebugDraw.h"
	#include "Modules/ModuleEditor.h"

	#include "Windows/WindowDebug.h"
#endif // ENGINE

#include "FileSystem/Json.h"

#include "Application.h"

#include "debugdraw.h"

ComponentDirLight::ComponentDirLight() : ComponentLight(LightType::DIRECTIONAL, false)
{
}

ComponentDirLight::ComponentDirLight(GameObject* parent) : ComponentLight(LightType::DIRECTIONAL, parent, false)
{
}

ComponentDirLight::ComponentDirLight(const float3& color, float intensity) :
	ComponentLight(LightType::DIRECTIONAL, color, intensity, false)
{
}

ComponentDirLight::ComponentDirLight(const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::DIRECTIONAL, color, intensity, parent, false)
{
}

ComponentDirLight::~ComponentDirLight()
{
}

void ComponentDirLight::Draw() const
{
#ifndef ENGINE
	if (!App->GetModule<ModuleEditor>()->GetDebugOptions()->GetDrawDirLight())
	{
		return;
	}
	if (IsEnabled())
	{
		const ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

		float3 position = transform->GetGlobalPosition();
		float3 forward = transform->GetGlobalForward();

		float radius = 0.2f;

		for (int i = 0; i < 5; ++i)
		{
			float theta = (2.0f * math::pi * float(i)) / 5.0f;
			float x = radius * math::Cos(theta);
			float y = radius * math::Sin(theta);

			float3 from = position;
			from.x += x;
			from.y += y;

			float3 to = position + forward;
			to.x += x;
			to.y += y;

			dd::arrow(from, to, dd::colors::White, 0.05f);
		}
	}
#else
	if (IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject())
	{
		ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

		float3 position = transform->GetGlobalPosition();
		float3 forward = transform->GetGlobalForward();

		float radius = 0.2f;

		for (int i = 0; i < 5; ++i)
		{
			float theta = (2.0f * math::pi * float(i)) / 5.0f;
			float x = radius * math::Cos(theta);
			float y = radius * math::Sin(theta);

			float3 from = position;
			from.x += x;
			from.y += y;

			float3 to = position + forward;
			to.x += x;
			to.y += y;

			dd::arrow(from, to, dd::colors::White, 0.05f);
		}
	}
#endif // ENGINE
}

void ComponentDirLight::OnTransformChanged()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	currentScene->RenderDirectionalLight();
}

void ComponentDirLight::InternalSave(Json& meta)
{
	meta["color_light_X"] = (float) color.x;
	meta["color_light_Y"] = (float) color.y;
	meta["color_light_Z"] = (float) color.z;

	meta["intensity"] = (float) intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();
}

void ComponentDirLight::InternalLoad(const Json& meta)
{
	color.x = (float) meta["color_light_X"];
	color.y = (float) meta["color_light_Y"];
	color.z = (float) meta["color_light_Z"];

	intensity = (float) meta["intensity"];

	lightType = GetLightTypeByName(meta["lightType"]);
}