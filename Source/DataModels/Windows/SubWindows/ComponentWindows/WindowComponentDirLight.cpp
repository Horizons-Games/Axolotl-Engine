#include "StdAfx.h"

#include "WindowComponentDirLight.h"

#include "Application.h"

#include "DataModels/Components/ComponentDirLight.h"
#include "DataModels/Scene/Scene.h"

#include "ModuleRender.h"
#include "ModuleScene.h"

WindowComponentDirLight::WindowComponentDirLight(ComponentDirLight* component) :
	WindowComponentLight("DIRECTIONAL LIGHT", component)
{
}

WindowComponentDirLight::~WindowComponentDirLight()
{
}

void WindowComponentDirLight::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentDirLight* asDirLight = static_cast<ComponentDirLight*>(component);

	if (asDirLight)
	{
		const char* lightTypes[] = { "Point", "Spot" };

		bool modified = false;

		ImGui::Dummy(ImVec2(0.0f, 2.5f));

		if (ImGui::BeginTable("DirLightTable", 2))
		{
			ImGui::TableNextColumn();

			ImGui::Text("Intensity");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			float intensity = asDirLight->GetIntensity();
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

				asDirLight->SetIntensity(intensity);
				modified = true;
			}
			ImGui::PopStyleVar();

			ImGui::Text("Color");
			ImGui::SameLine();
			float3 color = asDirLight->GetColor();
			if (ImGui::ColorEdit3("MyColor##1", (float*) &color))
			{
				asDirLight->SetColor(color);
				modified = true;
			}

			float2 shadowBias = asDirLight->GetShadowBias();

			ImGui::Text("Bias");
			ImGui::SameLine();
			if (ImGui::DragFloat2("##bias", &shadowBias[0], 0.00001f, 0.0f, 0.0f, "%.5f"))
			{
				asDirLight->SetShadowBias(shadowBias);
			}

			if (modified)
			{
				App->GetModule<ModuleScene>()->GetLoadedScene()->RenderDirectionalLight();
			}

			Shadows* shadows = App->GetModule<ModuleRender>()->GetShadows();
			float lambda = shadows->GetLambda();

			ImGui::Text("Shadow frustum lambda:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(math::Max(50.0f, ImGui::GetContentRegionAvail().x - 20.0f));
			if (ImGui::DragFloat("##lambda", &lambda, 0.001f, 0.001f, 0.999f, "%.3f"))
			{
				if (lambda >= 1.0f)
				{
					lambda = 0.999f;
				}
				else if (lambda <= 0.0f)
				{
					lambda = 0.001f;
				}

				shadows->SetLambda(lambda);
			};

			float offset = asDirLight->GetZNearOffset();

			ImGui::Text("Z near frustum offset:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(math::Max(50.0f, ImGui::GetContentRegionAvail().x - 20.0f));
			if (ImGui::DragFloat("##zOffset", &offset, 0.1f, -100.0f, 0.0f, "%.3f"))
			{
				if (offset < -100.0f)
				{
					offset = -100.0f;
				}
				else if (offset > 0.0f)
				{
					offset = 0.0f;
				}

				asDirLight->SetZNearOffset(offset);
			};

			float amount = asDirLight->GetBleedingAmount();

			ImGui::Text("Reduced bleeding amount:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(math::Max(50.0f, ImGui::GetContentRegionAvail().x - 20.0f));
			if (ImGui::DragFloat("##amount", &amount, 0.001f, 0.0f, 1.0f, "%.3f"))
			{
				if (amount > 1.0f)
				{
					amount = 1.0f;
				}
				else if (amount < 0.0f)
				{
					amount = 0.0f;
				}

				asDirLight->SetBleedingAmount(amount);
			};

			if (modified)
			{
				App->GetModule<ModuleScene>()->GetLoadedScene()->RenderDirectionalLight();
			}

			ImGui::EndTable();
		}
	}
}
