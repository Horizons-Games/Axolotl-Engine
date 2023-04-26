#include "WindowComponentSpotLight.h"

#include "Application.h"

#include "ModuleScene.h"
#include "ModuleEditor.h"

#include "DataModels/Scene/Scene.h"

#include "DataModels/Components/ComponentSpotLight.h"
#include "DataModels/Components/ComponentPointLight.h"

WindowComponentSpotLight::WindowComponentSpotLight(ComponentSpotLight* component) :
	WindowComponentLight("SPOT LIGHT", component)
{
}

WindowComponentSpotLight::~WindowComponentSpotLight()
{
}

void WindowComponentSpotLight::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentSpotLight* asSpotLight = static_cast<ComponentSpotLight*>(component);

	if (asSpotLight)
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
							ComponentPointLight* newPoint =
								static_cast<ComponentPointLight*>(asSpotLight->GetOwner()
									->CreateComponentLight(LightType::POINT));

							newPoint->SetColor(asSpotLight->GetColor());
							newPoint->SetIntensity(asSpotLight->GetIntensity());
							newPoint->SetRadius(asSpotLight->GetRadius());

							asSpotLight->GetOwner()->RemoveComponent(asSpotLight);

							App->GetModule<ModuleScene>()->GetLoadedScene()->UpdateScenePointLights();
							App->GetModule<ModuleScene>()->GetLoadedScene()->RenderPointLights();

							modified = true;
							App->GetModule<ModuleEditor>()->RefreshInspector();
							ImGui::EndCombo();
							ImGui::EndTable();
							return;
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
			float intensity = asSpotLight->GetIntensity();
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

				asSpotLight->SetIntensity(intensity);
				modified = true;
			}
			ImGui::PopStyleVar();

			ImGui::Text("Color"); ImGui::SameLine();
			float3 color = asSpotLight->GetColor();
			if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			{
				asSpotLight->SetColor(color);
				modified = true;
			}

			ImGui::Text("Radius"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			float radius = asSpotLight->GetRadius();
			if (ImGui::DragFloat("##Radius", &radius, 0.01f, 0.0001f, std::numeric_limits<float>::max()))
			{
				asSpotLight->SetRadius(radius);
				modified = true;
			}
			ImGui::PopStyleVar();

			float innerAngle = RadToDeg(asSpotLight->GetInnerAngle());
			float outerAngle = RadToDeg(asSpotLight->GetOuterAngle());

			ImGui::Text("Inner Angle"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##Inner", &innerAngle, 0.1f, 0.0f, 90.0f, "%.1f"))
			{
				innerAngle > 90.0f ? innerAngle = 90.0f : innerAngle;
				innerAngle < 0.0f ? innerAngle = 0.0f : innerAngle;

				if (innerAngle <= outerAngle)
				{
					asSpotLight->SetInnerAngle(DegToRad(innerAngle));
					modified = true;
				}
			}
			ImGui::PopStyleVar();

			ImGui::Text("Outer Angle"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##Outer", &outerAngle, 0.1f, 0.0f, 90.0f, "%.1f"))
			{
				outerAngle > 90.0f ? outerAngle = 90.0f : outerAngle;
				outerAngle < 0.0f ? outerAngle = 0.0f : outerAngle;

				if (outerAngle >= innerAngle)
				{
					asSpotLight->SetOuterAngle(DegToRad(outerAngle));
					modified = true;
				}
			}
			ImGui::PopStyleVar();

			if (modified)
			{
				App->GetModule<ModuleScene>()->GetLoadedScene()->UpdateSceneSpotLights();
				App->GetModule<ModuleScene>()->GetLoadedScene()->RenderSpotLights();
			}

			ImGui::EndTable();
		}
	}
}
