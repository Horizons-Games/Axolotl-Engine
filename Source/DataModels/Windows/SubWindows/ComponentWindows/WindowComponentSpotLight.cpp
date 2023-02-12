#include "WindowComponentSpotLight.h"

#include "Application.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"

#include "DataModels/Components/ComponentSpotLight.h"
#include "DataModels/Components/ComponentPointLight.h"

WindowComponentSpotLight::WindowComponentSpotLight(const std::weak_ptr<ComponentSpotLight>& component) :
	ComponentWindow("SPOT LIGHT", component)
{
}

void WindowComponentSpotLight::DrawWindowContents()
{
	this->DrawEnableAndDeleteComponent();

	std::shared_ptr<ComponentSpotLight> asSharedSpotLight =
		std::dynamic_pointer_cast<ComponentSpotLight>(this->component.lock());

	if (asSharedSpotLight)
	{
		const char* lightTypes[] = { "Point", "Spot" };

		const char* currentType = "Spot";

		bool modified = false;

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
						if (lightTypes[i] == "Point")
						{
							std::shared_ptr<ComponentPointLight> newPoint =
								std::static_pointer_cast<ComponentPointLight>(asSharedSpotLight->GetOwner().lock()
									->CreateComponentLight(LightType::POINT));

							newPoint->SetColor(asSharedSpotLight->GetColor());
							newPoint->SetIntensity(asSharedSpotLight->GetIntensity());
							newPoint->SetRadius(asSharedSpotLight->GetRadius());

							asSharedSpotLight->GetOwner().lock()->RemoveComponent(asSharedSpotLight);

							App->scene->GetLoadedScene()->UpdateScenePointLights();
							App->scene->GetLoadedScene()->RenderPointLights();

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
			float intensity = asSharedSpotLight->GetIntensity();
			if (ImGui::DragFloat("##Intensity", &intensity, 0.01f, 0.0f, 1.0f))
			{
				asSharedSpotLight->SetIntensity(intensity);
				modified = true;
			}
			ImGui::PopStyleVar();

			ImGui::Text("Color"); ImGui::SameLine();
			float3 color = asSharedSpotLight->GetColor();
			if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			{
				asSharedSpotLight->SetColor(color);
				modified = true;
			}

			ImGui::Text("Radius"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			float radius = asSharedSpotLight->GetRadius();
			if (ImGui::DragFloat("##Radius", &radius, 0.01f, 0.0001f, std::numeric_limits<float>::max()))
			{
				asSharedSpotLight->SetRadius(radius);
				modified = true;
			}
			ImGui::PopStyleVar();

			float innerAngle = asSharedSpotLight->GetInnerAngle();
			float outerAngle = asSharedSpotLight->GetOuterAngle();

			ImGui::Text("Inner Angle"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##Inner", &innerAngle, 0.01f, 0.0001f, 180.0f))
			{
				if (innerAngle <= outerAngle)
				{
					asSharedSpotLight->SetInnerAngle(innerAngle);
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
					asSharedSpotLight->SetOuterAngle(outerAngle);
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
}
