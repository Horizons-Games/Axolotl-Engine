#include "WindowComponentDirLight.h"

#include "Application.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"
#include "DataModels/Components/ComponentDirLight.h"

WindowComponentDirLight::WindowComponentDirLight(const std::weak_ptr<ComponentDirLight>& component) :
	ComponentWindow("DIRECTIONAL LIGHT", component)
{
}

void WindowComponentDirLight::DrawWindowContents()
{
	this->DrawEnableAndDeleteComponent();

	std::shared_ptr<ComponentDirLight> asSharedDirLight =
		std::dynamic_pointer_cast<ComponentDirLight>(this->component.lock());

	if (asSharedDirLight)
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
			float intensity = asSharedDirLight->GetIntensity();
			if (ImGui::DragFloat("##Intensity", &intensity, 0.01f, 0.0f, 1.0f))
			{
				asSharedDirLight->SetIntensity(intensity);
				modified = true;
			}
			ImGui::PopStyleVar();

			ImGui::Text("Color"); ImGui::SameLine();
			float3 color = asSharedDirLight->GetColor();
			if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			{
				asSharedDirLight->SetColor(color);
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
