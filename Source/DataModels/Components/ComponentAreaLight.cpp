#include "ComponentAreaLight.h"

#include "ComponentTransform.h"

#include "FileSystem/Json.h"

#include "Application.h"

#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#ifndef ENGINE
	#include "Modules/ModuleDebugDraw.h"
	#include "Modules/ModuleEditor.h"

	#include "Windows/WindowDebug.h"
#endif // ENGINE

#include "debugdraw.h"

ComponentAreaLight::ComponentAreaLight() :
	ComponentLight(LightType::AREA, true),
	areaType(AreaType::SPHERE),
	shapeRadius(1.f),
	attRadius(1.f),
	height(1.f)
{
}

ComponentAreaLight::ComponentAreaLight(const ComponentAreaLight& componentAreaLight) :
	ComponentLight(componentAreaLight),
	areaType(componentAreaLight.areaType),
	shapeRadius(componentAreaLight.shapeRadius),
	attRadius(componentAreaLight.attRadius),
	height(componentAreaLight.height)
{
}

ComponentAreaLight::ComponentAreaLight(AreaType areaType, GameObject* parent) :
	ComponentLight(LightType::AREA, parent, true),
	areaType(areaType),
	shapeRadius(1.f),
	attRadius(1.f),
	height(1.f)
{
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, true),
	areaType(areaType),
	shapeRadius(1.f),
	attRadius(1.f),
	height(1.f)
{
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, GameObject* parent, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, parent, true),
	areaType(areaType),
	shapeRadius(1.f),
	attRadius(1.f),
	height(1.f)
{
}

ComponentAreaLight::~ComponentAreaLight()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (currentScene)
	{
		currentScene->UpdateSceneAreaLights();
		currentScene->RenderAreaLights();
	}
}

void ComponentAreaLight::Draw() const
{
#ifndef ENGINE
	if (!App->GetModule<ModuleEditor>()->GetDebugOptions()->GetDrawAreaLight())
	{
		return;
	}
	if (IsEnabled())
	{
		ComponentTransform* transform = owner->GetComponent<ComponentTransform>();

		float3 position = transform->GetGlobalPosition();
		if (areaType == AreaType::SPHERE)
		{
			dd::sphere(position, dd::colors::White, shapeRadius);
			dd::sphere(position, dd::colors::Coral, attRadius + shapeRadius);
		}
		else if (areaType == AreaType::TUBE)
		{
			Quat matrixRotation = transform->GetGlobalRotation();
			float3 forward = (matrixRotation * float3(0, 1.f, 0)).Normalized();
			float3 translation = transform->GetGlobalPosition();
			float3 pointA = float3(0, 0.5f, 0) * height;
			float3 pointB = float3(0, -0.5f, 0) * height;

			// Apply rotation & translation
			pointA = (matrixRotation * pointA) + translation;
			pointB = (matrixRotation * pointB) + translation;

			dd::cone(pointB, forward * height, dd::colors::White, shapeRadius, shapeRadius);
			dd::cone(pointB, forward * height, dd::colors::Coral, attRadius + shapeRadius, attRadius + shapeRadius);
			dd::sphere(pointA, dd::colors::Coral, attRadius + shapeRadius);
			dd::sphere(pointB, dd::colors::Coral, attRadius + shapeRadius);
		}
		else if (areaType == AreaType::QUAD)
		{
		}
		else if (areaType == AreaType::DISK)
		{
		}
	}
#else
	if (IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject())
	{
		ComponentTransform* transform = owner->GetComponent<ComponentTransform>();

		float3 position = transform->GetGlobalPosition();
		if (areaType == AreaType::SPHERE)
		{
			dd::sphere(position, dd::colors::White, shapeRadius);
			dd::sphere(position, dd::colors::Coral, attRadius + shapeRadius);
		}
		else if (areaType == AreaType::TUBE)
		{
			Quat matrixRotation = transform->GetGlobalRotation();
			float3 forward = (matrixRotation * float3(0, 1.f, 0)).Normalized();
			float3 translation = transform->GetGlobalPosition();
			float3 pointA = float3(0, 0.5f, 0) * height;
			float3 pointB = float3(0, -0.5f, 0) * height;

			// Apply rotation & translation
			pointA = (matrixRotation * pointA) + translation;
			pointB = (matrixRotation * pointB) + translation;

			dd::cone(pointB, forward * height, dd::colors::White, shapeRadius, shapeRadius);
			
			// attenuation shape
			dd::cone(pointB, forward * height, dd::colors::Coral, attRadius + shapeRadius, attRadius + shapeRadius);
			dd::sphere(pointA, dd::colors::Coral, attRadius + shapeRadius);
			dd::sphere(pointB, dd::colors::Coral, attRadius + shapeRadius);
		}
		else if (areaType == AreaType::QUAD)
		{
		}
		else if (areaType == AreaType::DISK)
		{
		}
	}
#endif // ENGINE
}

void ComponentAreaLight::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;

	meta["color_light_X"] = (float) color.x;
	meta["color_light_Y"] = (float) color.y;
	meta["color_light_Z"] = (float) color.z;

	meta["intensity"] = (float) intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();

	meta["areaType"] = GetNameByAreaType(areaType).c_str();
	meta["shapeRadius"] = (float) shapeRadius;
	meta["height"] = (float) height;
	meta["attRadius"] = (float) attRadius;
}

void ComponentAreaLight::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];

	color.x = (float) meta["color_light_X"];
	color.y = (float) meta["color_light_Y"];
	color.z = (float) meta["color_light_Z"];

	intensity = (float) meta["intensity"];

	lightType = GetLightTypeByName(meta["lightType"]);

	areaType = GetAreaTypeByName(meta["areaType"]);
	shapeRadius = (float) meta["shapeRadius"];
	height = (float) meta["height"];
	attRadius = (float) meta["attRadius"];
}
