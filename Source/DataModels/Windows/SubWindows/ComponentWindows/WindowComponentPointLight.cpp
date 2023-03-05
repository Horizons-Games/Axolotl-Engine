#include "WindowComponentPointLight.h"

#include "Application.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"

#include "DataModels/Components/ComponentPointLight.h"
#include "DataModels/Components/ComponentSpotLight.h"

WindowComponentPointLight::WindowComponentPointLight(ComponentPointLight* component) :
	WindowComponentLight("POINT LIGHT", component)
{
}

WindowComponentPointLight::~WindowComponentPointLight()
{
}

void WindowComponentPointLight::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentPointLight* asPointLight = static_cast<ComponentPointLight*>(component);

	if (asPointLight)
	{
		const char* lightTypes[] = { "Point", "Spot" };

		const char* currentType = "Point";

		bool modified = false;

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
							ComponentSpotLight* newSpot =
								static_cast<ComponentSpotLight*>(asPointLight->GetOwner()
									->CreateComponentLight(LightType::SPOT));

							newSpot->SetColor(asPointLight->GetColor());
							newSpot->SetIntensity(asPointLight->GetIntensity());
							newSpot->SetRadius(asPointLight->GetRadius());

							asPointLight->GetOwner()->RemoveComponent(asPointLight);

							App->scene->GetLoadedScene()->UpdateSceneSpotLights();
							App->scene->GetLoadedScene()->RenderSpotLights();

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
			float intensity = asPointLight->GetIntensity();
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

				asPointLight->SetIntensity(intensity);
				modified = true;
			}
			ImGui::PopStyleVar();

			ImGui::Text("Color"); ImGui::SameLine();
			float3 color = asPointLight->GetColor();
			if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			{
				asPointLight->SetColor(color);
				modified = true;
			}

			ImGui::Text("Radius"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			float radius = asPointLight->GetRadius();
			if (ImGui::DragFloat("##Radius", &radius, 0.01f, 0.0001f, std::numeric_limits<float>::max()))
			{
				asPointLight->SetRadius(radius);
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
}
