#include "WindowComponentAmbient.h"

#include "Application.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"
#include "DataModels/Components/ComponentAmbient.h"

WindowComponentAmbient::WindowComponentAmbient(const std::weak_ptr<ComponentAmbient>& component) :
	ComponentWindow("AMBIENT LIGHT", component)
{
}

void WindowComponentAmbient::DrawWindowContents()
{
	this->DrawEnableAndDeleteComponent();

	std::shared_ptr<ComponentAmbient> asSharedAmbient =
		std::dynamic_pointer_cast<ComponentAmbient>(this->component.lock());

	if (asSharedAmbient)
	{
		ImGui::Dummy(ImVec2(0.0f, 2.5f));

		if (ImGui::BeginTable("AmbientLightTable", 2))
		{
			ImGui::TableNextColumn();

			ImGui::Text("Color");
			ImGui::SameLine();

			float3 color = asSharedAmbient->GetColor();
			if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			{
				asSharedAmbient->SetColor(color);
				App->scene->GetLoadedScene()->RenderAmbientLight();
			}
			ImGui::EndTable();
		}
	}
}
