#include "ComponentSpotLight.h"
#include "ComponentTransform.h"
#include "ComponentPointLight.h"

#include "Application.h"

#include "../Modules/ModuleScene.h"

#include "Scene/Scene.h"

#include "FileSystem/Json.h"

#include "debugdraw.h"
#include "imgui.h"

ComponentSpotLight::ComponentSpotLight() : ComponentLight(LightType::SPOT, true)
{
}

ComponentSpotLight::ComponentSpotLight(const std::shared_ptr<GameObject>& parent) :
	ComponentLight(LightType::SPOT, parent, true)
{
}

ComponentSpotLight::ComponentSpotLight(float radius, float innerAngle, float outerAngle, 
									   const float3& color, float intensity) :
	ComponentLight(LightType::SPOT, color, intensity, true)
{
	this->radius = radius;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
}

ComponentSpotLight::ComponentSpotLight(float radius, float innerAngle, float outerAngle, 
									   const float3& color, float intensity,
									   const std::shared_ptr<GameObject>& parent) :
	ComponentLight(LightType::SPOT, color, intensity, parent, true)
{
	this->radius = radius;
	this->innerAngle = innerAngle;
	this->outerAngle = outerAngle;
}

void ComponentSpotLight::Display()
{
	const char* lightTypes[] = { "Point", "Spot" };

	const char* currentType = "Spot";

	bool modified = false;

	if (ImGui::CollapsingHeader("SPOT LIGHT", ImGuiTreeNodeFlags_DefaultOpen))
	{
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
						std::shared_ptr<ComponentPointLight> newPoint =
							std::static_pointer_cast<ComponentPointLight>(this->GetOwner()
								->CreateComponentLight(LightType::POINT));

						newPoint->SetColor(this->color);
						newPoint->SetIntensity(this->intensity);
						newPoint->SetRadius(this->radius);

						this->GetOwner()->RemoveComponent(shared_from_this());

						App->scene->GetLoadedScene()->UpdateScenePointLights();
						App->scene->GetLoadedScene()->RenderPointLights();

						modified = true;
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

			ImGui::Text("Radius"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##Radius", &radius, 0.01f, 0.0001f, std::numeric_limits<float>::max()))
			{
				modified = true;
			}
			ImGui::PopStyleVar();

			float innerAngle = GetInnerAngle();
			float outerAngle = GetOuterAngle();

			ImGui::Text("Inner Angle"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##Inner", &innerAngle, 0.01f, 0.0001f, 180.0f))
			{
				if (innerAngle <= outerAngle)
				{
					SetInnerAngle(innerAngle);
					modified = true;
				}
			}
			ImGui::PopStyleVar();

			ImGui::Text("Outer Angle"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##Outer", &outerAngle, 0.01f, 0.0001f, 180.0f))
			{
				if (outerAngle >= innerAngle)
				{
					SetOuterAngle(outerAngle);
					modified = true;
				}
			}
			ImGui::PopStyleVar();

			if (modified)
			{
				App->scene->GetLoadedScene()->UpdateSceneSpotLights();
				App->scene->GetLoadedScene()->RenderSpotLights();
			}

			ImGui::EndTable();
		}
	}
	
	ImGui::Separator();
}

void ComponentSpotLight::Draw()
{
	if (this->GetActive())
	{
		std::shared_ptr<ComponentTransform> transform =
			std::static_pointer_cast<ComponentTransform>(this->GetOwner()->GetComponent(ComponentType::TRANSFORM));

		float3 position = transform->GetPosition();
		float3 forward = transform->GetGlobalForward().Normalized();

		dd::cone(position, forward * radius, dd::colors::White, outerAngle * radius , 0.0f);
		dd::cone(position, forward * radius, dd::colors::Yellow, innerAngle * radius, 0.0f);
	}
}

void ComponentSpotLight::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	meta["color_light_X"] = (float)color.x;
	meta["color_light_Y"] = (float)color.y;
	meta["color_light_Z"] = (float)color.z;

	meta["intensity"] = (float)intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();
	meta["radius"] = (float)radius;
	meta["innerAngle"] = (float)innerAngle;
	meta["outerAngle"] = (float)outerAngle;
}

void ComponentSpotLight::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

	color.x = (float)meta["color_light_X"];
	color.y = (float)meta["color_light_Y"];
	color.z = (float)meta["color_light_Z"];

	intensity = (float)meta["intensity"];

	lightType = GetLightTypeByName(meta["lightType"]);

	radius = (float)meta["radius"];
	innerAngle = (float)meta["innerAngle"];
	outerAngle = (float)meta["outerAngle"];
}