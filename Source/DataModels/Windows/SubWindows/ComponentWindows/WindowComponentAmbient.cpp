#include "WindowComponentAmbient.h"

#include "Application.h"
#include "DataModels/Components/ComponentAmbient.h"
#include "DataModels/Scene/Scene.h"
#include "ModuleScene.h"

WindowComponentAmbient::WindowComponentAmbient(ComponentAmbient* component) :
	ComponentWindow("AMBIENT LIGHT", component)
{
}

WindowComponentAmbient::~WindowComponentAmbient()
{
}

void WindowComponentAmbient::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentAmbient* asAmbient = static_cast<ComponentAmbient*>(component);

	if (asAmbient)
	{
		ImGui::Dummy(ImVec2(0.0f, 2.5f));

		if (ImGui::BeginTable("AmbientLightTable", 2))
		{
			ImGui::TableNextColumn();

			ImGui::Text("Color");
			ImGui::SameLine();

			float3 color = asAmbient->GetColor();
			if (ImGui::ColorEdit3("MyColor##1", (float*) &color))
			{
				asAmbient->SetColor(color);
				App->scene->GetLoadedScene()->RenderAmbientLight();
			}
			ImGui::EndTable();
		}
	}
}
