#include "StdAfx.h"

#include "WindowComponentAreaLight.h"

#include "Application.h"

#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentAreaLight.h"

WindowComponentAreaLight::WindowComponentAreaLight(ComponentAreaLight* component) : 
	WindowComponentLight("AREA LIGHT", component)
{
}

WindowComponentAreaLight::~WindowComponentAreaLight()
{
}

void WindowComponentAreaLight::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentAreaLight* asAreaLight = static_cast<ComponentAreaLight*>(component);

	if (asAreaLight)
	{
		const char* lightTypes[] = { "SPHERE", "TUBE" }; //TODO ADD the rest of types once there are implemented

		const char* currentType;

		switch (asAreaLight->GetAreaType())
		{
		case AreaType::SPHERE:
			currentType = "SPHERE";
			break;
		case AreaType::DISK:
			currentType = "DISK";
			break;
		case AreaType::QUAD:
			currentType = "QUAD";
			break;
		case AreaType::TUBE:
			currentType = "TUBE";
			break;
		default:
			currentType = "NONE";
			break;
		}

		bool modified = false;

		ImGui::Dummy(ImVec2(0.0f, 2.5f));

		if (ImGui::BeginTable("AreaLightTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Type"); ImGui::SameLine();

			if (ImGui::BeginCombo("##combo", currentType))
			{
				for (int i = 0; i < IM_ARRAYSIZE(lightTypes); i++)
				{
					bool isSelected = (currentType == lightTypes[i]);

					if (ImGui::Selectable(lightTypes[i], isSelected)) // Watch this
					{
						if (lightTypes[i] != currentType)
						{
							if (lightTypes[i] == "SPHERE")
							{
								modified = true;
								asAreaLight->SetAreaType(AreaType::SPHERE);
							}
							else if (lightTypes[i] == "TUBE")
							{
								modified = true;
								asAreaLight->SetAreaType(AreaType::TUBE);
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
			float intensity = asAreaLight->GetIntensity();
			if (ImGui::DragFloat("##Intensity", &intensity, 0.01f, 0.0f, max_intensity))
			{
				if (intensity > max_intensity)
				{
					intensity = max_intensity;
				}
				else if (intensity < 0.0f)
				{
					intensity = 0.0f;
				}

				asAreaLight->SetIntensity(intensity);
				modified = true;
			}
			ImGui::PopStyleVar();

			ImGui::Text("Color"); ImGui::SameLine();
			float3 color = asAreaLight->GetColor();
			if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			{
				asAreaLight->SetColor(color);
				modified = true;
			}

			if (currentType == "TUBE")
			{
				ImGui::Text("Height"); ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
				float height = asAreaLight->GetHeight();
				if (ImGui::DragFloat("##Height", &height, 0.01f, 0.0001f, std::numeric_limits<float>::max()))
				{
					asAreaLight->SetHeight(height);
					modified = true;
				}
				ImGui::PopStyleVar();
			}

			ImGui::Text("Shape Radius"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			float shapeRadius = asAreaLight->GetShapeRadius();
			if (ImGui::DragFloat("##ShapeRadius", &shapeRadius, 0.01f, 0.0001f, std::numeric_limits<float>::max()))
			{
				asAreaLight->SetShapeRadius(shapeRadius);
				modified = true;
			}
			ImGui::PopStyleVar();

			ImGui::Text("Attenuation Radius"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			float attRadius = asAreaLight->GetAttRadius();
			if (ImGui::DragFloat("##LightRadius", &attRadius, 0.01f, 0.0001f, std::numeric_limits<float>::max()))
			{
				asAreaLight->SetLightRadius(attRadius);
				modified = true;
			}
			ImGui::PopStyleVar();

			if (modified)
			{
				App->GetModule<ModuleScene>()->GetLoadedScene()->UpdateSceneAreaLights();
				App->GetModule<ModuleScene>()->GetLoadedScene()->RenderAreaLights();
			}

			ImGui::EndTable();
		}
	}
}
