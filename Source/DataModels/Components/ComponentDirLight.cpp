#include "ComponentDirLight.h"
#include "ComponentTransform.h"

#include "Application.h"

#include "../Modules/ModuleScene.h"

#include "Scene/Scene.h"

#include "FileSystem/Json.h"

#include "debugdraw.h"
#include "imgui.h"

#include "GL/glew.h"

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

void ComponentDirLight::Display()
{
	const char* lightTypes[] = { "Point", "Spot" };

	bool modified = false;

	if (ImGui::CollapsingHeader("DIRECTIONAL LIGHT", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Dummy(ImVec2(0.0f, 2.5f));

		if (ImGui::BeginTable("DirLightTable", 2))
		{
			ImGui::TableNextColumn();

			ImGui::Text("Intensity"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##Intensity", &intensity, 0.01f, 0.0f, 1.0f))
			{
				modified = true;
			}
			ImGui::PopStyleVar();

			ImGui::Text("Color"); ImGui::SameLine();
			if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			{
				modified = true;
			}

			if (modified)
			{
				App->scene->GetLoadedScene()->RenderDirectionalLight();
			}

			ImGui::EndTable();
		}

		ImGui::Separator();
	}
}

void ComponentDirLight::Draw()
{
	if (this->GetActive())
	{
		ComponentTransform* transform = (ComponentTransform*)this->GetOwner()->GetComponent(ComponentType::TRANSFORM);

		float3 position = transform->GetPosition();
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
}

void ComponentDirLight::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["owner"] = (GameObject*)owner;
	meta["removed"] = (bool)canBeRemoved;

	meta["color_light_X"] = (float)color.x;
	meta["color_light_Y"] = (float)color.y;
	meta["color_light_Z"] = (float)color.z;

	meta["intensity"] = (float)intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();
	
}

void ComponentDirLight::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	//owner = (GameObject*) meta["owner"];
	canBeRemoved = (bool)meta["removed"];

	color.x = (float)meta["color_light_X"];
	color.y = (float)meta["color_light_Y"];
	color.z = (float)meta["color_light_Z"];

	intensity = (float)meta["intensity"];

	lightType = GetLightTypeByName(meta["lightType"]);
}