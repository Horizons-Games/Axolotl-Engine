#include "ComponentSpotLight.h"
#include "ComponentPointLight.h"
#include "ComponentTransform.h"

#include "debugdraw.h"
#include "imgui.h"

ComponentSpotLight::ComponentSpotLight() : ComponentLight(LightType::SPOT)
{
}

ComponentSpotLight::ComponentSpotLight(GameObject* parent) : ComponentLight(LightType::SPOT, parent)
{
}

ComponentSpotLight::ComponentSpotLight(float radius, float innerAngle, float outerAngle, 
									   const float3& color, float intensity) :
	ComponentLight(LightType::SPOT, color, intensity)
{
	this->radius = radius;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
}

ComponentSpotLight::ComponentSpotLight(float radius, float innerAngle, float outerAngle, 
									   const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::SPOT, color, intensity, parent)
{
	this->radius = radius;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
}

void ComponentSpotLight::Display()
{
	const char* lightTypes[] = { "Point", "Spot" };

	const char* currentType = "Spot";
	float intensity = GetIntensity();
	float3 color = GetColor();
	float radius = GetRadius();

	ImGui::Text("SPOT LIGHT");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
	if (ImGui::BeginTable("SpotLightTable", 2))
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
					if (lightTypes[i] == "Point")
					{
						this->GetOwner()->RemoveComponent(this);
						this->GetOwner()->CreateComponentLight(LightType::POINT);
						std::vector<ComponentPointLight*> lights = this->GetOwner()->GetComponentsByType<ComponentPointLight>(ComponentType::LIGHT);
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

		float innerAngle = GetInnerAngle();
		float outerAngle = GetOuterAngle();

		ImGui::Text("Inner Angle"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##Inner", &innerAngle, 0.01f,
			0.0001f, 180.0f
		); ImGui::PopStyleVar();

		ImGui::Text("Outer Angle"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##Outer", &outerAngle, 0.01f,
			0.0001f, 180.0f
		); ImGui::PopStyleVar();

		if (innerAngle < outerAngle)
			SetInnerAngle(innerAngle);

		if (outerAngle > innerAngle)
			SetOuterAngle(outerAngle);

		ImGui::EndTable();
		ImGui::Separator();
	}
}

void ComponentSpotLight::Draw()
{
	if (this->GetActive())
	{
		ComponentTransform* transform = (ComponentTransform*)this->GetOwner()->GetComponent(ComponentType::TRANSFORM);

		float3 position = transform->GetPosition();
		float3 forward = transform->GetGlobalForward().Normalized();

		dd::cone(position, forward * radius, dd::colors::White, outerAngle, 0.0f);
		dd::cone(position, forward * radius, dd::colors::Yellow, innerAngle, 0.0f);
	}
}