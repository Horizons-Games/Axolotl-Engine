#include "ComponentDirLight.h"
#include "ComponentTransform.h"

#include "debugdraw.h"
#include "imgui.h"

ComponentDirLight::ComponentDirLight() : ComponentLight(LightType::DIRECTIONAL) 
{
}

ComponentDirLight::ComponentDirLight(GameObject* parent) : ComponentLight(LightType::DIRECTIONAL, parent)
{
}

ComponentDirLight::ComponentDirLight(const float3& color, float intensity) :
	ComponentLight(LightType::DIRECTIONAL, color, intensity)
{
}

ComponentDirLight::ComponentDirLight(const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::DIRECTIONAL, color, intensity, parent)
{
}

void ComponentDirLight::Display()
{
	const char* lightTypes[] = { "Point", "Spot" };

	ImGui::Text("DIRECTIONAL LIGHT");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
	if (ImGui::BeginTable("DirLightTable", 2))
	{
		ImGui::TableNextColumn();

		float intensity = GetIntensity();
		ImGui::Text("Intensity"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##Intensity", &intensity, 0.01f,
			0.0f, 1.0f
		); ImGui::PopStyleVar();
		SetIntensity(intensity);

		static float3 color = GetColor();
		ImGui::Text("Color"); ImGui::SameLine();
		if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			SetColor(color);

		ImGui::EndTable();
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