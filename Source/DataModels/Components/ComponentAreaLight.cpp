#include "StdAfx.h"

#include "ComponentAreaLight.h"

#include "ComponentTransform.h"

#include "FileSystem/Json.h"

#include "Application.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleDebugDraw.h"

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
	deleting = true;
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	
	if (currentScene)
	{
		if (areaType == AreaType::SPHERE)
		{
			currentScene->UpdateSceneAreaSpheres();
			currentScene->RenderAreaSpheres();
		}
		else
		{
			currentScene->UpdateSceneAreaTubes();
			currentScene->RenderAreaTubes();
		}
	}
}

void ComponentAreaLight::SignalEnable()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (areaType == AreaType::SPHERE)
	{
		currentScene->UpdateSceneAreaSpheres();
		currentScene->RenderAreaSpheres();
	}
	else
	{
		currentScene->UpdateSceneAreaTubes();
		currentScene->RenderAreaTubes();
	}
}

void ComponentAreaLight::SignalDisable()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (areaType == AreaType::SPHERE)
	{
		currentScene->UpdateSceneAreaSpheres();
		currentScene->RenderAreaSpheres();
	}
	else
	{
		currentScene->UpdateSceneAreaTubes();
		currentScene->RenderAreaTubes();
	}
}

void ComponentAreaLight::Draw() const
{
	bool canDrawLight =
#ifdef ENGINE
		IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject();
#else
		IsEnabled() && App->GetModule<ModuleEditor>()->GetDebugOptions()->GetDrawAreaLight();
#endif // ENGINE

	if (!canDrawLight)
	{
		return;
	}

	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();
	float3 position = transform->GetGlobalPosition();

	switch (areaType)
	{
	case AreaType::SPHERE:
	{
		dd::sphere(position, dd::colors::White, shapeRadius);
		// attenuation shape
		dd::sphere(position, dd::colors::Coral, attRadius + shapeRadius);
		break;
	}
	case AreaType::TUBE:
	{
		Quat matrixRotation = transform->GetGlobalRotation();
		float3 forward = (matrixRotation * float3(0, 1.f, 0)).Normalized();
		float3 translation = position;
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

		break;
	}
	case AreaType::QUAD:
		break;
	case AreaType::DISK:
		break;
	}

#ifdef ENGINE
	if (transform->IsDrawBoundingBoxes())
	{
		App->GetModule<ModuleDebugDraw>()->DrawBoundingBox(transform->GetObjectOBB());
	}
#endif
}

void ComponentAreaLight::SetShapeRadius(float newRadius)
{
	isDirty = true;
	shapeRadius = newRadius;

	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();
	float radius = (shapeRadius + attRadius) * 2.0f;

	switch (areaType)
	{
		case AreaType::SPHERE:
		{
			float3 scale = float3(radius);
			transform->ScaleLocalAABB(scale);
			break;
		}
		case AreaType::TUBE:
		{
			float3 scale = float3(radius, height + radius, radius);
			transform->ScaleLocalAABB(scale);
			break;
		}
	}
}

void ComponentAreaLight::SetLightRadius(float newRadius)
{
	isDirty = true;
	attRadius = newRadius;

	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();
	float radius = (shapeRadius + attRadius) * 2.0f;

	switch (areaType)
	{
		case AreaType::SPHERE:
		{
			float3 scale = float3(radius);
			transform->ScaleLocalAABB(scale);
			break;
		}
		case AreaType::TUBE:
		{
			float3 scale = float3(radius, height + radius, radius);
			transform->ScaleLocalAABB(scale);
			break;
		}
	}
}

void ComponentAreaLight::SetHeight(float newHeight)
{
	isDirty = true;
	height = newHeight;

	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();
	float radius = (shapeRadius + attRadius) * 2.0f;
	float3 scale = float3(radius, height + radius, radius);
	transform->ScaleLocalAABB(scale);
}

void ComponentAreaLight::OnTransformChanged()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	
	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();
	float radius = (shapeRadius + attRadius) * 2.0f;

	switch (areaType)
	{
		case AreaType::SPHERE:
		{
			currentScene->UpdateSceneAreaSphere(this);
			currentScene->RenderAreaSphere(this);

			float3 translation = float3(-0.5f);
			float3 scale = float3(radius);

			transform->TranslateLocalAABB(translation);
			transform->ScaleLocalAABB(scale);

			break;
		}

		case AreaType::TUBE:
		{
			currentScene->UpdateSceneAreaTube(this);
			currentScene->RenderAreaTube(this);

			float3 translation = float3(-0.5f);
			float3 scale = float3(radius, height + radius, radius);

			transform->TranslateLocalAABB(translation);
			transform->ScaleLocalAABB(scale);

			break;
		}
	}
}

void ComponentAreaLight::InternalSave(Json& meta)
{
	meta["color_light_X"] = color.x;
	meta["color_light_Y"] = color.y;
	meta["color_light_Z"] = color.z;

	meta["intensity"] = intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();

	meta["areaType"] = GetNameByAreaType(areaType).c_str();
	meta["shapeRadius"] = shapeRadius;
	meta["height"] = height;
	meta["attRadius"] = attRadius;
}

void ComponentAreaLight::InternalLoad(const Json& meta)
{
	color.x = static_cast<float>(meta["color_light_X"]);
	color.y = static_cast<float>(meta["color_light_Y"]);
	color.z = static_cast<float>(meta["color_light_Z"]);

	intensity = static_cast<float>(meta["intensity"]);

	lightType = GetLightTypeByName(meta["lightType"]);

	areaType = GetAreaTypeByName(meta["areaType"]);
	shapeRadius = static_cast<float>(meta["shapeRadius"]);
	height = static_cast<float>(meta["height"]);
	attRadius = static_cast<float>(meta["attRadius"]);
}
