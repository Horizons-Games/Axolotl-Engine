#include "ComponentPointLight.h"
#include "ComponentTransform.h"
#include "ComponentSpotLight.h"

#include "Application.h"

#include "../Modules/ModuleScene.h"

#include "Scene/Scene.h"

#include "FileSystem/Json.h"

#include "debugdraw.h"
#include "imgui.h"

ComponentPointLight::ComponentPointLight() : ComponentLight(LightType::POINT, true) 
{
}

ComponentPointLight::ComponentPointLight(GameObject* parent) : ComponentLight(LightType::SPOT, parent, true)
{
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity) :
	ComponentLight(LightType::POINT, color, intensity, true)
{
	this->radius = radius;
}

ComponentPointLight::ComponentPointLight(float radius, const float3& color, float intensity, GameObject* parent) :
	ComponentLight(LightType::POINT, color, intensity, parent, true)
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

	bool modified = false;

	if (ImGui::CollapsingHeader("POINT LIGHT", ImGuiTreeNodeFlags_DefaultOpen))
	{
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
						if (lightTypes[i] == "Spot")
						{
							ComponentSpotLight* newSpot = (ComponentSpotLight*)this->GetOwner()->
								CreateComponentLight(LightType::SPOT);
							
							newSpot->SetColor(color);
							newSpot->SetIntensity(intensity);
							newSpot->SetRadius(radius);

							this->GetOwner()->RemoveComponent(this);

							modified = true;
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

			if (modified)
			{
				App->scene->GetLoadedScene()->UpdateScenePointLights();
				App->scene->GetLoadedScene()->RenderPointLights();
			}

			ImGui::EndTable();
		}
	}

	ImGui::Separator();
}

void ComponentPointLight::SaveOptions(Json& meta)
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
	meta["radius"] = (float)radius;
	
}

void ComponentPointLight::LoadOptions(Json& meta)
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
	radius = meta["radius"];
}