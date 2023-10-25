#include "StdAfx.h"

#include "ComponentSpotLight.h"
#include "ComponentTransform.h"

#include "FileSystem/Json.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleDebugDraw.h"

#include "Scene/Scene.h"

#ifndef ENGINE
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
	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

	float baseOuterRadius = radius * math::Tan(outerAngle);
	float3 translation = float3(-0.5f, -0.5f, -0.5f + radius / 2.0f);
	float3 scale = float3(baseOuterRadius * 2.0f, baseOuterRadius * 2.0f, radius);

	transform->TranslateLocalAABB(translation);
	transform->ScaleLocalAABB(scale);
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
	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

	float baseOuterRadius = radius * math::Tan(outerAngle);
	float3 translation = float3(-0.5f, -0.5f, -0.5f + radius / 2.0f);
	float3 scale = float3(baseOuterRadius * 2.0f, baseOuterRadius * 2.0f, radius);

	transform->TranslateLocalAABB(translation);
	transform->ScaleLocalAABB(scale);
}

ComponentSpotLight::ComponentSpotLight(
	float radius, float innerAngle, float outerAngle, const float3& color, float intensity) :
	ComponentLight(LightType::SPOT, color, intensity, true),
	radius(radius),
	innerAngle(innerAngle),
	outerAngle(outerAngle)
{
	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

	float baseOuterRadius = radius * math::Tan(outerAngle);
	float3 translation = float3(-0.5f, -0.5f, -0.5f + radius / 2.0f);
	float3 scale = float3(baseOuterRadius * 2.0f, baseOuterRadius * 2.0f, radius);

	transform->TranslateLocalAABB(translation);
	transform->ScaleLocalAABB(scale);
}

ComponentSpotLight::ComponentSpotLight(
	float radius, float innerAngle, float outerAngle, const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::SPOT, color, intensity, parent, true),
	radius(radius),
	innerAngle(innerAngle),
	outerAngle(outerAngle)
{
	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

	float baseOuterRadius = radius * math::Tan(outerAngle);
	float3 translation = float3(-0.5f, -0.5f, -0.5f + radius / 2.0f);
	float3 scale = float3(baseOuterRadius * 2.0f, baseOuterRadius * 2.0f, radius);

	transform->TranslateLocalAABB(translation);
	transform->ScaleLocalAABB(scale);
}

ComponentSpotLight::~ComponentSpotLight()
{
	deleting = true;
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (currentScene)
	{
		currentScene->UpdateSceneSpotLights();
		currentScene->RenderSpotLights();
	}
}

void ComponentSpotLight::Draw() const
{
	bool canDrawLight =
#ifdef ENGINE
		IsEnabled() && App->GetPlayState() != Application::PlayState::RUNNING &&
		GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject();
#else
		IsEnabled() && App->GetModule<ModuleEditor>()->GetDebugOptions()->GetDrawSpotLight();
#endif // ENGINE

	if (!canDrawLight)
	{
		return;
	}
	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

	float3 position = transform->GetGlobalPosition();
	float3 forward = transform->GetGlobalForward().Normalized();
	
	float baseOuterRadius = radius * math::Tan(outerAngle);
	float baseInnerRadius = radius * math::Tan(innerAngle);

	dd::cone(position, forward * radius, dd::colors::White, baseOuterRadius, 0.0001f);
	dd::cone(position, forward * radius, dd::colors::Yellow, baseInnerRadius, 0.0001f);

#ifdef ENGINE
	if (transform->IsDrawBoundingBoxes())
	{
		App->GetModule<ModuleDebugDraw>()->DrawBoundingBox(transform->GetObjectOBB());
	}
#endif
}

void ComponentSpotLight::SetRadius(float radius)
{
	isDirty = true;
	this->radius = radius;

	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();
	
	float baseOuterRadius = radius * math::Tan(outerAngle);
	float3 translation = float3(-0.5f, -0.5f, -0.5f + radius / 2.0f);
	float3 scale = float3(baseOuterRadius * 2.0f, baseOuterRadius * 2.0f, radius);

	transform->TranslateLocalAABB(translation);
	transform->ScaleLocalAABB(scale);
}

void ComponentSpotLight::OnTransformChanged()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	currentScene->UpdateSceneSpotLight(this);
	currentScene->RenderSpotLight(this);

	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

	float baseOuterRadius = radius * math::Tan(outerAngle);
	float3 translation = float3(-0.5f, -0.5f, -0.5f + radius/2.0f);
	float3 scale = float3(baseOuterRadius * 2.0f, baseOuterRadius * 2.0f, radius);
	
	transform->TranslateLocalAABB(translation);
	transform->ScaleLocalAABB(scale);
}

void ComponentSpotLight::SignalEnable()
{
	ModuleScene* moduleScene = App->GetModule<ModuleScene>();

	if (moduleScene->IsLoading())
	{
		return;
	}

	Scene* currentScene = moduleScene->GetLoadedScene();

	currentScene->UpdateSceneSpotLights();
	currentScene->RenderSpotLights();
}

void ComponentSpotLight::SignalDisable()
{
	ModuleScene* moduleScene = App->GetModule<ModuleScene>();

	if (moduleScene->IsLoading())
	{
		return;
	}

	Scene* currentScene = moduleScene->GetLoadedScene();

	currentScene->UpdateSceneSpotLights();
	currentScene->RenderSpotLights();
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