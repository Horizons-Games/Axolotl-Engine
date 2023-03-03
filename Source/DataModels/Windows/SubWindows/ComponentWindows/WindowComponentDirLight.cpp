#include "WindowComponentDirLight.h"

#include "Application.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"
#include "DataModels/Components/ComponentDirLight.h"

WindowComponentDirLight::WindowComponentDirLight(ComponentDirLight* component) :
	ComponentWindow("DIRECTIONAL LIGHT", component)
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

			ImGui::Text("Intensity"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			float intensity = asDirLight->GetIntensity();
			if (ImGui::DragFloat("##Intensity", &intensity, 0.01f, 0.0f, 8.0f))
			{
				if (intensity > 8.0f)
					intensity = 8.0f;

				asDirLight->SetIntensity(intensity);
				modified = true;
			}
			ImGui::PopStyleVar();

			ImGui::Text("Color"); ImGui::SameLine();
			float3 color = asDirLight->GetColor();
			if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			{
				asDirLight->SetColor(color);
				modified = true;
			}

			if (modified)
			{
				App->scene->GetLoadedScene()->RenderDirectionalLight();
			}

			ImGui::EndTable();
		}
	}
}
