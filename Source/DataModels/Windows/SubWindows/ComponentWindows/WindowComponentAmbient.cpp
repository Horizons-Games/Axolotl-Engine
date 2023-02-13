#include "WindowComponentAmbient.h"

#include "Application.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"
#include "DataModels/Components/ComponentAmbient.h"

WindowComponentAmbient::WindowComponentAmbient(ComponentAmbient* component) :
	ComponentWindow("AMBIENT LIGHT", component)
{
}

void WindowComponentAmbient::DrawWindowContents()
{
	this->DrawEnableAndDeleteComponent();

	ComponentAmbient* asAmbient = static_cast<ComponentAmbient*>(this->component);

	if (asAmbient)
	{
		ImGui::Dummy(ImVec2(0.0f, 2.5f));

		if (ImGui::BeginTable("AmbientLightTable", 2))
		{
			ImGui::TableNextColumn();

			ImGui::Text("Color");
			ImGui::SameLine();

			float3 color = asAmbient->GetColor();
			if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			{
				asAmbient->SetColor(color);
				App->scene->GetLoadedScene()->RenderAmbientLight();
			}
			ImGui::EndTable();
		}
	}
}
