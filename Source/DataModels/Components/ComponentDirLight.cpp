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

#define DEFAULT_SHADOW_BIAS_X 0.0999f
#define DEFAULT_SHADOW_BIAS_Y 0.005f
#define DEFAULT_ZNEAR_OFFSET -50.0f
#define DEFAULT_BLEEDING_AMOUNT 0.415f
#define DEFAULT_LAMBDA 0.85f

ComponentDirLight::ComponentDirLight() : ComponentLight(LightType::DIRECTIONAL, false)
{
	shadowBias = float2(DEFAULT_SHADOW_BIAS_X, DEFAULT_SHADOW_BIAS_Y);
	zNearFrustumOffset = DEFAULT_ZNEAR_OFFSET;
	bleedingAmount = DEFAULT_BLEEDING_AMOUNT;
}

ComponentDirLight::ComponentDirLight(GameObject* parent) : ComponentLight(LightType::DIRECTIONAL, parent, false)
{
	shadowBias = float2(DEFAULT_SHADOW_BIAS_X, DEFAULT_SHADOW_BIAS_Y);
	zNearFrustumOffset = DEFAULT_ZNEAR_OFFSET;
	bleedingAmount = DEFAULT_BLEEDING_AMOUNT;
}

ComponentDirLight::ComponentDirLight(const float3& color, float intensity) :
	ComponentLight(LightType::DIRECTIONAL, color, intensity, false)
{
	shadowBias = float2(DEFAULT_SHADOW_BIAS_X, DEFAULT_SHADOW_BIAS_Y);
	zNearFrustumOffset = DEFAULT_ZNEAR_OFFSET;
	bleedingAmount = DEFAULT_BLEEDING_AMOUNT;
}

ComponentDirLight::ComponentDirLight(const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::DIRECTIONAL, color, intensity, parent, false)
{
	shadowBias = float2(DEFAULT_SHADOW_BIAS_X, DEFAULT_SHADOW_BIAS_Y);
	zNearFrustumOffset = DEFAULT_ZNEAR_OFFSET;
	bleedingAmount = DEFAULT_BLEEDING_AMOUNT;
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
	meta["color_light_X"] = static_cast<float>(color.x);
	meta["color_light_Y"] = static_cast<float>(color.y);
	meta["color_light_Z"] = static_cast<float>(color.z);

	meta["intensity"] = intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();

	meta["shadow_bias_X"] = static_cast<float>(shadowBias.x);
	meta["shadow_bias_Y"] = static_cast<float>(shadowBias.y);
	meta["z_near_offset"] = zNearFrustumOffset;
	meta["bleeding_amount"] = bleedingAmount;
	meta["lambda"] = lambda;
}

void ComponentDirLight::InternalLoad(const Json& meta)
{
	color.x = static_cast<float>(meta["color_light_X"]);
	color.y = static_cast<float>(meta["color_light_Y"]);
	color.z = static_cast<float>(meta["color_light_Z"]);

	intensity = static_cast<float>(meta["intensity"]);

	lightType = GetLightTypeByName(meta["lightType"]);

	shadowBias.x = static_cast<float>(meta["shadow_bias_X"]);
	shadowBias.y = static_cast<float>(meta["shadow_bias_Y"]);
	zNearFrustumOffset = static_cast<float>(meta["z_near_offset"]);
	bleedingAmount = static_cast<float>(meta["bleeding_amount"]);
	lambda = static_cast<float>(meta["lambda"]);

	if (shadowBias.x == NULL)
	{
		shadowBias.x = DEFAULT_SHADOW_BIAS_X;
		shadowBias.y = DEFAULT_SHADOW_BIAS_Y;
	}
	if (zNearFrustumOffset == NULL)
	{
		zNearFrustumOffset = DEFAULT_ZNEAR_OFFSET;
	}
	if (bleedingAmount == NULL)
	{
		bleedingAmount = DEFAULT_BLEEDING_AMOUNT;
	}
	if (lambda == NULL)
	{
		lambda = DEFAULT_LAMBDA;
	}
}