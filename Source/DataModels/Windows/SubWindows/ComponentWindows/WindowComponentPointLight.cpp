#include "WindowComponentPointLight.h"

#include "Application.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"

#include "DataModels/Components/ComponentPointLight.h"
#include "DataModels/Components/ComponentSpotLight.h"

WindowComponentPointLight::WindowComponentPointLight(const std::weak_ptr<ComponentPointLight>& component) :
	ComponentWindow("POINT LIGHT", component)
{
}

void WindowComponentPointLight::DrawWindowContents()
{
	this->DrawEnableAndDeleteComponent();

	std::shared_ptr<ComponentPointLight> asSharedPointLight =
		std::dynamic_pointer_cast<ComponentPointLight>(this->component.lock());

	if (asSharedPointLight)
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
							std::shared_ptr<ComponentSpotLight> newSpot =
								std::static_pointer_cast<ComponentSpotLight>(asSharedPointLight->GetOwner().lock()
									->CreateComponentLight(LightType::SPOT));

							newSpot->SetColor(asSharedPointLight->GetColor());
							newSpot->SetIntensity(asSharedPointLight->GetIntensity());
							newSpot->SetRadius(asSharedPointLight->GetRadius());

							asSharedPointLight->GetOwner().lock()->RemoveComponent(asSharedPointLight);

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
			float intensity = asSharedPointLight->GetIntensity();
			if (ImGui::DragFloat("##Intensity", &intensity, 0.01f, 0.0f, 1.0f))
			{
				asSharedPointLight->SetIntensity(intensity);
				modified = true;
			}
			ImGui::PopStyleVar();

			ImGui::Text("Color"); ImGui::SameLine();
			float3 color = asSharedPointLight->GetColor();
			if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			{
				asSharedPointLight->SetColor(color);
				modified = true;
			}

			ImGui::Text("Radius"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			float radius = asSharedPointLight->GetRadius();
			if (ImGui::DragFloat("##Radius", &radius, 0.01f, 0.0001f, std::numeric_limits<float>::max()))
			{
				asSharedPointLight->SetRadius(radius);
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
