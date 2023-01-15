#include "ComponentPointLight.h"
#include "ComponentSpotLight.h"
#include "ComponentTransform.h"

#include "debugdraw.h"
#include "imgui.h"

ComponentPointLight::ComponentPointLight() : ComponentLight(LightType::POINT) 
{
}

ComponentPointLight::ComponentPointLight(GameObject* parent) : ComponentLight(LightType::SPOT, parent)
{
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity) :
	ComponentLight(LightType::POINT, color, intensity)
{
	this->radius = radius;
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::POINT, color, intensity, parent)
{
	this->radius = radius;
}

void ComponentPointLight::Draw()
{
	if (this->GetActive())
	{
		ComponentTransform* transform = (ComponentTransform*)this->GetOwner()->GetComponent(ComponentType::TRANSFORM);

		float3 position = transform->GetPosition();

		dd::sphere(position, dd::colors::White, radius);
	}
}

void ComponentPointLight::Display()
{
	const char* lightTypes[] = { "Point", "Spot" };

	const char* currentType = "Point";
	float intensity = GetIntensity();
	float3 color = GetColor();
	float radius = GetRadius();

	ImGui::Text("POINT LIGHT");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
	if (ImGui::BeginTable("PointLightTable", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Type"); ImGui::SameLine();
		if (ImGui::BeginCombo("##combo", currentType)) 
		{
			for (int i = 0; i < IM_ARRAYSIZE(lightTypes); i++)
			{
				bool isSelected = (currentType == lightTypes[i]);
				if (ImGui::Selectable(lightTypes[i], isSelected))
				{
					//changes type of light
					if (lightTypes[i] == "Spot")
					{
						this->GetOwner()->RemoveComponent(this);
						this->GetOwner()->CreateComponentLight(LightType::SPOT);
						std::vector<ComponentSpotLight*> lights = this->GetOwner()->GetComponentsByType<ComponentSpotLight>(ComponentType::LIGHT);
						for (int i = 0; i < lights.size(); i++) {
							lights[i]->SetIntensity(intensity);
							lights[i]->SetColor(color);
							lights[i]->SetRadius(radius);
						}
					}
				}
				if (isSelected)
				{
					//Shows list of lights
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Text("Intensity"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##Intensity", &intensity, 0.01f,
			0.0f, 1.0f
		); ImGui::PopStyleVar();
		SetIntensity(intensity);

		ImGui::Text("Color"); ImGui::SameLine();
		if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			SetColor(color);

		ImGui::Text("Radius"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##Radius", &radius, 0.01f,
			0.0001f, std::numeric_limits<float>::max()
		); ImGui::PopStyleVar();
		SetRadius(radius);

		ImGui::EndTable();
		ImGui::Separator();
	}
}